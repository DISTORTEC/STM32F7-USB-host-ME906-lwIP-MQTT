/**
 * \file
 * \brief HuaweiMe906 class implementation
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "HuaweiMe906.hpp"

#include "usbh_core.h"

#include "distortos/assert.h"
#include "distortos/Semaphore.hpp"

#include "estd/ScopeGuard.hpp"

#include <mutex>

#include <cinttypes>

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

std::pair<int, size_t> HuaweiMe906::read(const Port port, void* const buffer, const size_t size)
{
	assert(host_ != nullptr);

	auto& serialPort = port == Port::pcui ? pcuiPort_ : port == Port::networkCard ? networkCardPort_ : gpsPort_;
	return serialPort.read(*host_, buffer, size);
}

void HuaweiMe906::registerClass(USBH_HandleTypeDef& host)
{
	assert(host_ == nullptr);

	const auto ret = USBH_RegisterClass(&host, &classHandle_);
	assert(ret == USBH_OK);

	host_ = &host;
}

int HuaweiMe906::write(const Port port, const void* const buffer, const size_t size)
{
	assert(host_ != nullptr);

	auto& serialPort = port == Port::pcui ? pcuiPort_ : port == Port::networkCard ? networkCardPort_ : gpsPort_;
	return serialPort.write(*host_, buffer, size);
}

/*---------------------------------------------------------------------------------------------------------------------+
| SerialPort public functions
+---------------------------------------------------------------------------------------------------------------------*/

void HuaweiMe906::SerialPort::backgroundProcess(USBH_HandleTypeDef& host)
{
	const std::lock_guard lockGuard {mutex_};

	if (readSemaphore_ != nullptr)
	{
		if (readPending_ == false)
			requestRead(host);
		else if (USBH_LL_GetURBState(&host, readPipe_) == USBH_URB_DONE)
		{
			readPending_ = {};

			const auto size = USBH_LL_GetLastXferSize(&host, readPipe_);
			assert(size != 0);
			USBH_UsrLog("HuaweiMe906::SerialPort(%p)::backgroundProcess: received %" PRIu32 " bytes", this, size);

			readBegin_ += size;

			auto& readSemaphore = *readSemaphore_;
			readSemaphore_ = {};
			const auto ret = readSemaphore.post();
			assert(ret == 0);
		}
	}

	if (writeSemaphore_ != nullptr)
	{
		if (writePendingSize_ == 0)
			requestWrite(host);
		else if (USBH_LL_GetURBState(&host, writePipe_) == USBH_URB_DONE)
		{
			USBH_UsrLog("HuaweiMe906::SerialPort(%p)::backgroundProcess: transmitted %zu bytes",
					this, writePendingSize_);

			writeBegin_ += writePendingSize_;
			writePendingSize_ = {};

			if (writeBegin_ != writeEnd_)
				requestWrite(host);
			else
			{
				auto& writeSemaphore = *writeSemaphore_;
				writeSemaphore_ = {};
				const auto ret = writeSemaphore.post();
				assert(ret == 0);
			}
		}
	}
}

void HuaweiMe906::SerialPort::interfaceDeinitialize(USBH_HandleTypeDef& host)
{
	const std::lock_guard lockGuard {mutex_};

	USBH_UsrLog("HuaweiMe906::SerialPort(%p)::interfaceDeinitialize", this);

	active_ = {};

	if (readPipe_ != 0)
	{
		{
			const auto ret = USBH_ClosePipe(&host, readPipe_);
			assert(ret == USBH_OK);
		}
		{
			const auto ret = USBH_FreePipe(&host, readPipe_);
			assert(ret == USBH_OK);
		}
		readPipe_ = {};
	}

	if (writePipe_ != 0)
	{
		{
			const auto ret = USBH_ClosePipe(&host, writePipe_);
			assert(ret == USBH_OK);
		}
		{
			const auto ret = USBH_FreePipe(&host, writePipe_);
			assert(ret == USBH_OK);
		}
		writePipe_ = {};
	}

	if (readSemaphore_ != nullptr)
	{
		readPending_ = {};

		auto& readSemaphore = *readSemaphore_;
		readSemaphore_ = {};
		const auto ret = readSemaphore.post();
		assert(ret == 0);
	}

	if (writeSemaphore_ != nullptr)
	{
		writePendingSize_ = {};

		auto& writeSemaphore = *writeSemaphore_;
		writeSemaphore_ = {};
		const auto ret = writeSemaphore.post();
		assert(ret == 0);
	}
}

USBH_StatusTypeDef HuaweiMe906::SerialPort::interfaceInitialize(USBH_HandleTypeDef& host)
{
	const std::lock_guard lockGuard {mutex_};

	const auto interface = USBH_FindInterface(&host, 0xff, 0x01, protocol_);
	if (interface == 0xff)
		return USBH_FAIL;

	const auto& interfaceDescriptor = host.device.CfgDesc.Itf_Desc[interface];
	const USBH_EpDescTypeDef* readEndpointDescriptor {};
	const USBH_EpDescTypeDef* writeEndpointDescriptor {};
	for (size_t i {}; i < interfaceDescriptor.bNumEndpoints; ++i)
	{
		const auto& endpointDescriptor = interfaceDescriptor.Ep_Desc[i];
		if (endpointDescriptor.bmAttributes != USB_EP_TYPE_BULK)
			continue;

		if ((endpointDescriptor.bEndpointAddress & USB_EP_DIR_MSK) == USB_EP_DIR_IN)
			readEndpointDescriptor = &endpointDescriptor;
		else
			writeEndpointDescriptor = &endpointDescriptor;
	}

	if (readEndpointDescriptor == nullptr || writeEndpointDescriptor == nullptr)
		return USBH_FAIL;

	readPipe_ = USBH_AllocPipe(&host, readEndpointDescriptor->bEndpointAddress);
	assert(readPipe_ != 0xffff);
	readEndpointSize_ = readEndpointDescriptor->wMaxPacketSize;
	{
		const auto ret = USBH_OpenPipe(&host, readPipe_, readEndpointDescriptor->bEndpointAddress, host.device.address,
				host.device.speed, USB_EP_TYPE_BULK, readEndpointSize_);
		if (ret != USBH_OK)
			return ret;
	}
	{
		const auto ret = USBH_LL_SetToggle(&host, readPipe_, 0);
		if (ret != USBH_OK)
			return ret;
	}

	writePipe_ = USBH_AllocPipe(&host, writeEndpointDescriptor->bEndpointAddress);
	assert(writePipe_ != 0xffff);
	writeEndpointSize_ = writeEndpointDescriptor->wMaxPacketSize;
	{
		const auto ret = USBH_OpenPipe(&host, writePipe_, writeEndpointDescriptor->bEndpointAddress,
				host.device.address, host.device.speed, USB_EP_TYPE_BULK, writeEndpointSize_);
		if (ret != USBH_OK)
			return ret;
	}
	{
		const auto ret = USBH_LL_SetToggle(&host, writePipe_, 0);
		if (ret != USBH_OK)
			return ret;
	}

	active_ = true;

	USBH_UsrLog("HuaweiMe906::SerialPort(%p)::interfaceInitialize: interface %" PRIu8 ", "
			"read endpoint address 0x%" PRIx8 ", read endpoint size %" PRIu16 ", read pipe %" PRIu8 ", "
			"write endpoint address 0x%" PRIx8 ", write endpoint size %" PRIu16 ", write pipe %" PRIu8,
			this, interface, readEndpointDescriptor->bEndpointAddress, readEndpointSize_, readPipe_,
			writeEndpointDescriptor->bEndpointAddress, writeEndpointSize_, writePipe_);

	return USBH_OK;
}

std::pair<int, size_t> HuaweiMe906::SerialPort::read(USBH_HandleTypeDef& host, void* const buffer, const size_t size)
{
	const std::lock_guard readLockGuard {readMutex_};

	assert(buffer != nullptr);

	if (size == 0)
		return {{}, {}};

	distortos::Semaphore semaphore {0};

	{
		const std::lock_guard lockGuard {mutex_};

		if (active_ == false)
			return {ENOTCONN, {}};

		readBegin_ = static_cast<decltype(readBegin_)>(buffer);
		readEnd_ = readBegin_ + size;
		readSemaphore_ = &semaphore;
	}

	const auto scopeGuard = estd::makeScopeGuard([this]()
			{
				readBegin_ = {};
				readEnd_ = {};
			});

#if USBH_USE_OS == 1
	{
		host.os_msg = USBH_CLASS_EVENT;
		const auto ret = osMessageQueuePut(host.os_event, &host.os_msg, 0U, osWaitForever);
		assert(ret == osOK);
	}
#endif	// USBH_USE_OS == 1

	{
		const auto ret = semaphore.wait();
		assert(ret == 0);
	}

	const auto bytesRead = readBegin_ - static_cast<decltype(readBegin_)>(buffer);
	return {bytesRead != 0 ? 0 : ENOTCONN, bytesRead};
}

int HuaweiMe906::SerialPort::write(USBH_HandleTypeDef& host, const void* const buffer, const size_t size)
{
	const std::lock_guard writeLockGuard {writeMutex_};

	distortos::Semaphore semaphore {0};

	{
		const std::lock_guard lockGuard {mutex_};

		if (active_ == false)
			return ENOTCONN;

		writeBegin_ = static_cast<decltype(writeBegin_)>(buffer);
		writeEnd_ = writeBegin_ + size;
		writeSemaphore_ = &semaphore;
	}

	const auto scopeGuard = estd::makeScopeGuard([this]()
			{
				writeBegin_ = {};
				writeEnd_ = {};
			});

#if USBH_USE_OS == 1
	{
		host.os_msg = USBH_CLASS_EVENT;
		const auto ret = osMessageQueuePut(host.os_event, &host.os_msg, 0U, osWaitForever);
		assert(ret == osOK);
	}
#endif	// USBH_USE_OS == 1

	{
		const auto ret = semaphore.wait();
		assert(ret == 0);
	}

	const auto bytesWritten = writeBegin_ - static_cast<decltype(writeBegin_)>(buffer);
	return bytesWritten != 0 ? 0 : ENOTCONN;
}

/*---------------------------------------------------------------------------------------------------------------------+
| SerialPort private functions
+---------------------------------------------------------------------------------------------------------------------*/

void HuaweiMe906::SerialPort::requestRead(USBH_HandleTypeDef& host)
{
	assert(readPending_ == false);
	assert(readBegin_ != nullptr && readEnd_ != nullptr);

	const auto size = std::min<size_t>(readEnd_ - readBegin_, readEndpointSize_);
	assert(size != 0);

	USBH_UsrLog("HuaweiMe906::SerialPort(%p)::requestRead: requesting %zu bytes", this, size);

	const auto ret = USBH_BulkReceiveData(&host, readBegin_, size, readPipe_);
	assert(ret == USBH_OK);
	readPending_ = true;
}

void HuaweiMe906::SerialPort::requestWrite(USBH_HandleTypeDef& host)
{
	assert(writePendingSize_ == 0);
	assert(writeBegin_ != nullptr && writeEnd_ != nullptr);

	const auto size = std::min<size_t>(writeEnd_ - writeBegin_, writeEndpointSize_);
	assert(size != 0);

	USBH_UsrLog("HuaweiMe906::SerialPort(%p)::requestWrite: requesting %zu bytes", this, size);

	const auto ret = USBH_BulkSendData(&host, const_cast<uint8_t*>(writeBegin_), size, writePipe_, 1);
	assert(ret == USBH_OK);
	writePendingSize_ = size;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

void HuaweiMe906::backgroundProcess(USBH_HandleTypeDef& host)
{
	pcuiPort_.backgroundProcess(host);
	networkCardPort_.backgroundProcess(host);
	gpsPort_.backgroundProcess(host);
}

void HuaweiMe906::interfaceDeinitialize(USBH_HandleTypeDef& host)
{
	pcuiPort_.interfaceDeinitialize(host);
	networkCardPort_.interfaceDeinitialize(host);
	gpsPort_.interfaceDeinitialize(host);
}

USBH_StatusTypeDef HuaweiMe906::interfaceInitialize(USBH_HandleTypeDef& host)
{
	const auto vid = host.device.DevDesc.idVendor;
	const auto pid = host.device.DevDesc.idProduct;
	const auto match = (vid == 0x12d1 && pid == 0x1573) || (vid == 0x3f0 && pid == 0x581d);

	if (match == false)
		return USBH_FAIL;

	{
		const auto ret = pcuiPort_.interfaceInitialize(host);
		if (ret != USBH_OK)
			return ret;
	}
	{
		const auto ret = networkCardPort_.interfaceInitialize(host);
		if (ret != USBH_OK)
			return ret;
	}
	{
		const auto ret = gpsPort_.interfaceInitialize(host);
		if (ret != USBH_OK)
			return ret;
	}

	return USBH_OK;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static functions
+---------------------------------------------------------------------------------------------------------------------*/

USBH_StatusTypeDef HuaweiMe906::backgroundProcess(USBH_HandleTypeDef* const host)
{
	assert(host != nullptr && host->pActiveClass->pData != nullptr);
	auto& that = *static_cast<HuaweiMe906*>(host->pActiveClass->pData);
	that.backgroundProcess(*host);
	return USBH_OK;	// returned value of BgndProcess() is ignored in usbh_core.c
}

USBH_StatusTypeDef HuaweiMe906::handleControlRequests(USBH_HandleTypeDef* const host)
{
	assert(host != nullptr && host->pActiveClass->pData != nullptr);
	if (host->pUser != nullptr)
		host->pUser(host, HOST_USER_CLASS_ACTIVE);
	return USBH_OK;
}

USBH_StatusTypeDef HuaweiMe906::interfaceDeinitialize(USBH_HandleTypeDef* const host)
{
	assert(host != nullptr && host->pActiveClass->pData != nullptr);
	auto& that = *static_cast<HuaweiMe906*>(host->pActiveClass->pData);
	that.interfaceDeinitialize(*host);
	return USBH_OK;	// returned value of DeInit() is ignored in usbh_core.c
}

USBH_StatusTypeDef HuaweiMe906::interfaceInitialize(USBH_HandleTypeDef* const host)
{
	assert(host != nullptr && host->pActiveClass->pData != nullptr);
	auto& that = *static_cast<HuaweiMe906*>(host->pActiveClass->pData);
	return that.interfaceInitialize(*host);
}

USBH_StatusTypeDef HuaweiMe906::sofProcess(USBH_HandleTypeDef* const host)
{
	assert(host != nullptr && host->pActiveClass->pData != nullptr);
	return USBH_OK;
}

/**
 * \file
 * \brief HuaweiMe906 class header
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HUAWEIME906_HPP_
#define HUAWEIME906_HPP_

#include "usbh_def.h"

#include "distortos/ConditionVariable.hpp"
#include "distortos/Mutex.hpp"

#include <memory>

class HuaweiMe906AsynchronousReader;

namespace distortos
{

class Semaphore;

}	// namespace distortos

/// HuaweiMe906 class is a driver for Huawei ME906 modem
class HuaweiMe906
{
public:

	/// import HuaweiMe906AsynchronousReader as AsynchronousReader
	using AsynchronousReader = HuaweiMe906AsynchronousReader;

	/// identifier of Huawei ME906 port
	enum class Port : uint8_t
	{
		/// PCUI port
		pcui,
		/// network card port
		networkCard,
		/// GPS port
		gps
	};

	/**
	 * \brief HuaweiMe906's constructor
	 */

	constexpr HuaweiMe906() :
			classHandle_{"Huawei ME906", 0xff, &HuaweiMe906::interfaceInitialize, &HuaweiMe906::interfaceDeinitialize,
					&HuaweiMe906::handleControlRequests, &HuaweiMe906::backgroundProcess, &HuaweiMe906::sofProcess,
					this},
			pcuiPort_{0x02},
			networkCardPort_{0x01},
			gpsPort_{0x05},
			host_{}
	{

	}

	/**
	 * \brief Reads data from one of serial ports.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \pre Device is registered.
	 * \pre \a buffer is valid.
	 *
	 * \param [in] port selects the serial port that will be accessed
	 * \param [out] buffer is the buffer to which the data will be written, must be valid
	 * \param [in] size is the size of \a buffer, bytes
	 *
	 * \return pair with return code (0 on success, error code otherwise) and number of read bytes (valid even when
	 * error code is returned); error codes:
	 * - ENOTCONN - the device is disconnected;
	 */

	std::pair<int, size_t> read(Port port, void* buffer, size_t size);

	/**
	 * \brief Registers the device with USB host.
	 *
	 * \pre Device is not registered.
	 *
	 * \post Device is registered.
	 *
	 * \param [in] host is a reference to USB host with which this device will be registered
	 */

	void registerClass(USBH_HandleTypeDef& host);

	/**
	 * \brief Starts asynchronous read of one of serial ports.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \pre Device is registered.
	 *
	 * \param [in] port selects the serial port for which the asynchronous read will be started
	 * \param [in] asynchronousReader is a reference to the asynchronous reader which will be used
	 *
	 * \return 0 on success, error code otherwise:
	 * - ENOTCONN - the device is disconnected;
	 */

	int startAsynchronousRead(Port port, AsynchronousReader& asynchronousReader);

	/**
	 * \brief Stops asynchronous read of one of serial ports.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \pre Device is registered.
	 * \pre Asynchronous read is in progress on selected serial port.
	 *
	 * \post Asynchronous read is not in progress.
	 *
	 * \param [in] port selects the serial port for which the asynchronous read will be stopped
	 */

	void stopAsynchronousRead(Port port);

	/**
	 * \brief Writes data to one of serial ports.
	 *
	 * \warning This function must not be called from interrupt context!
	 *
	 * \pre Device is registered.
	 * \pre \a buffer is valid.
	 *
	 * \param [in] port selects the serial port that will be accessed
	 * \param [in] buffer is the buffer with data that will be transmitted, must be valid
	 * \param [in] size is the size of \a buffer, bytes
	 *
	 * \return 0 on success, error code otherwise:
	 * - ENOTCONN - the device is disconnected;
	 */

	int write(Port port, const void* buffer, size_t size);

private:

	/// SerialPort class is an internal serial port of Huawei ME906 modem
	class SerialPort
	{
	public:

		/**
		 * \brief SerialPort's constructor
		 *
		 * \param [in] protocol is the value of `bInterfaceProtocol` field in interface descriptor of this serial port
		 */

		constexpr SerialPort(const uint8_t protocol) :
				mutex_{distortos::Mutex::Type::recursive, distortos::Mutex::Protocol::priorityInheritance},
				readConditionVariable_{},
				writeMutex_{distortos::Mutex::Protocol::priorityInheritance},
				asynchronousReader_{},
				readBuffer_{},
				writeBegin_{},
				writeEnd_{},
				writeSemaphore_{},
				readBegin_{},
				readEnd_{},
				readEndpointSize_{},
				writeEndpointSize_{},
				writePendingSize_{},
				active_{},
				protocol_{protocol},
				readPending_{},
				readPipe_{},
				writePipe_{}
		{

		}

		/**
		 * \brief Background process of the serial port.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 */

		void backgroundProcess(USBH_HandleTypeDef& host);

		/**
		 * \brief Deinitializes the serial port.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 */

		void interfaceDeinitialize(USBH_HandleTypeDef& host);

		/**
		 * \brief Tries to initialize the serial port.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 *
		 * \return USBH_OK on success, error code otherwise:
		 * - USBH_FAIL - initialization of the serial port failed;
		 * - error codes returned by USBH_LL_SetToggle();
		 * - error codes returned by USBH_OpenPipe();
		 */

		USBH_StatusTypeDef interfaceInitialize(USBH_HandleTypeDef& host);

		/**
		 * \brief Reads data from the serial ports.
		 *
		 * \warning This function must not be called from interrupt context!
		 *
		 * \pre \a buffer is valid.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 * \param [out] buffer is the buffer to which the data will be written, must be valid
		 * \param [in] size is the size of \a buffer, bytes
		 *
		 * \return pair with return code (0 on success, error code otherwise) and number of read bytes (valid even when
		 * error code is returned); error codes:
		 * - ENOTCONN - the device is disconnected;
		 */

		std::pair<int, size_t> read(USBH_HandleTypeDef& host, void* buffer, size_t size);

		/**
		 * \brief Starts asynchronous read of serial port.
		 *
		 * \warning This function must not be called from interrupt context!
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 * \param [in] asynchronousReader is a reference to the asynchronous reader which will be used
		 *
		 * \return 0 on success, error code otherwise:
		 * - ENOTCONN - the device is disconnected;
		 */

		int startAsynchronousRead(USBH_HandleTypeDef& host, AsynchronousReader& asynchronousReader);

		/**
		 * \brief Stops asynchronous read of serial port.
		 *
		 * \warning This function must not be called from interrupt context!
		 *
		 * \pre Asynchronous read is in progress.
		 *
		 * \post Asynchronous read is not in progress.
		 */

		void stopAsynchronousRead();

		/**
		 * \brief Writes data to the serial ports.
		 *
		 * \warning This function must not be called from interrupt context!
		 *
		 * \pre \a buffer is valid.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 * \param [in] buffer is the buffer with data that will be transmitted, must be valid
		 * \param [in] size is the size of \a buffer, bytes
		 *
		 * \return 0 on success, error code otherwise:
		 * - ENOTCONN - the device is disconnected;
		 */

		int write(USBH_HandleTypeDef& host, const void* buffer, size_t size);

	private:

		/**
		 * \brief Requests writing of next block of data to the serial port.
		 *
		 * \pre No write is in progress.
		 * \pre \a writeBegin_ and \a writeEnd_ are valid.
		 * \pre Size of next block is not 0.
		 *
		 * \post Write is in progress.
		 *
		 * \param [in] host is a reference to USB host with which this device is registered
		 */

		void requestWrite(USBH_HandleTypeDef& host);

		/// mutex for general access serialization
		distortos::Mutex mutex_;

		/// condition variable for read access serialization, associated with \a mutex_
		distortos::ConditionVariable readConditionVariable_;

		/// mutex for write access serialization
		distortos::Mutex writeMutex_;

		/// pointer to currently associated asynchronous reader
		AsynchronousReader* asynchronousReader_;

		/// read buffer, \a readEndpointSize_ bytes long
		std::unique_ptr<uint8_t[]> readBuffer_;

		/// pointer to first byte of write block
		const uint8_t* writeBegin_;

		/// pointer to "one past the last" byte of write block
		const uint8_t* writeEnd_;

		/// pointer to semaphore used for synchronization of write operation
		distortos::Semaphore* writeSemaphore_;

		/// index of first byte of data available for reading
		uint16_t readBegin_;

		/// index of "one past the last" byte of data available for reading
		uint16_t readEnd_;

		/// size of read endpoint, bytes
		uint16_t readEndpointSize_;

		/// size of write endpoint, bytes
		uint16_t writeEndpointSize_;

		/// size of pending write block
		uint16_t writePendingSize_;

		/// true if serial port is active, false otherwise
		bool active_;

		/// value of `bInterfaceProtocol` field in interface descriptor of this serial port
		uint8_t protocol_;

		/// true if read is pending, false otherwise
		bool readPending_;

		/// identifier of read pipe
		uint8_t readPipe_;

		/// identifier of write pipe
		uint8_t writePipe_;
	};

	class SynchronousReader;

	/**
	 * \brief Background process of the device.
	 *
	 * \param [in] host is a reference to USB host with which this device is registered
	 */

	void backgroundProcess(USBH_HandleTypeDef& host);

	/**
	 * \brief Deinitializes the device.
	 *
	 * \param [in] host is a reference to USB host with which this device is registered
	 */

	void interfaceDeinitialize(USBH_HandleTypeDef& host);

	/**
	 * \brief Tries to initialize the device.
	 *
	 * \param [in] host is a reference to USB host with which this device is registered
	 *
	 * \return USBH_OK on success, error code otherwise:
	 * - error codes returned by HuaweiMe906::SerialPort::interfaceInitialize();
	 */

	USBH_StatusTypeDef interfaceInitialize(USBH_HandleTypeDef& host);

	/**
	 * \brief Background process of the device.
	 *
	 * \pre \a host is valid.
	 * \pre `host->pActiveClass->pData` is valid and points to an instance of HuaweiMe906 class.
	 *
	 * \param [in] host is a pointer to USB host with which this device is registered, must be valid
	 *
	 * \return always USBH_OK
	 */

	static USBH_StatusTypeDef backgroundProcess(USBH_HandleTypeDef* host);

	/**
	 * \brief Handles control request of the device.
	 *
	 * \pre \a host is valid.
	 * \pre `host->pActiveClass->pData` is valid and points to an instance of HuaweiMe906 class.
	 *
	 * \param [in] host is a pointer to USB host with which this device is registered, must be valid
	 *
	 * \return always USBH_OK
	 */

	static USBH_StatusTypeDef handleControlRequests(USBH_HandleTypeDef* host);

	/**
	 * \brief Deinitializes the device.
	 *
	 * \pre \a host is valid.
	 * \pre `host->pActiveClass->pData` is valid and points to an instance of HuaweiMe906 class.
	 *
	 * \param [in] host is a pointer to USB host with which this device is registered, must be valid
	 *
	 * \return always USBH_OK
	 */

	static USBH_StatusTypeDef interfaceDeinitialize(USBH_HandleTypeDef* host);

	/**
	 * \brief Tries to initialize the device.
	 *
	 * \pre \a host is valid.
	 * \pre `host->pActiveClass->pData` is valid and points to an instance of HuaweiMe906 class.
	 *
	 * \param [in] host is a pointer to USB host with which this device is registered, must be valid
	 *
	 * \return USBH_OK on success, error code otherwise:
	 * - error codes returned by HuaweiMe906::interfaceInitialize(USBH_HandleTypeDef&);
	 */

	static USBH_StatusTypeDef interfaceInitialize(USBH_HandleTypeDef* host);

	/**
	 * \brief Processes device's operation from SOF interrupt.
	 *
	 * \pre \a host is valid.
	 * \pre `host->pActiveClass->pData` is valid and points to an instance of HuaweiMe906 class.
	 *
	 * \param [in] host is a pointer to USB host with which this device is registered, must be valid
	 *
	 * \return always USBH_OK
	 */

	static USBH_StatusTypeDef sofProcess(USBH_HandleTypeDef* host);

	/// class handle for USB host
	USBH_ClassTypeDef classHandle_;

	/// PCUI port
	SerialPort pcuiPort_;

	/// network card port
	SerialPort networkCardPort_;

	/// GPS port
	SerialPort gpsPort_;

	/// pointer to USB host with which this device is registered
	USBH_HandleTypeDef* host_;
};

#endif	// HUAWEIME906_HPP_

/**
 * \file
 * \brief PpposManager class implementation
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PpposManager.hpp"

#include "openHuaweiMe906.hpp"

#include "distortos/DynamicThread.hpp"
#include "distortos/ThisThread.hpp"

#include "estd/ScopeGuard.hpp"

#include "lwip/dns.h"

#include "netif/ppp/pppapi.h"

#include <mutex>

#include <cstring>

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

void PpposManager::initialize()
{
	const std::lock_guard lockGuard {mutex_};

	assert(pcb_ == nullptr);

	pcb_ = pppapi_pppos_create(&netif_, &PpposManager::ppposOutput, &PpposManager::pppLinkStatus, this);
	assert(pcb_ != nullptr);

	ppp_set_usepeerdns(pcb_, 1);

	{
		const auto ret = pppapi_set_default(pcb_);
		assert(ret == ERR_OK);
	}
	{
		const auto ret = distortos::makeAndStartDynamicThread({4096, 1}, &PpposManager::threadFunction, this).detach();
		assert(ret == 0);
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

void PpposManager::chatScriptPhase() const
{
	static const char* const chatScript[][2]
	{
			// http://lists.nongnu.org/archive/html/lwip-users/2019-03/msg00096.html
			{"ATI", "OK"},
			{"AT^SYSCFGEX=\"030201\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,", "OK"},
			{"AT+COPS=0", "OK"},
			{"AT+CGDCONT=1,\"IP\",\"internet\",\"0.0.0.0\",0,0", "OK"},
			{"AT+CGDCONT=16,\"IP\",\"\",\"0.0.0.0\",0,0", "OK"},
			{"AT+CGATT=1", "OK"},
			{"ATD*99#", "CONNECT"},
	};

	static const char* const abortStrings[]
	{
			"+CME ERROR:",
			"ERROR",
			"NO CARRIER"
	};

	const auto inputStream = openHuaweiMe906(huaweiMe906_, HuaweiMe906::Port::networkCard, "r");
	const auto outputStream = openHuaweiMe906(huaweiMe906_, HuaweiMe906::Port::networkCard, "w");

	const auto closeScopeGuard = estd::makeScopeGuard([inputStream, outputStream]()
			{
				fclose(inputStream);
				fclose(outputStream);
			});

	size_t index {};
	while (index < std::size(chatScript))
	{
		const auto success = executeCommand(*outputStream, *inputStream, chatScript[index][0], chatScript[index][1],
				estd::ContiguousRange<const char* const>{abortStrings});
		if (success == false)
		{
			distortos::ThisThread::sleepFor(std::chrono::seconds{5});
			index = {};
			continue;
		}

		++index;
	}
}

bool PpposManager::executeCommand(FILE& outputStream, FILE& inputStream, const char* const command,
		const char* const expectedResponse, const estd::ContiguousRange<const char* const> abortStrings) const
{
	{
		const auto ret = fiprintf(&outputStream, "%s\r\n", command);
		if (ret < 0)
		{
			fiprintf(standardOutputStream, "PpposManager::executeCommand: write failed, errno = %d\r\n", errno);
			clearerr(&outputStream);
			return false;
		}

		fiprintf(standardOutputStream, "PpposManager::executeCommand: wrote %d bytes: \"%s\"\r\n", ret, command);
	}

	while (1)
	{
		char buffer[64];
		if (fgets(buffer, sizeof(buffer), &inputStream) == nullptr)
		{
			fiprintf(standardOutputStream, "PpposManager::executeCommand: read failed, errno = %d\r\n", errno);
			clearerr(&inputStream);
			return false;
		}

		const auto fullLength = strlen(buffer);
		auto length = fullLength;
		// trim trailing whitespace
		while (length > 0 && (buffer[length - 1] == '\r' || buffer[length - 1] == '\n'))
			--length;

		if (length == 0)
			continue;

		buffer[length] = '\0';
		fiprintf(standardOutputStream, "PpposManager::executeCommand: read %zu bytes: \"%s\"\r\n", fullLength, buffer);

		if (strncmp(buffer, expectedResponse, strlen(expectedResponse)) == 0)
			return true;

		for (const auto abortString : abortStrings)
			if (strncmp(buffer, abortString, strlen(abortString)) == 0)
				return false;
	}
}

void PpposManager::pppLinkStatus(const int errorCode)
{
	switch (errorCode)
	{
		case PPPERR_NONE:	// no error
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_NONE\r\n");
#if LWIP_IPV4
			fiprintf(standardOutputStream, "  ip4 = %s\r\n", ip4addr_ntoa(netif_ip4_addr(&netif_)));
			fiprintf(standardOutputStream, "  gateway = %s\r\n", ip4addr_ntoa(netif_ip4_gw(&netif_)));
			fiprintf(standardOutputStream, "  netmask = %s\r\n", ip4addr_ntoa(netif_ip4_netmask(&netif_)));
#endif	// LWIP_IPV4
#if LWIP_IPV6
			fiprintf(standardOutputStream, "  ip6 = %s\r\n", ip6addr_ntoa(netif_ip6_addr(&netif_, 0)));
#endif	// LWIP_IPV6
#if LWIP_DNS
			fiprintf(standardOutputStream, "  dns1 = %s\r\n", ipaddr_ntoa(dns_getserver(0)));
			fiprintf(standardOutputStream, "  dns2 = %s\r\n", ipaddr_ntoa(dns_getserver(1)));
#endif	// LWIP_DNS
			break;
		case PPPERR_PARAM:	// invalid parameter
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_PARAM\r\n");
			break;
		case PPPERR_OPEN:	// unable to open PPP session
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_OPEN\r\n");
			break;
		case PPPERR_DEVICE:	// invalid I/O device for PPP
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_DEVICE\r\n");
			break;
		case PPPERR_ALLOC:	// unable to allocate resources
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_ALLOC\r\n");
			break;
		case PPPERR_USER:	// user interrupt
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_USER\r\n");
			break;
		case PPPERR_CONNECT:	// connection lost
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_CONNECT\r\n");
			break;
		case PPPERR_AUTHFAIL:	// failed authentication challenge
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_AUTHFAIL\r\n");
			break;
		case PPPERR_PROTOCOL:	// failed to meet protocol
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_PROTOCOL\r\n");
			break;
		case PPPERR_PEERDEAD:	// connection timeout
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_PEERDEAD\r\n");
			break;
		case PPPERR_IDLETIMEOUT:	// idle Timeout
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_IDLETIMEOUT\r\n");
			break;
		case PPPERR_CONNECTTIME:	// max connect time reached
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_CONNECTTIME\r\n");
			break;
		case PPPERR_LOOPBACK:	// connection timeout
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: PPPERR_LOOPBACK\r\n");
			break;
		default:
			fiprintf(standardOutputStream, "PpposManager::pppLinkStatus: error code = %d\r\n", errorCode);
			break;
	}

	{
		const std::lock_guard lockGuard {mutex_};

		connected_ = errorCode == PPPERR_NONE;

		if (semaphore_ != nullptr)
		{
			auto& semaphore = *semaphore_;
			semaphore_ = {};
			const auto ret = semaphore.post();
			assert(ret == 0);
		}
	}
}

u32_t PpposManager::ppposOutput(u8_t* const buffer, const u32_t size) const
{
	const auto ret = huaweiMe906_.write(HuaweiMe906::Port::networkCard, buffer, size);
	if (ret != 0)
	{
		fiprintf(standardOutputStream, "PpposManager::ppposOutput: write failed, ret = %d\r\n", ret);
		return {};
	}

	return size;
}

void PpposManager::ppposPhase() const
{
	{
		const auto ret = pppapi_connect(pcb_, 0);
		if (ret != ERR_OK)
		{
			fiprintf(standardOutputStream, "PpposManager::ppposPhase: pppapi_connect() failed, ret = %d\r\n", ret);
			return;
		}
	}

	const auto disconnectScopeGuard = estd::makeScopeGuard([this]()
			{
				const auto ret = pppapi_close(pcb_, 0);
				if (ret != ERR_OK)
					fiprintf(standardOutputStream, "PpposManager::ppposPhase: pppapi_close() failed, ret = %d\r\n",
							ret);
			});

	while (1)
	{
		uint8_t buffer[64];
		size_t bytesRead;
		{
			int ret;
			std::tie(ret, bytesRead) = huaweiMe906_.read(HuaweiMe906::Port::networkCard, buffer, sizeof(buffer));
			if (ret != 0)
			{
				fiprintf(standardOutputStream, "PpposManager::ppposPhase: read failed, ret = %d\r\n", ret);
				return;
			}
		}

		if (bytesRead == 0)
			continue;

		const auto ret = pppos_input_tcpip(pcb_, buffer, bytesRead);
		if (ret != ERR_OK)
		{
			fiprintf(standardOutputStream, "PpposManager::ppposPhase: pppos_input_tcpip() failed, ret = %d\r\n", ret);
			return;
		}
	}
}

void PpposManager::threadFunction()
{
	while (1)
	{
		chatScriptPhase();
		ppposPhase();

		{
			std::unique_lock uniqueLock {mutex_};

			if (connected_ == true)
			{
				distortos::Semaphore semaphore {0};
				semaphore_ = &semaphore;
				uniqueLock.unlock();
				const auto ret = semaphore.wait();
				assert(ret == 0);
			}
		}
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static functions
+---------------------------------------------------------------------------------------------------------------------*/

void PpposManager::pppLinkStatus(ppp_pcb* const pcb, const int errorCode, void* const context)
{
	assert(context != nullptr);
	auto& that = *static_cast<PpposManager*>(context);
	assert(pcb == that.pcb_);
	that.pppLinkStatus(errorCode);
}

u32_t PpposManager::ppposOutput(ppp_pcb* const pcb, u8_t* const buffer, const u32_t size, void* const context)
{
	assert(context != nullptr);
	auto& that = *static_cast<const PpposManager*>(context);
	assert(pcb == that.pcb_);
	return that.ppposOutput(buffer, size);
}

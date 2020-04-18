/**
 * \file
 * \brief lwIP configuration
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LWIP_CONFIGURATION_H_
#define LWIP_CONFIGURATION_H_

#define _GNU_SOURCE

#include "distortos/board/standardOutputStream.h"

#ifndef NDEBUG

#include "distortos/FATAL_ERROR.h"

#endif	/* ndef NDEBUG */

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif	/* def __cplusplus */

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 *
 * The queue size value itself is platform-dependent, but is passed to sys_mbox_new() when the acceptmbox is created.
 */

#define DEFAULT_ACCEPTMBOX_SIZE					16

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a NETCONN_RAW.
 *
 * The queue size value itself is platform-dependent, but is passed to sys_mbox_new() when the recvmbox is created.
 */

#define DEFAULT_RAW_RECVMBOX_SIZE				16

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a NETCONN_TCP.
 *
 * The queue size value itself is platform-dependent, but is passed to sys_mbox_new() when the recvmbox is created.
 */

#define DEFAULT_TCP_RECVMBOX_SIZE				16

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 *
 * The priority value itself is platform-dependent, but is passed to sys_thread_new() when the thread is created.
 */

#define DEFAULT_THREAD_PRIO						(UINT8_MAX / 2)

/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 *
 * The stack size value itself is platform-dependent, but is passed to sys_thread_new() when the thread is created.
 */

#define DEFAULT_THREAD_STACKSIZE				2048

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a NETCONN_UDP.
 *
 * The queue size value itself is platform-dependent, but is passed to sys_mbox_new() when the recvmbox is created.
 */

#define DEFAULT_UDP_RECVMBOX_SIZE				16

/**
 * LWIP_COMPAT_SOCKETS==1: Enable BSD-style sockets functions names through defines. LWIP_COMPAT_SOCKETS==2: Same as ==1
 * but correctly named functions are created. While this helps code completion, it might conflict with existing
 * libraries. (only used if you use sockets.c)
 */

#define LWIP_COMPAT_SOCKETS						0

/**
 * Enable debug message printing, but only if debug message type is enabled AND is of correct type AND is at least
 * LWIP_DBG_LEVEL.
 */

#define LWIP_DEBUG								1

/**
 * LWIP_DNS==1: Turn on DNS module.
 *
 * UDP must be available for DNS transport.
 */

#define LWIP_DNS								1

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */

#define LWIP_NETIF_API							1

#ifndef NDEBUG

/**
 * \brief Handler for failed assertion in lwIP
 *
 * \param [in] message is the assertion's message
 */

#define LWIP_PLATFORM_ASSERT(message)			FATAL_ERROR("lwIP assertion \"" message "\" failed")

#endif	/* ndef NDEBUG */

/**
 * \brief Implementation of LWIP_PLATFORM_DIAG()
 *
 * \param [in] __VA_ARGS__ is the format string and all required arguments
 */

#define LWIP_PLATFORM_DIAG_IMPLEMENTATION(...)	fiprintf(standardOutputStream, __VA_ARGS__)

/**
 * \brief Prints lwIP's diagnostic message.
 *
 * \param [in] message is the format string and all required arguments, enclosed in parentheses
 */

#define LWIP_PLATFORM_DIAG(message)				LWIP_PLATFORM_DIAG_IMPLEMENTATION message

/**
 * LWIP_POSIX_SOCKETS_IO_NAMES==1: Enable POSIX-style sockets functions names.
 *
 * Disable this option if you use a POSIX operating system that uses the same names (read, write & close). (only used if
 * you use sockets.c)
 */

#define LWIP_POSIX_SOCKETS_IO_NAMES				0

/** rand()-like function for lwIP */

#define LWIP_RAND()								rand()

/**
 * MEM_SIZE: the size of the heap memory.
 *
 * If the application will send a lot of data that needs to be copied, this should be set high.
 */

#define MEM_SIZE								(10 * 1024)

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 *
 * The default number of timeouts is calculated here for all enabled modules. The formula expects settings to be either
 * '0' or '1'.
 */

#define MEMP_NUM_SYS_TIMEOUT					(LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)

/**
 * NUM_PPP: the number of PPP sessions.
 */

#define NUM_PPP									1

/**
 * PPPOS_SUPPORT==1: Enable PPP Over Serial
 */

#define PPPOS_SUPPORT							1

/**
 * PPP_NOTIFY_PHASE==1: Support PPP notify phase support
 *
 * PPP notify phase support allows you to set a callback which is called on change of the internal PPP state machine.
 *
 * This can be used for example to set a LED pattern depending on the current phase of the PPP session.
 */

#define PPP_NOTIFY_PHASE						1

/**
 * PPP_SUPPORT==1: Enable PPP.
 */

#define PPP_SUPPORT								1

/**
 * PRINTPKT_SUPPORT==1: Enable PPP print packet support
 *
 * Mandatory for debugging, it displays exchanged packet content in debug trace.
 */

#define PRINTPKT_SUPPORT						1

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */

#define SO_REUSE								1

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 *
 * The queue size value itself is platform-dependent, but is passed to sys_mbox_new() when tcpip_init is called.
 */

#define TCPIP_MBOX_SIZE							16

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 *
 * The priority value itself is platform-dependent, but is passed to sys_thread_new() when the thread is created.
 */

#define TCPIP_THREAD_PRIO						(UINT8_MAX / 2)

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 *
 * The stack size value itself is platform-dependent, but is passed to sys_thread_new() when the thread is created.
 */

#define TCPIP_THREAD_STACKSIZE					4096

/**
 * VJ_SUPPORT==1: Support VJ header compression.
 */

#define VJ_SUPPORT								0

#if LWIP_DEBUG == 1

/**
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is compared against this value. If it is smaller, then
 * debugging messages are written.
 *
 * @see debugging_levels
 */

#define LWIP_DBG_MIN_LEVEL						LWIP_DBG_LEVEL_ALL

/**
 * ACD_DEBUG: Enable debugging in acd.c.
 */

#define ACD_DEBUG								LWIP_DBG_OFF

/**
 * Configure debug level of altcp_tls_mbedtls.c
 */

#define ALTCP_MBEDTLS_DEBUG						LWIP_DBG_OFF

/**
 * Configure lwIP debug level of the mbed-TLS library
 */

#define ALTCP_MBEDTLS_LIB_DEBUG					LWIP_DBG_OFF

/**
 * Configure minimum internal debug level of the mbed-TLS library
 */

#define ALTCP_MBEDTLS_LIB_DEBUG_LEVEL_MIN		0

/**
 * API_LIB_DEBUG: Enable debugging in api_lib.c.
 */

#define API_LIB_DEBUG							LWIP_DBG_OFF

/**
 * API_MSG_DEBUG: Enable debugging in api_msg.c.
 */

#define API_MSG_DEBUG							LWIP_DBG_OFF

/**
 * AUTOIP_DEBUG: Enable debugging in autoip.c.
 */

#define AUTOIP_DEBUG							LWIP_DBG_OFF

/**
 * DHCP6_DEBUG: Enable debugging in dhcp6.c.
 */

#define DHCP6_DEBUG								LWIP_DBG_OFF

/**
 * DHCP_DEBUG: Enable debugging in dhcp.c.
 */

#define DHCP_DEBUG								LWIP_DBG_OFF

/**
 * DNS_DEBUG: Enable debugging for DNS.
 */

#define DNS_DEBUG								LWIP_DBG_ON

/**
 * ETHARP_DEBUG: Enable debugging in etharp.c.
 */

#define ETHARP_DEBUG							LWIP_DBG_OFF

/**
 * ICMP_DEBUG: Enable debugging in icmp.c.
 */

#define ICMP_DEBUG								LWIP_DBG_OFF

/**
 * IGMP_DEBUG: Enable debugging in igmp.c.
 */

#define IGMP_DEBUG								LWIP_DBG_OFF

/**
 * INET_DEBUG: Enable debugging in inet.c.
 */

#define INET_DEBUG								LWIP_DBG_OFF

/**
 * IP6_DEBUG: Enable debugging for IPv6.
 */

#define IP6_DEBUG								LWIP_DBG_OFF

/**
 * IP_DEBUG: Enable debugging for IP.
 */

#define IP_DEBUG								LWIP_DBG_OFF

/**
 * IP_REASS_DEBUG: Enable debugging in ip_frag.c for both frag & reass.
 */

#define IP_REASS_DEBUG							LWIP_DBG_OFF

/**
 * MEMP_DEBUG: Enable debugging in memp.c.
 */

#define MEMP_DEBUG								LWIP_DBG_OFF

/**
 * MEM_DEBUG: Enable debugging in mem.c.
 */

#define MEM_DEBUG								LWIP_DBG_OFF

/**
 * MQTT_DEBUG: Enable debugging in mqtt.c.
 */

#define MQTT_DEBUG								LWIP_DBG_ON

/**
 * NETIF_DEBUG: Enable debugging in netif.c.
 */

#define NETIF_DEBUG								LWIP_DBG_OFF

/**
 * PBUF_DEBUG: Enable debugging in pbuf.c.
 */

#define PBUF_DEBUG								LWIP_DBG_OFF

/**
 * PPP_DEBUG: Enable debugging for PPP.
 */

#define PPP_DEBUG								LWIP_DBG_ON

/**
 * RAW_DEBUG: Enable debugging in raw.c.
 */

#define RAW_DEBUG								LWIP_DBG_OFF

/**
 * SLIP_DEBUG: Enable debugging in slipif.c.
 */

#define SLIP_DEBUG								LWIP_DBG_OFF

/**
 * SOCKETS_DEBUG: Enable debugging in sockets.c.
 */

#define SOCKETS_DEBUG							LWIP_DBG_OFF

/**
 * SYS_DEBUG: Enable debugging in sys.c.
 */

#define SYS_DEBUG								LWIP_DBG_OFF

/**
 * TCPIP_DEBUG: Enable debugging in tcpip.c.
 */

#define TCPIP_DEBUG								LWIP_DBG_OFF

/**
 * TCP_CWND_DEBUG: Enable debugging for TCP congestion window.
 */

#define TCP_CWND_DEBUG							LWIP_DBG_OFF

/**
 * TCP_DEBUG: Enable debugging for TCP.
 */

#define TCP_DEBUG								LWIP_DBG_OFF

/**
 * TCP_FR_DEBUG: Enable debugging in tcp_in.c for fast retransmit.
 */

#define TCP_FR_DEBUG							LWIP_DBG_OFF

/**
 * TCP_INPUT_DEBUG: Enable debugging in tcp_in.c for incoming debug.
 */

#define TCP_INPUT_DEBUG							LWIP_DBG_OFF

/**
 * TCP_OUTPUT_DEBUG: Enable debugging in tcp_out.c output functions.
 */

#define TCP_OUTPUT_DEBUG						LWIP_DBG_OFF

/**
 * TCP_QLEN_DEBUG: Enable debugging for TCP queue lengths.
 */

#define TCP_QLEN_DEBUG							LWIP_DBG_OFF

/**
 * TCP_RST_DEBUG: Enable debugging for TCP with the RST message.
 */

#define TCP_RST_DEBUG							LWIP_DBG_OFF

/**
 * TCP_RTO_DEBUG: Enable debugging in TCP for retransmit timeout.
 */

#define TCP_RTO_DEBUG							LWIP_DBG_OFF

/**
 * TCP_WND_DEBUG: Enable debugging in tcp_in.c for window updating.
 */

#define TCP_WND_DEBUG							LWIP_DBG_OFF

/**
 * TIMERS_DEBUG: Enable debugging in timers.c.
 */

#define TIMERS_DEBUG							LWIP_DBG_OFF

/**
 * UDP_DEBUG: Enable debugging in UDP.
 */

#define UDP_DEBUG								LWIP_DBG_OFF

#endif	/* LWIP_DEBUG == 1 */

#ifdef __cplusplus
}	/* extern "C" */
#endif	/* def __cplusplus */

#endif	/* LWIP_CONFIGURATION_H_ */

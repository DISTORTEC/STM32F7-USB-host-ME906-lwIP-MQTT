/**
 * \file
 * \brief Main code block.
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define _GNU_SOURCE

#include "HuaweiMe906.hpp"
#include "openHuaweiMe906.hpp"
#include "PpposManager.hpp"

#include "stm32f7xx_hal.h"

#include "usbh_core.h"

#include "distortos/board/buttons.hpp"
#include "distortos/board/initializeStreams.hpp"
#include "distortos/board/leds.hpp"

#include "distortos/chip/InputPin.hpp"
#include "distortos/chip/OutputPin.hpp"

#include "distortos/distortosVersion.h"
#include "distortos/DynamicThread.hpp"
#include "distortos/ThisThread.hpp"

#include "lwip/apps/mqtt.h"

#include "lwip/netdb.h"
#include "lwip/tcpip.h"

#include <cstring>

/*---------------------------------------------------------------------------------------------------------------------+
| local defines
+---------------------------------------------------------------------------------------------------------------------*/

/// common prefix of all topics used by this application
#define TOPIC_PREFIX	"distortos/" DISTORTOS_VERSION_STRING "/" DISTORTOS_BOARD

/// MQTT message published to ONLINE_TOPIC as client's "will" when connection is lost
#define OFFLINE_MESSAGE	"0"

/// MQTT message published to ONLINE_TOPIC when connected to MQTT broker
#define ONLINE_MESSAGE	"1"

/// MQTT "online" topic - ONLINE_MESSAGE is published when connected to MQTT broker and OFFLINE_MESSAGE as client's
// "will" when connection is lost
#define ONLINE_TOPIC	TOPIC_PREFIX "/online"

/// prefix for topic used for publishing requested state of LEDs
#define LEDS_TOPIC_PREFIX		TOPIC_PREFIX "/leds"

/// suffix for topic used for publishing requested state of LEDs
#define LEDS_TOPIC_SUFFIX		"/state"

/// prefix for topic used for publishing state of buttons
#define BUTTONS_TOPIC_PREFIX	TOPIC_PREFIX "/buttons"

/// suffix for topic used for publishing state of buttons
#define BUTTONS_TOPIC_SUFFIX	"/state"

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local types
+---------------------------------------------------------------------------------------------------------------------*/

/// type alias for data used for LED message
using LedMessage = std::optional<size_t>;

/// collection of data used by lwIP's MQTT client
struct MqttClient
{
	/// MQTT client connection info
	mqtt_connect_client_info_t connectionInfo;

	/// pointer to lwIP's MQTT client struct
	mqtt_client_t* client;

	/// MQTT client's status
	mqtt_connection_status_t status;

	/// true if connecting is in progress, false otherwise
	bool connecting;
};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief lwIP's MQTT connection callback
 *
 * \param [in] client is a pointer to lwIP's MQTT client struct
 * \param [in] argument is a argument which was passed to mqtt_client_connect(), must be MqttClient!
 * \param [in] status is the status of MQTT connection
 */

void mqttClientConnectionCallback(mqtt_client_t* const client, void* const argument,
		const mqtt_connection_status_t status)
{
	assert(argument != nullptr);
	auto& mqttClient = *static_cast<MqttClient*>(argument);
	assert(client == mqttClient.client);

	fiprintf(standardOutputStream, "mqttClientConnectionCallback: status = %d\r\n", status);

	mqttClient.status = status;
	mqttClient.connecting = {};
}

/**
 * \brief lwIP's MQTT incoming data callback
 *
 * \param [in] argument is a argument which was passed to mqtt_set_inpub_callback(), must be LedMessage!
 * \param [in] data is a pointer to incoming data
 * \param [in] length is the length of \a data
 * \param [in] flags are flags associated with \a data
 */

void mqttIncomingDataCallback(void* const argument, const u8_t* const data, const u16_t length, const u8_t flags)
{
	assert(argument != nullptr);
	auto& ledMessage = *static_cast<LedMessage*>(argument);

	fiprintf(standardOutputStream, "mqttIncomingDataCallback: length = %" PRIu16 ", flags = %" PRIu8 "\r\n",
			length, flags);

	if (ledMessage.has_value() == false)
	{
		fiprintf(standardOutputStream, "mqttIncomingDataCallback: ignoring\r\n");
		return;
	}

	assert(length == 1);
	assert((flags & MQTT_DATA_FLAG_LAST) != 0);

	if (*data != '0' && *data != '1')
	{
		fiprintf(standardOutputStream,
				"mqttIncomingDataCallback: invalid data, got '%c', expected {'0', '1'}, ignoring\r\n", *data);
				return;
	}

	distortos::board::leds[*ledMessage].set(*data == '1');
}

/**
 * \brief lwIP's MQTT incoming publish callback
 *
 * \param [in] argument is a argument which was passed to mqtt_set_inpub_callback(), must be LedMessage!
 * \param [in] topic is the topic of incoming publish
 * \param [in] totalLength is the total length of incoming data
 */

void mqttIncomingPublishCallback(void* const argument, const char* const topic, const u32_t totalLength)
{
	assert(argument != nullptr);
	auto& ledMessage = *static_cast<LedMessage*>(argument);
	ledMessage.reset();

	fiprintf(standardOutputStream, "mqttIncomingPublishCallback: topic = \"%s\", total length = %" PRIu32 "\r\n",
			topic, totalLength);

	if (totalLength != 1)
	{
		fiprintf(standardOutputStream,
				"mqttIncomingPublishCallback: invalid total length, got %" PRIu32 ", expected 1, ignoring\r\n",
				totalLength);
		return;
	}

	size_t i;
	const auto ret = siscanf(topic, LEDS_TOPIC_PREFIX "/%zu" LEDS_TOPIC_SUFFIX, &i);
	if (ret != 1)
	{
		fiprintf(standardOutputStream, "mqttIncomingPublishCallback: could not parse topic, ignoring\r\n");
		return;
	}
	if (i >= std::size(distortos::board::leds))
	{
		fiprintf(standardOutputStream,
				"mqttIncomingPublishCallback: invalid LED index, got %zu, expected [0; %zu), ignoring\r\n",
				i, std::size(distortos::board::leds));
		return;
	}

	ledMessage = i;
}

/**
 * \brief lwIP's MQTT request callback
 *
 * \param [in] argument is a argument which was passed to mqtt_publish(), must be MqttClient!
 * \param [in] error is the result of MQTT request
 */

void mqttRequestCallback(void*, const err_t error)
{
	fiprintf(standardOutputStream, "mqttRequestCallback: error = %d\r\n", error);
}

/**
 * \brief Thread which constantly reads selected HuaweiMe906 serial port and prints data that was read on
 * \a standardOutputStream.
 *
 * \param [in] huaweiMe906 is a reference to HuaweiMe906 object
 * \param [in] port selects the serial port that will be accessed
 */

void readerThread(HuaweiMe906& huaweiMe906, const HuaweiMe906::Port port)
{
	const auto stream = openHuaweiMe906(huaweiMe906, port, "r");
	const auto portString = port == HuaweiMe906::Port::pcui ? "PCUI" :
			port == HuaweiMe906::Port::networkCard ? "Network card" : "GPS";

	while (1)
	{
		char buffer[128];
		if (fgets(buffer, sizeof(buffer), stream) != nullptr)
		{
			auto length = strlen(buffer);
			// trim trailing whitespace
			while (length > 0 && (buffer[length - 1] == '\r' || buffer[length - 1] == '\n'))
			{
				buffer[length - 1] = '\0';
				--length;
			}
			if (length > 0)
				fiprintf(standardOutputStream, "%s port, read %zu bytes: \"%s\"\r\n", portString, length, buffer);
		}
		else
		{
			fiprintf(standardOutputStream, "%s port, read failed, errno = %d\r\n", portString, errno);
			clearerr(stream);
			distortos::ThisThread::sleepFor(std::chrono::seconds{5});
		}
	}
}

/**
 * \brief User callback for USB host events.
 *
 * \param [in] host is a pointer to USB host
 * \param [in] event is the event
 */

void usbHostEventCallback(USBH_HandleTypeDef*, const uint8_t event)
{
	switch (event)
	{
		case HOST_USER_SELECT_CONFIGURATION:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_SELECT_CONFIGURATION");
			break;

		case HOST_USER_CLASS_ACTIVE:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_CLASS_ACTIVE");
			break;

		case HOST_USER_CLASS_SELECTED:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_CLASS_SELECTED");
			break;

		case HOST_USER_CONNECTION:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_CONNECTION");
			break;

		case HOST_USER_DISCONNECTION:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_DISCONNECTION");
			break;

		case HOST_USER_UNRECOVERED_ERROR:
			USBH_UsrLog("usbHostEventCallback: HOST_USER_UNRECOVERED_ERROR");
			break;

		default:
			USBH_UsrLog("usbHostEventCallback: event = %" PRIu8, event);
			break;
	}
}

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Main code block of application
 *
 * \return doesn't return
 */

int main()
{
	distortos::board::initializeStreams();

	fiprintf(standardOutputStream, "Started %s board\r\n", DISTORTOS_BOARD);

	USBH_HandleTypeDef usbHost {};
	{
		const auto ret = USBH_Init(&usbHost, usbHostEventCallback, 0);
		assert(ret == USBH_OK);

	}
	HuaweiMe906 huaweiMe906 {};
	huaweiMe906.registerClass(usbHost);
	{
		const auto ret = USBH_Start(&usbHost);
		assert(ret == USBH_OK);
	}

	const auto pcuiReaderThread = distortos::makeAndStartDynamicThread({2048, 1},
			readerThread, std::ref(huaweiMe906), HuaweiMe906::Port::pcui);
	const auto gpsReaderThread = distortos::makeAndStartDynamicThread({2048, 1},
			readerThread, std::ref(huaweiMe906), HuaweiMe906::Port::gps);

	tcpip_init({}, {});

	PpposManager ppposManager {huaweiMe906};
	ppposManager.initialize();

	ip_addr_t ip;
	{
		int ret;
		struct addrinfo* addressInformation;
		while (ret = getaddrinfo("broker.hivemq.com", nullptr, nullptr, &addressInformation), ret != 0)
		{
			fiprintf(standardOutputStream, "getaddrinfo() failed, ret = %d\r\n", ret);
			distortos::ThisThread::sleepFor(std::chrono::seconds{5});
		}

		assert(addressInformation != nullptr && addressInformation->ai_family == AF_INET);
		assert(addressInformation->ai_addr != nullptr && addressInformation->ai_addr->sa_family == AF_INET);
		const auto internetAddress = reinterpret_cast<const sockaddr_in*>(addressInformation->ai_addr);
		inet_addr_to_ip4addr(&ip, &internetAddress->sin_addr);
		fiprintf(standardOutputStream, "%s is %s\r\n", addressInformation->ai_canonname, ip4addr_ntoa(&ip));

		freeaddrinfo(addressInformation);
	}

	MqttClient mqttClient {};

	LOCK_TCPIP_CORE();
	mqttClient.client = mqtt_client_new();
	assert(mqttClient.client != nullptr);
	UNLOCK_TCPIP_CORE();

	mqttClient.connectionInfo.client_id = "MQTT client";
	mqttClient.connectionInfo.client_user = {};
	mqttClient.connectionInfo.client_pass = {};
	mqttClient.connectionInfo.keep_alive = 60;
	mqttClient.connectionInfo.will_topic = ONLINE_TOPIC;
	mqttClient.connectionInfo.will_msg = OFFLINE_MESSAGE;
	mqttClient.connectionInfo.will_qos = {};
	mqttClient.connectionInfo.will_retain = {};
#if LWIP_ALTCP && LWIP_ALTCP_TLS
	mqttClient.connectionInfo.tls_config = {};
#endif

	while (1)
	{
		mqttClient.connecting = true;

		{
			LOCK_TCPIP_CORE();
			const auto ret = mqtt_client_connect(mqttClient.client, &ip, MQTT_PORT, mqttClientConnectionCallback,
					&mqttClient, &mqttClient.connectionInfo);
			UNLOCK_TCPIP_CORE();
			if (ret != ERR_OK)
			{
				fiprintf(standardOutputStream, "mqtt_client_connect() failed, ret = %d\r\n", ret);
				distortos::ThisThread::sleepFor(std::chrono::seconds{5});
				continue;
			}
		}

		while (mqttClient.connecting == true)
		{
			fiprintf(standardOutputStream, "Connecting to MQTT broker...\r\n");
			distortos::ThisThread::sleepFor(std::chrono::seconds{5});
		}

		LedMessage ledMessage {};
		LOCK_TCPIP_CORE();
		mqtt_set_inpub_callback(mqttClient.client, mqttIncomingPublishCallback, mqttIncomingDataCallback, &ledMessage);
		UNLOCK_TCPIP_CORE();

		bool onlinePublished = {};
		bool subscribed = {};
		bool buttonStates[DISTORTOS_BOARD_BUTTONS_COUNT] {};
		bool buttonsPublished = {};
		while (mqttClient.status == MQTT_CONNECT_ACCEPTED)
		{
			if (onlinePublished == false)
			{
				LOCK_TCPIP_CORE();
				const auto ret = mqtt_publish(mqttClient.client, ONLINE_TOPIC, ONLINE_MESSAGE, strlen(ONLINE_MESSAGE),
						{}, {}, mqttRequestCallback, &mqttClient);
				UNLOCK_TCPIP_CORE();
				if (ret != ERR_OK)
				{
					fiprintf(standardOutputStream, "mqtt_publish() failed, ret = %d\r\n", ret);
					continue;
				}

				onlinePublished = true;
			}

			if (subscribed == false)
			{
				LOCK_TCPIP_CORE();
				const auto ret = mqtt_subscribe(mqttClient.client, LEDS_TOPIC_PREFIX "/+" LEDS_TOPIC_SUFFIX, {},
						mqttRequestCallback, &mqttClient);
				UNLOCK_TCPIP_CORE();
				if (ret != ERR_OK)
				{
					fiprintf(standardOutputStream, "mqtt_subscribe() failed, ret = %d\r\n", ret);
					continue;
				}

				subscribed = true;
			}

			bool publishFailed = {};
			for (size_t i {}; i < std::size(distortos::board::buttons) && publishFailed == false; ++i)
			{
				const auto state = distortos::board::buttons[i].get();
				if (buttonStates[i] != state || buttonsPublished == false)
				{
					static_assert(std::size(distortos::board::buttons) < 10);
					char topic[std::size(BUTTONS_TOPIC_PREFIX "/?" BUTTONS_TOPIC_SUFFIX)];
					{
						const auto ret = sniprintf(topic, std::size(topic),
								BUTTONS_TOPIC_PREFIX "/%zu" BUTTONS_TOPIC_SUFFIX, i);
						assert(ret > 0 && static_cast<size_t>(ret) < std::size(topic));
					}
					const auto message = state == false ? '0' : '1';
					LOCK_TCPIP_CORE();
					const auto ret = mqtt_publish(mqttClient.client, topic, &message, 1, {}, {}, mqttRequestCallback,
							&mqttClient);
					UNLOCK_TCPIP_CORE();
					if (ret != ERR_OK)
					{
						fiprintf(standardOutputStream, "mqtt_publish() failed, ret = %d\r\n", ret);
						publishFailed = true;
						continue;
					}

					buttonStates[i] = state;
				}
			}

			if (publishFailed == true)
				continue;

			buttonsPublished = true;

			distortos::ThisThread::sleepFor(std::chrono::milliseconds{50});
		}

		LOCK_TCPIP_CORE();
		mqtt_disconnect(mqttClient.client);
		UNLOCK_TCPIP_CORE();
	}
}

extern "C" void OTG_FS_IRQHandler(void)
{
	extern HCD_HandleTypeDef hhcd;
	HAL_HCD_IRQHandler(&hhcd);
}

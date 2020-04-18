/**
 * \file
 * \brief PpposManager class header
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PPPOSMANAGER_HPP_
#define PPPOSMANAGER_HPP_

#include "distortos/Mutex.hpp"

#include "estd/ContiguousRange.hpp"

#include "netif/ppp/ppp.h"

class HuaweiMe906;

namespace distortos
{

class Semaphore;

}	// namespace distortos

/// PpposManager class is a manager of lwIP's PPPoS interface
class PpposManager
{
public:

	/**
	 * \brief PpposManager's constructor
	 *
	 * \param [in] huaweiMe906 is a reference to HuaweiMe906 object which will be used for communication
	 */

	constexpr explicit PpposManager(HuaweiMe906& huaweiMe906) :
			mutex_{distortos::Mutex::Protocol::priorityInheritance},
			netif_{},
			huaweiMe906_{huaweiMe906},
			pcb_{},
			semaphore_{},
			state_{State::disconnected}
	{

	}

	/**
	 * \brief Initializes PPPoS manager.
	 *
	 * \pre Object is not initialized.
	 *
	 * \post Object is initialized.
	 */

	void initialize();

private:

	/// state
	enum class State : uint8_t
	{
		/// disconnected
		disconnected,
		/// connecting
		connecting,
		/// connected
		connected,
	};

	/**
	 * \brief Executes chat script, connecting the modem to the network.
	 */

	void chatScriptPhase() const;

	/**
	 * \brief Executes one AT command and waits for either a response or an error.
	 *
	 * \param [in] outputStream is a reference to stream used for output
	 * \param [in] inputStream is a reference to stream used for input
	 * \param [in] command is the command that will be executed
	 * \param [in] expectedResponse is the expected response of \a command
	 * \param [in] abortStrings is a range of strings which abort wait for \a expectedResponse
	 *
	 * \return true if command was executed successfully, false otherwise
	 */

	bool executeCommand(FILE& outputStream, FILE& inputStream, const char* command, const char* expectedResponse,
			estd::ContiguousRange<const char* const> abortStrings) const;

	/**
	 * \brief lwIP's PPP link status callback
	 *
	 * \param [in] errorCode is the error code
	 */

	void pppLinkStatus(int errorCode);

	/**
	 * \brief lwIP's PPPoS output callback
	 *
	 * Wrapper for HuaweiMe906::write(HuaweiMe906::Port::networkCard, ...) which can be used as an output function for
	 * lwIP's PPPoS interface.
	 *
	 * \param [in] buffer is the buffer with data that will be transmitted
	 * \param [in] size is the size of \a buffer, bytes
	 *
	 * \return number of written bytes
	 */

	u32_t ppposOutput(u8_t* buffer, u32_t size) const;

	/**
	 * \brief Executes PPPos phase, switching the modem to PPPoS mode and routing the data read from modem back to lwIP.
	 */

	void ppposPhase();

	/**
	 * \brief Internal thread function of the object.
	 */

	void threadFunction();

	/**
	 * \brief lwIP's PPP link status callback
	 *
	 * \param [in] ppp is a pointer to PPP protocol control block
	 * \param [in] errorCode is the error code
	 * \param [in] context is a context which was passed to ppp..._create(), must be PpposManager!
	 */

	static void pppLinkStatus(ppp_pcb* pcb, int errorCode, void* context);

	/**
	 * \brief lwIP's PPPoS output callback
	 *
	 * \param [in] ppp is a pointer to PPP protocol control block
	 * \param [in] buffer is the buffer with data that will be transmitted
	 * \param [in] size is the size of \a buffer, bytes
	 * \param [in] context is a context which was passed to pppapi_pppos_create(), must be PpposManager!
	 *
	 * \return number of written bytes
	 */

	static u32_t ppposOutput(ppp_pcb* pcb, u8_t* buffer, u32_t size, void* context);

	/// mutex used for access serialization
	distortos::Mutex mutex_;

	/// lwIP's network interface
	netif netif_;

	/// reference to HuaweiMe906 object which will be used for communication
	HuaweiMe906& huaweiMe906_;

	/// lwIP's PPP protocol control block
	ppp_pcb* pcb_;

	/// pointer to semaphore used for notification about change of connection state
	distortos::Semaphore* semaphore_;

	/// current state
	State state_;
};

#endif	// PPPOSMANAGER_HPP_

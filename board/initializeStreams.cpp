/**
 * \file
 * \brief Definition initializeStreams()
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define _GNU_SOURCE

#include "distortos/board/initializeStreams.hpp"

#include "distortos/board/standardOutputStream.h"

#include "distortos/chip/ChipUartLowLevel.hpp"
#include "distortos/chip/uarts.hpp"

#include "distortos/devices/communication/SerialPort.hpp"

#include "distortos/assert.h"

#include "estd/ContiguousRange.hpp"

namespace distortos
{

namespace board
{

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local objects
+---------------------------------------------------------------------------------------------------------------------*/

/// read buffer for \a serialPort
uint8_t serialPortReadBuffer[256];

/// write buffer for \a serialPort
uint8_t serialPortWriteBuffer[256];

/// serial port instance
devices::SerialPort serialPort
{
#if defined(DISTORTOS_BOARD_ST_32F746GDISCOVERY)
		chip::usart1,
#elif defined(DISTORTOS_BOARD_ST_NUCLEO_F767ZI)
		chip::usart3,
#else
#error "Unsupported board!"
#endif
		serialPortReadBuffer, sizeof(serialPortReadBuffer), serialPortWriteBuffer, sizeof(serialPortWriteBuffer)
};

/// buffer for \a standardOutputStream
char standardOutputStreamBuffer[256];

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Wrapper for devices::SerialPort::close() which can be used with fopencookie()
 *
 * \param [in] cookie is a cookie which was passed to fopencookie(), must be devices::SerialPort!
 *
 * \return 0 on success, -1 otherwise
 */

int serialPortClose(void* const cookie)
{
	const auto ret = static_cast<devices::SerialPort*>(cookie)->close();
	if (ret != 0)
	{
		errno = ret;
		return -1;
	}

	return {};
}

/**
 * \brief Wrapper for devices::SerialPort::read() which can be used with fopencookie()
 *
 * \param [in] cookie is a cookie which was passed to fopencookie(), must be devices::SerialPort!
 * \param [out] buffer is the buffer to which the data will be written
 * \param [in] size is the size of \a buffer, bytes
 *
 * \return number of read bytes on success, -1 otherwise
 */

ssize_t serialPortRead(void* const cookie, char* const buffer, const size_t size)
{
	const auto ret = static_cast<devices::SerialPort*>(cookie)->read(buffer, size);
	if (ret.first != 0)
	{
		errno = ret.first;
		return -1;
	}

	return ret.second;
}

/**
 * \brief Wrapper for devices::SerialPort::write() which can be used with fopencookie()
 *
 * \param [in] cookie is a cookie which was passed to fopencookie(), must be devices::SerialPort!
 * \param [in] buffer is the buffer with data that will be transmitted
 * \param [in] size is the size of \a buffer, bytes
 *
 * \return number of written bytes on success, -1 otherwise
 */

ssize_t serialPortWrite(void* const cookie, const char* const buffer, const size_t size)
{
	const auto ret = static_cast<devices::SerialPort*>(cookie)->write(buffer, size);
	if (ret.first != 0)
	{
		errno = ret.first;
		return -1;
	}

	return ret.second;
}

/**
 * \brief Opens serial port, wraps it into a FILE and sets line buffering using provided buffer.
 *
 * \param [in] mode is the mode with which the stream is opened
 * \param [in] streamBuffer is a contiguous range with char, used as stream buffer
 *
 * \return pointer to opened FILE object
 */

FILE* openSerialPort(const char* const mode, const estd::ContiguousRange<char> streamBuffer)
{
	{
		const auto ret = serialPort.open(115200, 8, devices::UartParity::none, false);
		assert(ret == 0);
	}

	const auto stream = fopencookie(&serialPort, mode, {serialPortRead, serialPortWrite, {}, serialPortClose});
	assert(stream != nullptr);

	{
		const auto ret = setvbuf(stream, streamBuffer.begin(), _IOLBF, streamBuffer.size());
		assert(ret == 0);
	}

	return stream;
}

}	// namespace

extern "C"
{

/*---------------------------------------------------------------------------------------------------------------------+
| global objects
+---------------------------------------------------------------------------------------------------------------------*/

FILE* standardOutputStream;

}	// extern "C"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

void initializeStreams()
{
	standardOutputStream = openSerialPort("w", estd::ContiguousRange<char>{standardOutputStreamBuffer});
}

}	// namespace board

}	// namespace distortos

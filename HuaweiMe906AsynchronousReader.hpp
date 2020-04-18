/**
 * \file
 * \brief HuaweiMe906AsynchronousReader class header
 *
 * \author Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HUAWEIME906ASYNCHRONOUSREADER_HPP_
#define HUAWEIME906ASYNCHRONOUSREADER_HPP_

#include <cstddef>

/// HuaweiMe906AsynchronousReader class is an interface for HuaweiMe906's asynchronous reader
class HuaweiMe906AsynchronousReader
{
public:

	/**
	 * \brief HuaweiMe906AsynchronousReader's function call operator
	 *
	 * Called by HuaweiMe906's background process when new data is available for reading (\a size != 0) or when device
	 * disconnects (\a size == 0).
	 *
	 * \param [in] buffer is the buffer with data that was read
	 * \param [in] size is the size of \a buffer, bytes, 0 if device was disconnected
	 *
	 * \return number of consumed bytes, must be less than or equal to \a size
	 */

	virtual size_t operator()(const void* buffer, size_t size) = 0;
};

#endif	// HUAWEIME906ASYNCHRONOUSREADER_HPP_

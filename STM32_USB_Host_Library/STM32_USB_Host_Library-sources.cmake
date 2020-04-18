#
# file: STM32_USB_Host_Library-sources.cmake
#
# author: Copyright (C) 2019-2020 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
# distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

add_library(STM32_USB_Host_Library STATIC
		${CMAKE_CURRENT_LIST_DIR}/Core/Src/usbh_core.c
		${CMAKE_CURRENT_LIST_DIR}/Core/Src/usbh_ctlreq.c
		${CMAKE_CURRENT_LIST_DIR}/Core/Src/usbh_ioreq.c
		${CMAKE_CURRENT_LIST_DIR}/Core/Src/usbh_pipes.c)
target_compile_options(STM32_USB_Host_Library PRIVATE
		-Wno-int-conversion
		-Wno-shadow
		-Wno-unused-parameter)
target_include_directories(STM32_USB_Host_Library PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/Core/Inc)
target_link_libraries(STM32_USB_Host_Library PUBLIC
		CMSIS-RTOS2
		STM32F7xx_HAL_Driver)

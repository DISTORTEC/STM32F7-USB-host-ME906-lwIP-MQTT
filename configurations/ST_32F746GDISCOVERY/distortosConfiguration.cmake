#
# \file
# \brief distortos configuration
#
# \warning
# Automatically generated file - do not edit!
#

if(DEFINED ENV{DISTORTOS_PATH})
	set(DISTORTOS_PATH "$ENV{DISTORTOS_PATH}")
else()
	set(DISTORTOS_PATH "../distortos")
endif()

set("distortos_Build_02_Floating_point_ABI"
		"hard"
		CACHE
		"STRING"
		"Select floating-point ABI.")
set("distortos_Build_00_Static_destructors"
		"OFF"
		CACHE
		"BOOL"
		"Enable static destructors.\n\nEnable destructors for objects with static storage duration. As embedded applications almost never \"exit\", these destructors are usually never executed, wasting ROM.")
set("distortos_Scheduler_00_Tick_frequency"
		"1000"
		CACHE
		"STRING"
		"System's tick frequency, Hz.\n\nAllowed range: [1; 2147483647]")
set("distortos_Scheduler_01_Round_robin_frequency"
		"10"
		CACHE
		"STRING"
		"Round-robin frequency, Hz.\n\nAllowed range: [1; 1000]")
set("distortos_Scheduler_02_Support_for_signals"
		"OFF"
		CACHE
		"BOOL"
		"Enable support for signals.\n\nEnable namespaces, functions and classes related to signals:\n- ThisThread::Signals namespace;\n- Thread::generateSignal();\n- Thread::getPendingSignalSet();\n- Thread::queueSignal();\n- DynamicSignalsReceiver class;\n- SignalInformationQueueWrapper class;\n- SignalsCatcher class;\n- SignalsReceiver class;\n- StaticSignalsReceiver class;\n\nWhen this options is not selected, these namespaces, functions and classes are not available at all.")
set("distortos_Scheduler_03_Support_for_thread_detachment"
		"ON"
		CACHE
		"BOOL"
		"Enable support for thread detachment.\n\nEnable functions that \"detach\" dynamic threads:\n- ThisThread::detach();\n- Thread::detach();\n\nWhen this options is not selected, these functions are not available at all.\n\nWhen dynamic and detached thread terminates, it will be added to the global list of threads pending for deferred deletion. The thread will actually be deleted in idle thread, but only when two mutexes are successfully locked:\n- mutex that protects dynamic memory allocator;\n- mutex that synchronizes access to the list of threads pending for deferred deletion;")
set("distortos_Scheduler_04_Main_thread_stack_size"
		"4096"
		CACHE
		"STRING"
		"Size (in bytes) of stack used by thread with main() function.\n\nAllowed range: [1; 2147483647]")
set("distortos_Scheduler_05_Main_thread_priority"
		"127"
		CACHE
		"STRING"
		"Initial priority of main thread.\n\nAllowed range: [1; 255]")
set("distortos_Checks_00_Context_of_functions"
		"ON"
		CACHE
		"BOOL"
		"Check context of functions.\n\nSome functions may only be used from thread context, as using them from interrupt context results in undefined behaviour. There are several groups of functions to which this restriction applies (some functions fall into several categories at once):\n- all blocking functions, like callOnce(), FifoQueue::push(), Semaphore::wait(), ..., as an attempt to block current thread of execution (not to be confused with current thread) is not possible in interrupt context;\n- all mutex functions, as the concept of ownership by a thread - core feature of mutex - cannot be fulfilled in interrupt context;\n- all functions from ThisThread namespace (including ThisThread::Signals namespace), as in interrupt context they would access a random thread that happened to be executing at that particular moment;\n\nUsing such functions from interrupt context is a common bug in applications which can be easily introduced and very hard to find, as the symptoms may appear only under certain circumstances.\n\nSelecting this option enables context checks in all functions with such requirements. If any of them is used from interrupt context, FATAL_ERROR() will be called.")
set("distortos_Checks_01_Stack_pointer_range_during_context_switch"
		"ON"
		CACHE
		"BOOL"
		"Check stack pointer range during context switch.\n\nSimple range checking of preempted thread's stack pointer can be performed during context switches. It is relatively fast, but cannot detect all stack overflows. The check is done before the software stack frame is pushed on thread's stack, but the size of this pending stack frame is accounted for - the intent is to detect a stack overflow which is about to happen, before it can cause (further) data corrution. FATAL_ERROR() will be called if the stack pointer is outside valid range.")
set("distortos_Checks_02_Stack_pointer_range_during_system_tick"
		"ON"
		CACHE
		"BOOL"
		"Check stack pointer range during system tick.\n\nSimilar to \"distortos_Checks_01_Stack_pointer_range_during_context_switch\", but executed during every system tick.")
set("distortos_Checks_03_Stack_guard_contents_during_context_switch"
		"ON"
		CACHE
		"BOOL"
		"Check stack guard contents during context switch.\n\nSelecting this option extends stacks for all threads (including main() thread) with a \"stack guard\" at the overflow end. This \"stack guard\" - just as the whole stack - is filled with a sentinel value 0xed419f25 during thread initialization. The contents of \"stack guard\" of preempted thread are checked during each context switch and if any byte has changed, FATAL_ERROR() will be called.\n\nThis method is slower than simple stack pointer range checking, but is able to detect stack overflows much more reliably. It is still sufficiently fast, assuming that the size of \"stack guard\" is reasonable.\n\nBe advised that uninitialized variables on stack which are larger than size of \"stack guard\" can create \"holes\" in the stack, thus circumventing this detection mechanism. This especially applies to arrays used as buffers.")
set("distortos_Checks_04_Stack_guard_contents_during_system_tick"
		"ON"
		CACHE
		"BOOL"
		"Check stack guard contents during system tick.\n\nSimilar to \"distortos_Checks_03_Stack_guard_contents_during_context_switch\", but executed during every system tick.")
set("distortos_Checks_05_Stack_guard_size"
		"32"
		CACHE
		"STRING"
		"Size (in bytes) of \"stack guard\".\n\nAny value which is not a multiple of stack alignment required by architecture, will be rounded up.\n\nAllowed range: [1; 2147483647]")
set("distortos_Checks_06_Asserts"
		"ON"
		CACHE
		"BOOL"
		"Enable asserts.\n\nSome errors, which are clearly program bugs, are never reported using error codes. When this option is enabled, these preconditions, postconditions, invariants and assertions are checked with assert() macro. On the other hand - with this option disabled, they are completely ignored.\n\nIt is highly recommended to keep this option enabled until the application is thoroughly tested.")
set("distortos_buttons"
		"ON"
		CACHE
		"BOOL"
		"Enable buttons")
set("distortos_buttons_B1"
		"ON"
		CACHE
		"BOOL"
		"Enable B1 (User)")
set("distortos_leds"
		"ON"
		CACHE
		"BOOL"
		"Enable leds")
set("distortos_leds_Ld1"
		"ON"
		CACHE
		"BOOL"
		"Enable Ld1 (Green)")
set("distortos_Peripherals_SDMMC1"
		"OFF"
		CACHE
		"BOOL"
		"Enable SDMMC1 low-level driver.")
set("distortos_Peripherals_SPI1"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI1 low-level driver.")
set("distortos_Peripherals_SPI2"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI2 low-level driver.")
set("distortos_Peripherals_SPI3"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI3 low-level driver.")
set("distortos_Peripherals_SPI4"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI4 low-level driver.")
set("distortos_Peripherals_SPI5"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI5 low-level driver.")
set("distortos_Peripherals_SPI6"
		"OFF"
		CACHE
		"BOOL"
		"Enable SPI6 low-level driver.")
set("distortos_Peripherals_USART1"
		"ON"
		CACHE
		"BOOL"
		"Enable USART1 low-level driver.")
set("distortos_Peripherals_USART2"
		"OFF"
		CACHE
		"BOOL"
		"Enable USART2 low-level driver.")
set("distortos_Peripherals_USART3"
		"OFF"
		CACHE
		"BOOL"
		"Enable USART3 low-level driver.")
set("distortos_Peripherals_UART4"
		"OFF"
		CACHE
		"BOOL"
		"Enable UART4 low-level driver.")
set("distortos_Peripherals_UART5"
		"OFF"
		CACHE
		"BOOL"
		"Enable UART5 low-level driver.")
set("distortos_Peripherals_USART6"
		"OFF"
		CACHE
		"BOOL"
		"Enable USART6 low-level driver.")
set("distortos_Peripherals_UART7"
		"OFF"
		CACHE
		"BOOL"
		"Enable UART7 low-level driver.")
set("distortos_Peripherals_UART8"
		"OFF"
		CACHE
		"BOOL"
		"Enable UART8 low-level driver.")
set("distortos_Peripherals_GPIOA"
		"ON"
		CACHE
		"BOOL"
		"Enable GPIOA.")
set("distortos_Peripherals_GPIOB"
		"ON"
		CACHE
		"BOOL"
		"Enable GPIOB.")
set("distortos_Peripherals_GPIOC"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOC.")
set("distortos_Peripherals_GPIOD"
		"ON"
		CACHE
		"BOOL"
		"Enable GPIOD.")
set("distortos_Peripherals_GPIOE"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOE.")
set("distortos_Peripherals_GPIOF"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOF.")
set("distortos_Peripherals_GPIOG"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOG.")
set("distortos_Peripherals_GPIOH"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOH.")
set("distortos_Peripherals_GPIOI"
		"ON"
		CACHE
		"BOOL"
		"Enable GPIOI.")
set("distortos_Peripherals_GPIOJ"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOJ.")
set("distortos_Peripherals_GPIOK"
		"OFF"
		CACHE
		"BOOL"
		"Enable GPIOK.")
set("distortos_Peripherals_DMA1"
		"OFF"
		CACHE
		"BOOL"
		"Enable DMA1 low-level driver.")
set("distortos_Peripherals_DMA2"
		"OFF"
		CACHE
		"BOOL"
		"Enable DMA2 low-level driver.")
set("distortos_Clocks_00_Standard_configuration_of_clocks"
		"ON"
		CACHE
		"BOOL"
		"Enable standard configuration of clocks.\n\nThis will set values selected below and additionally configure appropriate FLASH latency before switching system clock to selected source.\n\nIf disabled, no clock configuration will be done during chip initialization. The values entered below (frequencies, dividers, ...) will only be used to determine chip clocks. The user must configure the chip manually to match these settings.")
set("distortos_Clocks_01_Voltage_scale_mode"
		"1"
		CACHE
		"STRING"
		"Select voltage scale mode.\n\nAllowed range: [1; 3]")
set("distortos_Clocks_02_Over_drive_mode"
		"ON"
		CACHE
		"BOOL"
		"Enable over-drive mode.")
set("distortos_Clocks_03_HSE"
		"ON"
		CACHE
		"BOOL"
		"Enable HSE external user clock, 25000000 Hz.")
set("distortos_Clocks_04_PLLs"
		"ON"
		CACHE
		"BOOL"
		"Enable PLLs.")
set("distortos_Clocks_05_Clock_source_of_PLLs"
		"HSE"
		CACHE
		"STRING"
		"Select clock source of main and audio PLLs.")
set("distortos_Clocks_06_PLLM"
		"25"
		CACHE
		"STRING"
		"PLLM value for main and audio PLLs.\n\nIt is used to divide PLL input clock (PLLin) before it is fed to VCO of each PLL. VCO input frequency (VCOin) must be in the range [1 MHz; 2 MHz], 2 MHz is recommended to limit PLL jitter.\n\nVCOin = PLLin / PLLM\n\nAllowed range: [2; 63]")
set("distortos_Clocks_07_PLL"
		"ON"
		CACHE
		"BOOL"
		"Enable main PLL.")
set("distortos_Clocks_08_PLLN"
		"432"
		CACHE
		"STRING"
		"PLLN value for main PLL.\n\nIt is used to multiply VCO input frequency (VCOin). Resulting VCO output frequency (VCOout) must be in the range [100 MHz; 432 MHz].\n\nVCOout = VCOin * PLLN = PLLin / PLLM * PLLN\n\nAllowed range: [50; 432]")
set("distortos_Clocks_09_PLLP"
		"2"
		CACHE
		"STRING"
		"PLLP value for main PLL.\n\nIt is used to divide VCO output frequency (VCOout). PLL output frequency (PLLout) must not exceed:\n- 216 MHz (with over-drive mode) or 180 MHz (otherwise) in voltage scale 1 mode,\n- 180 MHz (with over-drive mode) or 168 MHz (otherwise) in voltage scale 2 mode,\n- 144 MHz in voltage scale 3 mode.\n\nPLLout = VCOout / PLLP = PLLin / PLLM * PLLN / PLLP")
set("distortos_Clocks_10_PLLQ"
		"9"
		CACHE
		"STRING"
		"PLLQ value for main PLL.\n\nIt is used to divide VCO output frequency (VCOout) to produce clock for USB OTG FS, SDMMC and RNG (PLLQout). USB OTG FS requires a 48 MHz clock for correct operation, while SDMMC and RNG need a frequency lower than or equal to 48 MHz to work correctly.\n\nPLLQout = VCOout / PLLQ = PLLin / PLLM * PLLN / PLLQ\n\nAllowed range: [2; 15]")
set("distortos_Clocks_12_PLLI2S"
		"OFF"
		CACHE
		"BOOL"
		"Enable PLLI2S.")
set("distortos_Clocks_17_PLLSAI"
		"OFF"
		CACHE
		"BOOL"
		"Enable PLLSAI.")
set("distortos_Clocks_22_PLL48_clock_source"
		"PLLQ"
		CACHE
		"STRING"
		"Select PLL48 clock source.")
set("distortos_Clocks_23_System_clock_source"
		"PLL"
		CACHE
		"STRING"
		"Select system clock source.")
set("distortos_Clocks_24_HPRE"
		"1"
		CACHE
		"STRING"
		"AHB clock division factor.\n\nAHBclk = SYSclk / AHBdivider")
set("distortos_Clocks_25_PPRE1"
		"4"
		CACHE
		"STRING"
		"APB1 (low speed) clock division factor.\n\nAPB1 clock must not exceed 54 MHz (with over-drive mode) or 45 MHz (otherwise).\n\nAPB1clk = AHBclk / APB1divider")
set("distortos_Clocks_26_PPRE2"
		"2"
		CACHE
		"STRING"
		"APB2 (high speed) clock division factor.\n\nAPB2 clock must not exceed 108 MHz (with over-drive mode) or 90 MHz (otherwise).\n\nAPB2clk = AHBclk / APB2divider")
set("distortos_Clocks_27_SDMMC1_clock_source"
		"PLL48CLK"
		CACHE
		"STRING"
		"Select SDMMC1 adapter clock source.")
set("distortos_Memory_00_Flash_prefetch"
		"ON"
		CACHE
		"BOOL"
		"Enable flash prefetch option in FLASH->ACR register.")
set("distortos_Memory_01_Flash_ART_accelerator"
		"ON"
		CACHE
		"BOOL"
		"Enable flash ART accelerator in FLASH->ACR register.")
set("distortos_Architecture_00_Interrupt_stack_size"
		"4096"
		CACHE
		"STRING"
		"Size (in bytes) of \"main\" stack used by core exceptions and interrupts in Handler mode.\n\nAllowed range: [8; 2147483647]")
set("distortos_Architecture_01_Interrupt_priority_disabled_in_critical_sections"
		"0"
		CACHE
		"STRING"
		"Interrupt priority disabled in critical sections.\n\nMinimal numerical priority (inclusive) of interrupt handlers that can use system's functions.\n\nDuring critical sections all interrupts with numerical priority above or equal to this value will be disabled. Interrupts with numerical priority below this value are never disabled, but they may not use any system's functions.\n\nNote - \"lower\" logical priority has \"higher\" numeric value! If this option is set to \"x\", then interrupts with priorities between \"x\" and 255 (both inclusive) may use system's functions, while interrupts with priorities between 0 and \"x - 1\" (both inclusive) may not. If 0 is chosen, then all interrupts (except HardFault and NMI) are disabled during critical sections, so they may use system's functions.\n\nAllowed range: [0; 15]")
set("distortos_Memory_regions_00_text_vectorTable"
		"flash"
		CACHE
		"STRING"
		"Memory region for .text.vectorTable section in linker script")
set("distortos_Memory_regions_01_text"
		"flash"
		CACHE
		"STRING"
		"Memory region for .text section in linker script")
set("distortos_Memory_regions_02_ARM_exidx"
		"flash"
		CACHE
		"STRING"
		"Memory region for .ARM.exidx section in linker script")
set("distortos_Memory_regions_03_Main_stack"
		"SRAM"
		CACHE
		"STRING"
		"Memory region for main stack in linker script")
set("distortos_Memory_regions_04_bss"
		"SRAM"
		CACHE
		"STRING"
		"Memory region for .bss section in linker script")
set("distortos_Memory_regions_05_data_VMA"
		"SRAM"
		CACHE
		"STRING"
		"VMA memory region for .data section in linker script")
set("distortos_Memory_regions_06_data_LMA"
		"flash"
		CACHE
		"STRING"
		"LMA memory region for .data section in linker script")
set("distortos_Memory_regions_07_noinit"
		"SRAM"
		CACHE
		"STRING"
		"Memory region for .noinit section in linker script")
set("distortos_Memory_regions_08_SRAM_data_LMA"
		"flash"
		CACHE
		"STRING"
		"LMA memory region for .SRAM.data section in linker script")
set("distortos_Memory_regions_09_ITCM_data_LMA"
		"flash"
		CACHE
		"STRING"
		"LMA memory region for .ITCM.data section in linker script")
set("distortos_Memory_regions_10_Process_stack"
		"SRAM"
		CACHE
		"STRING"
		"Memory region for process stack in linker script")
set("distortos_Memory_regions_11_Heap"
		"SRAM"
		CACHE
		"STRING"
		"Memory region for heap in linker script")
set("DISTORTOS_CONFIGURATION_VERSION"
		"4"
		CACHE
		"INTERNAL"
		"")
set("CMAKE_BUILD_TYPE"
		"Debug"
		CACHE
		"STRING"
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel ...")
set("CMAKE_CXX_FLAGS"
		"-fno-rtti -fno-exceptions -ffunction-sections -fdata-sections -Wall -Wextra -Wshadow -Wno-psabi -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mthumb -mfloat-abi=hard -fno-use-cxa-atexit"
		CACHE
		"STRING"
		"Flags used by the CXX compiler during all build types.")
set("CMAKE_CXX_FLAGS_DEBUG"
		"-Og -g -ggdb3"
		CACHE
		"STRING"
		"Flags used by the CXX compiler during DEBUG builds.")
set("CMAKE_CXX_FLAGS_MINSIZEREL"
		"-Os"
		CACHE
		"STRING"
		"Flags used by the CXX compiler during MINSIZEREL builds.")
set("CMAKE_CXX_FLAGS_RELEASE"
		"-O2"
		CACHE
		"STRING"
		"Flags used by the CXX compiler during RELEASE builds.")
set("CMAKE_CXX_FLAGS_RELWITHDEBINFO"
		"-O2 -g -ggdb3"
		CACHE
		"STRING"
		"Flags used by the CXX compiler during RELWITHDEBINFO builds.")
set("CMAKE_C_FLAGS"
		"-ffunction-sections -fdata-sections -Wall -Wextra -Wshadow -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mthumb -mfloat-abi=hard"
		CACHE
		"STRING"
		"Flags used by the C compiler during all build types.")
set("CMAKE_C_FLAGS_DEBUG"
		"-Og -g -ggdb3"
		CACHE
		"STRING"
		"Flags used by the C compiler during DEBUG builds.")
set("CMAKE_C_FLAGS_MINSIZEREL"
		"-Os"
		CACHE
		"STRING"
		"Flags used by the C compiler during MINSIZEREL builds.")
set("CMAKE_C_FLAGS_RELEASE"
		"-O2"
		CACHE
		"STRING"
		"Flags used by the C compiler during RELEASE builds.")
set("CMAKE_C_FLAGS_RELWITHDEBINFO"
		"-O2 -g -ggdb3"
		CACHE
		"STRING"
		"Flags used by the C compiler during RELWITHDEBINFO builds.")
set("CMAKE_EXE_LINKER_FLAGS"
		"-Wl,--gc-sections"
		CACHE
		"STRING"
		"Flags used by the linker during all build types.")
set("CMAKE_EXE_LINKER_FLAGS_DEBUG"
		""
		CACHE
		"STRING"
		"Flags used by the linker during DEBUG builds.")
set("CMAKE_EXE_LINKER_FLAGS_MINSIZEREL"
		""
		CACHE
		"STRING"
		"Flags used by the linker during MINSIZEREL builds.")
set("CMAKE_EXE_LINKER_FLAGS_RELEASE"
		""
		CACHE
		"STRING"
		"Flags used by the linker during RELEASE builds.")
set("CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO"
		""
		CACHE
		"STRING"
		"Flags used by the linker during RELWITHDEBINFO builds.")
set("CMAKE_EXPORT_COMPILE_COMMANDS"
		"ON"
		CACHE
		"BOOL"
		"Enable/Disable output of compile commands during generation.")
set("CMAKE_MODULE_LINKER_FLAGS"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of modules during all build types.")
set("CMAKE_MODULE_LINKER_FLAGS_DEBUG"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of modules during DEBUG builds.")
set("CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of modules during MINSIZEREL builds.")
set("CMAKE_MODULE_LINKER_FLAGS_RELEASE"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of modules during RELEASE builds.")
set("CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of modules during RELWITHDEBINFO builds.")
set("CMAKE_SHARED_LINKER_FLAGS"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of shared libraries during all build types.")
set("CMAKE_SHARED_LINKER_FLAGS_DEBUG"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of shared libraries during DEBUG builds.")
set("CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of shared libraries during MINSIZEREL builds.")
set("CMAKE_SHARED_LINKER_FLAGS_RELEASE"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of shared libraries during RELEASE builds.")
set("CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of shared libraries during RELWITHDEBINFO builds.")
set("CMAKE_STATIC_LINKER_FLAGS"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of static libraries during all build types.")
set("CMAKE_STATIC_LINKER_FLAGS_DEBUG"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of static libraries during DEBUG builds.")
set("CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of static libraries during MINSIZEREL builds.")
set("CMAKE_STATIC_LINKER_FLAGS_RELEASE"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of static libraries during RELEASE builds.")
set("CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO"
		""
		CACHE
		"STRING"
		"Flags used by the linker during the creation of static libraries during RELWITHDEBINFO builds.")
set("CMAKE_TOOLCHAIN_FILE"
		"${DISTORTOS_PATH}/source/board/ST_32F746GDISCOVERY/Toolchain-ST_32F746GDISCOVERY.cmake"
		CACHE
		"FILEPATH"
		"The CMake toolchain file")
set("CMAKE_VERBOSE_MAKEFILE"
		"OFF"
		CACHE
		"BOOL"
		"If this value is on, makefiles will be generated without the .SILENT directive, and all commands will be echoed to the console during the make.  This is useful for debugging only. With Visual Studio IDE projects all commands are done without /nologo.")
set("STM32F7xx_HAL_Driver_ADC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's ADC module.")
set("STM32F7xx_HAL_Driver_CAN"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's CAN module.")
set("STM32F7xx_HAL_Driver_CEC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's CEC module.")
set("STM32F7xx_HAL_Driver_CRC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's CRC module.")
set("STM32F7xx_HAL_Driver_CRYP"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's CRYP module.")
set("STM32F7xx_HAL_Driver_DAC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DAC module.")
set("STM32F7xx_HAL_Driver_DCMI"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DCMI module.")
set("STM32F7xx_HAL_Driver_DFSDM"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DFSDM module.")
set("STM32F7xx_HAL_Driver_DMA"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DMA module.")
set("STM32F7xx_HAL_Driver_DMA2D"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DMA2D module.")
set("STM32F7xx_HAL_Driver_DSI"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's DSI module.")
set("STM32F7xx_HAL_Driver_ETH"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's ETH module.")
set("STM32F7xx_HAL_Driver_HASH"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's HASH module.")
set("STM32F7xx_HAL_Driver_HCD"
		"ON"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's HCD module.")
set("STM32F7xx_HAL_Driver_I2C"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's I2C module.")
set("STM32F7xx_HAL_Driver_I2S"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's I2S module.")
set("STM32F7xx_HAL_Driver_IRDA"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's IRDA module.")
set("STM32F7xx_HAL_Driver_IWDG"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's IWDG module.")
set("STM32F7xx_HAL_Driver_JPEG"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's JPEG module.")
set("STM32F7xx_HAL_Driver_LPTIM"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's LPTIM module.")
set("STM32F7xx_HAL_Driver_LTDC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's LTDC module.")
set("STM32F7xx_HAL_Driver_MDIOS"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's MDIOS module.")
set("STM32F7xx_HAL_Driver_MMC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's MMC module.")
set("STM32F7xx_HAL_Driver_NAND"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's NAND module.")
set("STM32F7xx_HAL_Driver_NOR"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's NOR module.")
set("STM32F7xx_HAL_Driver_PCD"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's PCD module.")
set("STM32F7xx_HAL_Driver_PWR"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's PWR module.")
set("STM32F7xx_HAL_Driver_QSPI"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's QSPI module.")
set("STM32F7xx_HAL_Driver_RNG"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's RNG module.")
set("STM32F7xx_HAL_Driver_RTC"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's RTC module.")
set("STM32F7xx_HAL_Driver_SAI"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SAI module.")
set("STM32F7xx_HAL_Driver_SD"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SD module.")
set("STM32F7xx_HAL_Driver_SDRAM"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SDRAM module.")
set("STM32F7xx_HAL_Driver_SMARTCARD"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SMARTCARD module.")
set("STM32F7xx_HAL_Driver_SMBUS"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SMBUS module.")
set("STM32F7xx_HAL_Driver_SPDIFRX"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SPDIFRX module.")
set("STM32F7xx_HAL_Driver_SPI"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SPI module.")
set("STM32F7xx_HAL_Driver_SRAM"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's SRAM module.")
set("STM32F7xx_HAL_Driver_TIM"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's TIM module.")
set("STM32F7xx_HAL_Driver_UART"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's UART module.")
set("STM32F7xx_HAL_Driver_USART"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's USART module.")
set("STM32F7xx_HAL_Driver_WWDG"
		"OFF"
		CACHE
		"BOOL"
		"Enable STM32F7xx_HAL_Driver's WWDG module.")

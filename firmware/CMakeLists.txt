CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
SET(CMAKE_SYSTEM_NAME Generic)

#============================================================================================
PROJECT(ws-ost)

SET(DEVICE "atxmega32c4")
SET(PART "ATxmega32C4") # For defines in the C code
SET(FREQ "24000000")

#============================================================================================
enable_language(C ASM)

SET(DEBUGPRINTS "-DDEBUGPRINTS=1")
#SET(DEBUGPRINTS "");

SET(CMAKE_C_COMPILER /home/mcochrane/build/avr8-gnu-toolchain-linux_x86_64/bin/avr-gcc)
SET(CMAKE_ASM_COMPILER /home/mcochrane/build/avr8-gnu-toolchain-linux_x86_64/bin/avr-gcc)
SET(CMAKE_CXX_COMPILER /home/mcochrane/build/avr8-gnu-toolchain-linux_x86_64/bin/avr-g++)

SET(CMAKE_C_FLAGS  "-Os -mmcu=${DEVICE} ${DEBUGPRINTS} -DF_CPU=${FREQ}UL -D__${PART}__ -std=gnu99 -Wl,--gc-sections")
SET(CMAKE_CXX_FLAGS "-Os -mmcu=${DEVICE} ${DEBUGPRINTS} -DF_CPU=${FREQ}UL -D__${PART}__  -Wl,--gc-sections")
SET(ASM_FLAGS "-Os -mmcu=${DEVICE} ${DEBUGPRINTS} -DF_CPU=${FREQ}UL -D__${PART}__  -Wl,--gc-sections")

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${ASM_OPTIONS}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#============================================================================================

include_directories(
        libs
        libs/usb
        asf/xmega/utils
        asf/xmega/utils/preprocessor
        asf/common/utils
        asf/common/utils/interrupt
        asf/common/boards
        asf/common/boards/user_board
        asf/common/boards/user_board/board_config
        asf/common/services/usb/class/cdc/device
        asf/common/services/usb/class/cdc

        asf/common/services/sleepmgr
        asf/common/services/sleepmgr/xmega
        asf/common/services/sleepmgr/xmega/module_config

        asf/common/services/usb
        asf/common/services/usb/udc
        asf/common/services/usb/class/cdc
        asf/common/services/usb/class/cdc/device

        asf/common/services/clock
        asf/common/services/clock/xmega
        asf/common/services/clock/xmega/module_config_usb

        asf/xmega/drivers/sleep
        asf/xmega/drivers/nvm
        asf/xmega/drivers/cpu

        libs/eeprom_driver
        libs/exception
        libs/memory
)

SET(SOURCE_FILES

        main.cpp
        asf/common/services/usb/udc/udc.c
        asf/common/services/usb/class/cdc/device/udi_cdc.c
        asf/xmega/drivers/usb/usb_device.c
#        libs/usb/uart_xmega.c

        asf/common/services/clock/xmega/sysclk.c
        asf/common/services/sleepmgr/xmega/sleepmgr.c
        asf/xmega/drivers/nvm/nvm.c
        asf/xmega/drivers/nvm/nvm_asm.s
        asf/xmega/drivers/cpu/ccp.s

        #        libs/usb/conf_usb.h
        libs/asf.h
        libs/main.h
        asf/common/boards/user_board/init.c
        asf/common/services/usb/class/cdc/device/udi_cdc_desc.c
        libs/min/min.h
        libs/min/layer1.c
        libs/min/layer2.h
        libs/min/layer2.c
        libs/serial/usb_serial.h
        libs/serial/usb_serial.c
        libs/eeprom_driver/eeprom_driver.h
        libs/eeprom_driver/eeprom_driver.c
        libs/eeprom_driver/avr_compiler.h
        libs/exception/CException.h
        libs/exception/CException.c
        libs/exception/CExceptionConfig.h
        libs/exception/ExceptionValues.h
        libs/memory/MemoryWrapper.c
        libs/memory/MemoryWrapper.h

        libs/memory/CppNewDeleteOps.cpp
        libs/memory/CppNewDeleteOps.h libs/config_comms/rule_writer.cpp libs/config_comms/ruler_writer.h libs/min/min_received_cmds.cpp libs/min/min_transmit_cmds.cpp libs/min/min_received_cmds.h libs/min/min_transmit_cmds.h libs/min/layer2_helper.h libs/min/layer2_helper.c libs/TimerDescriptionLanguage/TdlRule.cpp libs/TimerDescriptionLanguage/TdlRule.h libs/TimerDescriptionLanguage/TdlAction.cpp libs/TimerDescriptionLanguage/TdlAction.h libs/TimerDescriptionLanguage/TdlActivator.cpp libs/TimerDescriptionLanguage/TdlActivator.h libs/datetime/DateTime.cpp libs/datetime/DateTime.h libs/TimerDescriptionLanguage/TdlChannel.cpp libs/TimerDescriptionLanguage/TdlChannel.h libs/TimerDescriptionLanguage/TdlChannels.cpp libs/TimerDescriptionLanguage/TdlChannels.h libs/datetime/Period.cpp libs/datetime/Period.h libs/datetime/PeriodInterval.cpp libs/datetime/PeriodInterval.h libs/datetime/Interval.cpp libs/datetime/Interval.h libs/simplib/array.cpp libs/simplib/array.h libs/TimerDescriptionLanguage/TdlEvent.cpp libs/TimerDescriptionLanguage/TdlEvent.h libs/TimerDescriptionLanguage/TdlRules.cpp libs/TimerDescriptionLanguage/TdlRules.h libs/TimerDescriptionLanguage/TdlRuleState.h libs/TimerDescriptionLanguage/TdlActivatorState.h libs/TimerDescriptionLanguage/TdlChannelState.h libs/DS3232SN/DS3232SN.cpp libs/DS3232SN/DS3232SN.h libs/hal/Pin.cpp libs/hal/Pin.h libs/DS3232SN/DS3232SN_defs.h libs/hal/i2c.c libs/hal/i2c.h libs/datetime/PeriodField.h libs/min/encode_decode.h libs/min/encode_decode.c libs/helpers/compile_time_sizeof.h libs/TimerDescriptionLanguage/TdlEvents.cpp libs/TimerDescriptionLanguage/TdlEvents.h libs/TimerDescriptionLanguage/TdlRuleRunner.cpp libs/TimerDescriptionLanguage/TdlRuleRunner.h)

#============================================================================================

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -R.eeprom ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.eep)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND touch ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.lss && avr-objdump -h -S ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.lss)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-size ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} --mcu=${DEVICE} --format=avr)

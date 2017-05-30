CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
SET(CMAKE_SYSTEM_NAME Generic)

#add_subdirectory(libs/cmocka-1.1.1)

#============================================================================================
PROJECT(ws-ost-tests)

SET(DEVICE "cortex-m0plus")
SET(PART "SAMD21E18A") # For defines in the C code
SET(FREQ "24000000")
SET(LDFILE "../../linker/samd21e18a_flash.ld")

SET(STACK_SIZE  0x2000)
SET(HEAP_SIZE   0x2000)

#SET(STACK_SIZE  5120)  # 5kB
#SET(HEAP_SIZE   20480) # 20kB

SET(FLASH_SIZE  0x40000)
SET(RAM_SIZE    0x8000)


#SET(PrintHex)

set(CMAKE_VERBOSE_MAKEFILE ON)
#============================================================================================
enable_language(C ASM)

SET(DEBUGPRINTS "-DDEBUGPRINTS=1")
#SET(DEBUGPRINTS "");

SET(COMPILER_BIN_PATH /home/mcochrane/build/arm-none-eabi/bin)

# Executables
SET(CMAKE_C_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-gcc)
SET(CMAKE_ASM_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-as)
SET(CMAKE_CXX_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-g++)
SET(OBJCOPY ${COMPILER_BIN_PATH}/arm-none-eabi-objcopy)
SET(OBJDUMP ${COMPILER_BIN_PATH}/arm-none-eabi-objdump)
SET(SIZE ${COMPILER_BIN_PATH}/arm-none-eabi-size)

#ADD_DEFINITIONS(-D__SAMD21E18A__)
# Defines
SET(DEFINES "-DEXTINT_CALLBACK_MODE=true \
        -DUDD_ENABLE \
        -DARM_MATH_CM0PLUS=true \
        -DUSB_DEVICE_LPM_SUPPORT \
        -DDONT_USE_CMSIS_INIT \
        ${DEBUGPRINTS} \
        -DF_CPU=${FREQ}UL \
        -D__${PART}__ \
        -DMCU_TESTING \
        -DHEAP_SIZE=${HEAP_SIZE} \
        -DSTACK_SIZE=${STACK_SIZE} \
        ")
# Flags
SET(CFLAGS "--specs=nosys.specs \
            -lm -larm_cortexM4l_math \
            -L\"../asf/thirdparty/Lib/GCC\" \
            -mthumb -Os -Wl,--start-group -Wl,--end-group \
            -mcpu=${DEVICE} \
            ${DEFINES} \
            -I\"${COMPILER_BIN_PATH}/../arm-none-eabi/include\" \
            -std=gnu99 \
            -ffunction-sections \
            -mlong-calls \
            -Wall \
            -fno-exceptions")

SET(CPPFLAGS "--specs=nosys.specs \
              -lm \
              -mthumb -Os \
              -Wl,--start-group -Wl,--end-group \
              -mcpu=${DEVICE} \
              ${DEFINES} \
              -I\"${COMPILER_BIN_PATH}/../arm-none-eabi/include\"  \
              -ffunction-sections \
              -mlong-calls \
              -Wall \
              -fno-rtti \
              -fno-exceptions")

SET(CMAKE_C_FLAGS  "${CFLAGS}")
SET(CMAKE_CXX_FLAGS "${CPPFLAGS}")
SET(ASM_FLAGS "-mthumb -Os -mcpu=${DEVICE} ${DEFINES} -Wa,-g")
SET(CMAKE_EXE_LINKER_FLAGS "-Xlinker --defsym=HEAP_SIZE=${HEAP_SIZE}  -Xlinker --defsym=STACK_SIZE=${STACK_SIZE} -Wl,--start-group -Wl,--end-group -Wl,--gc-sections -Wl,--script=${LDFILE} ")

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${ASM_OPTIONS}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#============================================================================================

include_directories(

        libs/cmocka

        ../libs
        ../libs/conf
        ../libs/usb

        ../asf/sam0/utils
        ../asf/sam0/drivers/usb
        ../asf/sam0/drivers/system
        ../asf/sam0/drivers/system/clock
        ../asf/sam0/drivers/system/clock/clock_samd21_r21_da
        ../asf/sam0/drivers/system/interrupt
        ../asf/sam0/drivers/system/interrupt/system_interrupt_samd21
        ../asf/sam0/drivers/system/pinmux
        ../asf/sam0/drivers/system/power/power_sam_d_r
        ../asf/sam0/drivers/system/reset/reset_sam_d_r
        ../asf/sam0/drivers/extint
        ../asf/sam0/drivers/port
        ../asf/sam0/utils/preprocessor
        ../asf/sam0/utils/header_files
        ../asf/common/utils
        ../asf/common/utils/interrupt
        ../asf/common/boards
        ../asf/common/boards/user_board
        ../asf/common/boards/user_board/board_config
        ../asf/common/services/usb/class/cdc/device
        ../asf/common/services/usb/class/cdc
#
        ../asf/common/services/sleepmgr
        ../asf/common/services/sleepmgr/sam0
        ../asf/common/services/sleepmgr/sam0/module_config
#
        ../asf/common/services/usb
        ../asf/common/services/usb/udc
        ../asf/common/services/usb/class/cdc
        ../asf/common/services/usb/class/cdc/device
#
        ../asf/common/services/clock
        ../asf/sam0/drivers/nvm
        ../asf/sam0/utils/cmsis/samd21/include
        ../asf/thirdparty/CMSIS/Include

#
#        asf/xmega/drivers/sleep
#        asf/xmega/drivers/nvm
#        asf/xmega/drivers/cpu
#
#        libs/eeprom_driver
#        libs/exception
#        libs/memory
)

SET(SOURCE_FILES

        tests_main.cpp
        ../startup_samd21.c
#        ../libs/hal/Pin.cpp

        libs/cmocka/cmocka.c
        libs/stdio/read.c
        libs/stdio/write.c
        libs/stdio/stdio_usb/stdio_usb.cpp

#        main.cpp
        ../asf/common/services/usb/udc/udc.c
        ../asf/common/services/usb/class/cdc/device/udi_cdc.c
        ../asf/common/services/usb/class/cdc/device/udi_cdc_desc.c
        ../asf/sam0/drivers/usb/usb_sam_d_r/usb.c
        ../asf/sam0/drivers/usb/stack_interface/usb_device_udd.c
        ../asf/sam0/drivers/usb/stack_interface/usb_dual.c

        ../asf/sam0/drivers/extint/extint_callback.c
        ../asf/sam0/drivers/extint/extint_sam_d_r/extint.c
        ../asf/sam0/drivers/system/system.c
        ../asf/sam0/drivers/system/clock/clock_samd21_r21_da/clock.c
        ../asf/sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c
        ../asf/sam0/drivers/system/interrupt/system_interrupt.c
        ../asf/sam0/drivers/system/pinmux/pinmux.c

        ../asf/common/utils/interrupt/interrupt_sam_nvic.c
        ../asf/common/services/sleepmgr/samd/sleepmgr.c

        ../asf/sam0/drivers/nvm/nvm.c

#        asf/common/services/clock/xmega/sysclk.c
#        asf/common/services/sleepmgr/xmega/sleepmgr.c
#        asf/xmega/drivers/nvm/nvm.c
#        asf/xmega/drivers/nvm/nvm_asm.s
#        asf/xmega/drivers/cpu/ccp.s
#
#        asf/common/boards/user_board/init.c
#        asf/common/services/usb/class/cdc/device/udi_cdc_desc.c
#        libs/min/min.h
#        libs/min/layer1.c
#        libs/min/layer2.c
#        libs/serial/usb_serial.c
#        libs/eeprom/eeprom_driver.c
        ../libs/exception/CException.c
        ../libs/memory/MemoryWrapper.c
#
        ../libs/memory/CppNewDeleteOps.cpp
#        libs/nvm/NvmRuleManager.cpp
#        libs/nvm/NvmRuleManager.h
#        libs/min/min_received_cmds.cpp
#        libs/min/min_transmit_cmds.cpp
#        libs/min/layer2_helper.c
#        libs/TimerDescriptionLanguage/TdlRule.cpp
#        libs/TimerDescriptionLanguage/TdlRule.h
#        libs/TimerDescriptionLanguage/TdlAction.cpp
#        libs/TimerDescriptionLanguage/TdlAction.h
#        libs/TimerDescriptionLanguage/TdlActivator.cpp
#        libs/TimerDescriptionLanguage/TdlActivator.h
#        libs/datetime/DateTime.cpp
#        libs/datetime/DateTime.h
#        libs/TimerDescriptionLanguage/TdlChannel.cpp
#        libs/TimerDescriptionLanguage/TdlChannel.h
#        libs/TimerDescriptionLanguage/TdlChannels.cpp
#        libs/TimerDescriptionLanguage/TdlChannels.h
#        libs/datetime/Period.cpp
#        libs/datetime/Period.h
#        libs/datetime/PeriodInterval.cpp
#        libs/datetime/PeriodInterval.h
#        libs/datetime/Interval.cpp
#        libs/datetime/Interval.h
#        libs/simplib/array.cpp
#        libs/simplib/array.h
#        libs/TimerDescriptionLanguage/TdlEvent.cpp
#        libs/TimerDescriptionLanguage/TdlEvent.h
#        libs/TimerDescriptionLanguage/TdlRules.cpp
#        libs/TimerDescriptionLanguage/TdlRules.h
#        libs/TimerDescriptionLanguage/TdlRuleState.h
#        libs/TimerDescriptionLanguage/TdlActivatorState.h
#        libs/TimerDescriptionLanguage/TdlChannelState.h
#        libs/DS3232SN/DS3232SN.cpp
#        libs/DS3232SN/DS3232SN.h
#        libs/hal/Pin.cpp
#        libs/hal/Pin.h
#        libs/DS3232SN/DS3232SN_defs.h
#        libs/hal/i2c.c
#        libs/hal/i2c.h
#        libs/datetime/PeriodField.h
#        libs/min/encode_decode.h
#        libs/min/encode_decode.c
#        libs/helpers/compile_time_sizeof.h
#        libs/TimerDescriptionLanguage/TdlEvents.cpp
#        libs/TimerDescriptionLanguage/TdlEvents.h
#        libs/TimerDescriptionLanguage/TdlRuleRunner.cpp
#        libs/TimerDescriptionLanguage/TdlRuleRunner.h
        ../system_samd21.c
        ../libs/usb/usb_handlers.cpp
        libs/stdio/write.c
        libs/stdio/read.c
        libs/stdio/stdio_usb/stdio_usb.cpp
        ../libs/nvm/SamdEmulatedEepromDriver.cpp

        mem_tests.cpp
        nvm_tests.cpp)

#============================================================================================

ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCE_FILES})

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${OBJCOPY} -O ihex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${OBJCOPY} -O binary ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin)

#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.eep)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND touch ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.lss && ${OBJDUMP} -h -S ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.lss)

SET(RUN_SIZE ${SIZE} -t --format=berkeley -x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${RUN_SIZE})

IF(DEFINED PrintHex)
    SET(BUILD_CMD touch cmd.sh && echo '' > cmd.sh && echo \"${RUN_SIZE} | awk 'END {printf \\"Ram_Size:.0x%x/${RAM_SIZE}\(%f%%\)\\", \\$$2 + \\$$3, \(\\$$2 + \\$$3\)/\\"${RAM_SIZE}\\"*100}' && echo ''\" > cmd.sh)
    SET(BUILD_CMD_FLASH touch cmd.sh && echo \"${RUN_SIZE} | awk 'END {printf \\"Flash_Size:.0x%x/${FLASH_SIZE}\(%f%%\)\\", \\$$1, \(\\$$1\)/\\"${FLASH_SIZE}\\"*100}' && echo ''\" >> cmd.sh)
ELSE()
    SET(BUILD_CMD touch cmd.sh && echo '' > cmd.sh && echo \"${RUN_SIZE} | awk 'END {printf \\"Ram_Size:.%lu/%lu.bytes.\(%f%%\)\\", \\$$2 + \\$$3, \\"${RAM_SIZE}\\", \(\\$$2 + \\$$3\)/\\"${RAM_SIZE}\\"*100}' && echo ''\" > cmd.sh)
    SET(BUILD_CMD_FLASH touch cmd.sh && echo \"${RUN_SIZE} | awk 'END {printf \\"Flash_Size:.%lu/%lu.bytes.\(%f%%\)\\", \\$$1, \\"${FLASH_SIZE}\\", \(\\$$1\)/\\"${FLASH_SIZE}\\"*100}' && echo ''\" >> cmd.sh)
ENDIF()
SET(RUN_CMD /bin/bash "cmd.sh")

#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${SIZE} -t --format=berkeley ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf)
#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf /media/WorkRed/${PROJECT_NAME}.elf)


ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${BUILD_CMD})
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${BUILD_CMD_FLASH})
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${RUN_CMD})
#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND ${mycmd} VERBATIM)# \;
#        ${RUN_SIZE} | awk 'END {printf \"0x%x\", $2 + $3}') #&& \
#        echo \"0x40000 \(flash\) ram_size=0x8000\" && \
#        echo \" flash is 256kB, ram is 32kB\"")
#flash size = 0x40000 /* 256k */
#ram size 0x8000 /* 32k */

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.elf /media/WorkRed/${PROJECT_NAME}.elf)

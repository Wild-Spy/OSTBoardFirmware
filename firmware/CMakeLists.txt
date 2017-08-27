CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
SET(CMAKE_SYSTEM_NAME Generic)

add_subdirectory(tests)
add_subdirectory(libs/time)

#============================================================================================
PROJECT(ws-ost)

SET(DEVICE "cortex-m0plus")
SET(PART "SAMD21E18A") # For defines in the C code
SET(FREQ "24000000")
SET(LDFILE "../linker/samd21e18a_flash.ld")

SET(STACK_SIZE  5120)  # 5kB
SET(HEAP_SIZE   20480) # 20kB

SET(FLASH_SIZE  0x40000)
SET(RAM_SIZE    0x8000)


#SET(PrintHex)

# Uncomment to remove debug symbols
#SET(STRIP_DEBUG_SYMBOLS "-Xlinker --strip-debug")

#set(CMAKE_VERBOSE_MAKEFILE ON)
#============================================================================================
enable_language(C ASM)

#SET(DEBUGPRINTS "-DDEBUGPRINTS=3")
SET(DEBUGPRINTS "")

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
        -DHEAP_SIZE=${HEAP_SIZE} \
        -DSTACK_SIZE=${STACK_SIZE} \
        -DSYSTICK_MODE \
        -DTC_ASYNC \
        -DSAVE_FILE_AND_LINE \
        ")

# Flags

SET(CFLAGS "--specs=nosys.specs \
            -lm \
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


#SET(CMAKE_C_FLAGS  "--specs=nosys.specs -lgcc -lc -lm -lrdimon -mthumb -Os -Wl,--start-group -lm -Wl,--end-group -mcpu=${DEVICE} ${DEFINES} -I\"${COMPILER_BIN_PATH}/../arm-none-eabi/include\" -std=gnu99 -ffunction-sections -mlong-calls -Wall -fno-exceptions") #) -Wl,--gc-sections")
#SET(CMAKE_CXX_FLAGS "--specs=nosys.specs -lgcc -lc -lm -lrdimon -mthumb -Os -Wl,--start-group -lm  -Wl,--end-group -mcpu=${DEVICE} ${DEFINES} -I\"${COMPILER_BIN_PATH}/../arm-none-eabi/include\" -ffunction-sections -mlong-calls -Wall -fno-rtti -fno-exceptions") #-Wl,--gc-sections")
#SET(ASM_FLAGS "-mthumb -Os -mcpu=${DEVICE} ${DEFINES} -Wa,-g")
#SET(CMAKE_EXE_LINKER_FLAGS "-lrdimon -mthumb -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m0plus -Wl,--script=${LDFILE}")

SET(CMAKE_C_FLAGS  "${CFLAGS}")
SET(CMAKE_CXX_FLAGS "${CPPFLAGS}")
SET(ASM_FLAGS "-mthumb -Os -mcpu=${DEVICE} ${DEFINES} -Wa,-g")
SET(CMAKE_EXE_LINKER_FLAGS "${STRIP_DEBUG_SYMBOLS} \
        -Xlinker --defsym=FLASH_SIZE=${FLASH_SIZE} \
        -Xlinker --defsym=RAM_SIZE=${RAM_SIZE} \
        -Xlinker --defsym=HEAP_SIZE=${HEAP_SIZE} \
        -Xlinker --defsym=STACK_SIZE=${STACK_SIZE} \
        -Wl,--start-group -Wl,--end-group -Wl,--gc-sections \
        -Wl,--section-start=.text=0x2000 \
        -Wl,--script=${LDFILE} ")

#-Wl,--section-start=.text=0x2000\
# The below line in the above linker flags MAKES THIS UPLOADABLE WITH THE BOOTLOADER
#-Wl,--section-start=.text=0x2000 \

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${ASM_OPTIONS}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#============================================================================================

include_directories(
        .
        libs
        libs/conf
        libs/usb
        libs/time
#        libs/hw
        asf/sam0/utils
        asf/sam0/drivers/usb
        asf/sam0/drivers/system
        asf/sam0/drivers/system/clock
        asf/sam0/drivers/system/clock/clock_samd21_r21_da
        asf/sam0/drivers/system/interrupt
        asf/sam0/drivers/system/interrupt/system_interrupt_samd21
        asf/sam0/drivers/system/pinmux
        asf/sam0/drivers/system/power/power_sam_d_r
        asf/sam0/drivers/system/reset/reset_sam_d_r
        asf/sam0/drivers/extint
        asf/sam0/drivers/port
        asf/sam0/drivers/sercom
        asf/sam0/drivers/sercom/i2c
        asf/sam0/utils/preprocessor
        asf/sam0/utils/header_files
        asf/common/utils
        asf/common/utils/interrupt
        asf/common/boards
        asf/common/boards/user_board
        asf/common/boards/user_board/board_config
        asf/common/services/usb/class/cdc/device
        asf/common/services/usb/class/cdc
        asf/sam0/drivers/tc/
#
        asf/common/services/sleepmgr
        asf/common/services/sleepmgr/sam0
        asf/common/services/sleepmgr/sam0/module_config
#
        asf/common/services/usb
        asf/common/services/usb/udc
        asf/common/services/usb/class/cdc
        asf/common/services/usb/class/cdc/device

        asf/common2/delay
        asf/common2/delay/sam0
#
        asf/common/services/clock
        asf/sam0/drivers/nvm
        asf/sam0/utils/cmsis/samd21/include
        asf/thirdparty/CMSIS/Include

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

        main1.cpp
        startup_samd21.c
        libs/hal/Pin.cpp

        asf/common/services/usb/udc/udc.c
        asf/common/services/usb/class/cdc/device/udi_cdc.c
        asf/common/services/usb/class/cdc/device/udi_cdc_desc.c
        asf/sam0/drivers/usb/usb_sam_d_r/usb.c
        asf/sam0/drivers/usb/stack_interface/usb_device_udd.c
        asf/sam0/drivers/usb/stack_interface/usb_dual.c

        asf/sam0/drivers/extint/extint_callback.c
        asf/sam0/drivers/extint/extint_sam_d_r/extint.c
        asf/sam0/drivers/system/system.c
        asf/sam0/drivers/system/clock/clock_samd21_r21_da/clock.c
        asf/sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c
        asf/sam0/drivers/system/interrupt/system_interrupt.c
        asf/sam0/drivers/system/pinmux/pinmux.c
        asf/sam0/drivers/sercom/sercom.c
        asf/sam0/drivers/sercom/i2c/i2c_sam0/i2c_master.c
        asf/sam0/drivers/port/port.c
        asf/sam0/drivers/tc/tc_interrupt.c
        asf/sam0/drivers/tc/tc_sam_d_r/tc.c

        asf/common/utils/interrupt/interrupt_sam_nvic.c
        asf/common/services/sleepmgr/samd/sleepmgr.c
        asf/common2/delay/sam0/systick_counter.c

        asf/sam0/drivers/nvm/nvm.c

        libs/min/layer1.c
        libs/min/layer2.c
        libs/serial/usb_serial.c

        libs/exception/CException.c
        libs/memory/MemoryWrapper.c
#
        libs/memory/CppNewDeleteOps.cpp
        libs/nvm/NvmRuleManager.cpp
        libs/nvm/NvmRuleLoader.cpp
        libs/min/min_received_cmds.cpp
        libs/min/min_transmit_cmds.cpp
        libs/TimerDescriptionLanguage/TdlRule.cpp
        libs/TimerDescriptionLanguage/TdlAction.cpp
        libs/TimerDescriptionLanguage/TdlActivator.cpp
        libs/datetime/DateTime.cpp
        libs/TimerDescriptionLanguage/TdlChannel.cpp
        libs/TimerDescriptionLanguage/TdlChannels.cpp
        libs/datetime/Period.cpp
        libs/datetime/PeriodInterval.cpp
        libs/datetime/Interval.cpp
        libs/simplib/array.cpp
        libs/TimerDescriptionLanguage/TdlEvent.cpp
        libs/TimerDescriptionLanguage/TdlRules.cpp
        libs/DS3232SN/DS3232SN.cpp
        libs/min/encode_decode.c
        libs/TimerDescriptionLanguage/TdlEvents.cpp
        libs/TimerDescriptionLanguage/TdlRuleRunner.cpp
        system_samd21.c
        libs/usb/usb_handlers.cpp
        libs/nvm/SamdEmulatedEepromDriver.cpp
        libs/hal/I2c.cpp libs/time_tick/time_tick_sam.c)

#============================================================================================
ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCE_FILES} )
target_link_libraries(${PROJECT_NAME}.elf timeAvrLib)

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
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin ~/${PROJECT_NAME}.bin)

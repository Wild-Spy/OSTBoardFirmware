CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM Generic)

#============================================================================================
PROJECT(ws-ost-bootloader NONE)

SET(DEVICE "cortex-m0plus")
SET(PART "SAMD21E18A") # For defines in the C code
SET(FREQ "24000000")
SET(LDFILE "../firmware/linker/samd21e18a_flash.ld")

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

#SET(DEBUGPRINTS "-DDEBUGPRINTS=1")
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
        -Wl,--section-start=.text=0x2000\
        -Wl,--script=${LDFILE} ")

# The below line in the above linker flags MAKES THIS UPLOADABLE WITH THE BOOTLOADER
#-Wl,--section-start=.text=0x2000 \

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${ASM_OPTIONS}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

#============================================================================================

include_directories(
        .
        drivers
        utils
        utils/interrupt
        utils/preprocessor
        atmel
        CMSIS/Include
)

SET(SOURCE_FILES
        main.c
        sam_ba_monitor.c
        startup_samd21.c
        usart_sam_ba.c
        utils/interrupt/interrupt_sam_nvic.c
        drivers/cdc_enumerate.c
        drivers/uart_driver.c
        )

#============================================================================================
ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCE_FILES} )

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

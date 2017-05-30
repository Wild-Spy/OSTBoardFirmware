cmake_minimum_required(VERSION 2.8)
#set(firmwarelibs_DIR ../../firmware/libs)
#set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
#set(test_includes_DIR ../includes)

SET(DEVICE "cortex-m0plus")
SET(PART "SAMD21E18A") # For defines in the C code
SET(FREQ "24000000")

enable_language(C ASM)

SET(COMPILER_BIN_PATH /home/mcochrane/build/arm-none-eabi/bin)

# Executables
SET(CMAKE_C_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-gcc)
SET(CMAKE_ASM_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-as)
SET(CMAKE_CXX_COMPILER ${COMPILER_BIN_PATH}/arm-none-eabi-g++)
SET(OBJCOPY ${COMPILER_BIN_PATH}/arm-none-eabi-objcopy)
SET(OBJDUMP ${COMPILER_BIN_PATH}/arm-none-eabi-objdump)
SET(SIZE ${COMPILER_BIN_PATH}/arm-none-eabi-size)

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

SET(CMAKE_C_FLAGS  "${CFLAGS}")
SET(CMAKE_CXX_FLAGS "${CPPFLAGS}")
SET(ASM_FLAGS "-mthumb -Os -mcpu=${DEVICE} ${DEFINES} -Wa,-g")
SET(CMAKE_EXE_LINKER_FLAGS "${STRIP_DEBUG_SYMBOLS} \
        -Xlinker --defsym=FLASH_SIZE=${FLASH_SIZE} \
        -Xlinker --defsym=RAM_SIZE=${RAM_SIZE} \
        -Xlinker --defsym=HEAP_SIZE=${HEAP_SIZE} \
        -Xlinker --defsym=STACK_SIZE=${STACK_SIZE} \
        -Wl,--start-group -Wl,--end-group -Wl,--gc-sections \
        -Wl,--script=${LDFILE} ")

set(time_test_src_DIR .)
set(SOURCE_FILES
        asc_store.c
        asctime.c
        ctime.c
        ctime_r.c
        daylight_seconds.c
        difftime.c
        dst_pointer.c
        equation_of_time.c
        fatfs_time.c
        geo_location.c
        gm_sidereal.c
        gmtime.c
        gmtime_r.c
        isLeap.c
        iso_week_date.c
        iso_week_date_r.c
        isotime.c
        isotime_r.c
        lm_sidereal.c
        localtime.c
        localtime_r.c
        mk_gmtime.c
        mktime.c
        month_length.c
        moon_phase.c
        print_lz.c
        set_dst.c
        set_position.c
        set_system_time.c
        set_zone.c
        solar_declination.c
        solar_noon.c
        strftime.c
        sun_rise.c
        sun_set.c
        system_time.c
        time.c
        tm_store.c
        utc_offset.c
        week_of_month.c
        week_of_year.c)

include_directories(.)

#include_directories(${gtest_SOURCE_DIR}/include
#        ${gtest_SOURCE_DIR}
#        ${firmwarelibs_DIR}
##        ${avrlibc_DIR}/include
#        ${time_test_include}
#        ${test_includes_DIR})

#add_definitions(-DTESTING=1)

add_library(timeAvrLib ${SOURCE_FILES})

#target_link_libraries(runDateTimeTests gtest runUnitTests)
#        ${avrlibc_DIR}/lib/libc.a)


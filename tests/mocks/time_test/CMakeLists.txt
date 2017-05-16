cmake_minimum_required(VERSION 2.8)
#set(firmwarelibs_DIR ../../firmware/libs)
#set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
#set(test_includes_DIR ../includes)

#set(time_test_include ../mocks/time_test.h)
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

include_directories(../)

#include_directories(${gtest_SOURCE_DIR}/include
#        ${gtest_SOURCE_DIR}
#        ${firmwarelibs_DIR}
##        ${avrlibc_DIR}/include
#        ${time_test_include}
#        ${test_includes_DIR})

#add_definitions(-DTESTING=1)

add_library(timeTestLib ${SOURCE_FILES})

#target_link_libraries(runDateTimeTests gtest runUnitTests)
#        ${avrlibc_DIR}/lib/libc.a)


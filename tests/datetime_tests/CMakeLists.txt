cmake_minimum_required(VERSION 2.8)
set(firmwarelibs_DIR ../../firmware/libs)
#set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
set(test_includes_DIR ../includes)

set(time_test_include_DIR ../mocks/)
#set(time_test_src_DIR ../mocks/time_test)

include_directories(${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${firmwarelibs_DIR}
#        ${avrlibc_DIR}/include
        ${time_test_include_DIR}
        ${test_includes_DIR})

add_definitions(-DTESTING=1)

add_executable(runDateTimeTests
        ${firmwarelibs_DIR}/datetime/DateTime.cpp
        ${firmwarelibs_DIR}/datetime/Period.cpp
        ${firmwarelibs_DIR}/datetime/PeriodInterval.cpp
        ${firmwarelibs_DIR}/exception/CException.c
        ${firmwarelibs_DIR}/min/encode_decode.c
        datetime_tests.cpp
        interval_tests.cpp
        period_tests.cpp
        period_decompile_tests.cpp
        period_interval_tests.cpp
        period_interval_decompile_tests.cpp)

target_link_libraries(runDateTimeTests gtest gtest_main timeTestLib)
#        ${avrlibc_DIR}/lib/libc.a)


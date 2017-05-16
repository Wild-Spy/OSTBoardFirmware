cmake_minimum_required(VERSION 2.8)
set(firmwarelibs_DIR ../../firmware/libs)
set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
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
        datetime_basic.cpp
        ${time_test_src_files}
        ${firmwarelibs_DIR}/datetime/DateTime.cpp)

target_link_libraries(runDateTimeTests gtest gtest_main timeTestLib)
#        ${avrlibc_DIR}/lib/libc.a)


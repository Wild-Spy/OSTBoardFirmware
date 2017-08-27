cmake_minimum_required(VERSION 2.8)
set(firmwarelibs_DIR ../../firmware/libs)
#set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
set(test_includes_DIR ../includes)

set(mock_include_DIR ../mocks/)
#set(time_test_src_DIR ../mocks/time_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}
        ${firmwarelibs_DIR}
#        ${avrlibc_DIR}/include
        ${mock_include_DIR}
        ${test_includes_DIR})

add_definitions(-DTESTING=1)

add_executable(runSimplibTests
        ${firmwarelibs_DIR}/simplib/array.cpp
        ${firmwarelibs_DIR}/exception/CException.c
        array_tests.cpp)

target_link_libraries(runSimplibTests gtest gtest_main)
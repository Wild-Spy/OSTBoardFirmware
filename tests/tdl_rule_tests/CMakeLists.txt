cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)


set(firmwarelibs_DIR ../../firmware/libs)
#set(avrlibc_DIR ../lib/avr-libc-bin-2.0.0/avr)
set(test_includes_DIR ../includes)

set(mock_include_DIR ../mocks/)
#set(time_test_src_DIR ../mocks/time_test)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}
        ${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR}
        ${firmwarelibs_DIR}
#        ${avrlibc_DIR}/include
        ${mock_include_DIR}
        ${test_includes_DIR})

add_definitions(-DTESTING=1)
add_definitions(-DRULETESTING=1)

add_executable(runTdlRuleTests
        # Exercised Code
#        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlRule.cpp
        ${firmwarelibs_DIR}/min/encode_decode.c
        ${firmwarelibs_DIR}/exception/CException.c
        ${firmwarelibs_DIR}/datetime/Period.cpp
        ${firmwarelibs_DIR}/datetime/PeriodInterval.cpp
        ${firmwarelibs_DIR}/datetime/Interval.cpp
        ${firmwarelibs_DIR}/datetime/DateTime.cpp
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlRule.cpp

        # Mocks
        ../mocks/TdlRulesMock.cpp
        ../mocks/TdlEventsMock.cpp
#        ../mocks/PinMock.cpp
#        ../mocks/TdlChannelMock.cpp
#        ../mocks/TdlRuleMock.cpp
#        ../mocks/delayMock.cpp
#        ../mocks/NvmRuleManagerMock.cpp

        # Tests
        rule_tests.cpp
        )

target_link_libraries(runTdlRuleTests gmock gtest gtest_main timeTestLib)
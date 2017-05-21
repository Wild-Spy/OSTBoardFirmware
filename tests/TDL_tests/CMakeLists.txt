cmake_minimum_required(VERSION 2.8)
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

add_executable(runTdlTests
        # Exercised Code
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlChannels.cpp
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlChannel.cpp
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlActivator.cpp
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlAction.cpp
        ${firmwarelibs_DIR}/TimerDescriptionLanguage/TdlRules.cpp
        ${firmwarelibs_DIR}/min/encode_decode.c
        ${firmwarelibs_DIR}/exception/CException.c

        # Mocks
        ../mocks/PinMock.cpp
        ../mocks/TdlChannelMock.cpp
        ../mocks/TdlRuleMock.cpp
        ../mocks/delayMock.cpp
        ../mocks/RuleWriterMock.cpp

        # Tests
        tdl_channel_tests.cpp
        tdl_activator_tests.cpp
        tdl_action_tests.cpp
        tdl_action_decompile_tests.cpp
        channels_tests.cpp
        rules_tests.cpp
        )

target_link_libraries(runTdlTests gmock gtest gtest_main)
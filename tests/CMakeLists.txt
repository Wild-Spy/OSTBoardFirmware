cmake_minimum_required(VERSION 3.3)

project(OSTBoardFirmware_Tests)

add_subdirectory(lib/gtest-1.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(basic_tests)
add_subdirectory(datetime_tests)
add_subdirectory(min_tests)
add_subdirectory(simplib_tests)
add_subdirectory(TDL_tests)
add_subdirectory(nvm_rule_manager_tests)
add_subdirectory(rule_writer_rule_tests)
add_subdirectory(mocks/time_test)

#set(googletest_SOURCE_DIR lib/gtest-1.8.0/googletest/)
#set(googlemock_SOURCE_DIR lib/gtest-1.8.0/googlemock/)
#set(firmwarelibs_SOURCE_DIR ../firmware/libs)


#
#enable_testing()
#
#include_directories(${gtest_SOURCE_DIR}/include
#        ${gtest_SOURCE_DIR}
#        ${firmwarelibs_SOURCE_DIR}
#        includes)
#
##set(test_SRCS ADD ALL TEST SOURCE FILES HERE)
##add_executable(runUnitTests gtest.cpp ${test_SRCS})
#add_executable(runUnitTests )
#target_link_libraries(runUnitTests gtest gtest_main)
##add_test(runUnitTests runUnitTests) #included in all tutorials but I don't know what it actually does.
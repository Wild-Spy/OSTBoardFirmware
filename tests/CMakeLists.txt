cmake_minimum_required(VERSION 3.3)

add_subdirectory(lib/gtest-1.8.0 EXCLUDE_FROM_ALL)
add_subdirectory(basic_tests)
add_subdirectory(datetime_tests)
add_subdirectory(mocks/time_test)

set(googletest_SOURCE_DIR lib/gtest-1.8.0/googletest/)
set(googlemock_SOURCE_DIR lib/gtest-1.8.0/googlemock/)
set(firmwarelibs_SOURCE_DIR ../firmware/libs)

project(Test)

enable_testing()

include_directories(${gtest_SOURCE_DIR}/include
        ${gtest_SOURCE_DIR}
        ${firmwarelibs_SOURCE_DIR}
        includes)

#set(test_SRCS ADD ALL TEST SOURCE FILES HERE)
add_executable(runUnitTests gtest.cpp ${test_SRCS})
target_link_libraries(runUnitTests gtest datetime_tests)
#add_test(runUnitTests runUnitTests) #included in all tutorials but I don't know what it actually does.
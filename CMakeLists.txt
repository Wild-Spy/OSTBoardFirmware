cmake_minimum_required(VERSION 3.6)
project(OSTBoardFirmware)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp)
add_executable(OSTBoardFirmware ${SOURCE_FILES})
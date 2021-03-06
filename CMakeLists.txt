cmake_minimum_required(VERSION 3.8)
project(EnumStrings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
enable_testing()

add_executable(testEnumStrings test.cpp)
add_test(NAME testEnumStrings COMMAND testEnumStrings)

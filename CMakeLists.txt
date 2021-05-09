cmake_minimum_required(VERSION 3.8)
project(EnumStrings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
enable_testing()

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(testEnumStrings test.cpp)
add_test(NAME testEnumStrings COMMAND testEnumStrings)

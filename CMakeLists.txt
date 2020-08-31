cmake_minimum_required(VERSION 3.12.4)
project(cpp-zip-utils)

if (UNIX)
    set(CMAKE_CXX_COMPILER g++-10)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Werror")
endif()
set(CMAKE_CXX_STANDARD 20)

add_executable(tests tests.cpp)
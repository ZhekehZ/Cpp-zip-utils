cmake_minimum_required(VERSION 3.17)
project(Cpp_enumerate)

set(CMAKE_CXX_COMPILER g++-10)
set(CMAKE_CXX_STANDARD 20)

add_library(Cpp_enumerate enumerate.hpp)
add_executable(main main.cpp)
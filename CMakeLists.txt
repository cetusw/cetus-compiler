cmake_minimum_required(VERSION 4.0)
project(cetus_compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(vm)

add_executable(cetus_compiler
        vm/common.h
        vm/main.cpp
        vm/chunk.h
        vm/debug.cpp
        vm/debug.h
        vm/chunk.cpp)

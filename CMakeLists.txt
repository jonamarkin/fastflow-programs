cmake_minimum_required(VERSION 3.29)
project(ff_programs)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(ff_programs
    simplepipe.cpp)

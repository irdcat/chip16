cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(chip16)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)

include_directories(./include)

add_subdirectory(src)
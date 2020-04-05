cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

file(GLOB_RECURSE chip16_source_files LIST_DIRECTORIES true *.hpp *.cpp)
set(chip16_source_files ${chip16_source_files})
set(chip16_binary_basename ${CMAKE_PROJECT_NAME})

if(${BUILD_TEST})
    add_library(${chip16_binary_basename}_lib STATIC ${chip16_source_files})
else()
    set(chip16_source_files ${chip16_source_files} main.cpp)
    add_executable(${chip16_binary_basename} ${chip16_source_files})
endif()
cmake_minimum_required(VERSION 3.10)
project(errorcheck C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(errorcheck SHARED error.c linter.c)

if(WIN32)
    set_target_properties(errorcheck PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(errorcheck PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(errorcheck PROPERTIES SUFFIX ".so")
endif()

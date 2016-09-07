cmake_minimum_required(VERSION 3.3)
project(wojna)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SOURCE_FILES main.c structs.h)
add_executable(wojna ${SOURCE_FILES})
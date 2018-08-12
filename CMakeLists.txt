cmake_minimum_required(VERSION 3.5)

project(ffbmp_project)

add_library(ffbmp SHARED ffbmp.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -Wall -Wextra -pipe")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Og -ggdb -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3 -march=native -flto")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} -O3 -march=native -flto -DDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS} -Os -march=native -flto")

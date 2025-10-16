cmake_minimum_required(VERSION 4.0)
project(eventchain_test C)

set(CMAKE_C_STANDARD 99)

add_executable(eventchain_test dijkstra_benchmark.c
        eventchains.c
        eventchains.h)

cmake_minimum_required(VERSION 3.10)
project(eventchain_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

# Find math library (needed for sqrt, etc.)
find_library(MATH_LIBRARY m)

add_executable(eventchain_test multi_tier_benchmark.c
        eventchains.c
        eventchains.h)

# Link math library if found (Linux requires explicit -lm)
if(MATH_LIBRARY)
    target_link_libraries(eventchain_test ${MATH_LIBRARY})
endif()

# Set compile options for all executables
target_compile_options(eventchain_test PRIVATE
        -Wall
        -Wextra
        -O3
)
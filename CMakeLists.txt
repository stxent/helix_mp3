cmake_minimum_required(VERSION 3.13)
project(helix_mp3 C)

option(HELIX_MP3_LTO "Enable Link Time Optimization." OFF)
option(HELIX_MP3_STATIC "Enable static buffers for a decoder." OFF)
set(HELIX_MP3_C_FLAGS "" CACHE STRING "Platform dependent flags for C language used when building for all configurations.")

# Default compiler flags

if(HELIX_MP3_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILE_OPTIONS_IPO -flto=auto -ffat-lto-objects)
    endif()
endif()

add_subdirectory(fixpnt)

add_library(${PROJECT_NAME} $<TARGET_OBJECTS:helix_mp3_fixpnt>)
target_link_libraries(${PROJECT_NAME} PUBLIC helix_mp3_fixpnt)

cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0069 NEW)
cmake_policy(SET CMP0079 NEW)
project(helix_mp3 C)

option(HELIX_MP3_LTO "Enable Link Time Optimization." OFF)
option(HELIX_MP3_STATIC "Enable static buffers for a decoder." OFF)

# Default compiler flags

set(FLAGS_DEFAULT "-fdata-sections -ffunction-sections")
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${FLAGS_DEFAULT} ${FLAGS_CPU}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

if(HELIX_MP3_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_COMPILE_OPTIONS_IPO -flto -ffat-lto-objects)
    endif()
endif()

add_subdirectory(fixpnt)

add_library(${PROJECT_NAME} $<TARGET_OBJECTS:helix_mp3_fixpnt>)
target_link_libraries(${PROJECT_NAME} PUBLIC helix_mp3_fixpnt)

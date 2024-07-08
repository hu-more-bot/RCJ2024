cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(rcj2024)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
)

# Sources
add_subdirectory(main)
add_subdirectory(lib)
add_subdirectory(ttt)
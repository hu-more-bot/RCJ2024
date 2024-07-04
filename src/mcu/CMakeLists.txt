cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(rcj2024)

if(PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
)

# Sources
add_subdirectory(main)
add_subdirectory(dyn_test)
add_subdirectory(lib)
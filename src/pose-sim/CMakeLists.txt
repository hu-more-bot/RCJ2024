cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Source Files
set(SOURCES
    src/main.cpp

    src/pose.cpp
    src/server.cpp
)

# Libraries
set(LIBRARIES
    m
    glfw
    GL
)

# Project
project(pose-sim VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC src/include)
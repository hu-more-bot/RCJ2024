cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Project
project(pose-sim)

add_executable(${PROJECT_NAME} src/main.cpp src/pose.cpp)
target_link_libraries(${PROJECT_NAME} GL glfw)
target_include_directories(${PROJECT_NAME} PUBLIC src/include)
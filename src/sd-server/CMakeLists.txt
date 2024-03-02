cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Source Files
set(SOURCES
    main.cpp

    # UI
    src/window.cpp
    src/renderer.cpp

    # AI
    src/sd.cpp
  
    src/sd-server.cpp
    src/GL/glad.c
)

# Libraries
set(LIBRARIES
    # Common    
    m
    websockets
    
    # UI
    GL
    glfw

    # AI
    stable-diffusion
    ggml
)

# Project
project(sd-server VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(external)
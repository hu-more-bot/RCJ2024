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
    src/llm.cpp

    src/GL/glad.c
)

# Libraries
set(LIBRARIES
    # Common    
    m
    
    # UI
    GL
    glfw
    
    # AI
    llama
    common
    
    # whisper
)

# Project (Library)
project(RCJ2024 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(lib)
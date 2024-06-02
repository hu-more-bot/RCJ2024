cmake_minimum_required(VERSION 3.5)

# Source Files
set(SOURCES
    main.cpp

    # AI
    src/llm.cpp
    src/sd.cpp
  
    src/server.cpp
)

# Libraries
set(LIBRARIES
    llama
    common
    
    stable-diffusion
    
    ggml
)

# Project
project(backend VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(external)
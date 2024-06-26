cmake_minimum_required(VERSION 3.5)

# Source Files
set(SOURCES
    main.cpp

    # AI
    lib/src/llm.cpp
    lib/src/sd.cpp
  
    lib/src/server.c
)

# Libraries
set(LIBRARIES
    llama
    common
    
    stable-diffusion
    
    ggml

    axCommon # for logging
)

project(backend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

add_subdirectory(external)
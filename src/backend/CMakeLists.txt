cmake_minimum_required(VERSION 3.5)

# Source Files
set(SOURCES
    main.cpp

    # AI
    src/llm.cpp
    src/sd.cpp

    src/stt.cpp
    src/tts.cpp

    src/piper.cpp
  
    src/server.cpp
    src/serial.cpp
)

# Libraries
set(LIBRARIES
    llama
    common
    
    stable-diffusion

    whisper
    wcommon
    common-sdl
    SDL2
    SDL2_mixer

    piper_phonemize
    espeak-ng
    
    onnxruntime
    ggml

    axCommon # for logging
)

project(backend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_subdirectory(external)
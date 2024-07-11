cmake_minimum_required(VERSION 3.5)

option(RCJ_BACKEND_CUDA "build with CUDA support" OFF)

# Source Files
set(SOURCES
    main.cpp

    # AI
    src/llm.cpp
    # src/sd.cpp

    src/stt.cpp
    src/tts.cpp

    src/piper.cpp
  
    src/server.cpp
    src/serial.cpp
)

# Libraries
set(LIBRARIES
    # llm
    onnxruntime-genai
    onnxruntime

    # sd
    # stable-diffusion
    # ggml

    whisper
    # ggml
    common
    common-sdl
    SDL2
    SDL2_mixer

    # tts
    piper_phonemize
    espeak-ng

    # logging & audio
    axCommon
    axMixer
)

project(backend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_subdirectory(external)
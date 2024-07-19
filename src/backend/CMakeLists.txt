cmake_minimum_required(VERSION 3.5)

# Get Libaries
set(LIB_URL "https://github.com/hu-more-bot/RCJ2024/releases/download/RCJ24/lib-backend.tar.xz")
set(LIB_TAR_PATH "${CMAKE_BINARY_DIR}/dl/lib.tar.xz")
set(LIB_PATH "${CMAKE_BINARY_DIR}/lib")

if (NOT EXISTS ${LIB_TAR_PATH})
	file(DOWNLOAD ${LIB_URL} ${LIB_TAR_PATH})
endif()

add_custom_command(
	OUTPUT ${LIB_PATH}
	COMMAND tar -xf ${LIB_TAR_PATH}
	DEPENDS ${LIB_TAR_PATH})
add_custom_target(libs DEPENDS ${LIB_PATH})

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
)

# Libraries
set(LIBRARIES
    # llm
    onnxruntime-genai

    onnxruntime

    # sd
    stable-diffusion

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
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
add_dependencies(${PROJECT_NAME} libs)

target_include_directories(${PROJECT_NAME} PUBLIC external external/ggml external/oga external/ort external/piper external/whisper)
target_include_directories(${PROJECT_NAME} PUBLIC src/include)
target_link_directories(${PROJECT_NAME} PUBLIC ${LIB_PATH})

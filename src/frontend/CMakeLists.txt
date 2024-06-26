cmake_minimum_required(VERSION 3.5)

set(SOURCES
    main.cpp

    # src/tts.cpp
    lib/src/stt.c
    # lib/src/piper.cpp
    # lib/src/tts.cpp
    lib/src/client.c
    lib/src/yolo.c
)

set(LIBRARIES
    # Common
    m
    openal

    # AI
    espeak-ng
    piper_phonemize
    onnxruntime

    whisper
    # wcommon
    # common-sdl
    # SDL2

    # Artifex
    axCamera
    axRenderer
    axWindow
    axCommon
)

project(frontend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

add_subdirectory(external)
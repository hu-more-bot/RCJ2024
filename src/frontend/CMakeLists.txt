cmake_minimum_required(VERSION 3.5)

set(SOURCES
    main.cpp

    src/tts.cpp
    src/stt.cpp
    src/piper.cpp

    src/client.cpp

    src/log.cpp
)

set(LIBRARIES
    # Common    
    m
    websockets

    # AI
    espeak-ng
    onnxruntime
    piper_phonemize

    openal

    whisper
    wcommon
    common-sdl
    SDL2
)

project(frontend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(external)
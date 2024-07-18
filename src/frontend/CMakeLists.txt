cmake_minimum_required(VERSION 3.5)

set(SOURCES
    main.c

    src/yolo.c
    
    src/client.c
    src/serial.c
)

set(LIBRARIES
    m

    onnxruntime

    axCamera
    axRenderer
    axWindow
    axCommon
)

project(frontend)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_subdirectory(external)
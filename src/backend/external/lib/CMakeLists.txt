if (RCJ_BACKEND_CUDA)
    add_subdirectory(cuda)
else()
    add_subdirectory(cpu)
endif()

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ax)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/piper)
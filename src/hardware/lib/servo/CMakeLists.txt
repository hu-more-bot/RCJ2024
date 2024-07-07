add_library(servo servo.c)

target_link_libraries(servo hardware_pwm hardware_clocks)

target_include_directories(servo PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
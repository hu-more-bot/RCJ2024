add_executable(ttt main.c)

target_link_libraries(ttt pico_stdlib hardware_pwm servo)

pico_add_extra_outputs(ttt)

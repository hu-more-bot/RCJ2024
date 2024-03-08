add_executable(dt main.c)

target_link_libraries(dt pico_stdlib servo)

pico_enable_stdio_usb(dt 1)
pico_enable_stdio_uart(dt 0)

pico_add_extra_outputs(dt)

add_executable(main main.c src/base.c src/pose.c src/anim.c)

target_link_libraries(main pico_stdlib servo pico_stdlib pico_multicore)
target_include_directories(main PUBLIC "include")

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

pico_add_extra_outputs(main)

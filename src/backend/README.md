# Backend
The back-end program that runs the LLM and the Image Generation and controls the movement of the robot

## Hardware
This code runs on a [HP Victus something something]() Laptop which is connected to a [Raspberry Pi Pico]() via USB \
The two laptops are connected with a crossover LAN cable

## Software
The backend uses [llama.cpp](https://github.com/ggerganov/llama.cpp/tree/a0fc62661f0fd2a9edd10ae5617345bbbf972f42) for running the LLM and [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp) for generating images which both use the [GGML Library](https://github.com/ggerganov/ggml/tree/57869ad3b7b1f49ae18e3238b0d69a9467a8f068)[^1]. \
It uses a custom networking library built on top of the linux networking sockets to communicate with the frontend (laptop)

### Models
This program uses [Zephyr 7B Beta]() as for the LLM and [some sort of SD]() for image generation.

[^1]: The Stable Diffusion Library uses an old version of the GGML Library which made us use an old version of llama.cpp \
This has caused a lot of headaches during development :/
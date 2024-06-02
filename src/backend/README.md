# Backend
The back-end program that runs the LLM and the Image Generation

## Hardware
This code runs on a [HP Victus something something]() Laptop... and this is also the code that sends the pose info to the microcontroller for the hardware.

## Software
The backend uses [llama.cpp](https://github.com/ggerganov/llama.cpp/tree/a0fc62661f0fd2a9edd10ae5617345bbbf972f42) for running the LLM and [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp) for generating images which both use the [GGML Library](https://github.com/ggerganov/ggml/tree/57869ad3b7b1f49ae18e3238b0d69a9467a8f068)[^1]

### Models
This program uses [Zephyr 7B Beta]() as for the LLM and [some sort of SD]() for image generation.




## Appendix

**Definitions:** \
LLM: Large Language Model \
SD: Stable Diffusion

**Footnotes:** \
[^1]: The SD Library uses an old version of the GGML Library which made us use an old version of llama.cpp \
This has caused a lot of headaches during development :/
# Backend
This program runs the heavy AI models (LLM, SD, STT, TTS) and controls the movement of the robot

## Hardware
This code runs on a [HP Victus 16](https://www.hp.com/us-en/shop/mdp/victus/victus-16) Laptop featuring a powerful [NVIDIA RTX 3060](https://www.nvidia.com/en-eu/geforce/graphics-cards/30-series/rtx-3060-3060ti/) GPU

## Software
TODO

### Libraries
- LLM: [llama.cpp](https://github.com/ggerganov/llama.cpp/tree/a0fc62661f0fd2a9edd10ae5617345bbbf972f42)[^1]
- SD: [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)[^1]
- TTS: [piper](https://github.com/rhasspy/piper)
- STT: [whisper.cpp](https://github.com/ggerganov/whisper.cpp)

### Models
- LLM: [Zephyr 7B Beta](https://huggingface.co/SamTheDev/zephyr-7b-beta-4bit-gguf/resolve/main/zephyr-q4.gguf)
- SD: [Stable Diffusion XL Turbo](https://huggingface.co/stabilityai/sdxl-turbo)
- TTS: [Piper](https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/high/)/ryan
- STT: [Whisper](https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.en-q5_0.bin)

[^1]: Both the LLM & SD libraries use [GGML](https://github.com/ggerganov/ggml/tree/57869ad3b7b1f49ae18e3238b0d69a9467a8f068), but not with the same version. This has caused a lot of headache
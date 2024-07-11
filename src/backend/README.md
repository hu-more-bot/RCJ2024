# Backend
This program runs the heavy AI models (LLM, SD, STT, TTS) and controls the movement of the robot

## Hardware
This code runs on a [HP Victus 16](https://www.hp.com/us-en/shop/mdp/victus/victus-16) Laptop featuring a powerful [NVIDIA RTX 3060](https://www.nvidia.com/en-eu/geforce/graphics-cards/30-series/rtx-3060-3060ti/) GPU

## Software

### Setup

Init Git LFS
```bash
sudo apt install git-lfs
git lfs install
```

Clone Repo
```bash
git clone https://github.com/hu-more-bot/rcj2024
```

Install Required Packages:
```bash
sudo apt install libopenal1
```

### Libraries
- LLM: [ONNX Runtime GenAI](https://github.com/microsoft/onnxruntime-genai)
- SD: [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)
- TTS: [piper](https://github.com/rhasspy/piper)
- STT: [whisper.cpp](https://github.com/ggerganov/whisper.cpp)

### Models
- LLM: [Phi3](https://huggingface.co/microsoft/Phi-3-mini-4k-instruct-onnx)
- SD: [Stable Diffusion XL Turbo](https://huggingface.co/stabilityai/sdxl-turbo)
- TTS: [Piper](https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/high/)/ryan
- STT: [Whisper](https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.en-q5_0.bin)

### Communication
Sends:
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); generated image

Accepts:
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); picture of person
- **PERSON**: id (c[6]), position (f); person location
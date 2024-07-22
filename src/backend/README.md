# Backend
This program runs the heavy AI models (LLM, SD, STT, TTS) and controls the movement of the robot

## Hardware
This code runs on a HP ~~Victus~~ Omen 16 Laptop featuring a powerful NVIDIA RTX ~~3060~~ 4080 GPU

## Software

### Libraries
- LLM: [ONNX Runtime GenAI](https://github.com/microsoft/onnxruntime-genai)
- SD: [stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp)
- TTS: [piper](https://github.com/rhasspy/piper)
- STT: [whisper.cpp](https://github.com/ggerganov/whisper.cpp)
- Image Saving: [stb](https://github.com/nothings/stb/blob/master/stb_image_write.h)

### Models
- LLM: [Phi3](https://huggingface.co/microsoft/Phi-3-mini-4k-instruct-onnx)
- SD: [Stable Diffusion XL Turbo](https://huggingface.co/stabilityai/sdxl-turbo)
- TTS: [Piper](https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/high/)/ryan
- STT: [Whisper](https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.en-q5_0.bin)

### Communication
Sends:
- **IMREQ**: id (c[5]); image request
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); generated image
- **TEXT**:  id (c[4]), dur (f), len (u16), data (uc[len]); audio duration & generated text

Accepts:
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); picture of person
# Frontend
The front-end program that handles audio and manages the displays

## Software
The frontend uses [whisper.cpp]() to recognize speech and then the [Piper]() library to generate audio. To render to the screen, it uses my [Game Engine](https://github.com/samthedev32/Artifex)

[external/](external/) contains every external library binaries and includes, while \
[lib/](lib/) includes the libraries written for this project only

[models/](models/) has every AI model used and \
[res/](res/) is the folder that stores textures and other assets

### Libraries
Libraries used by this project:
- [Artifex](): my game engine that we use for rendering, capturing video and audio I/O
- [Onnxruntime](): an AI Runtime library from Microsoft
- [Piper](): a Neural Network-based TTS (Text-To-Speech) library
- [whisper.cpp](): a C++ runtime library for OpenAI's Whisper models by [ggerganov]()

## Hardware
This code runs on a [Dell Latitude 3140]() that is also the face of the robot
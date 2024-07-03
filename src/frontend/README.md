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

## Communication diagram
- Getting Text
  1. The frontend recognizes speech and sends it to the backend
  2. The backend generates the response and parses the commands
  3. The backend sends motion commands to the hardware and an image request to the frontend
  4. The backend sends the generated text to the frontend
  5. The frontend generates audio and plays it
- Getting Images
  1. The backend sends an image request (3.3)
  2. The frontend takes and crops an image
  3. The frontend sends the image to the backend
  4. The backend generates a new image
  5. The backend sends the image back to the frontend
  6. The frontend displays the image
- Turning
  1. The frontend calculates the person's position
  2. The frontend sends the position to the backend
  3. The backend forwards the position to the hardware
  4. The hardware moves the robot while correcting for the motion
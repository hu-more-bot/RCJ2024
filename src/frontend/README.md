# Frontend
This program runs the Object Detection and renders the images

## Hardware
This code runs on a [Dell Latitude 3140](https://www.dell.com/en-us/shop/cty/pdp/spd/latitude-11-3140-2-in-1-laptop/bto008l31402n1usvp?redirectTo=SOC&) that is also the face of the robot

## Software

### Libraries & Model(s)
- Rendering: [Artifex](https://github.com/samthedev32/Artifex)
- AI: [ONNX Runtime](https://onnxruntime.ai/) running [yolov7]() for object detection

### Communication
Sends:
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); cropped image *if detected & requested*

Accepts:
- **IMREQ**: id (c[5]); image request
- **IMAGE**: id (c[5]), width (u16), height (u16), channels (u8), data (uc[w * h * ch]); image to display
- **TEXT**:  id (c[4]), dur (f), len (u16), data (uc[len]); audio duration & generated text
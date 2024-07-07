# get models
rm -rf models
mkdir models

wget https://huggingface.co/SamTheDev/zephyr-7b-beta-4bit-gguf/resolve/main/zephyr-q4.gguf -O models/zephyr-q4.gguf

mkdir models/piper
wget https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/high/en_US-ryan-high.onnx -O models/piper/ryan.onnx
wget https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/ryan/high/en_US-ryan-high.onnx.json -O models/piper/ryan.onnx.json

wget https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-medium.en-q5_0.bin -O models/ggml-medium.en-q5_0.bin

wget https://huggingface.co/jiaowobaba02/stable-diffusion-v2-1-GGUF/resolve/main/stable_diffusion-ema-pruned-v2-1_768.q8_0.gguf -O models/sd.gguf
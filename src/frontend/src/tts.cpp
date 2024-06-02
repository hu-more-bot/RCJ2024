#include <tts.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

TTS::TTS(std::string model, std::string voice) {
    // tts.config.eSpeakDataPath = std::string(argv[2]);
    // auto outputPath = std::string(argv[3]);

    tts.config.eSpeakDataPath = model + "/espeak-ng-data";

    std::optional<piper::SpeakerId> speakerId;
    piper::loadVoice(tts.config, model + "/" + voice + ".onnx",
                     model + "/" + voice + ".onnx.json", tts.voice, speakerId,
                     false);

    piper::initialize(tts.config);
}

TTS::~TTS() { piper::terminate(tts.config); }

std::vector<int16_t> TTS::say(std::string text) {
    std::vector<int16_t> out;

    piper::SynthesisResult result;
    piper::textToAudio(tts.config, tts.voice, text, out, result, nullptr);

    return out;
}

bool TTS::sayToFile(std::string text, const char *output) {
    // Output audio to WAV file
    std::ofstream audioFile(output, std::ios::binary);

    if (!audioFile.is_open())
        return true;

    piper::SynthesisResult result;
    piper::textToWavFile(tts.config, tts.voice, text, audioFile, result);

    audioFile.close();

    return false;
}
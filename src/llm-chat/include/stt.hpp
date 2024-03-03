#pragma once

#include "whisper/whisper.h"

#include <string>

// Wrapper Class for the whisper.cpp library
class STT
{
public:
    STT(const char *model);
    ~STT();

    std::string listenBuf(std::vector<float> pcmf32);
    std::string listenWav(const char *path);

    struct language
    {
        std::string in = "auto";
        std::string out = "auto";
    };

private:
    struct whisper_context *ctx{};
};
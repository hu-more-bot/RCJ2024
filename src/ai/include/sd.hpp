#pragma once

#include <string>

namespace SD
{
    int txt2img(std::string model, std::string prompt, std::string out = "output.png");
    int img2img(std::string model, std::string prompt, std::string in, std::string out = "output.png");
}
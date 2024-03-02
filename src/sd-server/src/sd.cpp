#include <sd.hpp>

namespace SD
{
    std::string path = "~/rcj2024/src/ai/lib/sd";

    int txt2img(std::string model, std::string prompt, std::string out)
    {
        return std::system((std::string(path) + " -m " + model + " -p " + prompt + " -o " + out).c_str());
    }

    int img2img(std::string model, std::string prompt, std::string in, std::string out)
    {
        return std::system((std::string(path) + " -m " + model + " -p " + prompt + " -o " + out).c_str());
    }
}
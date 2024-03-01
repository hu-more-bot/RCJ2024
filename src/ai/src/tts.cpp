#include <tts.hpp>

namespace TTS
{
    std::string path = "~/rcj2024/src/ai/lib/piper";

    int generate(std::string model, std::string text, std::string out)
    {
        return std::system(("echo '" + text + "' | " + path + " --model " + model + " --output_file " + out).c_str());
    }
}
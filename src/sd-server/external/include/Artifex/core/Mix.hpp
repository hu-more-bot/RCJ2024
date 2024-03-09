#pragma once

#include <SDL2/SDL_mixer.h>

#include <cstdint>
#include <vector>

namespace Artifex {

class Engine;

// Mixer
struct Mix {
  public:
    // Initialize Mixer
    void init(Engine *pEngine);

    // DeInitialize Mixer
    void deinit();

    // Play Music
    void music(uint16_t id, int loops = 0);

    // Play Audio
    void audio(uint16_t id, int channel = -1, int loops = 0);

  private:
    bool initialized = false;
    Engine *engine = nullptr;
};

} // namespace Artifex
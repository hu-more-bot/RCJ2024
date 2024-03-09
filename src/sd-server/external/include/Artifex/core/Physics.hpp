#pragma once

#include <cstdint>

namespace Artifex {

class Engine;

class Physics {
  public:
    // Initialize Physics Engine
    void init(Engine *pEngine);

    // DeInitialize Loader
    void deinit();

    // Load Physics Shader
    uint16_t shader(const char *path);

  private:
    bool initialized = false;
    Engine *engine = nullptr;
};

} // namespace Artifex
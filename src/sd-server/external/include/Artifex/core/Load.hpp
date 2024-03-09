#pragma once

#include <Artifex/types/types.hpp>
#include <cstdint>

namespace Artifex {

class Engine;

enum class FILE_TYPE {
  AUTO,
  SHADER,
  IMAGE,
  AUDIO,
  MESH,
  FONT,
};

// Resource Manager for Artifex
struct Load {
public:
  // Initialize Loader
  void init(Engine *pEngine);

  // DeInitialize Loader
  void deinit();

  // TODO: asynchronous resource loading

  // Load Shader
  uint16_t shader(const char *vertex, const char *fragment, const char *geometry = "");
  uint16_t shader(const char *path, std::unordered_map<std::string, std::string> scripts = {});

  // Load Texture
  uint16_t texture(unsigned char *data, int width, int height, int channels);
  uint16_t texture(const char *path);

  // Load Mesh
  //    uint16_t mesh();
  uint16_t mesh(const char *path);

  // Load Audio
  uint16_t music(const char *path);
  uint16_t audio(const char *path);

  // Load Font
  //    uint16_t font();
  uint16_t font(const char *path);

  // Load Resource
  uint16_t load(const char *path, FILE_TYPE type = FILE_TYPE::AUTO);

private:
  bool initialized = false;
  Engine *engine = nullptr;
};

} // namespace Artifex
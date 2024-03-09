#pragma once

#include <Artifex/core/window.hpp>

#include <Artifex/core/Load.hpp>
#include <Artifex/core/Mix.hpp>
#include <Artifex/core/UI.hpp>
#include <Artifex/core/renderer.hpp>

#include <Artifex/types/types.hpp>

#include <cstdint>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace Artifex {

class Engine : public Window {
public:
  Engine(const std::string &title, const vec<2, uint32_t> &size);
  ~Engine();

  // Game Loop
  void loop(const vec<3> &clearColor = {}, void (*onUpdate)(float) = nullptr);

  // Update Engine & Window (for manual game loop)
  bool update(const vec<3> &clearColor);

  // Get Current Time (s)
  static float time();

  // Get Ratio of Window (width/height)
  inline float ratio() { return (float)size->width / (float)size->height; }

  // Register Callback Function
  void callback(void (*func)(int), void *userdata, int flags);

  // Add Module
  template <typename T> bool add(const std::string &name, uint32_t flags = 0);

  // Resource Loader
  Load load;

  // Basic Renderer
  Renderer render;

  // UI Renderer
  UI ui;

  // Audio Mixer
  Mix mix;

  // Selected Resources
  struct {
    uint16_t shader = 0;
    uint16_t font = 0;
  } current;

public:
  // Time when frame started
  float now = 0.0f;

  // Duration of Last Frame
  float deltaTime = 0.0f;

  // Resources
  struct {
    std::vector<Shader> shader;
    std::vector<Texture> texture;
    std::vector<GLuint> mesh;
    std::vector<Font> font;

    std::vector<Mix_Music *> music;
    std::vector<Mix_Chunk *> audio;
  } resource;

private:
  float past;

  std::unordered_map<std::string, Module *> module;
};

#include <type_traits>

template <typename T> bool Engine::add(const std::string &name, uint32_t flags) {
  static_assert(std::is_base_of<Module, T>::value, "Invalid Module");

  if (module.count(name)) {
    Log::warning("Engine::add", "Module with name '%s' already exists", name.c_str());
    return false;
  }

  module[name] = new T(*this, flags);

  if (!module[name]->onCreate()) {
    Log::warning("Engine::add"
                 "Failed to create module '%s'",
                 name.c_str());

    return false;
  }

  Log::verbose("Engine::add", "Added Module '%s'", name.c_str());
  return true;
}

} // namespace Artifex
#pragma once

#include <Artifex/types/types.hpp>

#include <string>
#include <unordered_map>

namespace Artifex {

class Engine;

struct UI {
public:
  // Initialize UI
  void init(Engine *pEngine);

  // DeInitialize UI
  void deinit();

  // Push Button
  bool button(vec<2> center, vec<2> size, std::string name, uint16_t a, uint16_t b);

  // Toggle Switch
  bool toggle(vec<2> center, vec<2> size, std::string name, uint16_t a, uint16_t b);

  // Touch
  bool touch(vec<2> center, vec<2> size, uint16_t a, uint16_t b);

  // Joystick
  // vec<2> joystick(vec<2> center = vec<2>(), std::string name
  // = "joystick",
  //               float radius = 0.2f, float nob_radius =
  //               0.04f, vec<3> color = vec<3>(1.0f, 0.4f, 0.0f),
  //               vec<3> nob_color = vec<3>(1.0f, 1.0f, 1.0f));

private:
  bool initialized = false;
  Engine *engine;

  std::unordered_map<std::string, int> input;
};

} // namespace Artifex
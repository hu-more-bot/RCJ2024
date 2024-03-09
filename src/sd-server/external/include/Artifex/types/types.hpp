#pragma once

#include <map>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "shader.hpp"
#include "module.hpp"

namespace Artifex {

class Engine;

// OpenGL Texture ID
struct Texture {
  int width, height, channels;

  GLuint id;
};

// Text Font
struct Font {
  Texture data;
  uint8_t rows, cols;

  vec<2> VERT[6];
  vec<2> UV[6];

  GLuint VAO, VBO;

  const struct {
    vec<2> pos, uv;
  } vertices[6] = {{{-1.0f, 1.0f}, {0.0f, 1.0f}}, {{-1.0f, -1.0f}, {0.0f, 0.0f}}, {{1.0f, -1.0f}, {1.0f, 0.0f}},
                   {{-1.0f, 1.0f}, {0.0f, 1.0f}}, {{1.0f, -1.0f}, {1.0f, 0.0f}},  {{1.0f, 1.0f}, {1.0f, 1.0f}}};
};

} // namespace Artifex
#pragma once

#include "window.hpp"

#include <cstdint>
#include <vector>

// Basic 2D Renderer
class Renderer : public Window {
public:
  Renderer(const std::string &name);
  ~Renderer();

  // Clear Screen
  static void clear(GLuint R = 0, GLuint G = 0, GLuint B = 0);

  // Add Mesh
  size_t mesh(std::vector<float> vertices, std::vector<uint32_t> indices);

  // Add Texture (Load into OpenGL)
  size_t add(unsigned char *data, int width, int height, int channels);

  // Draw Rect
  void draw(size_t id);

private:
  struct V {
    GLuint VAO, VBO, EBO;
    size_t size;
  };

  std::vector<V> v;

  GLuint shader;
  std::vector<GLuint> textures;
};
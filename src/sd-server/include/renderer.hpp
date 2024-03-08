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

  // Add Resources

  size_t mesh(std::vector<float> vertices, std::vector<uint32_t> indices);
  // size_t shader(std::string vertex, std::string fragment);
  size_t texture(unsigned char *data, int width, int height, int channels);

  // Draw Rect
  void draw(size_t vID, size_t tID);

private:
  struct V {
    GLuint VAO, VBO, EBO;
    size_t size;
  };

  GLuint shader;
  std::vector<V> v;
  // std::vector<GLint> s;
  std::vector<GLuint> t;
};
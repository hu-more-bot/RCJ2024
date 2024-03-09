#pragma once

#include <Artifex/core/window.hpp>
#include <Artifex/types/shader.hpp>

#include <cstdint>
#include <vector>

namespace Artifex {

// OpenGL Renderer
class Renderer : public Window {
public:
  Renderer(const std::string &name, const vec<2, uint32_t> &size);
  ~Renderer();

  // Clear Screen
  static void clear(vec<3> color);

  // Load Resource
  size_t load_shader(const char *vertex, const char *fragment,
                     const char *geometry = nullptr);
  size_t load_texture(void *data, const vec<2, uint32_t> &size,
                      uint8_t channels);
  size_t load_mesh(vec<2, float> *vertices, int vsize, uint32_t *indices,
                   int isize);

  // Primitive Shapes
  enum Shape {
    RECT = 0, // Simple Rectangle
    CIRCLE,   // Simple Circle
    TRIANGLE, // Simple Triangle
    CARD,     // Rectangle with Rounded Corners
  };

  // Render Something

  void draw(vec<2> center, vec<2> size, float rotation, int look, float corner,
            const vec<3> &color, size_t tex);
  // void draw_mesh(uint32_t id);
  // void draw_image(uint32_t id);
  // void draw_color(const vec<3> &color);

  //    void text(const vec<2> &center, float width);

  std::vector<GLuint> textures;

private:
  struct meshData {
    GLsizei size{};
    GLuint VAO{}, VBO{}, EBO{};
  };

  std::vector<Shader> shaders;
  std::vector<meshData> meshes;
};

} // namespace Artifex
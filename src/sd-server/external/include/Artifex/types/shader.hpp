#pragma once

#include <Artifex/math/mat.hpp>
#include <Artifex/math/vec.hpp>

#include <string>
#include <unordered_map>

#include <GL/glad.h>

namespace Artifex {

// OpenGL Shader Library for Artifex
struct Shader {
private:
  // Uniform Locations
  std::unordered_map<std::string, GLuint> uniforms;

  GLint get(const std::string& n);

public:
  explicit Shader(GLuint id = 0);

  // The Shader Program ID
  GLuint id;

  // Use This Shader
  void use() const;

  // 1D Values

  void set(const std::string& n, int value);
  void set(const std::string& n, unsigned int value);
  void set(const std::string& n, float value);
  void set(const std::string& n, double value);

  // 2D Values

  void set(const std::string& n, int x, int y);
  void set(const std::string& n, GLuint x, GLuint y);
  void set(const std::string& n, float x, float y);
  void set(const std::string& n, double x, double y);

  // 3D Values

  void set(const std::string& n, int x, int y, int z);
  void set(const std::string& n, GLuint x, GLuint y, GLuint z);
  void set(const std::string& n, float x, float y, float z);
  void set(const std::string& n, double x, double y, double z);

  // 4D Values

  void set(const std::string& n, int x, int y, int z, int w);
  void set(const std::string& n, GLuint x, GLuint y, GLuint z,
           GLuint w);
  void set(const std::string& n, float x, float y, float z, float w);
  void set(const std::string& n, double x, double y, double z,
           double w);

  // Vectors

  template <vec_t D, typename T>
  void set(const std::string& n, vec<D, T> vec);

  // Matrices

  template <mat_t R, mat_t C, typename T>
  void set(const std::string& n, mat<R, C, T> mat);
};

} // namespace Artifex
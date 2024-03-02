#pragma once

#include <GL/glad.h>
#include <GLFW/glfw3.h>

#include <set>
#include <string>

// Fullscreen OpenGL Window
class Window {
public:
  // Create Window
  Window(const std::string &name);

  // Destroy Window
  ~Window();

  // Update Window
  bool update();

  // Close Window
  void exit(bool sure = true) const;

  // Get Key State
  bool key(const std::string &k) const;

public:
  int width, height;

private:
  static void callback_resize(GLFWwindow *window, int w, int h);
  static void callback_key(GLFWwindow *window, int key, int scancode,
                           int action, int mods);

  GLFWwindow *window = nullptr;

  std::set<int> keyboard;
};
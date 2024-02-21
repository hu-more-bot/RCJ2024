#include <renderer.hpp>

int main() {
  Renderer renderer("asd");

  while (renderer.Window::update()) {
    if (renderer.key("esc"))
      renderer.exit();

    renderer.draw();
  }

  return 0;
}
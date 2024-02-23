#include <renderer.hpp>

#include <llm.hpp>
#include <sd.hpp>

int main()
{
  Renderer renderer("asd");

  // LLM llm("/home/samthedev/models/zephyr_q4.gguf", "The weebest thing is ");
  SD sd("/home/samthedev/models/sd_q4.gguf");

  while (renderer.Window::update())
  {
    if (renderer.key("esc"))
      renderer.exit();

    renderer.draw();
  }

  return 0;
}
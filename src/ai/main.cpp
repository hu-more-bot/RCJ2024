#include <renderer.hpp>

#include <llm.hpp>
#include <sd.hpp>

#include <thread>

// Generate Image
void sd_draw(std::string prompt)
{
  SD::txt2img("/home/sam/ai/sd-v1-4.ckpt", prompt);
}

int main()
{
  Renderer renderer("asd");

  std::thread render(sd_draw, "girl");

  // LLM llm("/home/samthedev/models/zephyr_q4.gguf", "The weebest thing is ");

  // while (renderer.Window::update())
  // {
  //   if (renderer.key("esc"))
  //     renderer.exit();

  //   if (render.joinable())
  //     renderer.draw();
  // }

  while (!render.joinable())
    ;

  render.detach();

  return 0;
}
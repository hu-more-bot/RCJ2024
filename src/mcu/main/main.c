#include <anim.h>
#include <base.h>

#include <pico/multicore.h>
#include <pico/stdio.h>

// Emergency Relay
#define EM_RELAY 28

#define MULTICORE_FLAG 1234

void core1() {
  // Wait for core0
  if (multicore_fifo_pop_blocking() != MULTICORE_FLAG) {
    while (1)
      ;
  }

  multicore_fifo_push_blocking(MULTICORE_FLAG);

  // core1 Loop
  while (1) {
    // TODO listen for commands
  }
}

int main() {
  stdio_init_all();

  // Launch 2nd core
  multicore_launch_core1(core1);

  multicore_fifo_push_blocking(MULTICORE_FLAG);

  if (multicore_fifo_pop_blocking() != MULTICORE_FLAG) {
    while (1)
      ;
  }

  // Arm Servos
  gpio_init(EM_RELAY);
  gpio_set_dir(EM_RELAY, GPIO_OUT);
  gpio_put(EM_RELAY, true);

  // pose pose_rest = {0.3, 0.0, 0.2, 1.0,  // left hand
  // 0.3, 0.0, 0.2, 1.0}; // right hand

  // Init Pose & Base
  anim_init(0, (anim_t){});
  base_init(8);

  while (true) {
    anim_set(0);

    // Update Arms
    anim_update();
  }

  // Disarm servos
  gpio_put(EM_RELAY, false);
}
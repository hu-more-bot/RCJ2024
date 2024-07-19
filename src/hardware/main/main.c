// Robot
#include "pose.h"

// PicoSDK
// #include <boards/pico.h>
#include <hardware/gpio.h>
// #include <pico/multicore.h>
// #include <pico/stdio.h>
#include <pico/time.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// Pin Definitions
#define RELAY 28
#define SWITCH 14
#define LED PICO_DEFAULT_LED_PIN

#define SERVO 0
#define STEPPER 8

const pose_t pose[] = {
    // rest
    {0.7, 0.3, 0.8, 0.8,  // left hand
     0.3, 0.0, 0.2, 0.0}, // right hand
                          // armsup
    {0.3, 0.0, 0.2, 0.5,  // left hand
     0.3, 0.0, 0.2, 0.5}, // right hand
};
const size_t poses = 1;

void blink(int pin);
// void core1();

int main() {
  // stdio_init_all();
  // multicore_launch_core1(core1);

  // Arm System
  gpio_init(RELAY);
  gpio_set_dir(RELAY, GPIO_OUT);
  gpio_put(RELAY, 0);

  gpio_init(SWITCH);
  gpio_set_dir(SWITCH, GPIO_IN);
  gpio_pull_up(SWITCH);

  gpio_init(LED);
  gpio_set_dir(LED, GPIO_OUT);
  gpio_put(LED, false);

  while (gpio_get(SWITCH))
    blink(LED);

  while (!gpio_get(SWITCH))
    gpio_put(LED, true);
  gpio_put(LED, false);

  gpio_put(RELAY, true);

  // Init Pose & Base
  pose_init(SERVO);
  pose_set(pose[0]);

  // // base_init(STEPPER);

  // // base_speed(700);
  // // base_step(1200, 1200);

  // // uint32_t state;
  while (true) {
    //   int swon = gpio_get(SWITCH);
    //   base_enable(swon, swon);

    //   // base_step(1, 1);

    pose_update();
  }

  // Disarm System
  gpio_put(RELAY, 0);
}

void blink(int pin) {
  gpio_put(pin, true);
  sleep_ms(100);
  gpio_put(pin, false);
  sleep_ms(100);
}

// void core1() {
//   while (1) {
//     char data[64];
//     if (scanf("%63s", data) == 1) {
//       char cmd[16];
//       int n;

//       if (sscanf(data, "%i%s", &n, cmd) == 2) {
//         if (!strcmp(cmd, "base")) {
//           multicore_fifo_drain();
//           multicore_fifo_push_blocking(n);
//           printf("base %i\n", n);
//         } else if (!strcmp(cmd, "pose")) {
//           if (n < poses) {
//             pose_set(pose[n]); // set pose
//             printf("Pose %i set\n", n);
//           } else {
//             printf("Pose %i does not exist\n", n);
//           }
//         }
//         // TODO animations
//       }
//     }
//   }
// }

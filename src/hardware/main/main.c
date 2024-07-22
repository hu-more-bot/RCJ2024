// Robot
#include "base.h"
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
    {0.3, 0.0, 0.25, 0.7,  // right hand
     0.3, 0.3, 0.25, 0.7}, // left hand
                           // armsup
    {0.5, 0.0, 0.25, 0.7,  // right hand
     0.3, 0.3, 0.25, 0.7}  // left hand
};
const size_t poses = sizeof(pose) / sizeof(*pose);

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
  base_init(STEPPER);

  base_speed(1000);
  base_step(1300 * 5.5, 1300 * 5.5);
  base_step(1300, -1300);
  sleep_ms(60 * 1000);
  base_step(1300, -1300);
  base_step(1300 * 5.5, 1300 * 5.5);
  while (1)
    ;
  // base_step(1600 * 2, 1600 * 2);
  // base_step(1600 * 2, -1600 * 2);

  // int state = 0;

  pose_init(SERVO);
  pose_set(pose[0]);

  uint32_t state;
  int t = 0;
  while (true) {
    int now = (int)(to_ms_since_boot(get_absolute_time()) / 1000.0f);
    if (now % 5 == 0 && t != now) {
      t = now;
      state = !state;
      pose_set(pose[state]);
    }

    // //   int swon = gpio_get(SWITCH);
    // //   base_enable(swon, swon);

    // //   // base_step(1, 1);

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

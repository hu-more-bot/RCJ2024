// SD Card
// #include "ff.h"
// #include "sd_card.h"
//
// #include "config.h"

// Robot
#include "boards/pico.h"
#include "hardware/gpio.h"
#include "pose.h"
#include <anim.h>
#include <base.h>

// PicoSDK
#include <pico/multicore.h>
#include <pico/stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pin Definitions
#define RELAY 28
#define SWITCH 14
#define LED PICO_DEFAULT_LED_PIN

#define SERVO 0
#define STEPPER 8

#define MULTICORE_FLAG 1234

float person = 0;

void blink(int pin);

void core1() {
  // Wait for core0
  if (multicore_fifo_pop_blocking() != MULTICORE_FLAG) {
    while (1)
      ;
  }

  multicore_fifo_push_blocking(MULTICORE_FLAG);

  // core1 Loop
  while (1) {
    // Read Person's location
    char line[32];
    memset(line, 0, 32);
    fgets(line, 32, stdin);
    printf("said: %s\n", line);

    // TODO parse line
    // sscanf(line, "%f", &person);

    // multicore_fifo_push_blocking(data);
  }
}

int main() {
  stdio_init_all();

  // Launch 2nd core
  multicore_launch_core1(core1);

  multicore_fifo_push_blocking(MULTICORE_FLAG);

  if (multicore_fifo_pop_blocking() != MULTICORE_FLAG) {
    while (1)
      blink(LED);
  }

  // Arm System
  gpio_init(RELAY);
  gpio_set_dir(RELAY, GPIO_OUT);
  gpio_put(RELAY, false);

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

  pose_t pose_rest = {0.3, 0.0, 0.2, 1.0,  // left hand
                      0.3, 0.0, 0.2, 1.0}; // right hand

  gpio_put(RELAY, true);

  // Init Pose & Base
  // anim_init(0, 0);
  pose_init(SERVO);
  base_init(STEPPER);

  // gpio_put(8, false);
  // gpio_put(9, true);
  // gpio_put(11, false);
  // gpio_put(12, true);

  // for (int i = 0; i < 1200; i++) {
  //   gpio_put(10, false);
  //   gpio_put(13, false);

  //   sleep_us(500);

  //   gpio_put(10, true);
  //   gpio_put(13, true);

  //   sleep_us(500);
  // }

  base_speed(700);
  base_step(1200, 1200);

  sleep_ms(500);

  // uint32_t state;
  while (true) {
    pose_set(pose_rest);
    pose_update();

    base_enable(0, 0);
    // base_step(1, 1);

    // anim_set(0);

    // Update Armsint leftint leftint left
    // anim_update();

    // base_step(10, 10);

    // if (multicore_fifo_rvalid())
    // multicore_fifo_pop_timeout_us(100, &state);
  }

  // Disarm System
  gpio_put(RELAY, false);
}

void blink(int pin) {
  gpio_put(pin, true);
  sleep_ms(100);
  gpio_put(pin, false);
  sleep_ms(100);
}
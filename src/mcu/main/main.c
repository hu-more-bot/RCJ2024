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

// SD Card
#include "config.h"
#include "ff.h"
#include "sd_card.h"

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
anim_t *loadAim(const char *path);

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
    // char line[32];
    // fgets(line, sizeof(line), stdin);

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

  // Init SD
  sd_init_driver();
  FATFS fs;
  if (f_mount(&fs, "0:", 1) != FR_OK) {
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
  // base_init(STEPPER);

  // uint32_t state;
  while (true) {
    pose_set(pose_rest);
    pose_update();
    // anim_set(0);

    // Update Arms
    // anim_update();

    // base_step(10, 10);

    // if (multicore_fifo_rvalid())
    // multicore_fifo_pop_timeout_us(100, &state);
  }

  f_unmount("0:");

  // Disarm System
  gpio_put(RELAY, false);
}

void blink(int pin) {
  gpio_put(pin, true);
  sleep_ms(100);
  gpio_put(pin, false);
  sleep_ms(100);
}

anim_t *loadAim(const char *path) {
  FIL f;
  unsigned int len;

  if (f_open(&f, path, FA_READ) != FR_OK) {
    return NULL;
  }

  // Read MAGIC
  char magic[4];

  if (f_read(&f, magic, 4, &len) != FR_OK || len != 4 ||
      strncmp(magic, "ANPO", 4)) {
    return NULL;
  }

  anim_t *out = malloc(sizeof(anim_t));

  // Read Poses
  if (f_read(&f, &out->poses, 1, &len) != FR_OK || len != 1) {
    free(out);
    return NULL;
  }

  if (f_read(&f, out->pose, out->poses * sizeof(*out->pose), &len) != FR_OK ||
      len != out->poses * sizeof(*out->pose)) {
    free(out);
    return NULL;
  }

  // Read Anims
  if (f_read(&f, &out->frames, 1, &len) != FR_OK || len != 1) {
    free(out);
    return NULL;
  }

  if (f_read(&f, out->frame, out->frames * sizeof(*out->frame), &len) !=
          FR_OK ||
      len != out->frames * sizeof(*out->frame)) {
    free(out);
    return NULL;
  }

  f_close(&f);

  return out;
}
#include <base.h>

#include "hardware/gpio.h"
#include "pico/time.h"

#include <math.h>
#include <stdlib.h>

// Global Start Pin
struct {
  int startPin;

  int speed;

  int isInit;
} _base = {0, 50, 0};

void base_init(int startPin) {
  if (_base.isInit)
    return;

  _base.startPin = startPin;

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) { // en, dir, pul
      gpio_init(startPin + i * 3 + j);
      gpio_set_dir(startPin + i * 3 + j, GPIO_OUT);
      gpio_put(startPin + i * 3 + j, true);
    }
  }

  _base.isInit = 1;
}

void base_enable(int left, int right) {
  gpio_put(_base.startPin + 0, !right);
  gpio_put(_base.startPin + 3, !left);
}

void base_speed(int us) { _base.speed = us; }

void base_step(int left, int right) {
  base_enable(left, right);

  // Set Dir
  gpio_put(_base.startPin + 1, right > 0);
  gpio_put(_base.startPin + 4, left > 0);

  // Step Motors
  for (int i = 0; i < MAX(abs(left), abs(right)); i++) {
    // Pulse
    if (abs(right) > i)
      gpio_put(_base.startPin + 2, true);

    if (abs(left) > i)
      gpio_put(_base.startPin + 5, true);

    sleep_us(_base.speed);

    gpio_put(_base.startPin + 2, false);
    gpio_put(_base.startPin + 5, false);

    sleep_us(_base.speed);
  }
}
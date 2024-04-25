#include <base.h>

#include "pico/time.h"
#include <stdlib.h>

// Global Start Pin
int m_startPin; // en pul dir x2
bool isInit = false;

void base_init(int startPin) {
  m_startPin = startPin;

  if (isInit) {
    // printf("base is already set up\n");
    return;
  }

  for (int i = 0; i < 2; i++) {
    gpio_set_dir(startPin + i * 3 + 0, GPIO_OUT); // en
    gpio_set_dir(startPin + i * 3 + 1, GPIO_OUT); // pul
    gpio_set_dir(startPin + i * 3 + 2, GPIO_OUT); // dir

    gpio_put(startPin + i * 3 + 0, false);
  }

  isInit = true;
}

void base_step(int left, int right) {
  // Enable / Disable
  gpio_put(m_startPin + 0, left != 0);
  gpio_put(m_startPin + 3, right != 0);

  // Set Dir
  gpio_put(m_startPin + 2, left > 0);
  gpio_put(m_startPin + 5, right > 0);

  // Step Motors
  for (int i = 0; i < MAX(abs(left), abs(right)); i++) {
    // Pulse
    if (abs(left) > i)
      gpio_put(m_startPin + 1, true);

    if (abs(right) > i)
      gpio_put(m_startPin + 4, true);

    sleep_us(50);

    gpio_put(m_startPin + 1, false);
    gpio_put(m_startPin + 4, false);

    sleep_us(50);
  }
}
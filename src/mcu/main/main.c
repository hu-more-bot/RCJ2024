#include "boards/pico.h"
#include "pico/stdlib.h"

// TODO: Listen on serial & drive motors

int main() {
  gpio_init(PICO_DEFAULT_LED_PIN);

  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  while (true) {

    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    sleep_ms(500);
  }
}

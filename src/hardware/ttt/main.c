#include <hardware/gpio.h>
#include <servo.h>

int main() {
  gpio_init(28);
  gpio_set_dir(28, GPIO_OUT);
  gpio_put(28, 1);

  servo_init(0);

  while (1)
    servo_setMillis(0, 700);

  return 0;
}
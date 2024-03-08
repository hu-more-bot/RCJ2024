#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/types.h"
#include "servo.h"
#include <stdio.h>

#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/pll.h"

int main() {
  stdio_init_all();

  servo_init(6);
  servo_init(2);

  float smooth, prev, target;
  smooth = prev = target = 1500;

  uint32_t last = 0, now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
  while (true) {
    last = now;
    now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
    // float deltaTime = now - last;

    // Update Servo (Smooth)
    smooth = (target * 0.0003) + (prev * 0.9997);
    prev = smooth;

    servo_setMillis(6, smooth);
    servo_setMillis(2, smooth - 500);
    printf("%.0f %.0f %.0f\n", target, prev, smooth);

    if ((int)(now * 2) % 2 == 0) {
      target = 1500;
      if ((int)now % 3 == 0) {
        target = 2600;
      }
    }
  }
}
#include "pico/stdlib.h"

#include "pico/time.h"
#include "servo.h"

// TODO: Listen on serial & drive motors

struct Servo {
  int pin;

  int angle;
  int smooth;
  int prev;

  int maxAngle;    // degrees
  float smoothing; // percentage
} servo[8];

struct Stepper {
  int EN, PUL, DIR;
} stepper[2];

int main() {
  /* Set Up Servos */ {
    int median[8] = {}; // resting points of servos
    const int start_pin = 0;
    for (int i = 0; i < 8; i++) {
      servo[i].pin = start_pin + i;
      servo[i].angle = servo[i].smooth = servo[i].prev = median[i];
      servo_init(servo[i].pin);

      // Set Constants
      servo[i].maxAngle = 270;   // 270 degrees
      servo[i].smoothing = 0.05; // 5%
    }
  }

  /* Set Up Steppers */ {
    const int start_pin = 8;
    for (int i = 0; i < 2; i++) {
      stepper[i].EN = start_pin + i * 3 + 0;
      stepper[i].PUL = start_pin + i * 3 + 1;
      stepper[i].DIR = start_pin + i * 3 + 2;

      gpio_set_dir(stepper[i].EN, GPIO_OUT);
      gpio_set_dir(stepper[i].PUL, GPIO_OUT);
      gpio_set_dir(stepper[i].DIR, GPIO_OUT);

      gpio_put(stepper[i].EN, false);
    }
  }

  float last = to_ms_since_boot(get_absolute_time()) / 1000.0, now = last;
  while (true) {
    last = now;
    now = to_ms_since_boot(get_absolute_time()) / 1000.0;
    float deltaTime = now - last;

    // Update Servos (Smooth)
    for (int i = 0; i < 8; i++) {
      servo[i].smooth =
          (servo[i].angle * servo[i].smoothing) +
          (servo[i].prev * (100.0 - servo[i].smoothing)) * deltaTime;
      servo[i].prev = servo[i].smooth;
      servo_setMillis(servo[i].pin,
                      ANGLE2MILLIS(servo[i].maxAngle, servo[i].smooth));
    }
  }
}

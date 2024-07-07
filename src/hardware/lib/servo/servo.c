#include "servo.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

float clockDiv = 64;
float wrap = 39062;

// Initalize Servo
void servo_init(int servoPin) {
  gpio_set_function(servoPin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(servoPin);

  pwm_config config = pwm_get_default_config();

  uint64_t clockspeed = clock_get_hz(5);
  clockDiv = 64;
  wrap = 39062;

  while (clockspeed / clockDiv / 50 > 65535 && clockDiv < 256)
    clockDiv += 64;
  wrap = clockspeed / clockDiv / 50;

  pwm_config_set_clkdiv(&config, clockDiv);
  pwm_config_set_wrap(&config, wrap);

  pwm_init(slice_num, &config, true);
}

// Set Servo Degrees (in millis)
void servo_setMillis(int servoPin, float millis) {
  pwm_set_gpio_level(servoPin, (millis / 20000.0) * wrap);
}

// Set Servo Angle (in micros)
void servo_setMicros(int servoPin, float micros) {
  pwm_set_gpio_level(servoPin, (micros / 20.0) * wrap);
}
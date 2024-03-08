#include "hardware/gpio.h"
#include "pico/platform.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "servo-stuff.h"

#define EM_RELAY 28

// int degs = 1600;
// while (true) {
//   servo_setMillis(0, degs); // ANGLE2MILLIS(270, degs));

//   printf("Degrees:\n");
//   scanf("%i", &degs);
//   fflush(stdin);
// }

struct Stepper {
  int EN, PUL, DIR;
} stepper[2];

// B-: green
// B+: yellow

// A-: red
// A+: blue

// TODO measure coil inductance

/*

int PUL=7; //define Pulse pin
int DIR=6; //define Direction pin
int ENA=5; //define Enable Pin
void setup() {
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);

}

void loop() {
  for (int i=0; i<6400; i++)    //Forward 5000 steps
  {
    digitalWrite(DIR,LOW);
    digitalWrite(ENA,HIGH);
    digitalWrite(PUL,HIGH);
    delayMicroseconds(50);
    digitalWrite(PUL,LOW);
    delayMicroseconds(50);
  }
  for (int i=0; i<6400; i++)   //Backward 5000 steps
  {
    digitalWrite(DIR,HIGH);
    digitalWrite(ENA,HIGH);
    digitalWrite(PUL,HIGH);
    delayMicroseconds(50);
    digitalWrite(PUL,LOW);
    delayMicroseconds(50);
  }
}
*/

/*

When “EN” is Low, the motor is in a free states(Off - line mode).In this mode,
    you can adjust the motor shaft position manually;
when “EN” is High(Vacant),
    the motor will be in an automatic control mode
        ."Direction" is the motor direction signal pin,
    "PULSE" is the motor pulse signal pin.Once the driver get a pulse,
    the motor move a step.

            */

int main() {
  stdio_init_all();

  // Servo Cut-Off Relay
  gpio_init(EM_RELAY);
  gpio_set_dir(EM_RELAY, GPIO_OUT);
  gpio_put(EM_RELAY, true);

  pose pose_rest = {0.3, 0.0, 0.2, 1.0,  // left hand
                    0.3, 0.0, 0.2, 1.0}; // right hand

  setupServos(servo, pose_rest);

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

  while (true) {
    gpio_put(stepper[0].DIR, false);
    gpio_put(stepper[0].EN, true);
    for (int i = 0; i < 6400; i++) // Forward 5000 steps
    {
      gpio_put(stepper[0].PUL, true);
      sleep_us(50);
      gpio_put(stepper[0].PUL, false);
      sleep_us(50);
    }

    gpio_put(stepper[0].DIR, true);
    gpio_put(stepper[0].EN, true);
    for (int i = 0; i < 6400; i++) // Backward 5000 steps
    {
      gpio_put(stepper[0].PUL, true);
      sleep_us(50);
      gpio_put(stepper[0].PUL, false);
      sleep_us(50);
    }
  }

  // struct Animation anim = (struct Animation){2, (uint32_t[2]){2000, 2000},
  //                                            (pose *){pose_rest, pose_rest},
  //                                            0};

  // anim.pose[0][1] = 0.0;
  // anim.pose[0][5] = 0.5;

  // anim.pose[0][2] = 0.2;
  // anim.pose[0][7] = 0.5;

  // anim.pose[0][3] = 1.0;
  // anim.pose[0][7] = 1.0;

  // anim.pose[1][1] = 0.5;
  // anim.pose[1][5] = 0.0;

  // anim.pose[1][2] = 0.5;
  // anim.pose[1][7] = 0.2;

  // anim.pose[1][3] = 0.5;
  // anim.pose[1][7] = 0.5;

  float last, now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
  while (true) {
    // Timing
    last = now;
    now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
    float deltaTime = now - last; // in seconds

    // Update Servos (Smooth)
    updateServos(servo);

    // if ((int)now % 2 == 0) {
    //   servo[1].value = 0.0;
    //   servo[5].value = 0.5;

    //   servo[2].value = 0.2;
    //   servo[7].value = 0.5;

    //   servo[3].value = 1.0;
    //   servo[7].value = 1.0;
    //   if ((int)now % 3 == 0) {
    //     servo[1].value = 0.5;
    //     servo[5].value = 0.0;

    //     servo[2].value = 0.5;
    //     servo[7].value = 0.2;

    //     servo[3].value = 0.5;
    //     servo[7].value = 0.5;
    //   }
    // }

    // animate(servo, anim);
  }

  gpio_put(EM_RELAY, false);
}
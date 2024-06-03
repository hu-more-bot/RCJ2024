#pragma once

#include "hardware/gpio.h"

// Initialize Base
void base_init(int startPin);

// Step Motors
void base_step(int left, int right);

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
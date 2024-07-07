#pragma once

#define MIN_PULSE_WIDTH 544  // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH 2400 // the longest pulse sent to a servo

/**
 * @brief Convert angle (degrees) to millis
 *
 * @param servo_max Max angle supported by servos
 * @param degrees Angle of servo
 *
 * @return PWM duty-cycle calculated for servo
 */
#define ANGLE2MILLIS(max, degrees)                                             \
  (MIN_PULSE_WIDTH + ((float)degrees / (float)max) * MAX_PULSE_WIDTH)

// Initialize Servo
void servo_init(int servoPin);

// Set Servo Degrees (in millis)
void servo_setMillis(int servoPin, float millis);

// Set Servo Angle (in micros)
void servo_setMicros(int servoPin, float micros);
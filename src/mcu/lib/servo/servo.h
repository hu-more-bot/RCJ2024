#pragma once

/**
 * @brief Convert angle (degrees) to millis
 *
 * @param servo_max Max angle supported by servos
 * @param degrees Angle of servo
 *
 * @return PWM duty-cycle calculated for servo
 */
#define ANGLE2MILLIS(max, degrees) (1.0 + (float)degrees / (float)max)

// Initialize Servo
void servo_init(int servoPin);

// Set Servo Degrees (in millis)
void servo_setMillis(int servoPin, float millis);

// Set Servo Angle (in micros)
void servo_setMicros(int servoPin, float micros);
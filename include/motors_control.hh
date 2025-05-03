#ifndef MOTORS_CONTROL_HH
#define MOTORS_CONTROL_HH
#include "main.hh"

void delta_steering(motors_t *motors, float delta);
void manual_motor_command(HardwareSerial &slave,
                          char *buf,
                          int16_t left, 
                          int16_t right);
#endif

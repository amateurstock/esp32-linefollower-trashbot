#include "motors_control.hh"
#include <cstdio>

#define SCALE 128

void delta_steering(motors_t *motors, float delta) {
    float degrees = 7.5f * delta;

    // Made sure that bot reaches max rotation since trigo is circular, not linear
    if (degrees > 90.0) degrees = 90.0;
    if (degrees < -90.0) degrees = -90.0;

    float radians = degrees * M_PI / 180.0;
    int x_comp = (int)(SCALE * cos(radians));
    int y_comp = (int)(SCALE * sin(radians));
    motors->left_motors = max(-SCALE , min(x_comp + y_comp, SCALE));
    motors->right_motors = max(-SCALE , min(x_comp - y_comp, SCALE));
}

void manual_motor_command(HardwareSerial &slave,
                          char *buf,
                          int16_t left, 
                          int16_t right) {
    sprintf(buf, "L:%d;R:%d;x", left, right);
    slave.print(buf);
}

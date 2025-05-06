#include "motors_control.hh"
#include <cstdio>

extern sensors_t sensors_state;

#define SCALE 100
#define STEERING_ASSIST 140
#define SIMPLE_STEERING

#ifndef SIMPLE_STEERING

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

#else

void delta_steering(motors_t *motors, float delta) {
    if (!sensors_state.line_state) {
        motors->left_motors = SCALE;
        motors->right_motors = SCALE;
        return;
    }
    if (delta < -1.0) {
        motors->left_motors = 0 - (SCALE + STEERING_ASSIST);
        motors->right_motors = 0 + (SCALE + STEERING_ASSIST);
    } else if (delta > 1.0) {
        motors->left_motors = 0 + (SCALE + STEERING_ASSIST);
        motors->right_motors = 0 - (SCALE + STEERING_ASSIST);
    }
}

void manual_motor_command(HardwareSerial &slave,
                          char *buf,
                          int16_t left,
                          int16_t right) {
    if (left < right) {
        sprintf(buf, "L:%d;R:%d;x", 
                0 - (SCALE + STEERING_ASSIST), 
                0 + (SCALE + STEERING_ASSIST)
                );
    } else if (right < left) {
        sprintf(buf, "L:%d;R:%d;x", 
                0 + (SCALE + STEERING_ASSIST), 
                0 - (SCALE + STEERING_ASSIST)
                );
    } else if ((left == 0) && (right == 0)){
        sprintf(buf, "L:%d;R:%d;x", 0, 0);
    } else {
        sprintf(buf, "L:%d;R:%d;x", SCALE, SCALE);
    }
}

#endif


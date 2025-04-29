#include "motors_control.hh"

#define SCALE 192

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

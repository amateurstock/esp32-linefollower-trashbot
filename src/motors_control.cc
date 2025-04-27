#include "motors_control.hh"

void delta_steering(motors_t *motors, float delta) {
    float degrees = 7.5f * delta;

    // Made sure that bot reaches max rotation since trigo is circular, not linear
    if (degrees > 90.0) degrees = 90.0;
    if (degrees < -90.0) degrees = -90.0;

    float radians = degrees * M_PI / 180.0;
    int x_comp = (int)(255 * cos(radians));
    int y_comp = (int)(255 * sin(radians));
    motors->left_motors = max(-255, min(x_comp + y_comp, 255));
    motors->right_motors = max(-255, min(x_comp - y_comp, 255));
}

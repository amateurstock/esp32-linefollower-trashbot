#include "main.hh"

// max error would be 6.0
// let's assume an error of +- 6.0
// reflects a +- 45 deg deflection
//
// therefore:
// for every 1.0 of error
// equates to 7.5 degrees of deflection
void delta_steering(motors_t *motors, float delta) {
    float degrees = 7.5f * delta;
}

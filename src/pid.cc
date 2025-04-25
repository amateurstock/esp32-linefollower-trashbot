#include "pid.hh"

PID::PID (
    float k_p,
    float k_i,
    float k_d,
    float setpoint
) {
    _k_p = k_p;
    _k_i = k_i;
    _k_d = k_d;
    _setpoint = setpoint;
}

float PID::calculate(
    float var,
    uint32_t current_time
) {
    float ret = 0.0f;
    float dt = get_dt(current_time);
    float error = _setpoint - var;

    // Proportional
    ret += _k_p * error;

    // Integral
    _integral += error * dt;
    ret += _k_i * _integral;

    // Differential
    _derivative = (error - _previous_error) / dt;
    ret += _k_d * _derivative;

    // For next iteration
    _previous_error = error;
    _previous_time = current_time;

    return ret;
}

float PID::get_dt(uint32_t current_time) {
    return (float) (current_time - _previous_time);
}

void PID::set_start_time(uint32_t current_time) {
    _previous_time = current_time;
}

#ifndef PID_HH
#define PID_HH

#include <Arduino.h>

class PID {
private:
    float _k_p = 0;
    float _k_i = 0;
    float _k_d = 0;
    float _setpoint = 0;
    float _previous_error = 0;
    float _integral = 0;
    float _derivative = 0;
    uint32_t _previous_time = 0;

    float get_dt(uint32_t current_time);

public:

    PID (
        float k_p,
        float k_i,
        float k_d,
        float setpoint
    );

    float calculate(
        float var,
        uint32_t current_time
    );

    void set_start_time(uint32_t current_time);
};

#endif

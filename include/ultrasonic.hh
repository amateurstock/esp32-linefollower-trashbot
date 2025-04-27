#ifndef ULTRASONIC_HH
#define ULTRASONIC_HH

#include <Arduino.h>

typedef enum {
    IDLE,
    WAIT_RISE,
    WAIT_FALL
} sonar_state_t;

class Ultrasonic {
public:
    uint8_t _trig_pin;
    uint8_t _echo_pin;
    uint32_t _max_distance;
    uint32_t _reading = UINT32_MAX;

    uint32_t _echo_start = 0;
    uint32_t _echo_end = 0;
    uint32_t _start_time = 0;
    uint32_t _last_time = 0;
    sonar_state_t _sonar = IDLE;

    void (*_isr_rise)(void);
    void (*_isr_fall)(void);

    Ultrasonic(
        uint8_t trig_pin,
        uint8_t echo_pin,
        uint32_t max_distance,
        void (*isr_rise)(void),
        void (*isr_fall)(void)
    );
};

void setup_ultrasonic_pins(Ultrasonic &ultrasonic);

void ultrasonic1_rise();
void ultrasonic1_fall();

void ultrasonic2_rise();
void ultrasonic2_fall();

void ultrasonic3_rise();
void ultrasonic3_fall();

#endif

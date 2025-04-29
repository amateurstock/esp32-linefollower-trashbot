#include "ultrasonic.hh"

extern Ultrasonic ultrasonic1;
extern Ultrasonic ultrasonic2;
extern Ultrasonic ultrasonic3;

Ultrasonic::Ultrasonic(
    uint8_t trig_pin,
    uint8_t echo_pin,
    uint32_t max_distance,
    void (*isr_rise)(void),
    void (*isr_fall)(void)
) {
    _trig_pin = trig_pin;
    _echo_pin = echo_pin;
    _max_distance = max_distance;
    _isr_rise = isr_rise;
    _isr_fall = isr_fall;
}

void setup_ultrasonic_pins(Ultrasonic &ultrasonic) {
    pinMode(ultrasonic._trig_pin, OUTPUT);
    pinMode(ultrasonic._echo_pin, INPUT);
    digitalWrite(ultrasonic._trig_pin, LOW);

    attachInterrupt(
        digitalPinToInterrupt(ultrasonic._echo_pin),
        ultrasonic._isr_rise,
        RISING);
    attachInterrupt(
        digitalPinToInterrupt(ultrasonic._echo_pin),
        ultrasonic._isr_fall,
        FALLING);
}

void poll_ultrasonic(Ultrasonic &ultrasonic) {
    switch (ultrasonic._state) {
        case IDLE: {
            ultrasonic._start_time = millis();

            digitalWrite(ultrasonic._trig_pin, LOW);
            delayMicroseconds(2);
            digitalWrite(ultrasonic._trig_pin, HIGH);
            delayMicroseconds(10);
            digitalWrite(ultrasonic._trig_pin, LOW);

            ultrasonic._state = WAIT_RISE;
            break;
        }
        case WAIT_RISE: {
            break;
        }
        case WAIT_FALL: {
            break;
        }
    }
}

uint32_t pull_ultrasonic(Ultrasonic &ultrasonic) {
    uint32_t duration = ultrasonic._echo_end - ultrasonic._echo_start;
    return (duration * 0.034 / 2);
}




// ++++++++++++++++++++++++++++++++++++++++++++++++++
// ISR Routines
// ==================================================

// ultrasonic1
void IRAM_ATTR ultrasonic1_rise() {
    ultrasonic1._echo_start = micros();
    ultrasonic1._state = WAIT_FALL;
}
void IRAM_ATTR ultrasonic1_fall() {
    ultrasonic1._echo_end = micros();
    ultrasonic1._state = IDLE;
    ultrasonic1._last_time = millis();
}

// ultrasonic2
void IRAM_ATTR ultrasonic2_rise() {
    ultrasonic2._echo_start = micros();
    ultrasonic2._state = WAIT_FALL;
}
void IRAM_ATTR ultrasonic2_fall() {
    ultrasonic2._echo_end = micros();
    ultrasonic2._state = IDLE;
    ultrasonic2._last_time = millis();
}

// ultrasonic3
void IRAM_ATTR ultrasonic3_rise() {
    ultrasonic3._echo_start = micros();
    ultrasonic3._state = WAIT_FALL;
}
void IRAM_ATTR ultrasonic3_fall() {
    ultrasonic3._echo_end = micros();
    ultrasonic3._state = IDLE;
    ultrasonic3._last_time = millis();
}

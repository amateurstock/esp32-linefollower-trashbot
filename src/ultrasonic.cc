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




// ++++++++++++++++++++++++++++++++++++++++++++++++++
// ISR Routines
// ==================================================

// ultrasonic1
void IRAM_ATTR ultrasonic1_rise() {
    ultrasonic1._echo_start = micros();
}
void IRAM_ATTR ultrasonic1_fall() {
    ultrasonic1._echo_end = micros();
}

// ultrasonic2
void IRAM_ATTR ultrasonic2_rise() {
    ultrasonic2._echo_start = micros();
}
void IRAM_ATTR ultrasonic2_fall() {
    ultrasonic2._echo_end = micros();
}

// ultrasonic3
void IRAM_ATTR ultrasonic3_rise() {
    ultrasonic3._echo_start = micros();
}
void IRAM_ATTR ultrasonic3_fall() {
    ultrasonic3._echo_end = micros();
}

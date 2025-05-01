#include "ultrasonic.hh"

constexpr uint32_t ULTRASONIC_TIMEOUT_MS = 100;

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
    uint32_t now = millis();
    switch (ultrasonic._state) {
        case IDLE: {
            ultrasonic._start_time = now;

            digitalWrite(ultrasonic._trig_pin, LOW);
            delayMicroseconds(2);
            digitalWrite(ultrasonic._trig_pin, HIGH);
            delayMicroseconds(10);
            digitalWrite(ultrasonic._trig_pin, LOW);

            ultrasonic._echo_start = micros();
            ultrasonic._state = WAIT_FALL;
            break;
        }
        case WAIT_RISE: {
            break;
        }
        case WAIT_FALL: {
            if (now - ultrasonic._start_time > ULTRASONIC_TIMEOUT_MS) {
                ultrasonic._state = IDLE;
                ultrasonic._reading = UINT32_MAX;
            }
            break;
        }
    }
}

uint32_t pull_ultrasonic(Ultrasonic &ultrasonic) {
    if (ultrasonic._state != IDLE) {
        return ultrasonic._reading;
    }
    Serial.printf("%d -> %d\n", ultrasonic1._echo_start, ultrasonic1._echo_end);
    uint32_t duration = ultrasonic._echo_end - ultrasonic._echo_start;
    ultrasonic._reading = (duration / 57.067) - 39;
    return ultrasonic._reading;
}




// ++++++++++++++++++++++++++++++++++++++++++++++++++
// ISR Routines
// ==================================================

// ultrasonic1
void IRAM_ATTR ultrasonic1_rise() {
    //if (ultrasonic1._state == WAIT_RISE) {
        ultrasonic1._echo_start = micros();
        ultrasonic1._state = WAIT_FALL;
    //}
}
void IRAM_ATTR ultrasonic1_fall() {
    if (ultrasonic1._state == WAIT_FALL) {
        ultrasonic1._echo_end = micros();
        ultrasonic1._state = IDLE;
        ultrasonic1._last_time = millis();
    }
}

// ultrasonic2
void IRAM_ATTR ultrasonic2_rise() {
    //if (ultrasonic2._state == WAIT_RISE) {
        ultrasonic2._echo_start = micros();
        ultrasonic2._state = WAIT_FALL;
    //}
}
void IRAM_ATTR ultrasonic2_fall() {
    //if (ultrasonic2._state == WAIT_FALL) {
        ultrasonic2._echo_end = micros();
        ultrasonic2._state = IDLE;
        ultrasonic2._last_time = millis();
    //}
}

// ultrasonic3
void IRAM_ATTR ultrasonic3_rise() {
    //if (ultrasonic3._state == WAIT_RISE) {
        ultrasonic3._echo_start = micros();
        ultrasonic3._state = WAIT_FALL;
    //}
}
void IRAM_ATTR ultrasonic3_fall() {
    //if (ultrasonic3._state == WAIT_FALL) {
        ultrasonic3._echo_end = micros();
        ultrasonic3._state = IDLE;
        ultrasonic3._last_time = millis();
    //}
}

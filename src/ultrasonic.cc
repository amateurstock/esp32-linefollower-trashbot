#include "ultrasonic.hh"

// Individual sonars shit
Ultrasonic::Ultrasonic(
    uint8_t trig_pin,
    uint8_t echo_pin,
    uint32_t max_distance
) {
    _trig_pin = trig_pin;
    _echo_pin = echo_pin;
    _max_distance = max_distance;

    pinMode(_trig_pin, OUTPUT);
    pinMode(_echo_pin, INPUT);

    digitalWrite(_trig_pin, LOW);
}

Ultrasonic::Ultrasonic(
    uint8_t trig_pin,
    uint8_t echo_pin
) {
    _trig_pin = trig_pin;
    _echo_pin = echo_pin;

    pinMode(_trig_pin, OUTPUT);
    pinMode(_echo_pin, INPUT);
}

uint32_t Ultrasonic::ping_distance_cm() {

}




// Arrays shit
void UltrasonicArray::ping_distances_cm() {
    switch (_select) {
        case SONAR1: {

            break;
        }
        case SONAR2: {

            break;
        }
        case SONAR3: {

            break;
        }
    }
}

uint32_t UltrasonicArray::return_distance(SonarSelect sonar) {
    uint32_t ret = 0;
    switch (sonar) {
        case SONAR1: ret = _sonar1_out; break;
        case SONAR2: ret = _sonar2_out; break;
        case SONAR3: ret = _sonar3_out; break;
        default: break;
    }

    return ret;
}

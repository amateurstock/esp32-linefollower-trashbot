#include "ultrasonic_array.hh"

UltrasonicArray::UltrasonicArray(uint8_t sharedTrig,
                                 uint8_t echo1,
                                 uint8_t echo2,
                                 uint8_t echo3) {
    _sharedTrig = sharedTrig;
    _echo1 = echo1;
    _echo2 = echo2;
    _echo3 = echo3;

    pinMode(_sharedTrig, OUTPUT);
    pinMode(_echo1, INPUT);
    pinMode(_echo2, INPUT);
    pinMode(_echo3, INPUT);

    digitalWrite(_sharedTrig, LOW);
}

void UltrasonicArray::getReadings(float out[3]) {
    switch (_turn) {
        
        case SENSOR1: {
            break;
        }

        case SENSOR2: {
            break;
        }

        case SENSOR3: {
            break;
        }

    }

    // Return output
    out[0] = _sensor1Reading;
    out[1] = _sensor2Reading;
    out[2] = _sensor3Reading;
}


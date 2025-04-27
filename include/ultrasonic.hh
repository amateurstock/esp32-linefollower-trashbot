#ifndef ULTRASONIC_HH
#define ULTRASONIC_HH

#include <Arduino.h>

class Ultrasonic {
private:
    uint8_t _trig_pin;
    uint8_t _echo_pin;
    uint32_t _max_distance = 100;

public:
    Ultrasonic(
        uint8_t trig_pin,
        uint8_t echo_pin,
        uint32_t max_distance
    );

    Ultrasonic(
        uint8_t trig_pin,
        uint8_t echo_pin
    );

    uint32_t ping_distance_cm();
};

class UltrasonicArray {
public:
    typedef enum {
        SONAR1 = 1,
        SONAR2,
        SONAR3
    }SonarSelect;

    UltrasonicArray(
        Ultrasonic sonar1,
        Ultrasonic sonar2,
        Ultrasonic sonar3
    );
    void ping_distances_cm();
    uint32_t return_distance(SonarSelect sonar);

private:
    Ultrasonic _sonar1;
    Ultrasonic _sonar2;
    Ultrasonic _sonar3;
    
    SonarSelect _select = SONAR1;

    // Just make it max so that the outside logic won't take "0" as a
    // signal to start doing something
    uint32_t _sonar1_out = UINT32_MAX;
    uint32_t _sonar2_out = UINT32_MAX;
    uint32_t _sonar3_out = UINT32_MAX;


};

#endif

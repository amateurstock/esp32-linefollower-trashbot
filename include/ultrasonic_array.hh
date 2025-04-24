#ifndef ULTRASONIC_ARRAY_HH
#define ULTRASONIC_ARRAY_HH

#include <Arduino.h>

/*
 * @brief: Ultrasonic array cuz this shit is so hard lmao. Three sensors only.
 * @param: Shared trigger pin
 * @param: Echo pin 1
 * @param: Echo pin 2
 * @param: Echo pin 3
 */
class UltrasonicArray {
private:
    enum Turn {
        SENSOR1 = 1,
        SENSOR2,
        SENSOR3
    };

    enum State {
        IDLE,
        WAIT_ECHO_RISE,
        WAIT_ECHO_FALL
    };

    uint8_t _sharedTrig;
    uint8_t _echo1;
    uint8_t _echo2;
    uint8_t _echo3;

    Turn _turn = SENSOR1;

    float _sensor1Reading = 0.0;
    float _sensor2Reading = 0.0;
    float _sensor3Reading = 0.0;

    State _sensor1State = IDLE;
    State _sensor2State = IDLE;
    State _sensor3State = IDLE;

public:
    UltrasonicArray(uint8_t sharedTrig,
                    uint8_t echo1,
                    uint8_t echo2,
                    uint8_t echo3);

    /*
     * @brief: Gets sensor readings from the sensor array
     * @param: float array output
     *
     * YOU MUST CREATE A FLOAT ARRAY OF SIZE 3 TO UTILIZE THIS
     */
    void getReadings(float out[3]);
};

#endif

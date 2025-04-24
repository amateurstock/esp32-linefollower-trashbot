#ifndef ULTRASONIC_HH
#define ULTRASONIC_HH

#include <Arduino.h>

class Ultrasonic {
private:
    enum ultrasonicState {
        IDLE,
        TRIG_HIGH,
        WAIT_ECHO_RISE,
        WAIT_ECHO_FALL
    };

    void beginTrigger();
    void update();
    bool isReady();

    uint8_t trigPin;
    uint8_t echoPin;
    ultrasonicState state = IDLE;
    uint32_t trigStart = 0;
    uint32_t echoStart = 0;
    uint32_t echoEnd = 0;
    uint32_t timeoutStart = 0;
    uint32_t timeout = 5000;
    uint32_t lastTriggerTime = 0;
    float distance = 0.0;

public:
    Ultrasonic(uint8_t TRIG_PIN, uint8_t ECHO_PIN);
    float pingCM();
};

#endif

#include <Arduino.h>


/*
 *  Class constructor for those using the
 *  L298N motor driver. One instance of a class
 *  is good for one motor only. Make multiple instances
 *  for multiple motors.
 *
 *  Example:
 *  // Motor with PWM control
 *  L298N leftMotor(0, 1, 2); 
 *
 *  // Motor with digital control
 *  L298N rightMotor(3, 4); 
 *
 */
class L298N {
private:
    bool isUsingPWM;
    uint8_t LOG_PIN_1;
    uint8_t LOG_PIN_2;
    uint8_t PWM_PIN;

public:
    typedef enum {
        STOP = 2,
        FORWARD = 1,
        BACKWARD = 0
    } MotorsType;

    L298N(
        uint8_t LOG_PIN_1,
        uint8_t LOG_PIN_2,
        uint8_t PWM_PIN
    );

    L298N(
        uint8_t LOG_PIN_1,
        uint8_t LOG_PIN_2
    );

    // Used with PWM
    void updateMotor(MotorsType dir, uint8_t value);

    // Used without PWM
    void updateMotor(MotorsType dir);

    // Universal
    void stopMotor(void);
};
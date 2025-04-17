#include "L298N.hh"

L298N::L298N(
        uint8_t LOG_PIN_1,
        uint8_t LOG_PIN_2,
        uint8_t PWM_PIN
    ): isUsingPWM(true) {
    this->PWM_PIN = PWM_PIN;
    this->LOG_PIN_1 = LOG_PIN_1;
    this->LOG_PIN_2 = LOG_PIN_2;

    pinMode(this->LOG_PIN_1, OUTPUT);
    pinMode(this->LOG_PIN_2, OUTPUT);
    pinMode(this->PWM_PIN, OUTPUT);
    digitalWrite(this->LOG_PIN_1, 0);
    digitalWrite(this->LOG_PIN_2, 0);
    analogWrite(this->PWM_PIN, 0);
}

L298N::L298N(
        uint8_t LOG_PIN_1,
        uint8_t LOG_PIN_2
    ): isUsingPWM(false), PWM_PIN(99) {
    this->LOG_PIN_1 = LOG_PIN_1;
    this->LOG_PIN_2 = LOG_PIN_2;

    pinMode(this->LOG_PIN_1, OUTPUT);
    pinMode(this->LOG_PIN_2, OUTPUT);
    digitalWrite(this->LOG_PIN_1, 0);
    digitalWrite(this->LOG_PIN_2, 0);
}

void L298N::updateMotor(MotorsType dir, uint8_t value) {
    if (!this->isUsingPWM) {
        Serial.println("You are using a digital-enabled motor with unexpected PWM value.");
        updateMotor(dir);
        return;
    }

    if (dir == STOP) {
        analogWrite(this->PWM_PIN, 0);
    } else {
        analogWrite(this->PWM_PIN, value);
        digitalWrite(this->LOG_PIN_1, (dir == FORWARD));
        digitalWrite(this->LOG_PIN_2, (dir == BACKWARD));
    }
}

void L298N::updateMotor(MotorsType dir) {
    if (this->isUsingPWM) {
        Serial.println("You are using a PWM-enabled motor without PWM value.");
        switch (dir) {
            case STOP: {
                updateMotor(dir, 0);
                break;
            }
            case FORWARD: {
                updateMotor(dir, 255);
                break;
            }
            case BACKWARD: {
                updateMotor(dir, 255);
                break;
            }
        }
        return;
    }

    switch (dir) {
        case STOP: {
            digitalWrite(this->LOG_PIN_1, 0);
            digitalWrite(this->LOG_PIN_2, 0);
            break;
        }
        case FORWARD: {
            digitalWrite(this->LOG_PIN_1, 1);
            digitalWrite(this->LOG_PIN_2, 0);
            break;
        }
        case BACKWARD: {
            digitalWrite(this->LOG_PIN_1, 0);
            digitalWrite(this->LOG_PIN_2, 1);
            break;
        }
    }
}

void L298N::stopMotor() {
    if (isUsingPWM) {
        updateMotor(STOP, 0);
    } else {
        updateMotor(STOP);
    }
}

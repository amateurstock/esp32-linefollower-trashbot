#include "ultrasonic.hh"

Ultrasonic::Ultrasonic(uint8_t TRIG_PIN, uint8_t ECHO_PIN) {
    this->trigPin = TRIG_PIN;
    this->echoPin = ECHO_PIN;
    pinMode(this->trigPin, OUTPUT);
    pinMode(this->echoPin, INPUT);
    digitalWrite(this->trigPin, LOW);
}

void Ultrasonic::beginTrigger() {
    if (this->state == IDLE) {
        digitalWrite (this->trigPin, HIGH);
        trigStart = micros();
        this->state = TRIG_HIGH;
    }
}

void Ultrasonic::update() {
    switch (this->state) {
        case TRIG_HIGH: {
            if (micros() - trigStart >= 10) {
                digitalWrite(this->trigPin, LOW);
                this->state = WAIT_ECHO_RISE;
                this->timeoutStart = micros();
            }
            break;
        }

        case WAIT_ECHO_RISE: {
            if (digitalRead(this->echoPin) == HIGH) {
                echoStart = micros();
                this->state = WAIT_ECHO_FALL;
            } else if (micros() - this->timeoutStart >= timeout) {
                this->distance = 0;
                this->state = IDLE;
            }
            break;
        }

        case WAIT_ECHO_FALL: {
            if (digitalRead(this->echoPin) == LOW) {
                this->echoEnd = micros();
                this->distance = (this->echoEnd - this->echoStart) * 0.034 / 2.0;
                this->state = IDLE;
            } else if (micros() - this->echoStart >= timeout) {
                this->distance = 0;
                this->state = IDLE;
            }
            break;
        }

        default: {

            break;
        }
    }
}

bool Ultrasonic::isReady() {
    return (this->state == IDLE);
}

float Ultrasonic::pingCM() {
    update();

    if (millis() - this->lastTriggerTime >= 50) {
        lastTriggerTime = millis();
        beginTrigger();
    }

    return this->distance;
}

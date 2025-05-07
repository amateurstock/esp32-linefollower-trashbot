#include "motors_control.hh"
#include <cstdio>

extern sensors_t sensors_state;

#define SCALE 80
#define STEERING_ASSIST 170
//#define SIMPLE_STEERING
#define BETTER_STEERING
//#define PID_STEERING

//#define PROPER_MANUAL

#ifdef PID_STEERING
void delta_steering(motors_t *motors, float delta) {
    float degrees = 7.5f * delta;

    // Made sure that bot reaches max rotation since trigo is circular, not linear
    if (degrees > 90.0) degrees = 90.0;
    if (degrees < -90.0) degrees = -90.0;

    float radians = degrees * M_PI / 180.0;
    int x_comp = (int)(SCALE * cos(radians));
    int y_comp = (int)(SCALE * sin(radians));
    motors->left_motors = max(-SCALE , min(x_comp + y_comp, SCALE));
    motors->right_motors = max(-SCALE , min(x_comp - y_comp, SCALE));
}

#endif

#ifdef SIMPLE_STEERING
void delta_steering(motors_t *motors, float delta) {
    if (!sensors_state.line_state) {
        motors->left_motors = SCALE;
        motors->right_motors = SCALE;
        return;
    }
    if (delta < -1.0) {
        motors->left_motors = 0 - (SCALE + STEERING_ASSIST);
        motors->right_motors = 0 + (SCALE + STEERING_ASSIST);
    } else if (delta > 1.0) {
        motors->left_motors = 0 + (SCALE + STEERING_ASSIST);
        motors->right_motors = 0 - (SCALE + STEERING_ASSIST);
    }
}
#endif

#ifdef BETTER_STEERING

void delta_steering(motors_t *motors, float delta) {
    switch (sensors_state.line_state) {

        case 0b0001: {
            motors->left_motors = -SCALE - STEERING_ASSIST;
            motors->right_motors = SCALE + STEERING_ASSIST;
            break;
        }

        case 0b0011: {
            motors->left_motors = -STEERING_ASSIST;
            motors->right_motors = SCALE + STEERING_ASSIST;
            break;
        }

        case 0b0010: {
            motors->left_motors = 0;
            motors->right_motors = SCALE + STEERING_ASSIST;
            break;
        }

        case 0b0100: {
            motors->left_motors = SCALE + STEERING_ASSIST;
            motors->right_motors = 0;
            break;
        }
        
        case 0b1100: {
            motors->left_motors = SCALE + STEERING_ASSIST;
            motors->right_motors = -STEERING_ASSIST;
            break;
        }

        case 0b1000: {
            motors->left_motors = SCALE + STEERING_ASSIST;
            motors->right_motors = -SCALE - STEERING_ASSIST;
            break;
        }

        default: {
            motors->left_motors = SCALE;
            motors->right_motors = SCALE;
            break;
        }

    }
}

#endif

#ifdef PROPER_MANUAL

void manual_motor_command(HardwareSerial &slave,
                          char *buf,
                          int16_t left, 
                          int16_t right) {
    sprintf(buf, "L:%d;R:%d;x", left, right);
    slave.print(buf);
}

#else

void manual_motor_command(HardwareSerial &slave,
                          char *buf,
                          int16_t left,
                          int16_t right) {
    if (left < right) {
        sprintf(buf, "L:%d;R:%d;x", 
                0 - (SCALE + STEERING_ASSIST), 
                0 + (SCALE + STEERING_ASSIST)
                );
    } else if (right < left) {
        sprintf(buf, "L:%d;R:%d;x", 
                0 + (SCALE + STEERING_ASSIST), 
                0 - (SCALE + STEERING_ASSIST)
                );
    } else if ((left == 0) && (right == 0)){
        sprintf(buf, "L:%d;R:%d;x", 0, 0);
    } else {
        sprintf(buf, "L:%d;R:%d;x", left, right);
    }

    slave.print(buf);
}

#endif

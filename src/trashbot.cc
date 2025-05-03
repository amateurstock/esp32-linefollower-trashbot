#include "trashbot.hh"
#include <cstdio>

const char *check_trash_obstacle_tag = "check_trash_obstacle";
const char *trash_collection_tag = "trash_collection";
const char *obstacle_avoidance_tag = "obstacle_avoidance";

extern sensors_t sensors_state;
extern motors_t motors;
extern lftb_mode_t current_mode;
extern HardwareSerial uno_serial;
extern uint16_t INIT_WAIT_TIME;

constexpr uint8_t TRASH_THRESHOLD = 28;
constexpr uint8_t OBSTACLE_THRESHOLD = 40;
constexpr uint8_t AVOIDANCE_TOLERANCE = 4;

void check_trash_obstacle(void *params) {
    user_logger(check_trash_obstacle_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        if (current_mode == LINE_FOLLOWER) {
            if (sensors_state.distance_1 <= TRASH_THRESHOLD || 
                sensors_state.distance_2 <= TRASH_THRESHOLD) {
                current_mode = TRASH_COLLECTION;
            }
            if (is_within_limits(sensors_state.distance_3,
                                 OBSTACLE_THRESHOLD,
                                 AVOIDANCE_TOLERANCE)) {
                current_mode = OBSTACLE_AVOIDANCE;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void trash_collection(void *params) {
    user_logger(trash_collection_tag, "Waiting for the rest to init.");
    trashbot_fsm_t state_machine = TRASH_INITIAL;
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        if (current_mode == TRASH_COLLECTION) {
            switch (state_machine) {
                case TRASH_INITIAL: {
                    state_machine = TRASH_WAIT_STOP;
                    break;
                }
                case TRASH_WAIT_STOP: {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    motors.servo_out1 = 0;
                    motors.servo_out2 = 135;
                    motors.servo_out3 = 45;
                    motors.servo_out4 = 180;
                    state_machine = TRASH_WAIT_DOWN;
                    break;
                }
                case TRASH_WAIT_DOWN: {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    motors.servo_out2 = 75;
                    motors.servo_out3 = 115;
                    state_machine = TRASH_WAIT_PICK;
                    break;
                }
                case TRASH_WAIT_PICK: {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    motors.servo_out1 = 105;
                    motors.servo_out4 = 75;
                    state_machine = TRASH_WAIT_UP;
                    break;
                }
                case TRASH_WAIT_UP: {
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    motors.servo_out1 = 105;
                    motors.servo_out2 = 135;
                    motors.servo_out3 = 45;
                    motors.servo_out4 = 75;
                    state_machine = TRASH_WAIT_REST;
                    break;
                }
                case TRASH_WAIT_REST: {
                    vTaskDelay(pdMS_TO_TICKS(500));
                    state_machine = TRASH_INITIAL;
                    current_mode = LINE_FOLLOWER;
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void obstacle_avoidance(void *params) {
    user_logger(obstacle_avoidance_tag, "Waiting for the rest to init.");
    obstacle_fsm_t state_machine = OBS_INITIAL;
    char buf[32] = {0};
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        if (current_mode == OBSTACLE_AVOIDANCE) {
            switch (state_machine) {
                case OBS_INITIAL: {
                    state_machine = OBS_WAIT_STOP1;
                    motors.left_motors = 0;
                    motors.right_motors = 0;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    break;
                }
                case OBS_WAIT_STOP1: {
                    vTaskDelay(pdMS_TO_TICKS(500));

                    motors.left_motors = -128;
                    motors.right_motors = 128;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    vTaskDelay(pdMS_TO_TICKS(250));

                    motors.left_motors = 128;
                    motors.right_motors = 128;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    state_machine = OBS_WAIT_SWERVE_LEFT;
                    break;
                }
                case OBS_WAIT_SWERVE_LEFT: {
                    vTaskDelay(pdMS_TO_TICKS(1000));

                    motors.left_motors = 0;
                    motors.right_motors = 0;
                    state_machine = OBS_WAIT_STOP2;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    break;
                }
                case OBS_WAIT_STOP2: {
                    vTaskDelay(pdMS_TO_TICKS(500));

                    motors.left_motors = 128;
                    motors.right_motors = -128;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    vTaskDelay(pdMS_TO_TICKS(250));

                    motors.left_motors = 128;
                    motors.right_motors = 128;
                    sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                    uno_serial.print(buf);
                    state_machine = OBS_WAIT_SWERVE_RIGHT;
                    break;
                }
                case OBS_WAIT_SWERVE_RIGHT: {
                    vTaskDelay(pdMS_TO_TICKS(1000));

                    motors.left_motors = 0;
                    motors.right_motors = 0;
                    state_machine = OBS_WAIT_LINE;
                    break;
                }
                case OBS_WAIT_LINE: {
                    if (sensors_state.line_state) {
                    }
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

bool is_within_limits(uint32_t test_num,
                      uint32_t threshold,
                      uint32_t tolerance) {
    return ((test_num >= (threshold - tolerance)) && (test_num < (threshold + tolerance)));
}

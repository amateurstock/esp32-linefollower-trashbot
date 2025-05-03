#include "trashbot.hh"

const char *check_trash_obstacle_tag = "check_trash_obstacle";
const char *trash_collection_tag = "trash_collection";
const char *obstacle_avoidance_tag = "obstacle_avoidance";

extern sensors_t sensors_state;
extern lftb_mode_t current_mode;
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
                    break;
                }
                case TRASH_WAIT_DOWN: {
                    break;
                }
                case TRASH_WAIT_PICK: {
                    break;
                }
                case TRASH_WAIT_UP: {
                    break;
                }
                case TRASH_WAIT_REST: {
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

void obstacle_avoidance(void *params) {
    user_logger(obstacle_avoidance_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        if (current_mode == OBSTACLE_AVOIDANCE) {
        }

        vTaskDelay(pdMS_TO_TICKS(15));
    }
}

bool is_within_limits(uint32_t test_num,
                      uint32_t threshold,
                      uint32_t tolerance) {
    return ((test_num >= (threshold - tolerance)) && (test_num < (threshold + tolerance)));
}

#include "trashbot.hh"

const char *check_trash_obstacle_tag = "check_trash_obstacle";
extern sensors_t sensors_state;

constexpr uint8_t TRASH_THRESHOLD = 50;
constexpr uint8_t OBSTACLE_THRESHOLD = 100;

void check_trash_obstacle(void *params) {
    user_logger(check_trash_obstacle_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(2000));
    for (;;) {
        if (sensors_state.distance_1 <= TRASH_THRESHOLD ||
            sensors_state.distance_2 <= TRASH_THRESHOLD) {
            trash_colletion();
            continue;
        }
        vPortYield();

        if (sensors_state.distance_3 <= OBSTACLE_THRESHOLD) {
            obstacle_avoidance();
        }
        vPortYield();

        vTaskDelay(10);
    }
}

void trash_colletion(void) {
}

void obstacle_avoidance(void) {
}

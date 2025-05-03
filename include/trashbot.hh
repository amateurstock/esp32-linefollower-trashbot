#ifndef TRASHBOT_HH
#define TRASHBOT_HH

#include "main.hh"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef enum {
    LINE_FOLLOWER,
    TRASH_COLLECTION,
    OBSTACLE_AVOIDANCE
} lftb_mode_t;

typedef enum {
    TRASH_INITIAL,
    TRASH_WAIT_STOP,
    TRASH_WAIT_DOWN,
    TRASH_WAIT_PICK,
    TRASH_WAIT_UP,
    TRASH_WAIT_REST
} trashbot_fsm_t;

typedef enum {
    OBS_INITIAL,
    OBS_WAIT_STOP1,
    OBS_WAIT_SWERVE_LEFT,
    OBS_WAIT_STOP2,
    OBS_WAIT_SWERVE_RIGHT,
    OBS_WAIT_LINE
} obstacle_fsm_t;

void check_trash_obstacle(void *params);
void trash_collection(void *params);
void obstacle_avoidance(void *params);

// Miscellaneous
bool is_within_limits(uint32_t test_num,
                      uint32_t threshold,
                      uint32_t tolerance);

#endif

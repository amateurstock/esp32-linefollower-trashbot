#ifndef TRASHBOT_HH
#define TRASHBOT_HH

#include "main.hh"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void check_trash_obstacle(void *params);
void trash_colletion(void);
void obstacle_avoidance(void);

// Miscellaneous
bool is_within_limits(uint32_t test_num,
                      uint32_t threshold,
                      uint32_t tolerance);

#endif

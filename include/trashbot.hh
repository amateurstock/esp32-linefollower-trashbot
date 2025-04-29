#ifndef TRASHBOT_HH
#define TRASHBOT_HH

#include "main.hh"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void check_trash_obstacle(void *params);
void trash_colletion(void);
void obstacle_avoidance(void);

#endif

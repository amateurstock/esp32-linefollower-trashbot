#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void init_tasks_demo();
static void beep(void *params);
static void doot(void *params);
void log_stuff(const char *TAG, char *message);

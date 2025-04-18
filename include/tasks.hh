#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void init_tasks();
void init_serial();
void user_logger(const char *TAG, char *message);
void read_sensors(void *params);
void update_motors(void *params);
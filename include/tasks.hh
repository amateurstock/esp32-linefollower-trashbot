#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// RTOS related
void init_tasks();
void read_line_sensors(void *params);
void update_motors(void *params);
void get_distances(void *params);

// Inits, debugs, comms, etc.
void init_serial();
void init_pins();
void user_logger(const char *TAG, char *message);

// Pin names
// |--- Line Sensors
#define LINE_1 32
#define LINE_2 35
#define LINE_3 34
#define LINE_4 39

// |--- Servos
#define ARM_1 23
#define ARM_2 22
#define ARM_3 21
#define ARM_4 19
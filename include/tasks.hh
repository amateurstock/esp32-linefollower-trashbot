#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "NewPing.h"

// RTOS related
esp_err_t init_tasks();
void read_line_sensors(void *params);
void update_motors(void *params);
void get_distances(void *params);
void get_analogs(void *params);

// Inits, debugs, comms, etc.
void init_serial();
void init_pins();
void user_logger(const char *TAG, char *message);

// Typedefs
typedef struct {
    uint8_t  line_state;
    uint32_t distance_1;
    uint32_t distance_2;
    uint32_t distance_3;
    uint16_t weight_out;
    uint16_t VB_out;
}state_t;

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

// |--- Ultrasonic Sensors
#define SHARED_TRIG 33
#define ECHO_1 25
#define ECHO_2 26
#define ECHO_3 27

// |--- Weight and Battery sensor
#define WEIGHT_PIN 14
#define VB_PIN 13

// Test
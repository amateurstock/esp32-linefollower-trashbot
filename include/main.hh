#ifndef MAIN_HH
#define MAIN_HH

#include <Arduino.h>
#include <WiFi.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_spiffs.h>

// Macros
#define PRINT_DBG
//#define WIFI_DBG
#define ULTRASONIC_ON
//#define IS_HALTING

// RTOS related
esp_err_t init_tasks();
void read_line_sensors(void *params);
void update_motors(void *params);
void get_distances(void *params);
void get_analogs(void *params);
void update_servos(void *param);
#ifdef PRINT_DBG
void print_debug(void *param);
#endif

// Inits, debugs, comms, etc.
void init_serial();
void init_pins();
void user_logger(const char *TAG, char *message);
void user_logger(const char *TAG, const char *message);
esp_err_t mount_spiffs(const char *base_path);
esp_err_t start_server();
esp_err_t init_wifi();
uint8_t fetch_bit(uint8_t val, uint8_t pos);
uint8_t count_highs(uint8_t val);
void stop_operations();

// Typedefs
typedef struct {
    uint8_t  line_state;
    uint32_t distance_1;
    uint32_t distance_2;
    uint32_t distance_3;
    uint16_t weight_out;
    uint16_t VB_out;
} sensors_t;

typedef struct {
    int16_t left_motors;
    int16_t right_motors;
    uint8_t servo_out1;
    uint8_t servo_out2;
    uint8_t servo_out3;
    uint8_t servo_out4;
}motors_t;


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
// |---|--- Do take note that these pins are unavailable for use during WiFi mode.
// |---|--- For prod, turn off wifi.

#endif

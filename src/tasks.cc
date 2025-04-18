#include "tasks.hh"

TaskHandle_t read_line_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;
TaskHandle_t get_distances_t = NULL;

const char *read_line_sensors_tag = "read_line_sensors";
const char *update_motors_tag = "update_motors";
const char *get_distances_tag = "get_distances";

HardwareSerial uno_serial(2);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// RTOS related +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void init_tasks() {
    const char *TAG = "init_tasks";
    user_logger(TAG, (char *)"Attempting to init tasks.");

    user_logger(TAG, (char *)"Initializing read_line_sensors.");
    xTaskCreate(
        read_line_sensors,
        "read_line_sensors task",
        4096,
        NULL,
        10,
        &read_line_sensors_t
    );
    if (read_line_sensors_t == NULL) {
        user_logger(read_line_sensors_tag, (char *)"Failed to create task...");
    }

    user_logger(TAG, (char *)"Initializing update_motors.");
    xTaskCreate(
        update_motors,
        "update_motors task",
        4096,
        NULL,
        10,
        &update_motors_t
    );
    if (update_motors_t == NULL) {
        user_logger(update_motors_tag, (char *)"Failed to create task...");
    }

}

void read_line_sensors(void *params) {

}

void update_motors(void *params) {

}

void get_distances(void *params) {

}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Inits, debugs, comms, etc. +++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void init_serial() {
    user_logger((const char *)"init_serial", (char *)"Starting master-slave connection.");
    uno_serial.begin(9600);
}

void init_pins() {
    
    // Init line sensors
    pinMode(LINE_1, INPUT);
    pinMode(LINE_2, INPUT);
    pinMode(LINE_3, INPUT);
    pinMode(LINE_4, INPUT);

    // Init servo arms
    pinMode(ARM_1, OUTPUT);
    pinMode(ARM_2, OUTPUT);
    pinMode(ARM_3, OUTPUT);
    pinMode(ARM_4, OUTPUT);

    analogWrite(ARM_1, 0);
    analogWrite(ARM_2, 0);
    analogWrite(ARM_3, 0);
    analogWrite(ARM_4, 0);
}

void user_logger(const char *TAG, char *message) {
    char buf[256];
    sprintf(buf, "%s: %s\n", TAG, message);
    Serial.print(buf);
}


#include "tasks.hh"
#include "L298N.hh"

TaskHandle_t read_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;

const char *read_sensors_tag = "read_sensors";
const char *update_motors_tag = "update_motors ";

HardwareSerial uno_serial(2);

void init_tasks() {
    const char *TAG = "init_tasks";
    user_logger(TAG, (char *)"Attempting to init tasks.");

    user_logger(TAG, (char *)"Initializing read_sensors.");
    xTaskCreate(
        read_sensors,
        "read_sensor task",
        4096,
        NULL,
        10,
        &read_sensors_t
    );
    if (read_sensors_t == NULL) {
        user_logger(read_sensors_tag, (char *)"Failed to create task...");
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

void init_serial() {
    uno_serial.begin(9600);
}

void user_logger(const char *TAG, char *message) {
    char buf[256];
    sprintf(buf, "%s: %s\n", TAG, message);
    Serial.print(buf);
}

void read_sensors(void *params) {

}

void update_motors(void *params) {

}

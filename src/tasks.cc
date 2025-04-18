#include "tasks.hh"

// RTOS task handles
TaskHandle_t read_line_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;
TaskHandle_t get_distances_t = NULL;
TaskHandle_t get_analogs_t = NULL;

// Function TAGs
const char *read_line_sensors_tag = "read_line_sensors";
const char *update_motors_tag = "update_motors";
const char *get_distances_tag = "get_distances";
const char *get_analogs_tag = "get analogs";

// OOP constructors
HardwareSerial uno_serial(2);
NewPing distance_1(SHARED_TRIG, ECHO_1, 100);
NewPing distance_2(SHARED_TRIG, ECHO_2, 100);
NewPing distance_3(SHARED_TRIG, ECHO_3, 100);

// Typedefs
state_t state {
    .line_state = 0b000,
    .distance_1 = 0,
    .distance_2 = 0,
    .distance_3 = 0,
    .weight_out = 0,
    .VB_out = 0
};




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// RTOS related +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
esp_err_t init_tasks() {
    const char *TAG = "init_tasks";
    user_logger(TAG, (char *)"Attempting to init tasks.");
    esp_err_t ret = ESP_OK;

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
        ret = ESP_FAIL;
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
        ret = ESP_FAIL;
    }

    user_logger(TAG, (char *)"Initializing get_distances.");
    xTaskCreate(
        get_distances,
        "get_distances task",
        4096,
        NULL,
        10,
        &get_distances_t
    );
    if (get_distances_t == NULL) {
        user_logger(get_distances_tag, (char *)"Failed to create task...");
        ret = ESP_FAIL;
    }

    return ret;
}

void read_line_sensors(void *params) {
    
}

void update_motors(void *params) {
    uno_serial.println("ha gottem");
}

void get_distances(void *params) {

    state.distance_1 = distance_1.ping_cm();
    vTaskDelay(pdMS_TO_TICKS(50));

    state.distance_2 = distance_2.ping_cm();
    vTaskDelay(pdMS_TO_TICKS(50));

    state.distance_3 = distance_3.ping_cm();
    vTaskDelay(pdMS_TO_TICKS(50));

}

void get_analogs(void *params) {
    state.VB_out = analogRead(VB_PIN);
    state.weight_out = analogRead(WEIGHT_PIN);
    vTaskDelay(pdMS_TO_TICKS(50));
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

    // Note, there's no such thing as a pinMode for ADC pins. Just call
    // analogRead() somewhere and the esp32 automatically knows what's up.
}

void user_logger(const char *TAG, char *message) {
    char buf[256];
    sprintf(buf, "%s: %s\n", TAG, message);
    Serial.print(buf);
}

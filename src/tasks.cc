#include "main.hh"

// RTOS task handles
TaskHandle_t read_line_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;
TaskHandle_t get_distances_t = NULL;
TaskHandle_t get_analogs_t = NULL;
TaskHandle_t update_servos_t = NULL;

// Function TAGs
const char *read_line_sensors_tag = "read_line_sensors";
const char *update_motors_tag = "update_motors";
const char *get_distances_tag = "get_distances";
const char *get_analogs_tag = "get analogs";
const char *update_servos_tag = "update_servos";

// OOP constructors
HardwareSerial uno_serial(2);
Ultrasonic distance_1(SHARED_TRIG, ECHO_1);
Ultrasonic distance_2(SHARED_TRIG, ECHO_2);
Ultrasonic distance_3(SHARED_TRIG, ECHO_3);

// Typedefs
sensors_t sensors_state{
    .line_state = 0b0000,
    .distance_1 = 0,
    .distance_2 = 0,
    .distance_3 = 0,
    .weight_out = 0,
    .VB_out = 0
};

motors_t motors {
    .left_motors = 0,
    .right_motors = 0,
    .servo_out1 = 0,
    .servo_out2 = 0,
    .servo_out3 = 0,
    .servo_out4 = 0,
};




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// RTOS related +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

esp_err_t init_tasks() {
    const char *TAG = "init_tasks";
    user_logger(TAG, "Attempting to init tasks.");
    esp_err_t ret = ESP_OK;

    user_logger(TAG, "Initializing read_line_sensors.");
    xTaskCreate(
        read_line_sensors,
        "read_line_sensors task",
        4096,
        NULL,
        10,
        &read_line_sensors_t
    );
    if (read_line_sensors_t == NULL) {
        user_logger(read_line_sensors_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

    user_logger(TAG, "Initializing update_motors.");
    xTaskCreate(
        update_motors,
        "update_motors task",
        4096,
        NULL,
        10,
        &update_motors_t
    );
    if (update_motors_t == NULL) {
        user_logger(update_motors_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

    user_logger(TAG, "Initializing get_distances.");
    xTaskCreate(
        get_distances,
        "get_distances task",
        4096,
        NULL,
        10,
        &get_distances_t
    );
    if (get_distances_t == NULL) {
        user_logger(get_distances_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

    user_logger(TAG, "Initializing get_analogs.");
    xTaskCreate(
        get_analogs,
        "get_analogs task",
        4096,
        NULL,
        10,
        &get_analogs_t
    );
    if (get_analogs_t == NULL) {
        user_logger(get_analogs_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

    user_logger(TAG, "Initializing update_servos");
    xTaskCreate(
        update_servos,
        "update_servos task",
        4096,
        NULL,
        10,
        &update_servos_t
    );
    if (update_servos_t == NULL) {
        user_logger(update_servos_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

    return ret;
}

void read_line_sensors(void *params) {
    for (;;) {
        uint8_t b0 = digitalRead(LINE_1);
        uint8_t b1 = digitalRead(LINE_2) << 1;
        uint8_t b2 = digitalRead(LINE_3) << 2;
        uint8_t b3 = digitalRead(LINE_4) << 3;
        sensors_state.line_state = 0b0000 | b0 | b1 | b2 | b3;

#ifdef PRINTDB
        Serial.printf("line_state = %d\n", sensors_state.line_state);
#endif

        vTaskDelay(pdMS_TO_TICKS(8));
    }
}

void update_motors(void *params) {
    user_logger(update_motors_tag, "2 seconds waiting for everything to settle down.");
    float weighted = 0;
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        weighted = (
            (-6.0f * fetch_bit(sensors_state.line_state, 0)) +
            (-2.0f * fetch_bit(sensors_state.line_state, 1)) +
            ( 2.0f * fetch_bit(sensors_state.line_state, 2)) +
            ( 6.0f * fetch_bit(sensors_state.line_state, 3)))
            /
            (float)(count_highs(sensors_state.line_state)
        );

#ifdef PRINTDB
        Serial.printf("Weighted -- %f\n", weighted);
#endif

        vTaskDelay(pdMS_TO_TICKS(8));
    }
}

void get_distances(void *params) {
    for (;;) {
        sensors_state.distance_1 = distance_1.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_2 = distance_2.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_3 = distance_3.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));

#ifdef PRINTDB
        Serial.printf("distance_1 = %d\n", sensors_state.distance_1);
        Serial.printf("distance_2 = %d\n", sensors_state.distance_2);
        Serial.printf("distance_3 = %d\n", sensors_state.distance_3);
#endif
    }
}

void get_analogs(void *params) {
    for (;;) {
        sensors_state.VB_out = analogRead(VB_PIN);
        sensors_state.weight_out = analogRead(WEIGHT_PIN);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void update_servos(void *param) {
    user_logger(update_servos_tag, "Waiting for 2 seconds for everything else to settle out...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    for (;;) {
        analogWrite(ARM_1, motors.servo_out1);
        analogWrite(ARM_2, motors.servo_out2);
        analogWrite(ARM_3, motors.servo_out3);
        analogWrite(ARM_4, motors.servo_out4);
        vTaskDelay(pdMS_TO_TICKS(8));
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Inits, debugs, comms, etc. +++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void init_serial() {
    user_logger((const char *)"init_serial", "Starting master-slave connection.");
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
    // Also, the NewPing constructors setup the pins necessary for the
    // ultrasonic sensors.
}

void user_logger(const char *TAG, char *message) {
    char buf[256];
    sprintf(buf, "%s: %s\n", TAG, message);
    Serial.print(buf);
}

void user_logger(const char *TAG, const char *message) {
    char buf[256];
    sprintf(buf, "%s: %s\n", TAG, message);
    Serial.print(buf);
}

uint8_t fetch_bit(uint8_t val, uint8_t pos) {
    return ((val >> pos) & 1);
}

uint8_t count_highs(uint8_t val) {
    uint8_t ret = 0;

    for (uint8_t pos = 0; pos < 4; pos++) {
        ret += ((val >> pos) & 1);
    }

    if (ret == 0) ret = 1;

    return ret;
}

#include "main.hh"
#include "motors_control.hh"
#include "pid.hh"
#include "ultrasonic.hh"
#include <cstdio>

#define K_P 4.0
#define K_I 0.0
#define K_D 0.0

// RTOS task handles
TaskHandle_t read_line_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;
TaskHandle_t get_distances_t = NULL;
TaskHandle_t get_analogs_t = NULL;
TaskHandle_t update_servos_t = NULL;

#ifdef PRINT_DBG
TaskHandle_t print_debug_t = NULL;
#endif

// Function TAGs
const char *read_line_sensors_tag = "read_line_sensors";
const char *update_motors_tag = "update_motors";
const char *get_distances_tag = "get_distances";
const char *get_analogs_tag = "get analogs";
const char *update_servos_tag = "update_servos";

#ifdef PRINT_DBG
const char *print_debug_tag = "print_debug";
#endif

// OOP constructors
HardwareSerial uno_serial(2);
PID pid_controller(K_P, K_I, K_D, 0);
Ultrasonic ultrasonic1 (
    SHARED_TRIG,
    ECHO_1,
    100,
    ultrasonic1_rise,
    ultrasonic1_fall
);
Ultrasonic ultrasonic2 (
    SHARED_TRIG,
    ECHO_2,
    100,
    ultrasonic2_rise,
    ultrasonic2_fall
);
Ultrasonic ultrasonic3 (
    SHARED_TRIG,
    ECHO_3,
    100,
    ultrasonic3_rise,
    ultrasonic3_fall
);

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
        5,
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
        8,
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
        9,
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
        6,
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
        7,
        &update_servos_t
    );
    if (update_servos_t == NULL) {
        user_logger(update_servos_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

#ifdef PRINT_DBG
    user_logger(TAG, "Initializing print_debug");
    xTaskCreate(
        print_debug,
        "print_debug task",
        8192,
        NULL,
        1,
        &print_debug_t
    );
    if (print_debug_t == NULL) {
        user_logger(print_debug_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }
#endif

    return ret;
}

void read_line_sensors(void *params) {
    user_logger(read_line_sensors_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        uint8_t b0 = digitalRead(LINE_1);
        uint8_t b1 = digitalRead(LINE_2) << 1;
        uint8_t b2 = digitalRead(LINE_3) << 2;
        uint8_t b3 = digitalRead(LINE_4) << 3;
        sensors_state.line_state = 0b0000 | b0 | b1 | b2 | b3;

        vTaskDelay(pdMS_TO_TICKS(8));
    }
}

void update_motors(void *params) {
    user_logger(update_motors_tag, "Waiting for the rest to init.");
    float error = 0;
    float delta = 0;
    char buf[32] = {0};
    vTaskDelay(pdMS_TO_TICKS(2000));

    pid_controller.set_start_time(millis());
    for (;;) {
        if (uno_serial.available()) {
            String buffer = uno_serial.readStringUntil('\n');
            Serial.printf("%s\n", buffer);
#ifdef IS_HALTING
            if ((buffer != "ok") || (buffer != "ok\n")) stop_operations();
#endif
        }

        error = (
            ( 6.0f * fetch_bit(sensors_state.line_state, 0)) +
            ( 2.0f * fetch_bit(sensors_state.line_state, 1)) +
            (-2.0f * fetch_bit(sensors_state.line_state, 2)) +
            (-6.0f * fetch_bit(sensors_state.line_state, 3)))
            /
            (float)(count_highs(sensors_state.line_state)
        );
        delta = pid_controller.calculate(error, millis());
        delta_steering(&motors, delta);

        sprintf(buf, "L:%d;R:%d;\n", motors.left_motors, motors.right_motors);
        //sprintf(buf, "L:100;R:100;\n");
        uno_serial.print(buf);

        memset(buf, 0, sizeof(buf));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void get_distances(void *params) {
    user_logger(get_distances_tag, "Waiting for the rest to init.");
    setup_ultrasonic_pins(ultrasonic1);
    //setup_ultrasonic_pins(ultrasonic1);
    //setup_ultrasonic_pins(ultrasonic1);
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
#ifdef ULTRASONIC_ON
        sensors_state.distance_1 = distance_1.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_2 = distance_2.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_3 = distance_3.pingCM();
        vTaskDelay(pdMS_TO_TICKS(50));
#else
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif

    }
}

void get_analogs(void *params) {
    user_logger(get_analogs_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        sensors_state.VB_out = analogRead(VB_PIN);
        sensors_state.weight_out = analogRead(WEIGHT_PIN);
        vTaskDelay(pdMS_TO_TICKS(1000));

#ifdef IS_HALTING
        if (sensors_state.weight_out >= WEIGHT_THRESHOLD
        || sensors_state.VB_out <= BATTERY_THRESHOLD) {
            user_logger(get_analogs_tag, "Threshold met. Halting operations.");
            stop_operations();
        }
#endif

    }
}

void update_servos(void *param) {
    user_logger(update_servos_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        analogWrite(ARM_1, motors.servo_out1);
        analogWrite(ARM_2, motors.servo_out2);
        analogWrite(ARM_3, motors.servo_out3);
        analogWrite(ARM_4, motors.servo_out4);
        vTaskDelay(pdMS_TO_TICKS(8));


    }

}

#ifdef PRINT_DBG
void print_debug(void *param) {
    user_logger(print_debug_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(2000));

    for (;;) {
        Serial.printf("<%d> || ", millis());
        Serial.printf("line_state = %d || ", sensors_state.line_state);
        Serial.printf("distances = %d, %d, %d\n",
                      sensors_state.distance_1,
                      sensors_state.distance_2,
                      sensors_state.distance_3);
        Serial.printf("motors = %d, %d\n\n", motors.left_motors, motors.right_motors);

        vTaskDelay(pdMS_TO_TICKS(20));

    }
}
#endif




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Inits, debugs, comms, etc. +++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void init_serial() {
    user_logger("init_serial", "Starting master-slave connection.");
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

void stop_operations() {
    user_logger("stop_operations", "Halting operations until further notice...");
    taskDISABLE_INTERRUPTS();
    vTaskSuspendAll();

    for (;;) {}
}

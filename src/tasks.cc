#include "main.hh"
#include <ESP32Servo.h>
#include "pid.hh"
#include "ultrasonic.hh"
#include "motors_control.hh"
#include "trashbot.hh"
#include <cstdio>

constexpr float K_P = 8.0;
constexpr float K_I = 0.0;
constexpr float K_D = 0.1;
constexpr uint32_t WEIGHT_THRESHOLD = 2048;
constexpr uint32_t BATTERY_THRESHOLD = 255;
constexpr uint8_t SERVO_TIMER = 0;

// idk how to extern a constexpr lmao
uint16_t INIT_WAIT_TIME = 2000;
char serial_buf[32] = {0};

constexpr uint32_t ARM1_INIT = 75;
constexpr uint32_t ARM2_INIT = 135;
constexpr uint32_t ARM3_INIT = 45;
constexpr uint32_t ARM4_INIT = 105;

// RTOS task handles
TaskHandle_t read_line_sensors_t = NULL;
TaskHandle_t update_motors_t = NULL;
TaskHandle_t get_distances_t = NULL;
TaskHandle_t get_analogs_t = NULL;
TaskHandle_t update_servos_t = NULL;
extern TaskHandle_t check_trash_obstacle_t;
extern TaskHandle_t trash_collection_t;
extern TaskHandle_t obstacle_avoidance_t;

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
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Ultrasonic ultrasonic1 (
    SHARED_TRIG,
    ECHO_1,
    100,
    ultrasonic1_fall
);
Ultrasonic ultrasonic2 (
    SHARED_TRIG,
    ECHO_2,
    100,
    ultrasonic2_fall
);
Ultrasonic ultrasonic3 (
    SHARED_TRIG,
    ECHO_3,
    100,
    ultrasonic3_fall
);
uint32_t ultrasonic4_reading = UINT32_MAX;

// Typedefs
sensors_t sensors_state{
    .line_state = 0b0000,
    .distance_1 = UINT32_MAX,
    .distance_2 = UINT32_MAX,
    .distance_3 = UINT32_MAX,
    .weight_out = 0,
    .VB_out = 0
};

motors_t motors {
    .left_motors = 0,
    .right_motors = 0,
    .servo_out1 = ARM1_INIT,
    .servo_out2 = ARM2_INIT,
    .servo_out3 = ARM3_INIT,
    .servo_out4 = ARM4_INIT,
};

lftb_mode_t current_mode = LINE_FOLLOWER;




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
        1,
        &get_analogs_t
    );
    if (get_analogs_t == NULL) {
        user_logger(get_analogs_tag, "Failed to create task...");
        ret = ESP_FAIL;
    }

#ifdef SERVOS_ON
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
#endif

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

#ifdef COLLECTION_ON
#ifndef CHECKER_FUNC
#define CHECKER_FUNC
    user_logger(TAG, "Initializing check_trash_obstacle");
    xTaskCreate(
        check_trash_obstacle,
        "check_trash_obstacle task",
        8192,
        NULL,
        5,
        &check_trash_obstacle_t
    );
    if (check_trash_obstacle_t == NULL) {
        user_logger("check_trash_obstacle", "Failed to create task...");
        ret = ESP_FAIL;
    }
#endif

    user_logger(TAG, "Initializing trash_collection");
    xTaskCreate(
        trash_collection,
        "trash_collection task",
        8192,
        NULL,
        5,
        &trash_collection_t
    );
    if (trash_collection_t == NULL) {
        user_logger("trash_collection", "Failed to create task...");
        ret = ESP_FAIL;
    }
#endif

#ifdef AVOIDANCE_ON
#ifndef CHECKER_FUNC
#define CHECKER_FUNC
    user_logger(TAG, "Initializing check_trash_obstacle");
    xTaskCreate(
        check_trash_obstacle,
        "check_trash_obstacle task",
        8192,
        NULL,
        5,
        &check_trash_obstacle_t
    );
    if (check_trash_obstacle_t == NULL) {
        user_logger("check_trash_obstacle", "Failed to create task...");
        ret = ESP_FAIL;
    }
#endif

    user_logger(TAG, "Initializing obstacle_avoidance");
    xTaskCreate(
        obstacle_avoidance,
        "obstacle_avoidance task",
        8192,
        NULL,
        5,
        &obstacle_avoidance_t
    );
    if (obstacle_avoidance_t == NULL) {
        user_logger("obstacle_avoidance", "Failed to create task...");
        ret = ESP_FAIL;
    }
#endif


    return ret;
}

void read_line_sensors(void *params) {
    user_logger(read_line_sensors_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
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
    String buffer;
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    pid_controller.set_start_time(millis());
    for (;;) {
#ifdef MOTORS_ON
        switch (current_mode) {
            case LINE_FOLLOWER: {
                if (uno_serial.available()) {
                    buffer = uno_serial.readStringUntil('x');
                    get_key_value(
                        serial_buf,
                        buffer.c_str(),
                        'd',
                        ';',
                        2
                    );
                    ultrasonic4_reading = atoi(serial_buf);
                }

                error = ( ( 6.0f * fetch_bit(sensors_state.line_state, 0)) +
                    ( 2.0f * fetch_bit(sensors_state.line_state, 1)) +
                    (-2.0f * fetch_bit(sensors_state.line_state, 2)) +
                    (-6.0f * fetch_bit(sensors_state.line_state, 3)))
                    /
                    (float)(count_highs(sensors_state.line_state) );
                delta = pid_controller.calculate(error, millis());
                delta_steering(&motors, delta);
                sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                uno_serial.print(buf);
                memset(buf, 0, sizeof(buf));

                vTaskDelay(pdMS_TO_TICKS(20));
                break;
            }

            case TRASH_COLLECTION: {
                motors.left_motors = 0;
                motors.right_motors = 0;
                sprintf(buf, "L:%d;R:%d;x", motors.left_motors, motors.right_motors);
                uno_serial.print(buf);
                memset(buf, 0, sizeof(buf));
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
                break;
            }

            case OBSTACLE_AVOIDANCE: {
                vTaskDelay(pdMS_TO_TICKS(100));
                break;
            }

        }

#endif
    }
}

void get_distances(void *params) {
    user_logger(get_distances_tag, "Waiting for the rest to init.");
    setup_ultrasonic_pins(ultrasonic1);
    setup_ultrasonic_pins(ultrasonic2);
    setup_ultrasonic_pins(ultrasonic3);
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
#ifdef ULTRASONIC_ON
        sensors_state.distance_3 = pull_ultrasonic(ultrasonic3);
        poll_ultrasonic(ultrasonic1);
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_1 = pull_ultrasonic(ultrasonic1);
        poll_ultrasonic(ultrasonic2);
        vTaskDelay(pdMS_TO_TICKS(50));
        sensors_state.distance_2 = pull_ultrasonic(ultrasonic2);
        poll_ultrasonic(ultrasonic3);
        vTaskDelay(pdMS_TO_TICKS(50));
#else
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif
    }
}

void get_analogs(void *params) {
    user_logger(get_analogs_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
#ifndef WIFI_DBG
        sensors_state.VB_out = analogRead(VB_PIN);
        sensors_state.weight_out = analogRead(WEIGHT_PIN);
#endif

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
    ESP32PWM::allocateTimer(SERVO_TIMER);
    servo1.setPeriodHertz(50); servo1.attach(ARM_1, 500, 2500);
    servo2.setPeriodHertz(50); servo2.attach(ARM_2, 500, 2500);
    servo3.setPeriodHertz(50); servo3.attach(ARM_3, 500, 2500);
    servo4.setPeriodHertz(50); servo4.attach(ARM_4, 500, 2500);
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        servo1.write(motors.servo_out1);
        servo2.write(motors.servo_out2);
        servo3.write(motors.servo_out3);
        servo4.write(motors.servo_out4);
        vTaskDelay(pdMS_TO_TICKS(8));
    }
}

#ifdef PRINT_DBG
void print_debug(void *param) {
    user_logger(print_debug_tag, "Waiting for the rest to init.");
    vTaskDelay(pdMS_TO_TICKS(INIT_WAIT_TIME));
    for (;;) {
        Serial.printf("<%d> || ", millis());
        Serial.printf("line_state = %d || ", sensors_state.line_state);
        Serial.printf("distances = %d, %d, %d,\n",
                      sensors_state.distance_1,
                      sensors_state.distance_2,
                      sensors_state.distance_3);
        Serial.printf("%s\n", serial_buf);
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

    for (;;) { }
}

void get_key_value(
    char *buf,
    const char *target,
    char query,
    char terminator,
    uint32_t offset
) {
    const char *p = strchr(target, query) + offset;
    if (!p) {
        buf[0] = '\0';
        return;
    }

    const char *end = strchr(p, terminator);
    if (!end) {
        buf[0] = '\0';
        return;
    }

    uint32_t index = 0;
    while (p != end) {
        buf[index] = *p;
        index += 1; p += 1;
    }
    buf[index] = '\0';
}

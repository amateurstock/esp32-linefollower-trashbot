#include "tasks.hh"

void setup() {
    Serial.begin(115200);
    init_pins();
    init_serial();
    ESP_ERROR_CHECK(init_tasks());
}

void loop() {
    // Nothing to do here. FreeRTOS handles everything :>
}

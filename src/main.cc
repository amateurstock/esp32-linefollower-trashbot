#include "tasks.hh"

uint32_t global_tick = 0;

void setup() {
    Serial.begin(115200);
    init_serial();
    init_tasks();
}

void loop() {
    // Nothing to do here. FreeRTOS handles everything :>
}

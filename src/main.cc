#include "tasks.hh"

void setup() {
    Serial.begin(115200);
    init_serial();
    init_tasks();
}

void loop() {
    // Nothing to do here. FreeRTOS handles everything :>
}

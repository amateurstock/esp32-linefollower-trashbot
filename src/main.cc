#include "main.hh"

const char *root_dir = "/data";
const char *html_path = "/index.html";
const char *js_path = "/scripts.js";

#define WIFI_DBG

void setup() {
    Serial.begin(115200);
    init_pins();

#ifdef WIFI_DBG
    ESP_ERROR_CHECK(mount_spiffs(root_dir));
#endif

    init_serial();
    ESP_ERROR_CHECK(init_tasks());

#ifdef WIFI_DBG
    ESP_ERROR_CHECK(init_wifi());
    ESP_ERROR_CHECK(start_server());
#endif
}

void loop() {
    // Nothing to do here. FreeRTOS handles everything :>
    vTaskDelay(pdMS_TO_TICKS(10000));
}

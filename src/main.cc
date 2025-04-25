#include "main.hh"

const char *root_dir = "/data";
const char *html_path = "/index.html";
const char *js_path = "/scripts.js";


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

    uint32_t remainingHeap = ESP.getFreeHeap();
    uint32_t totalHeap = ESP.getHeapSize();
    float percent = (remainingHeap * 100.0) / totalHeap;

    Serial.printf("After creating tasks: %u / %u bytes (%.2f%% free)\n",
                  remainingHeap, totalHeap, percent);
}

void loop() {
    // Nothing to do here. FreeRTOS handles everything :>
    vTaskDelay(pdMS_TO_TICKS(10000));
}

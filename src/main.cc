#include "main.hh"

const char *root_dir = "/data";
const char *html_path = "/index.html";
const char *js_path = "/scripts.js";


void setup() {
    Serial.begin(115200);
    init_pins();

    init_serial();
    ESP_ERROR_CHECK(init_tasks());

#ifdef WIFI_DBG
    ESP_ERROR_CHECK(mount_spiffs(root_dir));
    ESP_ERROR_CHECK(init_wifi());
    ESP_ERROR_CHECK(start_server());
#endif

}

void loop() {
}

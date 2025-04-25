#include "main.hh"

esp_err_t mount_spiffs(const char *base_path) {
    const char *TAG = "mount_spiffs";
    user_logger(TAG, (char *)"Using: ");
    user_logger(TAG, base_path);

    esp_vfs_spiffs_conf_t cfg = {
        .base_path = base_path,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&cfg);
    switch (ret) {

        case ESP_FAIL: {
            user_logger(TAG, (char *)"Failed to mount or format filesystem");
            break;
        }

        case ESP_ERR_NOT_FOUND: {
            user_logger(TAG, (char *)"Failed to find SPIFFS partition");
            break;
        }

        default: {
            user_logger(TAG, (char *)"Failed to initialize SPIFFS");
            break;
        }
        return ret;
    }
    user_logger(TAG, (char *)"SPIFFS mounted successfully!");
    return ret;
}

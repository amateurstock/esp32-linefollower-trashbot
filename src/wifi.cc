#include "WiFi.h"
#include "main.hh"
#include "server_handlers.hh"

const char *ssid = "Shortcircuiteers LFTB";
const char *pass = "12345678";

IPAddress local_ip (192, 168,  20,  25);
IPAddress gateway  (192, 168,  20,   1);
IPAddress subnet   (255, 255, 255,   0);

httpd_handle_t web_server = NULL;

esp_err_t init_wifi() {
    const char *TAG = "init_wifi";
    user_logger(TAG, "Starting WiFi");

    esp_err_t ret = ESP_OK;

    WiFi.persistent(false);

    if (!WiFi.mode(WIFI_AP)) {
        user_logger(TAG, "Failed to set WiFi mode!");
        ret = ESP_FAIL;
        return ret;
    }

    if (!WiFi.softAPConfig(local_ip, gateway, subnet)) {
        user_logger(TAG, "Failed to set AP config!");
        ret = ESP_FAIL;
        return ret;
    }

    if (!WiFi.softAP(ssid, pass)) {
        user_logger(TAG, "Failed to start access point!");
        ret = ESP_FAIL;
        return ret;
    }

    return ret;
}

esp_err_t start_server() {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_uri_handlers = 16;
    const char *TAG = "start_server";
    user_logger(TAG, "Starting server.");

    httpd_uri_t index_uri {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx= NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };

    httpd_uri_t scripts_uri {
        .uri = "/scripts.js",
        .method = HTTP_GET,
        .handler = scripts_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };

    httpd_uri_t updates_uri {
        .uri = "/updates",
        .method = HTTP_GET,
        .handler = updates_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };

    httpd_uri_t servos_uri {
        .uri = "/servos",
        .method = HTTP_GET,
        .handler = servos_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = false,
        .supported_subprotocol = NULL
    };

    Serial.printf("<%s> Starting web server on port: %d\n", TAG, cfg.server_port);
    if (httpd_start(&web_server, &cfg) == ESP_OK) {
        httpd_register_uri_handler(web_server, &index_uri);
        httpd_register_uri_handler(web_server, &scripts_uri);
        httpd_register_uri_handler(web_server, &updates_uri);
        httpd_register_uri_handler(web_server, &servos_uri);
    } else {
        user_logger(TAG, "Error! Web server won't start!");
        return ESP_FAIL;
    }

    user_logger(TAG, "Server has started!");
    return ESP_OK;
}

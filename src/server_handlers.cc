#include "main.hh"
#include "server_handlers.hh"

extern const char *root_dir;
extern const char *html_path;
extern const char *js_path;
extern sensors_t sensors_state;
extern motors_t motors;

constexpr uint16_t MIN_ANGLE = 0;
constexpr uint16_t MAX_ANGLE = 270;

esp_err_t index_handler(httpd_req_t *req) {
    const char *TAG = "index_handler";

    return serve_files(
        req,
        TAG,
        html_path,
        "text/html"
    );
}

esp_err_t scripts_handler(httpd_req_t *req) {
    const char *TAG = "scripts_handler";

    return serve_files(
        req,
        TAG,
        js_path,
        "application/javascript"
    );
}

esp_err_t updates_handler(httpd_req_t *req) {
    const char *TAG = "updates_handler";

    // Try to figure out how to send data or something idk...
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

esp_err_t servos_handler(httpd_req_t *req) {
    const char *TAG = "servos_uri";

    char *buf = NULL;
    char s1[32];
    char s2[32];
    char s3[32];
    char s4[32];

    if (parse_get(req, &buf) != ESP_OK) {
        user_logger(TAG, "Failed to parse request query.\n");
        return ESP_FAIL;
    }

    if (httpd_query_key_value(buf, "s1", s1, sizeof(s1)) != ESP_OK
        || httpd_query_key_value(buf, "s2", s2, sizeof(s2)) != ESP_OK
        || httpd_query_key_value(buf, "s3", s3, sizeof(s3)) != ESP_OK
        || httpd_query_key_value(buf, "s4", s4, sizeof(s4)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    free(buf);

    motors.servo_out1 = map(atoi(s1), MIN_ANGLE, MAX_ANGLE, 1000, 2000);
    motors.servo_out2 = map(atoi(s2), MIN_ANGLE, MAX_ANGLE, 1000, 2000);
    motors.servo_out3 = map(atoi(s3), MIN_ANGLE, MAX_ANGLE, 1000, 2000);
    motors.servo_out4 = map(atoi(s4), MIN_ANGLE, MAX_ANGLE, 1000, 2000);

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

esp_err_t serve_files(
    httpd_req_t *req, 
    const char *TAG, 
    const char *path,
    const char *type
) {
    String file_dir = "";
    file_dir += root_dir;
    file_dir += path;
    httpd_resp_set_type(req, type);

    Serial.printf("<%s> Attempting to open %s\n", TAG, file_dir.c_str());
    FILE *file = fopen(file_dir.c_str(), "r");
    if (!file) {
        Serial.printf("<%s> %s does not exist and/or is an invalid directory.\n",
                      TAG,
                      file_dir.c_str());
        httpd_resp_send_404(req);
        return ESP_ERR_NOT_FOUND;
    }

    user_logger(TAG, "Attempting to send file");
    char buf[256] = {0};
    while (fgets(buf, sizeof(buf) - 1, file)) {
        httpd_resp_send_chunk(req, buf, strlen(buf));
        memset(buf, '\0', sizeof(buf));
    }

    if (feof(file)) {
        httpd_resp_send_chunk(req, buf, 0);
        memset(buf, '\0', sizeof(buf));
    }

    fclose(file);
    user_logger(TAG, "File closed successfully!");
    return ESP_OK;
}

esp_err_t parse_get(httpd_req_t *req, char **obuf) {
    char *buf = NULL;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;

    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);

        if (!buf) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }

        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

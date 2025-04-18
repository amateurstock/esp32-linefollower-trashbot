#include "main.hh"
#include "server_handlers.hh"

extern const char *root_dir;
extern const char *html_path;
extern const char *js_path;

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

    user_logger(TAG, (char *)"Attempting to send file");
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
    user_logger(TAG, (char *)"File closed successfully!");
    return ESP_OK;
}

#include <esp_http_server.h>

esp_err_t index_handler (httpd_req_t *req);
esp_err_t serve_files(
    httpd_req_t *req, 
    const char *TAG, 
    const char *path,
    const char *type
);

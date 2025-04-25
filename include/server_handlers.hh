#ifndef SERVER_HANDLERS_HH
#define SERVER_HANDLERS_HH

#include <esp_http_server.h>

esp_err_t index_handler (httpd_req_t *req);
esp_err_t scripts_handler (httpd_req_t *req);
esp_err_t updates_handler(httpd_req_t *req);
esp_err_t servos_handler(httpd_req_t *req);

esp_err_t serve_files(
    httpd_req_t *req, 
    const char *TAG, 
    const char *path,
    const char *type
);
esp_err_t parse_get(httpd_req_t *req, char **obuf);

#endif

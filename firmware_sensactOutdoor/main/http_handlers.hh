#pragma once

#include <esp_http_server.h>

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
esp_err_t handle_get_root(httpd_req_t *req);
esp_err_t handle_put_webui(httpd_req_t *req);
esp_err_t handle_get_webui(httpd_req_t *req);


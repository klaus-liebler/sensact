#pragma once

#include <esp_http_server.h>

esp_err_t handle_get_common(httpd_req_t *req); /*Application itself*/
esp_err_t handle_put_iostate(httpd_req_t *req);
esp_err_t handle_get_iostate(httpd_req_t *req);
esp_err_t handle_get_iocfg(httpd_req_t *req);
esp_err_t handle_put_iocfg(httpd_req_t *req);
esp_err_t handle_put_devctrl(httpd_req_t *req);

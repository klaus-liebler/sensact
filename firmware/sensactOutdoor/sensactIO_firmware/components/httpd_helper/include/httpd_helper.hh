#pragma once
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_http_server.h>


esp_err_t helper_get_binary_file(httpd_req_t *req, const char *filepath);
esp_err_t helper_put_binary_file(httpd_req_t *req, const char *filepath, bool overwrite);
esp_err_t RegisterHandler(httpd_handle_t server, const char *uri, http_method method, esp_err_t (*handler)(httpd_req_t *r), void *userCtx);

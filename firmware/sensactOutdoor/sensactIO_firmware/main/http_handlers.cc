#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <array>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "http_handlers.hh"
#include "manager.hh"
#include "common_projectconfig.hh"

#define TAG "HTTP_handler"
#include "httpd_helper.hh"

extern const char index_html_gz_start[] asm("_binary_index_html_gz_start");
extern const char index_html_gz_end[] asm("_binary_index_html_gz_end");
extern const size_t index_html_gz_size asm("index_html_gz_length");

esp_err_t handle_get_common(httpd_req_t *req)//browser get application itself
{
    ESP_LOGI(TAG, "HTTPd get_root for file with lenght %d", index_html_gz_size);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    ESP_ERROR_CHECK(httpd_resp_send(req, index_html_gz_start, index_html_gz_size)); // -1 = use strlen()
    return ESP_OK;
}

esp_err_t handle_get_iostate(httpd_req_t *req) //brwoser gets state of IOs
{
    Manager *manager = (Manager *)(req->user_ctx);
    flatbuffers::FlatBufferBuilder builder(512);
    manager->FillBuilderWithStateForWebUI(&builder);
    //ESP_LOGI(TAG, "HTTPd returns iostate len  %d", builder.GetSize());
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req, (const char *)builder.GetBufferPointer(), builder.GetSize());
    return ESP_OK;
}

esp_err_t handle_put_iostate(httpd_req_t *req) //browser sends commands to change IOs
{
    Manager *manager = (Manager *)(req->user_ctx);
    int ret = 0;
    int remaining = req->content_len;
    uint8_t buf[req->content_len];
    while (remaining > 0)
    {
        // Read the data for the request
        if ((ret = httpd_req_recv(req, (char *)buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                // Retry receiving if timeout occurred
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    // End response
    // Get a pointer to the root object inside the buffer.
    //ESP_LOGI(TAG, "Received put_iostate length %d", req->content_len);
    auto cmd = flatbuffers::GetRoot<tCommand>(buf);
    manager->HandleCommand(cmd);
    flatbuffers::FlatBufferBuilder builder(512);
    manager->FillBuilderWithStateForWebUI(&builder);
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req, (const char *)builder.GetBufferPointer(), builder.GetSize());
    return ESP_OK;
}

esp_err_t handle_get_iocfg(httpd_req_t *req)//browser gets configuration of IOs to draw the correct user interface
{
    return helper_get_binary_file(req, Paths::DEFAULTCFG_PATH);
}

esp_err_t handle_put_iocfg(httpd_req_t *req)//browser sends new configuraion of IOs
{
    return helper_put_binary_file(req, Paths::DEFAULTCFG_PATH, true);
}

esp_err_t handle_put_devctrl(httpd_req_t *req)//reset system
{
    httpd_resp_sendstr(req, "Device will be restarted");
    ESP_LOGI(TAG, "Device will be restarted");
    esp_restart();
    return ESP_OK;
}
#undef TAG
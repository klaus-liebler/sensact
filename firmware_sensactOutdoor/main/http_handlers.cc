#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "http_handlers.hh"

static constexpr char *TAG = "HTTP_handler";

constexpr size_t SCRATCH_BUFSIZE = 8192;
char scratch[SCRATCH_BUFSIZE];



esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}



extern const char index_html_gz_start[] asm("_binary_index_html_gz_start");
extern const char index_html_gz_end[] asm("_binary_index_html_gz_end");
extern const size_t index_html_gz_size asm("index_html_gz_length");

esp_err_t handle_get_root(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_send(req, index_html_gz_start, index_html_gz_size); // -1 = use strlen()
    return ESP_OK;
}
esp_err_t handle_get_webui(httpd_req_t *req)
{
    return ESP_OK;
}

esp_err_t handle_put_webui(httpd_req_t *req)
{    
    /*PLCManager *plcmanager = (PLCManager *)(req->user_ctx);
    int ret=0;
    int remaining = req->content_len;
    if(remaining!=24){
        ESP_LOGE(TAG, "Unexpected Data length %d in experiment_put_handler", remaining);
        return ESP_FAIL;
    }
    uint8_t buf[remaining];
    while (remaining > 0) {
        // Read the data for the request
        if ((ret = httpd_req_recv(req, (char*)buf,  MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                // Retry receiving if timeout occurred
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    // End response
    float *bufF32 = (float*)buf;
    uint32_t *bufU32 = (uint32_t*)buf;
    uint32_t modeU32 = bufU32[0];
    float setpointTempOrHeater = bufF32[1];
    float setpointFan = bufF32[2];
    float KP = bufF32[3];
    float TN = bufF32[4];
    float TV = bufF32[5];
    
    ESP_LOGI(TAG, "Set mode %d and setpointTempOrHeater %F and Setpoint Fan %F", modeU32, setpointTempOrHeater, setpointFan);
    HeaterExperimentData returnData;
    switch (modeU32)
    {
    case 0: plcmanager->TriggerHeaterExperimentFunctionblock(&returnData); break;
    case 1: plcmanager->TriggerHeaterExperimentOpenLoop(setpointTempOrHeater, setpointFan, &returnData); break;
    case 2: plcmanager->TriggerHeaterExperimentClosedLoop(setpointTempOrHeater, setpointFan, KP, TN, TV, &returnData); break;
    default:break;
    }
    
    httpd_resp_set_type(req, "application/octet-stream");
    float retbuf[4];
    retbuf[0]=returnData.SetpointTemperature;
    retbuf[1]=returnData.Heater;
    retbuf[2]=returnData.Fan;
    retbuf[3]=returnData.ActualTemperature;
    httpd_resp_send(req, (const char*)retbuf, 16);
    */
    return ESP_OK;
}



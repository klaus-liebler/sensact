#pragma once
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <string.h>

#include <lwip/err.h>
#include <lwip/sys.h>
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "cJSON.h"

#define TAG "UPD"
extern const uint8_t sciebo_cer_start[] asm("_binary_sciebo_cer_start");
extern const uint8_t sciebo_cer_end[] asm("_binary_sciebo_cer_end");
const char* TIMER = "timer";
const char* MANUAL_REQUEST = "manual request";

class Updater
{
private:
    bool manualUpdateRequested{true}; //on boot or first start -->do a manual Update
    int64_t lastUpdateCheckUs{INT64_MAX};
    char *http_response_buffer;
    int http_response_buffer_len;

    static esp_err_t http_logging_event_handler(esp_http_client_event_t *evt)
    {
        switch (evt->event_id)
        {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        }
        return ESP_OK;
    }

    static esp_err_t http_event_handler(esp_http_client_event_t *evt)
    {
        
        Updater *myself = static_cast<Updater *>(evt->user_data);
        switch (evt->event_id)
        {
        case HTTP_EVENT_ERROR: ESP_LOGE(TAG, "HTTP_EVENT_ERROR"); break;
        case HTTP_EVENT_ON_CONNECTED: ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED"); break;
        case HTTP_EVENT_HEADER_SENT: ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT"); break;
        case HTTP_EVENT_ON_HEADER: ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)){
                
                if (myself->http_response_buffer == NULL){
                    myself->http_response_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
                    myself->http_response_buffer_len = 0;
                    if (myself->http_response_buffer == NULL){
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(myself->http_response_buffer + myself->http_response_buffer_len, evt->data, evt->data_len);
                myself->http_response_buffer_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (myself->http_response_buffer != NULL)
            {
                ESP_LOGI(TAG, "Response is accumulated in output_buffer %p of len %d.", myself->http_response_buffer, myself->http_response_buffer_len);
                //ESP_LOG_BUFFER_HEX(TAG, output_buffer, myself->http_response_buffer_len);
                cJSON *ota_json = cJSON_Parse(myself->http_response_buffer);
                if (cJSON_GetObjectItem(ota_json, "version"))
                {
                    char *version = cJSON_GetObjectItem(ota_json, "version")->valuestring;
                    ESP_LOGI(TAG, "version=%s", version);
                }
                ESP_LOGI(TAG, "Free response");
                free(myself->http_response_buffer);
                myself->http_response_buffer = NULL;
            }
            myself->http_response_buffer_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0)
            {
                if (myself->http_response_buffer != NULL)
                {
                    free(myself->http_response_buffer);
                    myself->http_response_buffer = NULL;
                }
                myself->http_response_buffer_len = 0;
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
        }
        return ESP_OK;
    }


    static void updaterTask(void *pvParameters)
    {
        Updater *myself = static_cast<Updater *>(pvParameters);
        int64_t now = esp_timer_get_time();
        if (myself->manualUpdateRequested || now - myself->lastUpdateCheckUs > 2 * 60 * 60 * 1000000)
        {
            ESP_LOGI(TAG "Update Process starts due to %s", myself->manualUpdateRequested ? MANUAL_REQUEST : TIMER);
            esp_http_client_config_t config = {};
            config.url = "https://w-hs.sciebo.de/s/91RHVZjf8TOtLm3/download";
            config.cert_pem = (const char *)sciebo_cer_start;
            config.event_handler = http_event_handler;
            config.user_data=(void*)myself;

            esp_http_client_handle_t client = esp_http_client_init(&config);
            esp_err_t err = esp_http_client_perform(client);

            if (err == ESP_OK)
            {
                ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
                         esp_http_client_get_status_code(client),
                         esp_http_client_get_content_length(client));
            }
            else
            {
                ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
            }
            esp_http_client_cleanup(client);
            const char *downloadURL = "https://w-hs.sciebo.de/s/2x4L5Q8z982ZDUG/download";

            config.url = downloadURL;
            config.event_handler = http_logging_event_handler;
            config.keep_alive_enable = true;
            esp_err_t ret = esp_https_ota(&config);
            if (ret == ESP_OK)
            {
                ESP_LOGI(TAG, "Firmware upgrade successful");
                esp_restart();
            }
            else
            {
                ESP_LOGE(TAG, "Firmware upgrade failed");
            }
        }
    }

    enum class VERSION_COMPARE
    {
        V1_GT_V2,
        V1_EQ_V2,
        V1_LT_V2,
        WRONG_BASE_NAME,
        INVALID_STRING_SYNTAX
    };

    VERSION_COMPARE cmpVersion(const char *v1, const char *v2)
    {
        int i;
        int oct_v1[3], oct_v2[3];
        char name_v1[24];
        char name_v2[24];
        if (4 != sscanf(v1, "%s-%d.%d.%d", name_v1, &oct_v1[0], &oct_v1[1], &oct_v1[2]))
            return VERSION_COMPARE::INVALID_STRING_SYNTAX;
        if (4 != sscanf(v2, "%s-%d.%d.%d", name_v2, &oct_v2[0], &oct_v2[1], &oct_v2[2]))
            return VERSION_COMPARE::INVALID_STRING_SYNTAX;
        if (strcmp(name_v1, name_v2) != 0)
            return VERSION_COMPARE::WRONG_BASE_NAME;
        for (i = 0; i < 3; i++)
        {
            if (oct_v1[i] > oct_v2[i])
                return VERSION_COMPARE::V1_GT_V2;
            else if (oct_v1[i] < oct_v2[i])
                return VERSION_COMPARE::V1_LT_V2;
        }
        return VERSION_COMPARE::V1_EQ_V2;
    }

public:
    void InitAndRun()
    {
        xTaskCreate(Updater::updaterTask, "updaterTask", 4096 * 4, this, 0, NULL);
    }

    void TriggerManualUpdate()
    {
        this->manualUpdateRequested = true;
    }
};
#undef TAG
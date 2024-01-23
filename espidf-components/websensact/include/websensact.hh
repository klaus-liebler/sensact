#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_http_server.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <esp_timer.h>
#include <esp_chip_info.h>
#include <esp_mac.h>
#include <hal/efuse_hal.h>
#include <common-esp32.hh>
#include <interfaces.hh>
#include <algorithm>
#include <esp_log.h>
#include <esp_http_server.h>
#include <ctime>
#include <sys/time.h>

#define TAG "WBSNS"

namespace websensact
{
    extern const char websensact_html_gz_start[] asm("_binary_websensact_html_gz_start");
    extern const size_t websensact_html_gz_length asm("websensact_html_gz_length");
    constexpr char UNIT_SEPARATOR{0x1F};
    constexpr char RECORD_SEPARATOR{0x1E};
    constexpr char GROUP_SEPARATOR{0x1D};
    constexpr char FILE_SEPARATOR{0x1C};
    constexpr size_t STORAGE_LENGTH{16};

    class M;
    class M : public iWebsensact
    {
    private:
        static M *singleton;
        uint8_t *http_buffer{nullptr};
        size_t http_buffer_len{0};

        SemaphoreHandle_t xSemaphore{nullptr};

        M()
        {
            xSemaphore = xSemaphoreCreateBinary();
            xSemaphoreGive(xSemaphore);
        }

        static esp_err_t handle_websensact_get(httpd_req_t *req)
        {
            httpd_resp_set_type(req, "text/html");
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
            httpd_resp_send(req, websensact_html_gz_start, websensact_html_gz_length);
            return ESP_OK;
        }

        static esp_err_t handle_websocket(httpd_req_t *req)
        {
            httpd_resp_set_type(req, "text/html");
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
            httpd_resp_send(req, websensact_html_gz_start, websensact_html_gz_length);
            return ESP_OK;
        }

        static int logging_vprintf(const char *fmt, va_list l)
        {
            // Convert according to format
            char* buffer;
            int buffer_len = vasprintf(&buffer, fmt, l);
            // printf("logging_vprintf buffer_len=%d\n",buffer_len);
            // printf("logging_vprintf buffer=[%.*s]\n", buffer_len, buffer);
            if (buffer_len == 0){
                return 0;
            }
            
            // Send MessageBuffer
            // printf("logging_vprintf sended=%d\n",sended);

            httpd_

            // Write to stdout
            return vprintf(fmt, l);
        }

        static esp_err_t handle_websensact_post(httpd_req_t *req)
        {
            // Empfange ein Befehl vom Client und lege ihn in einer Warteschlange (länge=3?) hier im Objekt ab.
            // Der Befehl ist letztlich eine can-Nachricht mit einer id [u32] und data [8*u8]
            // der NodeManager kommt analog zum CAN-Bus regelmäßig vorbei und holt sich die Befehle ab und speist sie so ins system ein, als ob sie über den CAN-Bus reinkämen

            return ESP_OK;
        }

    public:
        static M *GetSingleton()
        {
            if (!singleton)
            {
                singleton = new M();
            }
            return singleton;
        }

        esp_err_t Init(httpd_handle_t http_server, uint8_t *http_buffer, size_t http_buffer_len)
        {
            this->http_buffer = http_buffer;
            this->http_buffer_len = http_buffer_len;
            httpd_uri_t websensact_get = {"/websensact", HTTP_GET, handle_websensact_get, nullptr};
            httpd_uri_t websensact_post = {"/websensact", HTTP_POST, handle_websensact_post, nullptr};

            RETURN_ON_ERROR(httpd_register_uri_handler(http_server, &websensact_get));
            RETURN_ON_ERROR(httpd_register_uri_handler(http_server, &websensact_post));

            esp_log_set_vprintf(M::logging_vprintf);

            return ESP_OK;
        }

        ErrorCode TryReceiveWebMessage(sensact::WebMessage &m) override
        {
            return ErrorCode::OK;
        }
    };

}
#undef TAG

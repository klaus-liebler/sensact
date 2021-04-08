#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <esp_system.h>
#include "esp_spi_flash.h"

#include "esp_adc_cal.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <sys/param.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include "connect.h"
#include <esp_http_server.h>
#include "http_handlers.hh"
static const char *TAG = "main";

#include "ws2812_strip.hh"
#include "esp_log.h"
#include "hal_sensactOutdoorV1.hh"
#include "manager.hh"
#include "i2c_io.hh"
#include "i2c_mem.hh"

HAL *hal = new HAL_SensactOutdoorV1();
Manager *manager=new Manager(hal);
I2C_IO * const i2c_io = new I2C_IO();
I2C_MemoryEmulation i2c_mem(I2C_NUM_0, i2c_io);

extern "C"
{
    void app_main();
    void plcTask(void *);
}

void plcTask(void *pvParameters)
{
    ESP_LOGI(TAG, "plcTask started");
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    manager->Init();
    for(int i=0;i<3;i++)
    {
        hal->ColorizeLed(0, CRGB::DarkRed);
        hal->AfterLoop();
        vTaskDelay(150 / portTICK_PERIOD_MS);
        hal->ColorizeLed(0, CRGB::Black);
        hal->AfterLoop();
        vTaskDelay(150 / portTICK_PERIOD_MS);
    }
    hal->UnColorizeAllLed();
    
    ESP_LOGI(TAG, "manager main loop starts");
    while (true)
    {
        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        hal->BeforeLoop();
        manager->Loop();
        hal->AfterLoop();
    }
}


static const httpd_uri_t get_root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = handle_get_root,
    .user_ctx = manager,
};

static const httpd_uri_t put_webui = {
    .uri       = "/webui",
    .method    = HTTP_PUT,
    .handler   = handle_put_webui,
    .user_ctx = manager,
};

static const httpd_uri_t get_webui = {
    .uri       = "/webui",
    .method    = HTTP_GET,
    .handler   = handle_get_webui,
    .user_ctx = manager,
};


static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 12;
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting server!");
        esp_restart();
    }
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &get_root));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &put_webui));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &get_webui));
    return server;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void _lab_error_check_failed(ErrorCode rc, const char *file, int line, const char *function, const char *expression)
{
    printf("Error %d occured in File %s in line %d in expression %s", (int)rc, file, line, expression);
}

#ifdef NDEBUG
#define LAB_ERROR_CHECK(x) do {                                         \
        esp_err_t __err_rc = (x);                                       \
        (void) sizeof(__err_rc);                                        \
    } while(0)
#elif defined(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT)
#define LAB_ERROR_CHECK(x) do {                                         \
        esp_err_t __err_rc = (x);                                       \
        if (__err_rc != ESP_OK) {                                       \
            abort();                                                    \
        }                                                               \
    } while(0)
#else
#define LAB_ERROR_CHECK(x) do {                                         \
        ErrorCode __err_rc = (x);                                       \
        if (__err_rc != ErrorCode::OK) {                                       \
            _lab_error_check_failed(__err_rc, __FILE__, __LINE__,       \
                                    __ASSERT_FUNC, #x);                 \
        }                                                               \
    } while(0)
#endif

void app_main(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    
    printf("=======================================================\n");

    esp_log_level_set(TAG, ESP_LOG_INFO);
    LAB_ERROR_CHECK(hal->Init());


    static httpd_handle_t server;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(connect());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    server = start_webserver(); 

    //xTaskCreate(experimentTask, "experimentTask", 1024 * 2, NULL, 5, NULL);
    xTaskCreate(plcTask, "plcTask", 4096 * 4, NULL, 6, NULL);
    int i = 0;

    while (true)
    {
        printf("Start was %d seconds ago. Free heap: %d\n", i, esp_get_free_heap_size());
        i += 5;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

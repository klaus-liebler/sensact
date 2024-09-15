#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_system.h>
#include <spi_flash_mmap.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <sys/param.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_ota_ops.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <wifi_sta.hh>
#include <esp_http_server.h>
#include "http_handlers.hh"
#include <httpd_helper.hh>

#include "common_in_project.hh"

#include "rgbled.hh"
#include "esp_log.h"
#include "hal_sensactOutdoor20220821.hh"
#include "manager.hh"
//#include "i2c_io.hh"
//#include "i2c_mem.hh"
//#include "rx470c.hh"
#include "milight.hh"
#include "spiffs.hh"
#include <map>
#include "milight2manager.hh"
#include <sshd.hh>
#include <megacli.hh>
#include <otacommand.hh>

#define TAG "main"

EventGroupHandle_t tasksStoppedGroup{nullptr};
std::vector<IOSource *> ioSources{};
HAL*                   hal{nullptr};
Manager*               manager{nullptr};
MilightSensactOutdoor* milight2manager{nullptr};
Milight*               milight{nullptr};
uint8_t*               http_scatchpad{nullptr};
httpd_handle_t httpd_handle{nullptr};


namespace EVENT_BITS{
    constexpr int MANAGER_STOP_REQUEST{1};
    constexpr int MANAGER_STOPPED{2};
    constexpr int MILIGHT_STOP_REQUEST{4};
    constexpr int MILIGHT_STOPPED{8};
}
void FreeMemoryForOTA(){
    ESP_LOGI(TAG, "Requesting all Tasks to finish, Free Heap=%lu", esp_get_free_heap_size());
    xEventGroupSetBits(tasksStoppedGroup, EVENT_BITS::MANAGER_STOP_REQUEST | EVENT_BITS::MILIGHT_STOP_REQUEST);
    xEventGroupWaitBits(tasksStoppedGroup, EVENT_BITS::MANAGER_STOPPED | EVENT_BITS::MILIGHT_STOPPED, pdFALSE, pdTRUE, portMAX_DELAY);
    httpd_stop(httpd_handle);
    delete milight;
    delete milight2manager;
    delete manager;
    delete hal;
    heap_caps_free((void*)http_scatchpad);
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(TAG, "Free heap after deleting all objects %lu", esp_get_free_heap_size());
}

constexpr const char *hardcoded_example_host_key =
	R"foo("-----BEGIN OPENSSH PRIVATE KEY-----
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
BAUGBw==
-----END OPENSSH PRIVATE KEY-----)foo";

std::vector<sshd::User> USERS{
	{"klaus", "klaus", false},
	{"root", "root", true}};

extern "C" void app_main(void)
{

    
    esp_log_level_set(TAG, ESP_LOG_INFO);
    esp_log_level_set("wifi", ESP_LOG_WARN);
    ESP_ERROR_CHECK(SpiffsManager::Init());
    ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(WIFISTA::InitAndRun("smopla", "myosotis2020"));

    ESP_LOGI(TAG, "Init SSH CLI");
    OTACommand* ota_cmd = new OTACommand();
    std::vector<CLI::AbstractCommand *> custom_commands = {ota_cmd};
	CLI::MegaCli *cli = new CLI::MegaCli(true, custom_commands);
	cli->InitAndRunCli();
	sshd::SshDemon::InitAndRunSshD(hardcoded_example_host_key, cli, &USERS);
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(TAG, "Free heap after starting wifi and ssh %lu", esp_get_free_heap_size());
    
    tasksStoppedGroup = xEventGroupCreate();
    
    http_scatchpad=(uint8_t*)heap_caps_aligned_alloc(4, CONFIG_HTTP_SCRATCHPAD_SIZE, 0);
    hal = new HAL_SensactOutdoor();
    manager = new Manager{hal, ioSources, tasksStoppedGroup, EVENT_BITS::MANAGER_STOP_REQUEST};
    milight2manager = new MilightSensactOutdoor{manager};
    milight = new Milight{milight2manager, tasksStoppedGroup, EVENT_BITS::MILIGHT_STOP_REQUEST};


    ESP_LOGI(TAG, "Init milight");
    ESP_ERROR_CHECK(milight->SetupAndRun(HSPI_HOST, 1, PIN_SPI_CE, PIN_SPI_CSN, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SCLK));
    ESP_LOGI(TAG, "Init HAL");
    ERRORCODE_CHECK(hal->Setup());
    ERRORCODE_CHECK(manager->SetupAndRun());

    ESP_LOGI(TAG, "Starting Webserver");
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 12;
    config.global_user_ctx = http_scatchpad;
    ESP_ERROR_CHECK(httpd_start(&httpd_handle, &config));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/iostate", HTTP_PUT, handle_put_iostate, manager));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/iostate", HTTP_GET, handle_get_iostate, manager));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/iocfg", HTTP_GET, handle_get_iocfg, manager));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/iocfg", HTTP_PUT, handle_put_iocfg, manager));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/devctrl", HTTP_PUT, handle_put_devctrl, manager));
    ESP_ERROR_CHECK(RegisterHandler(httpd_handle, "/*", HTTP_GET, handle_get_common, manager));


    while (true)
    {
        //if(hal)hal->OutputOneLineStatus();
        ESP_LOGI(TAG, "Free heap %lu", esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

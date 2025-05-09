//#define HTTP
#define HTTPS

//c++ lib incudes
#include <cstdio>
#include <cstring>
#include <vector>

//FreeRTOS & Lwip includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//esp idf includes
#include <esp_system.h>
#include <esp_log.h>
#include <esp_littlefs.h>
#include <spi_flash_mmap.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"

#ifdef HTTPS
#include <esp_https_server.h>
#ifndef CONFIG_ESP_HTTPS_SERVER_ENABLE
#error "Enable HTTPS_SERVER in menuconfig!"
#endif
#endif
#ifdef HTTP
#include <esp_http_server.h>
#endif

//klaus-liebler component components
#include <common-esp32.hh>
#include <adc_buttons.hh>
#include <webmanager.hh>

//sensact in
#include <sensact_projectconfig.hh>
#include <can_message_builder_parser.hh>
#include <nodemaster.hh>
#include <busmaster.hh>
#include <model_node.hh>
#include <runtimeconfig_cpp/runtimeconfig.hh>
#include <webmanager_plugins/systeminfo_plugin.hh>


constexpr const char* NVS_PARTITION_NAME{NVS_DEFAULT_PART_NAME};

//board specific includes
#include <hal.inc>
#include <cApplications.hh>

#define TAG "main"
using namespace sensact;


iHAL* halobj{nullptr};

constexpr auto BUFFER_SIZE{1024};
uint8_t buffer[BUFFER_SIZE];

httpd_handle_t http_server{nullptr};

FLASH_FILE(esp32_pem_crt);
FLASH_FILE(esp32_pem_key);


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "\n%s", cfg::BANNER);
    ESP_LOGI(TAG, "%s is booting up. Firmware build at %s on Git %s", cfg::NODE_ID, cfg::CREATION_DT_STR, cfg::GIT_SHORT_HASH);
    
    // Configure NVS and SPIFFS
    size_t total = 0, used = 0;
    esp_vfs_littlefs_conf_t conf = {"/spiffs", "storage", nullptr, 0,0,0,0};
    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&conf));
    ESP_ERROR_CHECK(esp_littlefs_info(conf.partition_label, &total, &used));
    ESP_LOGI(TAG, "LittleFS Partition successfully mounted: total: %dbyte, used: %dbyte", total, used);
    ESP_ERROR_CHECK(nvs_flash_init_and_erase_lazily(NVS_PARTITION_NAME));

    //Configure USB
    ESP_LOGI(TAG, "USB initialization");

    tinyusb_config_t tusb_cfg = {};
    tusb_cfg.device_descriptor = nullptr;
    tusb_cfg.string_descriptor = nullptr;
    tusb_cfg.external_phy = false; // In the most cases you need to use a `false` value,
    tusb_cfg.configuration_descriptor = nullptr;
    tusb_cfg.self_powered=true;
    tusb_cfg.vbus_monitor_io=sensact::hal::SensactOutdoor::P::VBUS_SENSE; 
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {}; // the configuration uses default values

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

    ESP_LOGI(TAG, "USB initialization DONE");

    //Install Temperature sensor
    //Temperature Sensor is used in generic hal for generic use and is used in the SystemPlugin
    temperature_sensor_handle_t tempHandle;
    temperature_sensor_config_t temp_sensor_config = {-10, 80, TEMPERATURE_SENSOR_CLK_SRC_DEFAULT, {0}};
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &tempHandle));
    ESP_ERROR_CHECK(temperature_sensor_enable(tempHandle));
    
    std::vector<webmanager::iWebmanagerPlugin*> plugins;
    plugins.push_back(new SystemInfoPlugin(tempHandle));

    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }
    
    #include <station_config_hardware.inc>
    assert(halobj);

    cNodemaster* nodemaster = new cNodemaster(halobj, &busmasters, canMBP);
    std::vector<sensact::iHost *> hosts;
    
    sensact::cApplicationHost* applicationHost{nullptr};

    #include <station_config_hosts.inc>
    if(applicationHost!=nullptr){
        plugins.push_back(applicationHost);
    }
    nodemaster->Setup(hosts);
    
    //Configure Network
    webmanager::M* wm = webmanager::M::GetSingleton();
    ESP_ERROR_CHECK(wm->Begin(cfg::NODE_ID, "sensact1", cfg::NODE_ID, false, &plugins, true, false));
    
    const char *hostname = wm->GetHostname();
    #ifdef HTTPS
    httpd_ssl_config_t httpd_conf = HTTPD_SSL_CONFIG_DEFAULT();
    httpd_conf.servercert = esp32_pem_crt_start;
    httpd_conf.servercert_len = esp32_pem_crt_end-esp32_pem_crt_start;
    httpd_conf.prvtkey_pem = esp32_pem_key_start;
    httpd_conf.prvtkey_len = esp32_pem_key_end-esp32_pem_key_start;
    httpd_conf.httpd.uri_match_fn = httpd_uri_match_wildcard;
    httpd_conf.httpd.max_uri_handlers = 15;
    ESP_ERROR_CHECK(httpd_ssl_start(&http_server, &httpd_conf));
    ESP_LOGI(TAG, "HTTPS Server listening on https://%s:%d", hostname, httpd_conf.port_secure);
#elif defined(HTTP)
    httpd_config_t httpd_conf = HTTPD_DEFAULT_CONFIG();
    httpd_conf.uri_match_fn = httpd_uri_match_wildcard;
    httpd_conf.max_uri_handlers = 15;
    ESP_ERROR_CHECK(httpd_start(&http_server, &httpd_conf));
    ESP_LOGI(TAG, "HTTP Server (not secure!) listening on http://%s:%d", hostname, httpd_conf.server_port);
#else
    #error "Either define HTTP or HTTPS"
#endif
    
    wm->RegisterHTTPDHandlers(http_server);
    wm->CallMeAfterInitializationToMarkCurrentPartitionAsValid();
    
    nodemaster->RunEternalLoopInTask();
    
    int counter{0};
    while (true)
    {
        wm->Supervise();
        if (++counter % 10 == 0)
        {
            ESP_LOGI(TAG, "Heap %6lu nodemaster tells:\n %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}

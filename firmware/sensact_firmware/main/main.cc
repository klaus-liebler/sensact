//c++ lib incudes
#include <cstdio>
#include <cstring>
#include <vector>
//FreeRTOS & Lwip includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//esp idf includes
#include <esp_system.h>
#include <spi_flash_mmap.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_log.h>

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
    ESP_ERROR_CHECK(nvs_flash_init_and_erase_lazily(NVS_PARTITION_NAME));
    std::vector<webmanager::iWebmanagerPlugin*> plugins;
    plugins.push_back(new SystemInfoPlugin());

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
    
    webmanager::M* wm = webmanager::M::GetSingleton();

    //TODO: Der "Nodemaster" ist das "SensactPlugin" und muss auch eingehängt werden
    //TODO: Ersetze Hostname-Pattern mit NodeID
    ESP_ERROR_CHECK(wm->Begin("sensact%02x%02x%02x", "sensact1", "sensact%02x%02x%02x", false, &plugins, true));
    
    const char *hostname = wm->GetHostname();
    httpd_ssl_config_t httpd_conf = HTTPD_SSL_CONFIG_DEFAULT();
    httpd_conf.servercert = esp32_pem_crt_start;
    httpd_conf.servercert_len = esp32_pem_crt_end-esp32_pem_crt_start;
    httpd_conf.prvtkey_pem = esp32_pem_key_start;
    httpd_conf.prvtkey_len = esp32_pem_key_end-esp32_pem_key_start;
    httpd_conf.httpd.uri_match_fn = httpd_uri_match_wildcard;
    httpd_conf.httpd.max_uri_handlers = 15;
    ESP_ERROR_CHECK(httpd_ssl_start(&http_server, &httpd_conf));
    ESP_LOGI(TAG, "HTTPS Server listening on https://%s:%d", hostname, httpd_conf.port_secure);
    
    //Idee: Wenn bis hierher etwas schief läuft, dann wird eh resettet
    wm->CallMeAfterInitializationToMarkCurrentPartitionAsValid();
    
    nodemaster->RunEternalLoopInTask();
    
    
    while (true)
    {
        ESP_LOGI(TAG, "Heap %6lu nodemaster tells:\n %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

#include <stdio.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <spi_flash_mmap.h>
 
#include <esp_mac.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <string.h>
#include <common-esp32.hh>
#include "lvgl.h"
#include "ui/ui.h"
#include <adc_buttons.hh>
#include "lcd_manager.hh"

#include <lwip/err.h>
#include <lwip/sys.h>
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "mdns.h"
#include "netdb.h"
#include <hwinc.inc>
#include "nodemaster.hh"
#include "busmaster.hh"
#include "can_message_builder_parser.hh"
#include "sensact_projectconfig.hh"
#include <webmanager.hh>
#include <websensact.hh>
#include <model_node.hh>

#define TAG "main"
using namespace sensact;


iHAL* halobj{nullptr};





constexpr auto BUFFER_SIZE{1024};
uint8_t buffer[BUFFER_SIZE];

extern "C" void app_main(void)
{
    //esp_log_level_set("gdma", ESP_LOG_VERBOSE);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    httpd_handle_t http_server{nullptr};
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	ESP_ERROR_CHECK(httpd_start(&http_server, &config));
    
    webmanager::M* wman= webmanager::M::GetSingleton();
    ESP_ERROR_CHECK(wman->Init(http_server, buffer, BUFFER_SIZE));
    
    websensact::M* wwws = websensact::M::GetSingleton();
    
    ESP_ERROR_CHECK(nvs_flash_init_and_erase_lazily());

    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }

    char   *hostname;
    asprintf(&hostname, "%s", sensact::model::node::NodeName);
    
    #include <hwcfg.inc>
    if(!halobj){
        ESP_LOGE(TAG, "HAL has not been created in <hwcfg.inc>");
        esp_restart();
    }

    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", hostname);
    const char* MDNS_INSTANCE="SENSACT_MDNS_INSTANCE";
    ESP_ERROR_CHECK(mdns_instance_name_set(MDNS_INSTANCE));
    ESP_ERROR_CHECK(mdns_service_subtype_add_for_host("SENSACT-WebServer", "_http", "_tcp", NULL, "_server") );
    free(hostname);
    
    cNodemaster* nodemaster = new cNodemaster(&nodeRoles, halobj, wwws, &busmasters, canMBP);
    nodemaster->Setup();

    //Idee: Wenn bis hierher etwas schief läuft, dann wird eh resettet
    wman->CallMeAfterInitializationToMarkCurrentPartitionAsValid();
    
    nodemaster->RunEternalLoopInTask();
    
    
    while (true)
    {
        ESP_LOGI(TAG, "Heap %6lu nodemaster tells:\n %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

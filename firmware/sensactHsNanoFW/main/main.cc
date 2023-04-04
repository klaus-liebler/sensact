#include <stdio.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <spi_flash_mmap.h>
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
#include <hwinc.inc>
#include "nodemaster.hh"
#include "busmaster.hh"
#include "can_message_builder_parser.hh"
#include "sensact_projectconfig.hh"

#define TAG "main"
using namespace sensact;

extern "C" void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    nvs_handle_t nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        while (true)
            ;
    }

    
    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }
    
    #include <hwcfg.inc>
    cNodemaster* nodemaster = new cNodemaster(&nodeRoles, hal, &busmasters, canMBP);
    nodemaster->RunEternalLoopInTask();

    while (true)
    {
        ESP_LOGI(TAG, "Heap %6lu nodemaster tells: %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

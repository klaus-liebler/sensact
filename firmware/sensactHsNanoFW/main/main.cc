#include <stdio.h>
#include <vector>
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
#include "hal/hal_sensactHsNano2_L3.hh"
#include "nodemaster.hh"
#include "busmaster.hh"
#include "can_message_builder_parser.hh"
#include "common_projectconfig.hh"

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

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK)
    {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
        ESP_LOGI(TAG, "Running project name: %s", running_app_info.project_name);
    }
    else
    {
        ESP_LOGE(TAG, "Error while getting firmware info");
    }

    xSemaphoreTake(connectSemaphore, portMAX_DELAY);
    ESP_LOGI(TAG, "Semaphore for connection is taken from main thread");
    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }
    iHAL* hal = new sensact::hal::SensactHsNano2::L3::cHAL();
    //TODO Busmasters auch in den Namespace packen
    PCA9555Device pca9555_0(SensactHsNano2::I2C_EXTERNAL, PCA9555::Device::Dev0);
    PCA9685Device pca9685_0(SensactHsNano2::I2C_EXTERNAL, PCA9685::Device::Dev00, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::OpenDrain);
    std::vector<sensact::InOut16 *> inOuts16{&pca9555_0, &pca9685_0};
    std::vector<sensact::AbstractSubBusmaster *> subbusses{};
    I2CBusmaster i2cBusmaster("SensactBus", SensactHsNano2::I2C_EXTERNAL, SensactHsNano2::INTERRUPT_LINES, inOuts16, subbusses);
    std::vector<AbstractBusmaster*> busmasters{&i2cBusmaster};
    
    cNodemaster* nodemaster = new cNodemaster(hal::SensactHsNano2::L3::nodeRoles, hal, busmasters, canMBP);
    nodemaster->RunEternalLoopInTask();
    int secs = 0;
    while (true)
    {
        ESP_LOGI(TAG, "Run %4d, Heap %6d", secs, esp_get_free_heap_size());
        secs += 5;
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

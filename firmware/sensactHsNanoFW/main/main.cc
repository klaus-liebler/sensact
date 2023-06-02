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
#include <common-esp32.hh>
#include "lvgl.h"
#include "ui/ui.h"
#include "encoder_adc.hh"
#include "lcd_manager.hh"

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
#include <messagelog.hh>
#include <webmanager.hh>

#define TAG "main"
using namespace sensact;

lv_group_t *groupHome{nullptr};
lv_group_t *groupSettings{nullptr};
lv_indev_t *encoder_indev{nullptr};
iHAL* halobj{nullptr};

extern "C" void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static bool previousIsPressed{false};
    data->enc_diff = EncoderADC::GetChangesSinceLastCallOfThisFunction();
    data->state = EncoderADC::GetButtonPressed() ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    if (data->enc_diff != 0)
        //ESP_LOGI(TAG, "Encoder %i", data->enc_diff);
    if (data->state == LV_INDEV_STATE_PRESSED && !previousIsPressed)
    {
        //ESP_LOGI(TAG, "Encoder pressed");
        previousIsPressed = true;
    }
    if (data->state == LV_INDEV_STATE_RELEASED)
        previousIsPressed = false;
}

constexpr auto BUFFER_SIZE{1024};
uint8_t buffer[BUFFER_SIZE];

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    httpd_handle_t http_server{nullptr};
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	ESP_ERROR_CHECK(httpd_start(&http_server, &config));
    
    messagelog::M* ml= messagelog::M::GetSingleton();
    ESP_ERROR_CHECK(ml->Init(http_server, buffer, BUFFER_SIZE));
    ESP_ERROR_CHECK(webmanager::Init(http_server, buffer, BUFFER_SIZE));
    
    ESP_ERROR_CHECK(nvs_flash_init_and_erase_lazily());

    LCD::InitLCD();
    EncoderADC::InitEncoder(GPIO_NUM_2, true); 
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_ENCODER; /*See below.*/
    indev_drv.read_cb = encoder_read;       /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    encoder_indev = lv_indev_drv_register(&indev_drv);
    assert(encoder_indev!=NULL);

    
    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }
    
    #include <hwcfg.inc>
    if(!halobj){
        ESP_LOGE(TAG, "HAL has not been created in <hwcfg.inc>");
        esp_restart();
    }
    groupHome = lv_group_create();
    lv_group_add_obj(groupHome, ui_sldVolume);
    lv_group_add_obj(groupHome, ui_btnNext);
    lv_indev_set_group(encoder_indev, groupHome);
    groupSettings = lv_group_create();
    lv_group_add_obj(groupSettings, ui_btnPrev);
    LCD::StartLVGLTask();
    
    
    cNodemaster* nodemaster = new cNodemaster(&nodeRoles, halobj, &busmasters, canMBP);
    nodemaster->Setup();
    //Idee: Wenn bis hierher etwas schief läuft
    webmanager::CallMeAfterInitializationToMarkCurrentPartitionAsValid();
    nodemaster->RunEternalLoopInTask();

    while (true)
    {
        ESP_LOGI(TAG, "Heap %6lu nodemaster tells: %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

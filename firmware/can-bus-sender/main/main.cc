#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include <vector>
#include <array>
#include <cstring>
#include "can_message_builder_parser.hh"


constexpr twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
constexpr twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_50KBITS();
constexpr twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_27, GPIO_NUM_26, TWAI_MODE_NORMAL);
sensact::aCANMessageBuilderParser* canMBP = new sensact::cCANMessageBuilderParserOld();
constexpr gpio_num_t BUTTON_GPIO{GPIO_NUM_0};


#define TAG "SENDER"

constexpr auto RX_TASK_PRIO{11};



extern "C" void app_main(void)
{
    sensact::CANMessage txMessage;
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAG, "CAN Driver installed and started");
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);

    uint8_t cmd[]{1,2,3,4};
    bool messageSent{false};
    while(true){
        
        if(gpio_get_level(BUTTON_GPIO)){
            messageSent=false;
        }
        else if(!messageSent){
            if(canMBP->BuildApplicationCommandMessage(23, 43, cmd, 4, txMessage)!=ErrorCode::OK){
                ESP_LOGE(TAG, "Error while canMBP->BuildApplicationCommandMessage");
            }
            twai_message_t twaiMessage;
            twaiMessage.identifier = txMessage.Id;
            twaiMessage.extd = 1;
            twaiMessage.data_length_code = txMessage.DataLen;
            std::memcpy(twaiMessage.data, txMessage.Data, txMessage.DataLen);
            twai_status_info_t status_info;
            twai_get_status_info(&status_info);
            ESP_LOGI(TAG, "Sending Message with id %d and dlc %d. Current status is %s with tx_queue=%lu ",(int)twaiMessage.identifier, twaiMessage.data_length_code, status_info.state==twai_state_t::TWAI_STATE_RUNNING?"RUNNING":"STOPPED", status_info.msgs_to_tx);
            ESP_ERROR_CHECK(twai_transmit(&twaiMessage, portMAX_DELAY));
            messageSent=true;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }


}

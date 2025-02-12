#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#define TAG "CANMON"
#include <common.hh>
namespace CANMONITOR
{
   
    constexpr twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    class iCanmonitorHandler{
        public:
        virtual void HandleCanMessageReceived(uint32_t messageId, uint8_t data[8], size_t dataLen)=0;
    };


    class M
    {
    private:

        iCanmonitorHandler* handler;
     
        static void static_task(void *args)
        {
            M *myself = static_cast<M *>(args);
            myself->task();
        }

        void task()
        {      
            ESP_LOGI(TAG, "Start Can Receive");
            while (true) {
                twai_message_t rx_msg;
                ESP_ERROR_CHECK(twai_receive(&rx_msg, portMAX_DELAY));
                this->handler->HandleCanMessageReceived(rx_msg.identifier, rx_msg.data, rx_msg.data_length_code);
            }
            // ESP_LOGI(TAG, "Start Can Send");
            // twai_message_t tx_msg={};
            // tx_msg.identifier=0x00AA;
            // tx_msg.extd=1;
            // tx_msg.data[0]=42;
            // tx_msg.data_length_code=1;
            // while (true) {
            //     if(twai_transmit(&tx_msg, portMAX_DELAY)!=ESP_OK){
            //         ESP_LOGE(TAG, "twai_transmit(&tx_msg, portMAX_DELAY)!=ESP_OK");
            //     }else{
            //         ESP_LOGE(TAG, "twai_transmit OK");
            //     }
            //     tx_msg.data[0]++;
            //     delayMs(1000);
            // }
        }

    public:
        M(iCanmonitorHandler* handler) :handler(handler) {}
        
        void Begin(const twai_timing_config_t* t_config, const twai_general_config_t* g_config)
        {
            //Install and start TWAI driver
            ESP_ERROR_CHECK(twai_driver_install(g_config, t_config, &f_config));
            ESP_ERROR_CHECK(twai_start());
            ESP_LOGI(TAG, "Can Driver installed and started");
            xTaskCreatePinnedToCore(M::static_task, "TWAI_rx", 4096, this, 6, nullptr, tskNO_AFFINITY);
            return;
        }
    };

}
#undef TAG
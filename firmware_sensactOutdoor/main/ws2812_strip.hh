#pragma once
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "driver/rmt.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "crgb.hh"

constexpr uint32_t counter_clk_hz = 40000000;

constexpr uint32_t WS2812_T0H_NS =(350);
constexpr uint32_t WS2812_T0L_NS =(1000);
constexpr uint32_t WS2812_T1H_NS =(1000);
constexpr uint32_t WS2812_T1L_NS =(350);
constexpr uint32_t WS2812_RESET_US =(280);

// ns -> ticks
constexpr float ratio = (float)counter_clk_hz / 1e9;
constexpr uint32_t ws2812_t0h_ticks = (uint32_t)(ratio * WS2812_T0H_NS);
constexpr uint32_t ws2812_t0l_ticks = (uint32_t)(ratio * WS2812_T0L_NS);
constexpr uint32_t ws2812_t1h_ticks = (uint32_t)(ratio * WS2812_T1H_NS);
constexpr uint32_t ws2812_t1l_ticks = (uint32_t)(ratio * WS2812_T1L_NS);

/**
 * @brief Convert RGB data to RMT format.
 *
 * @note For WS2812, R,G,B each contains 256 different choices (i.e. uint8_t)
 *
 * @param[in] src: source data, to converted to RMT format
 * @param[in] dest: place where to store the convert result
 * @param[in] src_size: size of source data
 * @param[in] wanted_num: number of RMT items that want to get
 * @param[out] translated_size: number of source data that got converted
 * @param[out] item_num: number of RMT items which are converted from source data
 */
static void IRAM_ATTR ws2812_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size, size_t wanted_num, size_t *translated_size, size_t *item_num)
{
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ ws2812_t0h_ticks, 1, ws2812_t0l_ticks, 0 }}}; //Logical 0
    const rmt_item32_t bit1 = {{{ ws2812_t1h_ticks, 1, ws2812_t1l_ticks, 0 }}}; //Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pdest->val =  bit1.val;
            } else {
                pdest->val =  bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}

template <size_t LEDSIZE>
class WS2812_Strip{
private:
    const char *TAG = "WS2812_Strip";
    uint8_t buffer[3*LEDSIZE];
    rmt_channel_t channel;
public:
    WS2812_Strip(rmt_channel_t channel):channel(channel){
        memset(this->buffer, 0, 3*LEDSIZE);
    }

    esp_err_t SetPixel(size_t index, CRGB color){
        index=index%LEDSIZE;
        this->buffer[3*index+0]=color.g;
        this->buffer[3*index+1]=color.r;
        this->buffer[3*index+2]=color.b;
        return ESP_OK;
    }


    esp_err_t Refresh(uint32_t timeout_ms){
        ESP_ERROR_CHECK(rmt_write_sample(this->channel, this->buffer, 3*LEDSIZE, true));
        return rmt_wait_tx_done(this->channel, pdMS_TO_TICKS(timeout_ms));
    }

    esp_err_t Clear(uint32_t timeout_ms){
        memset(this->buffer, 0, 3*LEDSIZE);
        return Refresh(timeout_ms);
    }
   
    esp_err_t Init(gpio_num_t gpio){
        rmt_config_t config;
        config.rmt_mode=RMT_MODE_TX;
        config.channel=this->channel;
        config.gpio_num=gpio;
        config.clk_div=2;
        config.mem_block_num=1;
        config.tx_config.carrier_freq_hz=38000;
        config.tx_config.carrier_level=RMT_CARRIER_LEVEL_HIGH;
        config.tx_config.idle_level=RMT_IDLE_LEVEL_LOW;
        config.tx_config.carrier_duty_percent=33,
        config.tx_config.carrier_en=false;
        config.tx_config.loop_en=false;
        config.tx_config.idle_output_en=true;  
        ESP_ERROR_CHECK(rmt_config(&config));
        ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
        rmt_translator_init(this->channel, ws2812_rmt_adapter);
        return ESP_OK;
    }
};
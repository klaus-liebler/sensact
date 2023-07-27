#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "ui/ui.h"
#define TAG "LCD"

namespace LCD
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    static lv_disp_t *disp{nullptr};
    static SemaphoreHandle_t xSemaphore{nullptr};
    static bool backlightOn{true};
    ledc_channel_config_t ledc_channel = {};

    static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
    {
        lv_disp_flush_ready((lv_disp_drv_t *)user_ctx);
        return false;
    }

    static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
    {
        esp_lcd_panel_draw_bitmap((esp_lcd_panel_handle_t)drv->user_data, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
    }
    constexpr TickType_t TASK_PERIOD_TICKS{pdMS_TO_TICKS(50)};

    void SemaphoreTake(){
         xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(1000));
    }

    void SemaphoreGive(){
        xSemaphoreGive(xSemaphore);
    }

    void Task(void* args){
        assert(disp);
        TickType_t xLastWakeTime=xTaskGetTickCount();
        while (1)
        {
            TickType_t tmp = xLastWakeTime;
            bool actuallyDelayed=xTaskDelayUntil( &xLastWakeTime, TASK_PERIOD_TICKS);
            if(!actuallyDelayed){
                vTaskDelay(1);
            }
            int ms = pdTICKS_TO_MS(xLastWakeTime-tmp);
            ESP_LOGD(TAG, "LCDManagerTask 1 ms=%i", ms);
            lv_tick_inc(ms);
            ESP_LOGD(TAG, "LCDManagerTask 2");
            xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(1000));
            ESP_LOGD(TAG, "LCDManagerTask 3");
            lv_timer_handler();
            ESP_LOGD(TAG, "LCDManagerTask 4");
            xSemaphoreGive(xSemaphore);
            ESP_LOGD(TAG, "LCDManagerTask 5");
            uint32_t inactive_ms = lv_disp_get_inactive_time(disp);
            
            if(inactive_ms>3000 && backlightOn){
                ESP_ERROR_CHECK(ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, 50, 500));
                ESP_ERROR_CHECK(ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT));
                backlightOn=false;
            }else if(inactive_ms<=3000 && !backlightOn){
                const uint32_t maxDuty = (1<<LEDC_TIMER_12_BIT) - 1;
                ESP_ERROR_CHECK(ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, maxDuty, 200));
                ESP_ERROR_CHECK(ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT));
                backlightOn=true;
            }
            
        }
    }
  

    void InitLCD(
        spi_host_device_t spiHost = SPI3_HOST,
        gpio_num_t mosi = GPIO_NUM_18,
        gpio_num_t sclk = GPIO_NUM_8,
        uint32_t frequency = 20'000'000,
        gpio_num_t cs = GPIO_NUM_48,
        gpio_num_t rst = GPIO_NUM_NC,
        gpio_num_t dc = GPIO_NUM_9,
        gpio_num_t backlight = GPIO_NUM_45,
        uint16_t horizontalResolution = 135,
        uint16_t horizontalGap = 52,
        uint16_t verticalResolution = 240,
        uint16_t verticalGap = 40,
        ledc_timer_t backlightTimer = LEDC_TIMER_0,
        int intr_flags=ESP_INTR_FLAG_LEVEL1
        )
    {

        xSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(xSemaphore);

        ESP_LOGI(TAG, "Initialize LCD");
        ledc_timer_config_t ledc_timer = {};
        ledc_timer.duty_resolution = LEDC_TIMER_12_BIT;
        ledc_timer.freq_hz = 5000;                   // frequency of PWM signal
        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE; // timer mode
        ledc_timer.timer_num = backlightTimer;       // timer index
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;          // Auto select the source clock
        ledc_timer_config(&ledc_timer);

        ledc_channel.channel = LEDC_CHANNEL_0;
        ledc_channel.duty = 0;
        ledc_channel.gpio_num = backlight;
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel.hpoint = 0;
        ledc_channel.timer_sel = backlightTimer;
        ledc_channel.flags.output_invert = 0;
        ledc_channel_config(&ledc_channel);

        ESP_ERROR_CHECK(ledc_fade_func_install(0));

        ESP_LOGD(TAG, "Initialize SPI bus");
        spi_bus_config_t buscfg = {};
        buscfg.sclk_io_num = sclk;
        buscfg.mosi_io_num = mosi;
        buscfg.miso_io_num = -1;
        buscfg.quadwp_io_num = -1;
        buscfg.quadhd_io_num = -1;
        buscfg.flags=0;
        buscfg.intr_flags=intr_flags;
        buscfg.max_transfer_sz = horizontalResolution * 80 * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(spiHost, &buscfg, SPI_DMA_CH_AUTO));

        ESP_LOGD(TAG, "Install panel IO");
        esp_lcd_panel_io_handle_t io_handle{nullptr};
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.dc_gpio_num = dc;
        io_config.cs_gpio_num = cs;
        io_config.pclk_hz = frequency;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        io_config.spi_mode = 0;
        io_config.trans_queue_depth = 10;
        io_config.on_color_trans_done = notify_lvgl_flush_ready;
        io_config.user_ctx = &disp_drv;
        // Attach the LCD to the SPI bus
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)spiHost, &io_config, &io_handle));

        ESP_LOGD(TAG, "Install  panel driver");
        esp_lcd_panel_handle_t panel_handle{nullptr};
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = rst;
        panel_config.rgb_endian = LCD_RGB_ENDIAN_RGB;
        panel_config.bits_per_pixel = 16;

        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, horizontalGap, verticalGap));
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, true));
        
    
        ESP_LOGD(TAG, "Initialize LVGL library");
        lv_init();
        // alloc draw buffers used by LVGL
        // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
        lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(horizontalResolution * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
        assert(buf1);
        lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(horizontalResolution * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
        assert(buf2);
        // initialize LVGL draw buffers
        lv_disp_draw_buf_init(&disp_buf, buf1, buf2, horizontalResolution * 20);

        ESP_LOGD(TAG, "Register display driver to LVGL");
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res = horizontalResolution;
        disp_drv.ver_res = verticalResolution;
        disp_drv.flush_cb = lvgl_flush_cb;
        disp_drv.drv_update_cb = nullptr;
        disp_drv.draw_buf = &disp_buf;
        disp_drv.user_data = panel_handle;
        disp =lv_disp_drv_register(&disp_drv);

        ESP_LOGD(TAG, "Turn on LCD backlight");
        const uint32_t maxDuty = (1<<LEDC_TIMER_12_BIT) - 1;
        ESP_ERROR_CHECK(ledc_set_fade_with_time(ledc_channel.speed_mode, ledc_channel.channel, maxDuty, 200));
        ESP_ERROR_CHECK(ledc_fade_start(ledc_channel.speed_mode, ledc_channel.channel, LEDC_FADE_NO_WAIT));
        ESP_LOGD(TAG, "Start Timer Task");

        ESP_LOGD(TAG, "UI init");
        ui_init();
        ESP_LOGD(TAG, "Display on");//vorher deutlich weiter oben, vor Initialize LVGL library
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
        return;
    }

    void StartLVGLTask(){
        xTaskCreate(Task, "lvgl timer task", 4*4096, nullptr, 2, nullptr);
    }
}
#undef TAG
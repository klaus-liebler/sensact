#pragma once
#include "common.hh"
#include <limits>
#include <cmath>
#include <array>

#include <driver/mcpwm.h>
#include <driver/ledc.h>
#include <driver/i2c.h>
#include <driver/rmt.h>

#include "applicationBase.hh"
#include "ws2812_strip.hh"
#include <bh1750.hh>
#include <bme280.hh>
#include <owb.h>
#include <owb_rmt.h>
#include <ds18b20.h>
#include <i2c.hh>
#include <esp_log.h>
#define TAG "HAL"

typedef gpio_num_t Pintype;

static constexpr Pintype PIN_PROG_IO0 = (Pintype)0;
static constexpr Pintype PIN_PROG_TX = (Pintype)1;
static constexpr Pintype PIN_PROG_RX = (Pintype)3;

constexpr Pintype PIN_I2C_SCL = (Pintype)19;
constexpr Pintype PIN_I2C_SDA = (Pintype)23;

static constexpr Pintype PIN_PWM3 = (Pintype)4;
static constexpr Pintype PIN_PWM1 = (Pintype)0;
static constexpr Pintype PIN_PWM2 = (Pintype)2;

//static constexpr Pintype PIN_ONEWIRE = (Pintype)26;
//static constexpr Pintype PIN_LED_STRIP = (Pintype)5;


static constexpr std::array<Pintype, 0> RELAY_PINS{};
static constexpr std::array<Pintype, 3> PWM_PINS{PIN_PWM1, PIN_PWM2, PIN_PWM3};


static constexpr rmt_channel_t CHANNEL_WS2812 = RMT_CHANNEL_0;
static constexpr rmt_channel_t CHANNEL_ONEWIRE_TX = RMT_CHANNEL_1;
static constexpr rmt_channel_t CHANNEL_ONEWIRE_RX = RMT_CHANNEL_2;
static constexpr i2c_port_t I2C_PORT = I2C_NUM_1;

static constexpr auto PWM_RESOLUTION = LEDC_TIMER_10_BIT;
static constexpr uint32_t PWM_FREQ = 500;



extern "C" void sensorTask(void *pvParameters);

constexpr size_t LEDSIZE=1;
class HAL_WroverKitV41 : public HAL
{
private:
    bool needLedStripUpdate = false;
    //WS2812_Strip<LEDSIZE> *strip = NULL;

public:
    HAL_WroverKitV41() {}

    ErrorCode ConfigureIO(uint16_t pinId, IOMode mode){
        //HAL_WroverKitV3 IOs are not configurable!
        return ErrorCode::OK;
    }

    size_t GetPinCnt(){
        return 16;
    }

    ErrorCode GetBoolInputs(uint32_t *ptr){
        //HAL_WroverKitV3 IOs has no inputs
        *ptr=0xAAAAAAAA;
        return ErrorCode::OK;
    }
    ErrorCode SetU16Output(uint16_t pinId, uint16_t state) override
    {
        //1=R, 2=G, 3=B
        if(pinId==0) return ErrorCode::OK;
        
        if(pinId>pins::wroverkit::PIN_MAX){
            ESP_LOGI(TAG, "Set output %d to %d", pinId, state);
            return ErrorCode::OK;    
        }
        uint16_t index=pinId-1;
        uint32_t duty = state>>(16-PWM_RESOLUTION);
        if(duty==(1<<PWM_RESOLUTION)-1){
            duty=1<<PWM_RESOLUTION;
        }
        ESP_LOGI(TAG, "Set output %d to duty %d", pinId, duty);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)index, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)index);
        return ErrorCode::OK;

    }
    
    ErrorCode ColorizeLed(uint8_t index, CRGB color) override
    {
        CRGB colorCRGB(color);
        //strip->SetPixel((uint8_t)index, colorCRGB);
        //TODO: Hier Prüfung, ob sich tatsächlich etwas verändert hat und ein Update tatsächlich erforderlich ist
        this->needLedStripUpdate = true;
        return ErrorCode::OK;
    }

    ErrorCode UnColorizeAllLed() override
    {
        //strip->Clear(1000);
        this->needLedStripUpdate = true;
        return ErrorCode::OK;
    }

    int64_t IRAM_ATTR GetMicros() override
    {
        return esp_timer_get_time();
    }

    uint32_t GetMillis() override
    {
        return (uint32_t)(esp_timer_get_time() / 1000ULL);
    }

    int64_t GetMillis64() override
    {
        return esp_timer_get_time() / 1000ULL;
    }

    void SensorLoop_ForInternalUseOnly()
    {
        while (true)
        { 
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    ErrorCode GetAirTemperature(float *degreesCelcius)
    {
        return ErrorCode::NONE_AVAILABLE;
    }

    ErrorCode GetAirPressure(float *pa)
    {
        return ErrorCode::NONE_AVAILABLE;
    }
    ErrorCode GetAirRelHumidity(float *percent)
    {
        return ErrorCode::NONE_AVAILABLE;
    }

    ErrorCode GetAmbientBrightness(float *lux)
    {
        return ErrorCode::NONE_AVAILABLE;
    }

    ErrorCode Setup() override
    {
        for (const auto &pin : RELAY_PINS)
        {
            gpio_set_level(pin, 0);
            gpio_pad_select_gpio((uint8_t)pin);
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
            gpio_set_pull_mode(pin, GPIO_FLOATING);
        }

        for (const auto &pin : PWM_PINS)
        {
            gpio_set_level(pin, 0);
            gpio_pad_select_gpio((uint8_t)pin);
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
            gpio_set_pull_mode(pin, GPIO_FLOATING);
        }

        //PWM Output
        ledc_timer_config_t ledc_timer;
        ledc_timer.duty_resolution = PWM_RESOLUTION; // resolution of PWM duty
        ledc_timer.freq_hz = PWM_FREQ;                 // frequency of PWM signal
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
        
        ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;            // timer mode
        ledc_timer.timer_num = LEDC_TIMER_0;                     // timer index
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;            // timer mode
        ledc_timer.timer_num = LEDC_TIMER_1;                     // timer index
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        for (size_t i = 0; i < PWM_PINS.size(); i++)
        {
            ledc_channel_config_t ledc_channel;
            ledc_channel.channel = (ledc_channel_t)i;
            ledc_channel.duty = 0;
            ledc_channel.gpio_num = PWM_PINS[i];
            ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
            ledc_channel.hpoint = 0;
            ledc_channel.timer_sel = LEDC_TIMER_0;
            ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
        }


        //LED Strip
        //strip = new WS2812_Strip<LEDSIZE>(CHANNEL_WS2812);
       // ESP_ERROR_CHECK(strip->Init(PIN_LED_STRIP));
        //ESP_ERROR_CHECK(strip->Clear(100));
        //xTaskCreate(sensorTask, "sensorTask", 4096 * 4, this, 6, NULL);
        return ErrorCode::OK;
    }

    ErrorCode BeforeLoop()
    {

        return ErrorCode::OK;
    }

    ErrorCode AfterLoop()
    {
        if (needLedStripUpdate)
        {
            //strip->Refresh(100);
            needLedStripUpdate = false;
        }
        return ErrorCode::OK;
    }
};

void sensorTask(void *pvParameters)
{
    HAL_WroverKitV41 *hal = (HAL_WroverKitV41 *)pvParameters;
    hal->SensorLoop_ForInternalUseOnly();
}

#undef TAG
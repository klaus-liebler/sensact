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

//Outputs 1...6 sind die sechs Relays
//Outputs 7..11 sind die fünf MOSFETS
//Output 12 ist der Constant Current LED Treiber

#define TAG "HAL"

typedef gpio_num_t Pintype;

static constexpr Pintype PIN_MAINS = (Pintype)36;    //VP
static constexpr Pintype PIN_SPI_MISO = (Pintype)39; //VN
static constexpr Pintype PIN_SW = (Pintype)34;
static constexpr Pintype PIN_PROG_IO0 = (Pintype)0;
static constexpr Pintype PIN_PROG_TX = (Pintype)1;
static constexpr Pintype PIN_SPI_CE = (Pintype)2;
//static constexpr Pintype PIN_SPI_CSN = (Pintype)2; in V2
static constexpr Pintype PIN_PROG_RX = (Pintype)3;
static constexpr Pintype PIN_I2C_SCL = (Pintype)4;
static constexpr Pintype PIN_LED_STRIP = (Pintype)5;
static constexpr Pintype PIN_PWM5 = (Pintype)12;
static constexpr Pintype PIN_PWM_CC = (Pintype)13;
static constexpr Pintype PIN_CANR = (Pintype)14;
static constexpr Pintype PIN_SPI_SCLK = (Pintype)14;
static constexpr Pintype PIN_ONEWIRE = (Pintype)15;
static constexpr Pintype PIN_SPI_CSN = (Pintype)15;
//static constexpr Pintype PIN_SPI_CE = (Pintype)15; in V2
static constexpr Pintype PIN_I2C_SDA = (Pintype)16;
static constexpr Pintype PIN_K6 = (Pintype)17;
static constexpr Pintype PIN_K5 = (Pintype)18;
static constexpr Pintype PIN_K4 = (Pintype)19;
static constexpr Pintype PIN_K3 = (Pintype)21;
static constexpr Pintype PIN_K2 = (Pintype)22;
static constexpr Pintype PIN_K1 = (Pintype)23;
static constexpr Pintype PIN_PWM3 = (Pintype)25;
static constexpr Pintype PIN_PWM4 = (Pintype)26;
static constexpr Pintype PIN_CANT = (Pintype)27;
static constexpr Pintype PIN_SPI_MOSI = (Pintype)27;
static constexpr Pintype PIN_PWM1 = (Pintype)32;
static constexpr Pintype PIN_PWM2 = (Pintype)33;
static constexpr Pintype PIN_IRQ = (Pintype)35;

static constexpr std::array<Pintype, 6> RELAY_PINS{PIN_K1, PIN_K2, PIN_K3, PIN_K4, PIN_K5, PIN_K6};
static constexpr std::array<Pintype, 6> PWM_PINS{PIN_PWM1, PIN_PWM2, PIN_PWM3, PIN_PWM4, PIN_PWM5, PIN_PWM_CC};

static constexpr size_t LED_NUMBER = 1;
static constexpr rmt_channel_t CHANNEL_WS2812 = RMT_CHANNEL_0;
static constexpr rmt_channel_t CHANNEL_ONEWIRE_TX = RMT_CHANNEL_1;
static constexpr rmt_channel_t CHANNEL_ONEWIRE_RX = RMT_CHANNEL_2;
static constexpr rmt_channel_t CHANNEL_RX470C = RMT_CHANNEL_6;
static constexpr i2c_port_t I2C_PORT = I2C_NUM_1;

static constexpr auto PWM_RESOLUTION = LEDC_TIMER_10_BIT;
static constexpr uint32_t PWM_FREQ = 500;

extern "C" void sensorTask(void *pvParameters);

constexpr size_t LEDSIZE=1;
class HAL_SensactOutdoorV1 : public HAL
{
private:
    bool needLedStripUpdate = false;
    WS2812_Strip<LEDSIZE> *strip = NULL;
    float airTemperatureDegCel[2]{std::numeric_limits<float>::quiet_NaN()};
    float airPressurePa{std::numeric_limits<float>::quiet_NaN()};
    float airRelHumidityPercent{std::numeric_limits<float>::quiet_NaN()};
    float ambientBrightnessLux{std::numeric_limits<float>::quiet_NaN()};

public:
    HAL_SensactOutdoorV1() {}

    ErrorCode ConfigureIO(uint16_t io, IOMode mode){
        //SensactOutdoor IOs are not configurable!
        return ErrorCode::OK;
    }

    ErrorCode HardwareTest() override{

        ColorizeLed(0, CRGB::Red);
        ColorizeLed(1, CRGB::Red);
        ESP_LOGI(TAG, "WS2812 RED");
        strip->Refresh(100);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ColorizeLed(0, CRGB::Green);
        ColorizeLed(1, CRGB::Green);
        ESP_LOGI(TAG, "WS2812 Green");
        strip->Refresh(100);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ColorizeLed(0, CRGB::Blue);
        ColorizeLed(1, CRGB::Blue);
        ESP_LOGI(TAG, "WS2812 Blue");
        strip->Refresh(100);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY1");
        SetU16Output(pins::sensactOutdoor::RELAY1, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY2");
        SetU16Output(pins::sensactOutdoor::RELAY2, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY3");
        SetU16Output(pins::sensactOutdoor::RELAY3, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY4");
        SetU16Output(pins::sensactOutdoor::RELAY4, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY5");
        SetU16Output(pins::sensactOutdoor::RELAY5, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY6");
        SetU16Output(pins::sensactOutdoor::RELAY6, 1);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "ALL OFF");
        SetU16Output(pins::sensactOutdoor::RELAY1, 0);
        SetU16Output(pins::sensactOutdoor::RELAY2, 0);
        SetU16Output(pins::sensactOutdoor::RELAY3, 0);
        SetU16Output(pins::sensactOutdoor::RELAY4, 0);
        SetU16Output(pins::sensactOutdoor::RELAY5, 0);
        SetU16Output(pins::sensactOutdoor::RELAY6, 0);
        vTaskDelay(500/portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "MOSFET1");
        SetU16Output(pins::sensactOutdoor::MOSFET1, 32000);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET1 + 2");
        SetU16Output(pins::sensactOutdoor::MOSFET1, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET2, 32000);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET2 + 3");
        SetU16Output(pins::sensactOutdoor::MOSFET2, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET3, 32000);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET3 + 4");
        SetU16Output(pins::sensactOutdoor::MOSFET3, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET4, 32000);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET4 + 5");
        SetU16Output(pins::sensactOutdoor::MOSFET4, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET5, 32000);
        vTaskDelay(500/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET5 + LED_CC");
        SetU16Output(pins::sensactOutdoor::MOSFET5, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::LED_CC, 16000);
        vTaskDelay(250/portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, 32000);
        vTaskDelay(250/portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, UINT16_MAX);
        vTaskDelay(250/portTICK_PERIOD_MS);
         SetU16Output(pins::sensactOutdoor::LED_CC, 32000);
        vTaskDelay(250/portTICK_PERIOD_MS);
         SetU16Output(pins::sensactOutdoor::LED_CC, 16000);
        vTaskDelay(250/portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "All MOSFET off & LED_CC off");
        SetU16Output(pins::sensactOutdoor::MOSFET1, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET2, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET3, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET4, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET5, 0);
        SetU16Output(pins::sensactOutdoor::LED_CC, 0);
        return ErrorCode::OK;
    }

    ErrorCode GetBoolInputs(uint32_t *ptr){
        *ptr=0;
        return ErrorCode::OK;
    }
    ErrorCode SetU16Output(uint16_t pinId, uint16_t state) override
    {
        if(pinId==0) return ErrorCode::OK;

        if(pinId>=pins::sensactOutdoor::RELAY1 && pinId<=pins::sensactOutdoor::RELAY6){
            pinId-=pins::sensactOutdoor::RELAY1;
            gpio_set_level(RELAY_PINS[pinId], state); //alles !=0 wird als logisch 1 erkannt
            return ErrorCode::OK;
        }
        if(pinId>=pins::sensactOutdoor::MOSFET1 && pinId <=pins::sensactOutdoor::LED_CC){
            pinId-=pins::sensactOutdoor::MOSFET1;
            uint32_t duty = state>>(16-PWM_RESOLUTION);
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)pinId, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)pinId);
            return ErrorCode::OK;
        }
        return ErrorCode::NONE_AVAILABLE;
    }

    size_t GetPinCnt(){
        return pins::sensactOutdoor::PIN_MAX;
    }

    ErrorCode ColorizeLed(uint8_t index, CRGB color) override
    {
        if(!strip) return ErrorCode::TEMPORARYLY_NOT_AVAILABLE;
        strip->SetPixel((uint8_t)index, color);
        //TODO: Hier Prüfung, ob sich tatsächlich etwas verändert hat und ein Update tatsächlich erforderlich ist
        this->needLedStripUpdate = true;
        return ErrorCode::OK;
    }

    ErrorCode UnColorizeAllLed() override
    {
        if(!strip) return ErrorCode::TEMPORARYLY_NOT_AVAILABLE;
        strip->Clear(1000);
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
        int64_t nextOneWireReadout = INT64_MAX;
        int64_t nextBME280Readout = INT64_MAX;
        int64_t nextBH1750Readout = INT64_MAX;

        uint32_t oneWireReadoutIntervalMs = 200;
        uint32_t bme280ReadoutIntervalMs = UINT32_MAX;
        uint32_t bh1750ReadoutIntervalMs = 200;

        //OneWire
        DS18B20_Info *ds18b20_info = NULL;
        owb_rmt_driver_info rmt_driver_info;
        OneWireBus *owb = owb_rmt_initialize(&rmt_driver_info, PIN_ONEWIRE, CHANNEL_ONEWIRE_TX, CHANNEL_ONEWIRE_RX);
        owb_use_crc(owb, true); // enable CRC check for ROM code
        // Find all connected devices
        OneWireBus_ROMCode rom_code;
        owb_status status = owb_read_rom(owb, &rom_code);
        if (status == OWB_STATUS_OK)
        {

            ds18b20_info = ds18b20_malloc();                                 // heap allocation
            ds18b20_init_solo(ds18b20_info, owb);                            // only one device on bus
            ds18b20_use_crc(ds18b20_info, true);                             // enable CRC check on all reads
            ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION_10_BIT); //10bit -->187ms Conversion time
            ds18b20_convert_all(owb);
            nextOneWireReadout = GetMillis() + 200;
        }
        else
        {
            ESP_LOGE(TAG, "OneWire: An error occurred reading ROM code: %d", status);
        }

        //BME280
        BME280 *bme280 = new BME280(I2C_PORT, BME280_ADRESS::PRIM);
        if (bme280->Init(&bme280ReadoutIntervalMs) == ESP_OK)
        {
            bme280->TriggerNextMeasurement();
            bme280ReadoutIntervalMs += 20;
            bme280->TriggerNextMeasurement();
            nextBME280Readout = GetMillis() + bme280ReadoutIntervalMs;
            ESP_LOGI(TAG, "I2C: BME280 successfully initialized.");
        }
        else
        {
            ESP_LOGW(TAG, "I2C: BME280 not found");
        }

        //BH1750
        BH1750 *bh1750 = new BH1750(I2C_PORT, BH1750_ADRESS::LOW);
        if (bh1750->Init(BH1750_OPERATIONMODE::CONTINU_H_RESOLUTION) == ESP_OK)
        {
            bh1750ReadoutIntervalMs = 200;
            nextBH1750Readout = GetMillis() + bh1750ReadoutIntervalMs;
            ESP_LOGI(TAG, "I2C: BH1750 successfully initialized.");
        }
        else
        {
            ESP_LOGW(TAG, "I2C: BH1750 not found");
        }

        while (true)
        {

            if (GetMillis64() > nextOneWireReadout)
            {
                ds18b20_read_temp(ds18b20_info, &(this->airTemperatureDegCel[0]));
                ds18b20_convert_all(owb);
                nextOneWireReadout = GetMillis64() + oneWireReadoutIntervalMs;
            }
            if (GetMillis64() > nextBME280Readout)
            {
                bme280->GetDataAndTriggerNextMeasurement(&this->airTemperatureDegCel[1], &this->airPressurePa, &this->airRelHumidityPercent);
                nextBME280Readout = GetMillis64() + bme280ReadoutIntervalMs;
            }
            if (GetMillis64() > nextBH1750Readout)
            {
                bh1750->Read(&(this->ambientBrightnessLux));
                nextBH1750Readout = GetMillis64() + bh1750ReadoutIntervalMs;
            }
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }

    ErrorCode GetAirTemperature(float *degreesCelcius)
    {
        if (!std::isnan(this->airTemperatureDegCel[0]))
        {
            *degreesCelcius = this->airTemperatureDegCel[0];
            return ErrorCode::OK;
        }
        else if (!std::isnan(this->airTemperatureDegCel[1]))
        {
            *degreesCelcius = this->airTemperatureDegCel[1];
            return ErrorCode::OK;
        }
        return ErrorCode::NONE_AVAILABLE;
    }

    ErrorCode GetAirPressure(float *pa)
    {
        *pa = this->airPressurePa;
        return ErrorCode::OK;
    }
    ErrorCode GetAirRelHumidity(float *percent)
    {
        *percent = this->airRelHumidityPercent;
        return ErrorCode::OK;
    }

    ErrorCode GetAmbientBrightness(float *lux)
    {
        *lux = this->ambientBrightnessLux;
        return ErrorCode::OK;
    }

    ErrorCode Setup() override
    {
        //gpio_pad_select_gpio((uint8_t)PIN_MAINS);
        //gpio_set_direction(PIN_MAINS, GPIO_MODE_INPUT);
        //gpio_set_pull_mode(PIN_MAINS, GPIO_FLOATING);

        //gpio_pad_select_gpio((uint8_t)PIN_IRQ);
        //gpio_set_direction(PIN_IRQ, GPIO_MODE_INPUT);
        //gpio_set_pull_mode(PIN_IRQ, GPIO_FLOATING);

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
        ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;            // timer mode
        ledc_timer.timer_num = LEDC_TIMER_0;                     // timer index
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
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

        //I2C Master
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = PIN_I2C_SDA;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_io_num = PIN_I2C_SCL;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = 100000;
        i2c_param_config(I2C_PORT, &conf);
        ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));
        ESP_ERROR_CHECK(I2C::Init());

        //LED Strip
        strip = new WS2812_Strip<LEDSIZE>(CHANNEL_WS2812);
        ESP_ERROR_CHECK(strip->Init(PIN_LED_STRIP));
        ESP_ERROR_CHECK(strip->Clear(100));
        
        //Read Sensors
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
            strip->Refresh(100);
            needLedStripUpdate = false;
        }
        return ErrorCode::OK;
    }
};

void sensorTask(void *pvParameters)
{
    HAL_SensactOutdoorV1 *hal = (HAL_SensactOutdoorV1 *)pvParameters;
    hal->SensorLoop_ForInternalUseOnly();
}

#undef TAG
#pragma once
#include "common_in_project.hh"
#include <limits>
#include <cmath>
#include <array>

#include <driver/ledc.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <esp_adc/adc_continuous.h>
//#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>

#include "applicationBase.hh"
#include "rgbled.hh"
#include <otamanager.hh>

#include <i2c.hh>

// Outputs 1...6 sind die sechs Relays
// Outputs 7..11 sind die fünf MOSFETS
// Output 12 ist der Constant Current LED Treiber

#define TAG "HAL"

typedef gpio_num_t Pintype;

static constexpr Pintype PIN_PROG_IO0 = (Pintype)0;
static constexpr Pintype PIN_PROG_TX = (Pintype)1;
static constexpr Pintype PIN_SPI_CSN = (Pintype)2;
static constexpr Pintype PIN_PROG_RX = (Pintype)3;
static constexpr Pintype PIN_I2C_SCL = (Pintype)4;
static constexpr Pintype PIN_RGBLED = (Pintype)5;
static constexpr Pintype PIN_PWM5 = (Pintype)12;
// static constexpr Pintype PIN_CAN_TX = (Pintype)12;
static constexpr Pintype PIN_PWM_CC = (Pintype)13;
static constexpr Pintype PIN_SPI_SCLK = (Pintype)14;
static constexpr Pintype PIN_SPI_CE = (Pintype)15;
static constexpr Pintype PIN_I2C_SDA = (Pintype)16;
static constexpr Pintype PIN_K6 = (Pintype)17;
static constexpr Pintype PIN_K5 = (Pintype)18;
static constexpr Pintype PIN_K4 = (Pintype)19;
static constexpr Pintype PIN_K3 = (Pintype)21;
static constexpr Pintype PIN_K2 = (Pintype)22;
static constexpr Pintype PIN_K1 = (Pintype)23;
static constexpr Pintype PIN_PWM3 = (Pintype)25;
static constexpr Pintype PIN_PWM4 = (Pintype)26;
static constexpr Pintype PIN_SPI_MOSI = (Pintype)27;
static constexpr Pintype PIN_PWM1 = (Pintype)32;
static constexpr Pintype PIN_PWM2 = (Pintype)33;
static constexpr Pintype PIN_SPI_MISO = (Pintype)34;
static constexpr Pintype PIN_THERM = (Pintype)35;
static constexpr Pintype PIN_MAINS = (Pintype)36; // VP
// static constexpr Pintype PIN_CAN_RX = (Pintype)36; //VP
static constexpr Pintype PIN_IRQ = (Pintype)39; // VN

static constexpr std::array<Pintype, 6> RELAY_PINS{PIN_K1, PIN_K2, PIN_K3, PIN_K4, PIN_K5, PIN_K6};
static constexpr std::array<Pintype, 6> PWM_PINS{PIN_PWM1, PIN_PWM2, PIN_PWM3, PIN_PWM4, PIN_PWM5, PIN_PWM_CC};

static constexpr size_t LED_NUMBER{2};
static constexpr i2c_port_t I2C_PORT{I2C_NUM_1};

static constexpr auto PWM_RESOLUTION{LEDC_TIMER_10_BIT};
static constexpr uint32_t PWM_FREQ{1000};//according to BP1808 datasheet: 100Hz...1kHz. Hörbares Fiepen bei 500Hz


static constexpr int ntcNominal = 10000;         // Wiederstand des NTC bei Nominaltemperatur
static constexpr int tempNominal = 25;           // Temperatur bei der der NTC den angegebenen Wiederstand hat
static constexpr int bCoefficient = 3950;        // Beta Coefficient(aus Datenblatt des NTC)
static constexpr int serialResistor = 10000;               // Wert des Widerstands der mit dem NTC in Serie geschalten ist

static constexpr size_t ADC_READ_LEN{128};

RGBLED::BlinkPattern blinkFastRedBlack(CRGB::Red, 300, CRGB::Black, 300); // booting
RGBLED::BlinkPattern blinkFastGreenBlack(CRGB::Green, 300, CRGB::Black, 300);
RGBLED::BlinkPattern blinkFastBlueGreen(CRGB::Blue, 300, CRGB::Green, 300);
RGBLED::BlinkPattern blinkFastYellowRed(CRGB::Yellow, 300, CRGB::Red, 300); // OTA Process
RGBLED::BlinkPattern flashGreen(CRGB::Green, 100, CRGB::Black, 3000); // Run


class HAL_SensactOutdoor : public HAL
{
private:
    RGBLED::M<LED_NUMBER, RGBLED::DeviceType::PL9823> *strip = NULL;
    volatile uint16_t rawAnalogTemperatureValue;
    volatile float airPressurePa{std::numeric_limits<float>::quiet_NaN()};
    volatile float airRelHumidityPercent{std::numeric_limits<float>::quiet_NaN()};
    volatile float ambientBrightnessLux{std::numeric_limits<float>::quiet_NaN()};

    WIFISTA::ConnectionState lastWifiState{WIFISTA::ConnectionState::NONE};
    
    //adc_oneshot_unit_handle_t adc_handle{nullptr};
    adc_cali_handle_t adc1_cali_handle{nullptr};
    adc_channel_t adc_channel_therm{ADC_CHANNEL_0};
    adc_continuous_handle_t adc_handle{nullptr};
    static bool adc_continuous_callback_static(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
    {
        HAL_SensactOutdoor* myself = static_cast<HAL_SensactOutdoor*>(user_data);
        return myself->adc_continuous_callback(handle, edata);
    }

    bool adc_continuous_callback(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata){
        adc_digi_output_data_t *p = (adc_digi_output_data_t *)&edata->conv_frame_buffer[0];
        rawAnalogTemperatureValue = p->type1.data;
        return false;
    }

    void InitADC()
    {
        adc_unit_t adc_unit{ADC_UNIT_1};
        adc_atten_t adc_atten{ADC_ATTEN_DB_11};
        adc_bitwidth_t adc_bitwidth{ADC_BITWIDTH_12};

        ESP_ERROR_CHECK(adc_continuous_io_to_channel(PIN_THERM, &adc_unit, &adc_channel_therm));
        

        adc_continuous_handle_cfg_t adc_config = {};
        adc_config.max_store_buf_size = 2 * SOC_ADC_DIGI_RESULT_BYTES * ADC_READ_LEN;
        adc_config.conv_frame_size = SOC_ADC_DIGI_RESULT_BYTES * ADC_READ_LEN;
        ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

        
        adc_cali_line_fitting_config_t cali_config = {};
        cali_config.unit_id = adc_unit;
        cali_config.atten = adc_atten;
        cali_config.bitwidth = adc_bitwidth;
        ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle));

        adc_continuous_config_t dig_cfg = {};
        dig_cfg.sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_LOW;
        dig_cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
        dig_cfg.format = ADC_DIGI_OUTPUT_FORMAT_TYPE1;//for ESP32
        adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX];
        adc_pattern[0].atten = adc_atten;
        adc_pattern[0].channel = adc_channel_therm;
        adc_pattern[0].unit = adc_unit;
        adc_pattern[0].bit_width = adc_bitwidth;
        dig_cfg.pattern_num = 1;
        dig_cfg.adc_pattern = adc_pattern;
        ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg));

        adc_continuous_evt_cbs_t callback_cfg={};
        callback_cfg.on_conv_done=adc_continuous_callback_static;
        callback_cfg.on_pool_ovf=nullptr;

        ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &callback_cfg, this));

        ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
        

        ESP_LOGI(TAG, "ADC successfully initialized");
    }
    
/*
   void InitADC()
    {
        adc_unit_t adc_unit{ADC_UNIT_1};
        adc_atten_t adc_atten{ADC_ATTEN_DB_11};
        adc_bitwidth_t adc_bitwidth{ADC_BITWIDTH_12};
        ESP_ERROR_CHECK(adc_oneshot_io_to_channel(PIN_THERM, &adc_unit, &adc_channel_therm));

        gpio_set_level(PIN_THERM, 0);
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT_OD;
        io_conf.pin_bit_mask = 1ULL<<(int)PIN_THERM;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        
        adc_oneshot_unit_init_cfg_t init_config1 = {};
        init_config1.unit_id = adc_unit;    
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handle));

        adc_oneshot_chan_cfg_t chan_config = {};
        chan_config.bitwidth = adc_bitwidth;
        chan_config.atten = adc_atten;
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, adc_channel_therm, &chan_config));
    
        adc_cali_line_fitting_config_t cali_config = {};
        cali_config.unit_id = adc_unit;
        cali_config.atten = adc_atten;
        cali_config.bitwidth = adc_bitwidth;
        ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &adc1_cali_handle));

        ESP_LOGI(TAG, "ADC successfully initialized");
    }
*/
public:
    HAL_SensactOutdoor() {
        strip = new RGBLED::M<LED_NUMBER, RGBLED::DeviceType::PL9823>();
    }

    ~HAL_SensactOutdoor(){
        delete strip;
    }

    ErrorCode ConfigureIO(uint16_t io, IOMode mode)
    {
        // SensactOutdoor IOs are not configurable!
        return ErrorCode::OK;
    }

    void OutputOneLineStatus()
    {

        float degreesCelcius{1.1};
        this->GetAirTemperature(&degreesCelcius);
        ESP_LOGI(TAG, "heap=%lu; temp=%f ",esp_get_free_heap_size(), degreesCelcius);
    }

    ErrorCode HardwareTest() override
    {

        ColorizeLed(0, CRGB::Red);
        ColorizeLed(1, CRGB::Red);
        ESP_LOGI(TAG, "WS2812 RED");
        strip->Refresh(100);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ColorizeLed(0, CRGB::Green);
        ColorizeLed(1, CRGB::Green);
        ESP_LOGI(TAG, "WS2812 Green");
        strip->Refresh(100);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ColorizeLed(0, CRGB::Blue);
        ColorizeLed(1, CRGB::Blue);
        ESP_LOGI(TAG, "WS2812 Blue");
        strip->Refresh(100);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "RELAY1");
        SetU16Output(pins::sensactOutdoor::RELAY1, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY2");
        SetU16Output(pins::sensactOutdoor::RELAY2, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY3");
        SetU16Output(pins::sensactOutdoor::RELAY3, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY4");
        SetU16Output(pins::sensactOutdoor::RELAY4, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY5");
        SetU16Output(pins::sensactOutdoor::RELAY5, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "RELAY6");
        SetU16Output(pins::sensactOutdoor::RELAY6, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "ALL OFF");
        SetU16Output(pins::sensactOutdoor::RELAY1, 0);
        SetU16Output(pins::sensactOutdoor::RELAY2, 0);
        SetU16Output(pins::sensactOutdoor::RELAY3, 0);
        SetU16Output(pins::sensactOutdoor::RELAY4, 0);
        SetU16Output(pins::sensactOutdoor::RELAY5, 0);
        SetU16Output(pins::sensactOutdoor::RELAY6, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "MOSFET1");
        SetU16Output(pins::sensactOutdoor::MOSFET1, 32000);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET1 + 2");
        SetU16Output(pins::sensactOutdoor::MOSFET1, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET2, 32000);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET2 + 3");
        SetU16Output(pins::sensactOutdoor::MOSFET2, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET3, 32000);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET3 + 4");
        SetU16Output(pins::sensactOutdoor::MOSFET3, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET4, 32000);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET4 + 5");
        SetU16Output(pins::sensactOutdoor::MOSFET4, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::MOSFET5, 32000);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "MOSFET5 + LED_CC");
        SetU16Output(pins::sensactOutdoor::MOSFET5, UINT16_MAX);
        SetU16Output(pins::sensactOutdoor::LED_CC, 16000);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, 32000);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, UINT16_MAX);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, 32000);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        SetU16Output(pins::sensactOutdoor::LED_CC, 16000);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "All MOSFET off & LED_CC off");
        SetU16Output(pins::sensactOutdoor::MOSFET1, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET2, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET3, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET4, 0);
        SetU16Output(pins::sensactOutdoor::MOSFET5, 0);
        SetU16Output(pins::sensactOutdoor::LED_CC, 0);
        return ErrorCode::OK;
    }

    ErrorCode GetBoolInputs(uint32_t *ptr)
    {
        *ptr = 0;
        return ErrorCode::OK;
    }
    ErrorCode SetU16Output(uint16_t pinId, uint16_t state) override
    {
        if (pinId == 0)
            return ErrorCode::OK;

        if (pinId >= pins::sensactOutdoor::RELAY1 && pinId <= pins::sensactOutdoor::RELAY6)
        {
            pinId -= pins::sensactOutdoor::RELAY1;
            gpio_set_level(RELAY_PINS[pinId], state); // alles !=0 wird als logisch 1 erkannt
            return ErrorCode::OK;
        }
        if (pinId >= pins::sensactOutdoor::MOSFET1 && pinId <= pins::sensactOutdoor::LED_CC)
        {
            pinId -= pins::sensactOutdoor::MOSFET1;
            uint32_t duty{0};
            if(state==0){
                duty=0;
            }else if(state==UINT16_MAX){
                duty=(1<<PWM_RESOLUTION) - 1;
            }else{
                duty = state >> (16 - PWM_RESOLUTION);
            }
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)pinId, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)pinId);
            return ErrorCode::OK;
        }
        return ErrorCode::NONE_AVAILABLE;
    }

    size_t GetPinCnt()
    {
        return pins::sensactOutdoor::PIN_MAX;
    }

    ErrorCode ColorizeLed(uint8_t index, CRGB color) override
    {
        if (!strip)
            return ErrorCode::TEMPORARYLY_NOT_AVAILABLE;
        RETURN_ERRORCODE_ON_ERROR((strip->SetPixel((uint8_t)index, color)), ErrorCode::GENERIC_ERROR);
        return ErrorCode::OK;
    }

    ErrorCode UnColorizeAllLed() override
    {
        if (!strip)
            return ErrorCode::TEMPORARYLY_NOT_AVAILABLE;
        strip->Clear(1000);
        return ErrorCode::OK;
    }



    uint32_t GetMillis() override
    {
        return (uint32_t)(esp_timer_get_time() / 1000ULL);
    }

    int64_t GetMillis64() override
    {
        return esp_timer_get_time() / 1000ULL;
    }

    ErrorCode GetAirTemperature(float *degreesCelsius)
    {
        if(!adc1_cali_handle) return ErrorCode::HARDWARE_NOT_INITIALIZED;
        int calibratedVoltage_mV{0};

        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, rawAnalogTemperatureValue, &calibratedVoltage_mV));

        float R_x=(calibratedVoltage_mV*serialResistor)/(3300-calibratedVoltage_mV);
        float temp = R_x / ntcNominal;     // (R/Ro)
        temp = log(temp);                     // ln(R/Ro)
        temp /= bCoefficient;                 // 1/B * ln(R/Ro)
        temp += 1.0 / (tempNominal + 273.15); // + (1/To)
        temp = 1.0 / temp;                    // Invertieren
        temp -= 273.15;                       // Umwandeln in °C
        *degreesCelsius=temp;
        return ErrorCode::OK;
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
        gpio_reset_pin(PIN_MAINS);
        gpio_set_direction(PIN_MAINS, GPIO_MODE_INPUT);
        gpio_set_pull_mode(PIN_MAINS, GPIO_FLOATING);

        gpio_reset_pin(PIN_IRQ);
        gpio_set_direction(PIN_IRQ, GPIO_MODE_INPUT);
        gpio_set_pull_mode(PIN_IRQ, GPIO_FLOATING);

        for (const auto &pin : RELAY_PINS)
        {
            gpio_set_level(pin, 0);
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        }

        for (const auto &pin : PWM_PINS)
        {
            gpio_set_level(pin, 0);
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        }

        // PWM Output
        ledc_timer_config_t ledc_timer;
        ledc_timer.duty_resolution = PWM_RESOLUTION;  // resolution of PWM duty
        ledc_timer.freq_hz = PWM_FREQ;                // frequency of PWM signal
        ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE; // timer mode
        ledc_timer.timer_num = LEDC_TIMER_0;          // timer index
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        ledc_channel_config_t ledc_channel;
        ledc_channel.duty = 0;

        ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channel.hpoint = 0;
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel.intr_type = LEDC_INTR_DISABLE;

        for (size_t i = 0; i < PWM_PINS.size(); i++)
        {
            ledc_channel.channel = (ledc_channel_t)i;
            ledc_channel.gpio_num = PWM_PINS[i];
            ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
        }

        // LED Strip
        ESP_ERROR_CHECK(strip->Init(VSPI_HOST, PIN_RGBLED, 2));
        ESP_ERROR_CHECK(strip->Clear(100));
        ESP_ERROR_CHECK(strip->AnimatePixel(0, &blinkFastRedBlack));
        ESP_ERROR_CHECK(strip->SetPixel(1, CRGB::Black));
        ESP_ERROR_CHECK(strip->Refresh());

        // I2C Master -->Disable, not necessary!
        //ESP_ERROR_CHECK(I2C::Init(I2C_PORT, PIN_I2C_SCL, PIN_I2C_SDA));
        //NTC-Widerstand -->Disable
        //this->InitADC(); könnte Probleme verursachen, wenn die Objekte gelöscht werden

        return ErrorCode::OK;
    }

    ErrorCode BeforeLoop()
    {
        return ErrorCode::OK;
    }

    ErrorCode AfterLoop()
    {
        WIFISTA::ConnectionState newWifiState = WIFISTA::GetState();

        if (newWifiState != lastWifiState)
        {
            switch (newWifiState)
            {
            case WIFISTA::ConnectionState::CONNECTING:
                strip->AnimatePixel(0, &blinkFastBlueGreen);
                break;
            case WIFISTA::ConnectionState::UNCONNECTED:
                strip->AnimatePixel(0, &blinkFastRedBlack);
                break;
            case WIFISTA::ConnectionState::CONNECTED:
                strip->AnimatePixel(0, &flashGreen);
                break;
            default:
                break;
            }
            lastWifiState = newWifiState;
        }
        strip->Refresh();
        return ErrorCode::OK;
    }
};

#undef TAG
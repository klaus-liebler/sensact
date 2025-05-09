#pragma once
#include <driver/spi_master.h>
#include <i2c.hh>
#include <audioplayer.hh>
#include "../max98357.hh"
#include "hal_ESP32.hh"
#include <led_animator.hh>
#include <rgbled.hh>
#include <esp_log.h>
#define TAG "HAL"

/*
Outputs
0=K1
1=K2
2=K3
3=K4
4=LED0
5=LED1

*/

namespace sensact::hal::SensactOutdoor
{
    namespace P
    {
        constexpr gpio_num_t CAN_TX{GPIO_NUM_1};
        constexpr gpio_num_t CAN_RX{GPIO_NUM_42};
        
        constexpr gpio_num_t NRF_CE{GPIO_NUM_4};
        constexpr gpio_num_t NRF_CS{GPIO_NUM_5};
        constexpr gpio_num_t NRF_SCLK{GPIO_NUM_6};
        constexpr gpio_num_t NRF_MOSI{GPIO_NUM_7};
        constexpr gpio_num_t NRF_MISO{GPIO_NUM_15};
        constexpr gpio_num_t NRF_IRQ{GPIO_NUM_16};
        
        
        constexpr gpio_num_t K1{GPIO_NUM_17};
        constexpr gpio_num_t K2{GPIO_NUM_18};
        constexpr gpio_num_t K3{GPIO_NUM_9};
        constexpr gpio_num_t K4{GPIO_NUM_10};
        
        constexpr gpio_num_t I2C_SCL{GPIO_NUM_12};
        constexpr gpio_num_t I2C_SDA{GPIO_NUM_13};
        constexpr gpio_num_t ONEWIRE{GPIO_NUM_14};
        
        constexpr gpio_num_t MAINS{GPIO_NUM_21};
        
        constexpr gpio_num_t I2S_DATA{GPIO_NUM_38};
        constexpr gpio_num_t I2S_BCLK{GPIO_NUM_39};
        constexpr gpio_num_t I2S_LRC{GPIO_NUM_40};
        constexpr gpio_num_t I2S_GAIN{GPIO_NUM_41};
        constexpr gpio_num_t I2S_SD{GPIO_NUM_45};
        
        
        constexpr gpio_num_t LED_PL9823{GPIO_NUM_3};
        constexpr gpio_num_t LED_INFO{GPIO_NUM_46};
        constexpr gpio_num_t LED0{GPIO_NUM_48};
        constexpr gpio_num_t LED1{GPIO_NUM_47};
        
        constexpr gpio_num_t USB_DP{GPIO_NUM_20};
        constexpr gpio_num_t USB_DM{GPIO_NUM_19};
        constexpr gpio_num_t VBUS_SENSE{GPIO_NUM_8};
        
        constexpr gpio_num_t U0TXD{GPIO_NUM_43};
        constexpr gpio_num_t U0RXD{GPIO_NUM_44};
    }

    namespace R
    {
        constexpr i2c_port_t I2C_INTERNAL_IDF{I2C_NUM_0};
        constexpr sensact::hal::I2CPortIndex I2C_INTERNAL{sensact::hal::I2CPortIndex::I2C_0};
 
        
    }

    namespace BlinkPatterns
    {
        led::BlinkPattern BlinkSlow(1000, 1000);
        led::BlinkPattern BlinkFast(200, 200);
        led::BlinkPattern Flash(50, 3000);
        RGBLED::BlinkPattern BlinkSlowGreen(CRGB::DarkGreen, 50, CRGB::Black, 10000);
        RGBLED::BlinkPattern BlinkFastRed(CRGB::Red, 200, CRGB::Black, 200);
    }

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        MAX98357::M *max98357;
        AudioPlayer::Player *mp3player;
        iI2CPort *i2c_bus[1];
        led::Animator* led{nullptr};
        RGBLED::M<2, RGBLED::DeviceType::PL9823> *rgbled{nullptr};

    public:
        cHAL(temperature_sensor_handle_t tempHandle)
        {
            temp_handle=tempHandle;
            i2c_bus[(uint8_t)R::I2C_INTERNAL] = new iI2CPort_Impl(R::I2C_INTERNAL_IDF);
        }

        ErrorCode Setup() override
        {
            ESP_ERROR_CHECK(I2C::Init(R::I2C_INTERNAL_IDF, P::I2C_SCL, P::I2C_SDA));

            max98357 = new MAX98357::M(P::I2S_GAIN, P::I2S_SD);
            max98357->Init(P::I2S_BCLK, P::I2S_LRC, P::I2S_DATA);
            mp3player = new AudioPlayer::Player(max98357);
            MESSAGELOG_ON_ERRORCODE(mp3player->Init(), messagecodes::C::I2S_INIT);
            this->led = new led::Animator(P::LED_INFO, false, &BlinkPatterns::BlinkSlow);
            this->led->Begin(&BlinkPatterns::BlinkFast);

            this->rgbled = new RGBLED::M<2, RGBLED::DeviceType::PL9823>();
            ERRORCODE_CHECK(this->rgbled->Begin(SPI3_HOST, P::LED_PL9823));
            ERRORCODE_CHECK(this->rgbled->AnimatePixel(0, &BlinkPatterns::BlinkSlowGreen));
            ERRORCODE_CHECK(this->rgbled->AnimatePixel(1, &BlinkPatterns::BlinkSlowGreen));
            ERRORCODE_CHECK(this->rgbled->Refresh(1000, true));

            this->SetupCAN(P::CAN_TX, P::CAN_RX, ESP_INTR_FLAG_LOWMED);
            gpio_set_direction(P::K1, GPIO_MODE_OUTPUT);
            gpio_set_direction(P::K2, GPIO_MODE_OUTPUT);
            gpio_set_direction(P::K3, GPIO_MODE_OUTPUT);
            gpio_set_direction(P::K4, GPIO_MODE_OUTPUT);
            this->SetupLedcCommonTimer(LEDC_TIMER_0, 30000);
            this->SetupLedcChannel(LEDC_TIMER_0, LEDC_CHANNEL_0, P::LED0);
            this->SetupLedcChannel(LEDC_TIMER_0, LEDC_CHANNEL_1, P::LED1);
            //this->SetupMCPWMForLEDs(P::LED0, P::LED1);
            return ErrorCode::OK;
        }
        // So always use the appropriate pin. No inversion, Logic 1 means: Motor On
        ErrorCode SetU16Output(InOutId id, uint16_t value) override
        {
            switch (id)
            {
            case sensact::magic::OUTPUT_NULL:
                return ErrorCode::OK;
            case 0:
                gpio_set_level(P::K1, value);
                return ErrorCode::OK;
            case 1:
                gpio_set_level(P::K2, value);
                return ErrorCode::OK;
            case 2:
                gpio_set_level(P::K3, value);
                return ErrorCode::OK;
            case 3:
                gpio_set_level(P::K4, value);
                return ErrorCode::OK;
            case 4:
                this->SetPowerLed(value);
                return ErrorCode::OK;
            default:
                ESP_LOGW(TAG, "sensactOutdoor has only inOutId 0...4, not %d", id);
                return ErrorCode::PIN_NOT_AVAILABLE;
            }
        }

        bool highPowerMode{true};

        void SetPowerLed(uint16_t value) 
        {
            
                this->SetDuty(LEDC_CHANNEL_0,value);
                this->SetDuty(LEDC_CHANNEL_1,value);
           
        }

        iI2CPort *GetI2CPort(I2CPortIndex portIndex) override
        {
            return i2c_bus[0];
        }

        ErrorCode HardwareTest() override
        {
            return ErrorCode::OK;
        }

        ErrorCode AfterAppLoop() override
        {
            this->led->Refresh();
            ERRORCODE_CHECK(this->rgbled->Refresh(1000));
            return ErrorCode::OK;
        }

        ErrorCode BeforeAppLoop() override
        {
            return ErrorCode::OK;
        }

        ErrorCode StageRGBLed(uint8_t index, CRGB color) override
        {
            return this->rgbled->SetPixel(index, color);
        }

        ErrorCode SetInfoLed(led::AnimationPattern* pattern, tms_t timeToAutoOff) override{
            this->led->AnimatePixel(pattern, timeToAutoOff);
            return ErrorCode::OK;
        }
        
        ErrorCode StageUnColorizeAllRGBLed() override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        
        ErrorCode CommitRGBLed() override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        
        ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        

        ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            mp3player->PlayMP3(buf, len, volume0_255, true);
            return ErrorCode::OK;
        }

        ErrorCode IsPlayingMP3(bool& isPlaying) override{
            isPlaying= mp3player->IsEmittingSamples();
            return ErrorCode::OK;
        }

        ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }

        ErrorCode PlayNotes(const BUZZER::Note *note) override{
            return ErrorCode::OK;
        }
        ErrorCode StopSound() override
        {
            mp3player->Stop();
            return ErrorCode::OK;
        }
    };
}
#undef TAG
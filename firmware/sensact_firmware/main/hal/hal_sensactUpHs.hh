//Das ist der Knoten, der logisch im L2_BATH_UP sitzt,
//aber in Wirklichkeiteit im Drempel von L3_WORK sitzt.
#pragma once
#include "hal_ESP32.hh"
#include <audioplayer.hh>
#include <codec_manager.hh>
#include <max98357.hh>

#include <driver/spi_master.h>
#include <driver/ledc.h>
#include <esp_log.h>
#include <adc_buttons.hh>
#include <messagecodes.hh>
#include <logger.hh>
#define TAG "HAL"


namespace sensact::hal::usb{
    constexpr gpio_num_t VBUS_SENSE{GPIO_NUM_NC};
}

namespace sensact::hal::SensactUpHs
{
    namespace P{
        constexpr gpio_num_t ROT1_S{GPIO_NUM_4};
        constexpr gpio_num_t ROT1_B{GPIO_NUM_5};
        constexpr gpio_num_t ROT1_A{GPIO_NUM_6};
        
        constexpr gpio_num_t ROT2_S{GPIO_NUM_7};
        constexpr gpio_num_t ROT2_B{GPIO_NUM_15};
        constexpr gpio_num_t ROT2_A{GPIO_NUM_16};
        
        
        constexpr gpio_num_t I2C_EXT_SDA{GPIO_NUM_13};
        constexpr gpio_num_t I2C_EXT_SCL{GPIO_NUM_14};
        constexpr gpio_num_t I2C_EXT_INT0{GPIO_NUM_9};
        constexpr gpio_num_t I2C_EXT_INT1{GPIO_NUM_21};
        constexpr gpio_num_t I2C_EXT_INT2{GPIO_NUM_10};
        
        constexpr gpio_num_t CAN_RX{GPIO_NUM_11};
        constexpr gpio_num_t CAN_TX{GPIO_NUM_12};
        
        constexpr gpio_num_t RGB1{GPIO_NUM_36};
        constexpr gpio_num_t RGB2{GPIO_NUM_17};
        
        constexpr gpio_num_t PWM0{GPIO_NUM_35};
        constexpr gpio_num_t PWM1{GPIO_NUM_18};
        constexpr gpio_num_t PWM2{GPIO_NUM_8};
        constexpr gpio_num_t PWM3{GPIO_NUM_48};
        constexpr gpio_num_t PWM4{GPIO_NUM_3};
        constexpr gpio_num_t PWM5{GPIO_NUM_47};

        
        constexpr gpio_num_t I2C_INT_SDA{GPIO_NUM_37};
        constexpr gpio_num_t I2C_INT_SCL{GPIO_NUM_38};

        
        constexpr gpio_num_t AMP_I2S_DATA{GPIO_NUM_41};
        constexpr gpio_num_t AMP_I2S_SCLK{GPIO_NUM_40};
        constexpr gpio_num_t AMP_I2S_LRCLK{GPIO_NUM_39};
        constexpr gpio_num_t AMP_POWERDOWN{GPIO_NUM_42};
        
        constexpr gpio_num_t USB_DP{GPIO_NUM_20};
        constexpr gpio_num_t USB_DM{GPIO_NUM_19};

        constexpr gpio_num_t U0TXD{GPIO_NUM_43};
        constexpr gpio_num_t U0RXD{GPIO_NUM_44};
    }

    namespace R{
        constexpr i2c_port_t I2C_EXT_PORT{I2C_NUM_0};
        constexpr i2c_port_t I2C_INT_PORT{I2C_NUM_1};
        constexpr i2s_port_t AMP_I2S_PORT{I2S_NUM_0};
    }

    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{P::I2C_EXT_INT0, P::I2C_EXT_INT1, P::I2C_EXT_INT2,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        MAX98357::M *max98357;
        
        AudioPlayer::Player *mp3player;

        iI2CPort* i2c_bus[2];
        const char* hostname;
        
    public:
        cHAL(temperature_sensor_handle_t tempHandle){
            temp_handle=tempHandle;
            i2c_bus[(uint8_t)R::I2C_EXT_PORT] = new iI2CPort_Impl(R::I2C_EXT_PORT);
            i2c_bus[(uint8_t)R::I2C_INT_PORT] = new iI2CPort_Impl(R::I2C_INT_PORT);
        }

        ErrorCode Setup() override
        {
            ESP_ERROR_CHECK(I2C::Init(R::I2C_EXT_PORT, P::I2C_EXT_SCL, P::I2C_EXT_SDA, ESP_INTR_FLAG_SHARED));
            ESP_ERROR_CHECK(I2C::Init(R::I2C_INT_PORT, P::I2C_INT_SCL, P::I2C_INT_SDA, ESP_INTR_FLAG_SHARED));
          
            max98357 = new MAX98357::M(GPIO_NUM_NC, P::AMP_POWERDOWN, 127, 44100);
            MESSAGELOG_ON_ERRORCODE(max98357->Init(P::AMP_I2S_LRCLK, P::AMP_I2S_SCLK, P::AMP_I2S_DATA), messagecodes::C::I2S_INIT);
            mp3player = new AudioPlayer::Player(max98357);
    
            //I2C::Discover(I2C_EXTERNAL_IDF);
            this->SetupCAN(P::CAN_TX, P::CAN_RX);
        
            return ErrorCode::OK;
        }

        iI2CPort* GetI2CPort(uint8_t portIndex) override{
            return i2c_bus[portIndex%2];
        }
        
        ErrorCode HardwareTest() override
        {
            return ErrorCode::OK;
        }
        ErrorCode AfterAppLoop() override
        {
            mp3player->Loop();
            return ErrorCode::OK;
        }

        ErrorCode BeforeAppLoop() override
        {
            return ErrorCode::OK;
        }

        ErrorCode StageRGBLed(uint8_t index, CRGB color) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
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
            value=0;
            isPressed=false;
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
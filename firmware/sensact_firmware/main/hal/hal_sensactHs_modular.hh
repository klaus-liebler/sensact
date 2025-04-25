#pragma once
#include "hal_ESP32.hh"
#include <MP3Player.hh>
#include <Alarm.mp3.h>

namespace sensact::hal::SensactHsModular
{
    constexpr i2c_port_t I2C_{I2C_NUM_0};
    constexpr gpio_num_t I2C_SCL{GPIO_NUM_15};
    constexpr gpio_num_t I2C_SDA{GPIO_NUM_7};


    constexpr i2s_port_t I2S_PORT{I2S_NUM_0};
    constexpr gpio_num_t I2S_DIN{GPIO_NUM_40};
    constexpr gpio_num_t I2S_SCLK{GPIO_NUM_41};
    constexpr gpio_num_t I2S_LRCLK{GPIO_NUM_39};
    constexpr gpio_num_t AMP_POWERDOWN{GPIO_NUM_38};

    constexpr gpio_num_t ETH_POWER{GPIO_NUM_2};
    constexpr gpio_num_t ETH_MDC{GPIO_NUM_18};
    constexpr gpio_num_t ETH_MDIO{GPIO_NUM_5};

    constexpr gpio_num_t CAN_TX{GPIO_NUM_1};
    constexpr gpio_num_t CAN_RX{GPIO_NUM_2};

    constexpr gpio_num_t LED{GPIO_NUM_48};

    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{GPIO_NUM_6, GPIO_NUM_5,GPIO_NUM_4,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        MP3Player *mp3player;

        ErrorCode SetupSound()
        {
            mp3player = new MP3Player();
            mp3player->InitExternalI2SDAC(I2S_PORT, I2S_SCLK, I2S_LRCLK, I2S_DIN);
            return ErrorCode::OK;
        }

    public:
        ErrorCode Setup() override
        {
            this->SetupNetworkCommon();
            this->SetupWIFI();
            this->SetupETH(ETH_MDC, ETH_MDIO, ETH_POWER);
            this->SetupI2C(I2, I2C_SCL, I2C_SDA);
            this->SetupCAN(CAN_TX, CAN_RX);
            this->SetupSound();
            return ErrorCode::OK;
        }
        bool HasRole(NodeRole role) override
        {
            return role == NodeRole::GATEWAY || role == NodeRole::APPLICATION_HOST;
        }
        ErrorCode HardwareTest() override
        {
            return ErrorCode::OK;
        }
        ErrorCode AfterAppLoop() override
        {
            tas580x->Mute(!mp3player->IsEmittingSamples());
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
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        ErrorCode SetAmplifierVolume(uint8_t volume0_255) override
        {
            return this->tas580x->SetVolume(volume0_255)==ESP_OK?ErrorCode::OK:ErrorCode::DEVICE_NOT_RESPONDING;
        }
        /**
         * @brief
         *
         * @param volume0_255: Volume=0 means: Do not change current volume
         * @param buf
         * @param len
         * @return ErrorCode
         */
        ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            if (volume0_255 != 0)
            {
                this->tas580x->SetVolume(volume0_255);
            }
            mp3player->Play(buf, len);
            this->tas580x->Mute(false);
            return ErrorCode::OK;
        }

        ErrorCode IsPlayingMP3(bool& isPlaying) override{
            isPlaying=mp3player->IsEmittingSamples();
            return ErrorCode::OK;
        }

        ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        ErrorCode StopSound() override
        {
            this->tas580x->Mute(true);
            mp3player->Stop();
            return ErrorCode::OK;
        }
    };
}
#pragma once
#include "hal_ESP32.hh"
#include <MP3Player.hh>
#include <Alarm.mp3.h>
#include <tas580x.hh>

namespace sensact::hal::SensactHsNano2
{
    constexpr i2c_port_t I2C_EXTERNAL{I2C_NUM_0};
    constexpr gpio_num_t I2C_EXTERNAL_SCL{GPIO_NUM_16};
    constexpr gpio_num_t I2C_EXTERNAL_SDA{GPIO_NUM_4};

    constexpr i2c_port_t I2C_INTERNAL{I2C_NUM_1};
    constexpr gpio_num_t I2C_INTERNAL_SCL{GPIO_NUM_17};
    constexpr gpio_num_t I2C_INTERNAL_SDA{GPIO_NUM_15};

    constexpr i2s_port_t I2S_PORT{I2S_NUM_0};
    constexpr gpio_num_t I2S_DIN{GPIO_NUM_14};
    constexpr gpio_num_t I2S_SCLK{GPIO_NUM_12};
    constexpr gpio_num_t I2S_LRCLK{GPIO_NUM_13};
    constexpr gpio_num_t AMP_POWERDOWN{GPIO_NUM_33};

    constexpr gpio_num_t ETH_POWER{GPIO_NUM_2};
    constexpr gpio_num_t ETH_MDC{GPIO_NUM_18};
    constexpr gpio_num_t ETH_MDIO{GPIO_NUM_5};

    constexpr gpio_num_t CAN_TX{GPIO_NUM_32};
    constexpr gpio_num_t CAN_RX{GPIO_NUM_35};

    constexpr gpio_num_t LED{GPIO_NUM_23};

    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{GPIO_NUM_NC, GPIO_NUM_NC,GPIO_NUM_NC,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        TAS580x::M *tas580x;
        MP3Player *mp3player;
        ErrorCode SetupSound()
        {
            tas580x = new TAS580x::M(I2C_INTERNAL, TAS580x::ADDR7bit::DVDD_4k7, AMP_POWERDOWN);
            mp3player = new MP3Player();
            mp3player->InitExternalI2SDAC(I2S_PORT, I2S_SCLK, I2S_LRCLK, I2S_DIN);
            tas580x->Init(100);
            return ErrorCode::OK;
        }

    public:
        ErrorCode Setup() override
        {
            this->SetupNetworkCommon();
            this->SetupWIFI();
            this->SetupETH(ETH_MDC, ETH_MDIO, ETH_POWER);
            this->SetupI2C(I2C_EXTERNAL, I2C_EXTERNAL_SCL, I2C_EXTERNAL_SDA);
            this->SetupI2C(I2C_INTERNAL, I2C_INTERNAL_SCL, I2C_INTERNAL_SDA);
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
        ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value) override
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
            return ErrorCode::OK;
        }
        ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        ErrorCode StopSound() override
        {
            mp3player->Stop();
            return ErrorCode::OK;
        }
    };
}
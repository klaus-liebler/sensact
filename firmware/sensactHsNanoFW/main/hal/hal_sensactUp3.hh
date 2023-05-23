#pragma once
#include "hal_ESP32.hh"
#include <audioplayer.hh>
#include <tas580x.hh>
#include <driver/spi_master.h>
#include <ethernet.hh>

namespace sensact::hal::SensactHsNano3
{
    constexpr gpio_num_t MON{GPIO_NUM_1};
    constexpr gpio_num_t SW{GPIO_NUM_2};
    constexpr gpio_num_t RS485_DE{GPIO_NUM_3};

    constexpr i2s_port_t AMP_I2S_PORT{I2S_NUM_0};
    constexpr gpio_num_t AMP_I2S_DATA{GPIO_NUM_4};
    constexpr gpio_num_t AMP_I2S_SCLK{GPIO_NUM_5};
    constexpr gpio_num_t AMP_I2S_LRCLK{GPIO_NUM_6};
    constexpr gpio_num_t AMP_POWERDOWN{GPIO_NUM_39};
    
    constexpr gpio_num_t CAN_TX{GPIO_NUM_7};
    constexpr gpio_num_t CAN_RX{GPIO_NUM_15};
    
    constexpr spi_host_device_t LCD_SPI_HOST{SPI3_HOST};
    constexpr gpio_num_t LCD_SCLK{GPIO_NUM_8};
    constexpr gpio_num_t LCD_RS{GPIO_NUM_9};
    constexpr gpio_num_t LCD_MOSI{GPIO_NUM_18};
    constexpr gpio_num_t LCD_BACKLIGHT{GPIO_NUM_45};
    constexpr gpio_num_t LCD_BUZZER{GPIO_NUM_46};

    constexpr spi_host_device_t ETH_SPI_HOST{SPI2_HOST};
    constexpr gpio_num_t ETH_CS{GPIO_NUM_10};
    constexpr gpio_num_t ETH_MOSI{GPIO_NUM_11};
    constexpr gpio_num_t ETH_CLK{GPIO_NUM_12};
    constexpr gpio_num_t ETH_MISO{GPIO_NUM_13};
    constexpr gpio_num_t ETH_INT{GPIO_NUM_14};
    constexpr gpio_num_t ETH_RESET{GPIO_NUM_21};

    constexpr gpio_num_t RS485_RO{GPIO_NUM_16};
    constexpr gpio_num_t RS485_DIO{GPIO_NUM_17};

    constexpr gpio_num_t USB_DP{GPIO_NUM_20};
    constexpr gpio_num_t USB_DM{GPIO_NUM_19};

    //constexpr gpio_num_t U0TXD{GPIO_NUM_43};
    //constexpr gpio_num_t U0RXD{GPIO_NUM_44};

    constexpr gpio_num_t COMMON_RESET{GPIO_NUM_48};
    constexpr gpio_num_t RF24_CS{GPIO_NUM_47};
    constexpr gpio_num_t RF24_MISO{GPIO_NUM_35};

    constexpr gpio_num_t ANALOG_OUT_A{GPIO_NUM_40};
    constexpr gpio_num_t ANALOG_OUT_B{GPIO_NUM_36};

    constexpr i2c_port_t I2C_INTERNAL_IDF{I2C_NUM_0};
    constexpr sensact::hal::I2CPortIndex I2C_INTERNAL{sensact::hal::I2CPortIndex::I2C_0};
    constexpr gpio_num_t I2C_INTERNAL_SCL{GPIO_NUM_42};
    constexpr gpio_num_t I2C_INTERNAL_SDA{GPIO_NUM_41};

    constexpr i2c_port_t I2C_EXTERNAL_IDF{I2C_NUM_1};
    constexpr sensact::hal::I2CPortIndex I2C_EXTERNAL{sensact::hal::I2CPortIndex::I2C_1};
    constexpr gpio_num_t I2C_EXTERNAL_SCL{GPIO_NUM_38};
    constexpr gpio_num_t I2C_EXTERNAL_SDA{GPIO_NUM_37};








    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{GPIO_NUM_NC, GPIO_NUM_NC,GPIO_NUM_NC,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        AudioPlayer::Player *mp3player;
        iI2CPort* i2c_bus[2];

        ErrorCode SetupSound()
        {
            mp3player = new AudioPlayer::Player();
            mp3player->InitExternalI2SDAC(AMP_I2S_SCLK, AMP_I2S_LRCLK, AMP_I2S_DATA);
            return ErrorCode::OK;
        }

    public:
        cHAL(){
            i2c_bus[(uint8_t)I2C_EXTERNAL] = new MyI2CPort(I2C_EXTERNAL_IDF);
            i2c_bus[(uint8_t)I2C_INTERNAL] = new MyI2CPort(I2C_INTERNAL_IDF);
        }

        ErrorCode Setup() override
        {
            WIFI_ETH::initETH(false, SPI2_HOST, GPIO_NUM_13, GPIO_NUM_11, GPIO_NUM_12, SPI_MASTER_FREQ_20M, GPIO_NUM_NC, GPIO_NUM_10, GPIO_NUM_14, 1);
            ESP_ERROR_CHECK(I2C::Init(I2C_EXTERNAL_IDF, I2C_EXTERNAL_SCL, I2C_EXTERNAL_SDA));
            ESP_ERROR_CHECK(I2C::Init(I2C_INTERNAL_IDF, I2C_INTERNAL_SCL, I2C_INTERNAL_SDA));

            //I2C::Discover(I2C_EXTERNAL_IDF);
            
            this->SetupCAN(CAN_TX, CAN_RX);
            this->SetupSound();
            return ErrorCode::OK;
        }

        iI2CPort* GetI2CPort(I2CPortIndex portIndex) override{
            return i2c_bus[((uint8_t)portIndex)%2];
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
        ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }
        ErrorCode SetAmplifierVolume(uint8_t volume0_255) override
        {
            return this->tas580x->SetVolume(volume0_255);
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
            mp3player->PlayMP3(buf, len, volume0_255, true);
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
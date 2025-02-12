#pragma once
#include "hal_ESP32.hh"
#include <audioplayer.hh>
#include <rotenc.hh>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <webmanager.hh>
#define TAG "HAL"

namespace sensact::hal::SensactUp3
{
    constexpr gpio_num_t ONEWIRE{GPIO_NUM_1};
    constexpr gpio_num_t DAC{GPIO_NUM_4};
    
    constexpr spi_host_device_t LCD_SPI_HOST{SPI3_HOST};
    constexpr gpio_num_t LCD_BACKLIGHT{GPIO_NUM_5};
    constexpr gpio_num_t LCD_RESET{GPIO_NUM_6};
    constexpr gpio_num_t LCD_MOSI{GPIO_NUM_7};
    constexpr gpio_num_t LCD_SCLK{GPIO_NUM_15};
    constexpr gpio_num_t LCD_RS{GPIO_NUM_16};
    

    constexpr i2s_port_t AMP_I2S_PORT{I2S_NUM_0};
    constexpr gpio_num_t AMP_I2S_DATA{GPIO_NUM_17};
    constexpr gpio_num_t AMP_I2S_SCLK{GPIO_NUM_18};
    constexpr gpio_num_t AMP_I2S_LRCLK{GPIO_NUM_8};
    
    constexpr gpio_num_t CAN_TX{GPIO_NUM_47};
    constexpr gpio_num_t CAN_RX{GPIO_NUM_48};
    
    constexpr gpio_num_t RS485_DI{GPIO_NUM_35};
    constexpr gpio_num_t RS485_DE{GPIO_NUM_36};
    constexpr gpio_num_t RS485_RO{GPIO_NUM_37};

    constexpr gpio_num_t USB_DP{GPIO_NUM_20};
    constexpr gpio_num_t USB_DM{GPIO_NUM_19};

    constexpr gpio_num_t U0TXD{GPIO_NUM_43};
    constexpr gpio_num_t U0RXD{GPIO_NUM_44};

    constexpr gpio_num_t LED_RGB{GPIO_NUM_39};
   
    
    constexpr gpio_num_t ROT_A{GPIO_NUM_40};
    constexpr gpio_num_t ROT_B{GPIO_NUM_41};
    constexpr gpio_num_t ROT_C{GPIO_NUM_42};

    constexpr i2c_port_t I2C_INTERNAL_IDF{I2C_NUM_0};
    constexpr sensact::hal::I2CPortIndex I2C_INTERNAL{sensact::hal::I2CPortIndex::I2C_0};
    constexpr gpio_num_t I2C_INTERNAL_SCL{GPIO_NUM_10};
    constexpr gpio_num_t I2C_INTERNAL_SDA{GPIO_NUM_9};

    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{GPIO_NUM_NC, GPIO_NUM_NC,GPIO_NUM_NC,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        AudioPlayer::Player *mp3player;
        iI2CPort* i2c_bus[1];
        uint8_t volume_0mute_255max{120};
        cRotaryEncoder* rotenc{nullptr};
        uint16_t rotenc_value_with_overflow{0};
        bool rotenc_button{false};

    public:
        cHAL(){
            i2c_bus[(uint8_t)I2C_INTERNAL] = new MyI2CPort(I2C_INTERNAL_IDF);
        }

        ErrorCode Setup() override
        {

            ESP_ERROR_CHECK(I2C::Init(I2C_INTERNAL_IDF, I2C_INTERNAL_SCL, I2C_INTERNAL_SDA));
            mp3player = new AudioPlayer::Player();
            mp3player->InitExternalI2SDAC(AMP_I2S_SCLK, AMP_I2S_LRCLK, AMP_I2S_DATA, nullptr);
            this->SetupCAN(CAN_TX, CAN_RX, ESP_INTR_FLAG_LOWMED);
            this->SetupInternalTemperatureSensor();
            this->rotenc = new cRotaryEncoder(ROT_A, ROT_B, ROT_C);
            this->rotenc->Init();
            this->rotenc->Start();
            return ErrorCode::OK;
        }

        iI2CPort* GetI2CPort(I2CPortIndex portIndex) override{
            return i2c_bus[0];
        }
        
        bool HasRole(NodeRole role) override
        {
            return role == NodeRole::APPLICATION_HOST;
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
            int16_t local_value;
            rotenc->GetValue(local_value, this->rotenc_button, true);
            local_value/=4;
            this->rotenc_value_with_overflow+=local_value;
            //correct behaviour checked wit onlinegdb.com
            // #include <stdio.h>
            // #include <inttypes.h>
            // using namespace std;
            // int main()
            // {  
            //     uint16_t value=1;
            //     int16_t add = -3;
            //     value+=add;  
            //     printf("Hello World %i", value);
            //     return 0;
            // }
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
            value=this->rotenc_value_with_overflow;
            isPressed=this->rotenc_button;
            return ErrorCode::OK;

        }
        ErrorCode SetAmplifierVolume(uint8_t volume0_255) override
        {
            this->volume_0mute_255max=volume_0mute_255max;
            ESP_LOGI(TAG, "Set Amplifier Volume to %d", volume_0mute_255max);
            this->mp3player->SetGainU8(volume0_255);
            return ErrorCode::OK;
        }

        uint8_t GetAmplifierVolume() override{
            return this->volume_0mute_255max;
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
#undef TAG
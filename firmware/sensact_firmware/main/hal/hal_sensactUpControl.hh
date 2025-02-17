#pragma once
#include "hal_ESP32.hh"
#include <driver/spi_master.h>
#include <esp_log.h>
#include <buzzer.hh>
#include <led_animator.hh>
#include <i2c.hh>
#define TAG "HAL"

namespace sensact::hal::SensactUpControl
{
    namespace P
    {
        constexpr gpio_num_t NRF_CS{GPIO_NUM_15};
        constexpr gpio_num_t NRF_SCLK{GPIO_NUM_16};
        constexpr gpio_num_t FINGER_IRQ{GPIO_NUM_39};
        constexpr gpio_num_t FINGER_TX{GPIO_NUM_40};
        constexpr gpio_num_t ONEWIRE{GPIO_NUM_41};
        constexpr gpio_num_t MOTOR{GPIO_NUM_42};

        constexpr gpio_num_t LED1{GPIO_NUM_1};
        constexpr gpio_num_t LED2{GPIO_NUM_2};

        constexpr gpio_num_t RS485_DI{GPIO_NUM_3};
        constexpr gpio_num_t I2C_INTERNAL_SCL{GPIO_NUM_4};
        constexpr gpio_num_t I2C_INTERNAL_SDA{GPIO_NUM_5};

        constexpr gpio_num_t BUZZER{GPIO_NUM_6};
        constexpr gpio_num_t NRF_CE{GPIO_NUM_7};
        constexpr gpio_num_t NRF_IRQ{GPIO_NUM_8};

        constexpr gpio_num_t RS485_RO{GPIO_NUM_9};
        constexpr gpio_num_t CAN_RX{GPIO_NUM_10};
        constexpr gpio_num_t CAN_TX{GPIO_NUM_11};

        constexpr gpio_num_t LCD_CS{GPIO_NUM_12};
        constexpr gpio_num_t LCD_RS{GPIO_NUM_13};
        constexpr gpio_num_t LCD_SCLK{GPIO_NUM_14};

        constexpr gpio_num_t NRF_MOSI{GPIO_NUM_17};
        constexpr gpio_num_t NRF_MISO{GPIO_NUM_18};

        constexpr gpio_num_t LCD_MOSI{GPIO_NUM_21};

        constexpr gpio_num_t FINGER_RX{GPIO_NUM_38};
        constexpr gpio_num_t LCD_BACKLIGHT{GPIO_NUM_45};
        constexpr gpio_num_t RS485_DE{GPIO_NUM_46};

        constexpr gpio_num_t CONTACT1{GPIO_NUM_47};
        constexpr gpio_num_t CONTACT2{GPIO_NUM_48};

        constexpr spi_host_device_t LCD_SPI_HOST{SPI3_HOST};
        constexpr gpio_num_t LCD_RESET{GPIO_NUM_NC};

        constexpr gpio_num_t USB_DP{GPIO_NUM_20};
        constexpr gpio_num_t USB_DM{GPIO_NUM_19};

        constexpr gpio_num_t U0TXD{GPIO_NUM_43};
        constexpr gpio_num_t U0RXD{GPIO_NUM_44};
    }

    namespace R
    {
        constexpr i2c_port_t I2C_INTERNAL_IDF{I2C_NUM_0};
        constexpr sensact::hal::I2CPortIndex I2C_INTERNAL{sensact::hal::I2CPortIndex::I2C_0};
    }

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        iI2CPort *i2c_bus[1];
        BUZZER::M *buzzer{nullptr};
        led::Animator* led{nullptr};

    public:
        cHAL(temperature_sensor_handle_t tempHandle)
        {
            temp_handle=tempHandle;
            i2c_bus[(uint8_t)R::I2C_INTERNAL] = new iI2CPort_Impl(R::I2C_INTERNAL_IDF);
        }

        ErrorCode Setup() override
        {
            ESP_ERROR_CHECK(I2C::Init(R::I2C_INTERNAL_IDF, P::I2C_INTERNAL_SCL, P::I2C_INTERNAL_SDA));
            this->buzzer = new BUZZER::M();
            this->buzzer->Begin(P::BUZZER);
            this->led = new led::Animator(P::LED1);
            this->led->Begin();
            this->SetupCAN(P::CAN_TX, P::CAN_RX, ESP_INTR_FLAG_LOWMED);
            gpio_set_direction(P::MOTOR, GPIO_MODE_OUTPUT);
            return ErrorCode::OK;
        }
        // The only output is the motor output. So always use the appropriate pin. No inversion, Logic 1 means: Motor On
        ErrorCode SetU16Output(InOutId id, uint16_t value) override
        {
            if (id == sensact::magic::OUTPUT_NULL)
            {
                return ErrorCode::OK;
            }
            gpio_set_level(P::MOTOR, value);
            return ErrorCode::OK;
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
            return ErrorCode::OK;
        }
        ErrorCode BeforeAppLoop() override { return ErrorCode::OK; }

        ErrorCode StageRGBLed(uint8_t index, CRGB color) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
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
        ErrorCode SetAmplifierVolume(uint8_t volume0_255) override
        {
            return ErrorCode::OK;
        }

        uint8_t GetAmplifierVolume() override
        {
            return 255;
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
            return ErrorCode::OK;
        }
        ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t *buf, size_t len) override
        {
            return ErrorCode::FUNCTION_NOT_AVAILABLE;
        }

        ErrorCode PlayNotes(const BUZZER::Note *note) override{
            buzzer->PlayNotes(note);
            return ErrorCode::OK;
        }
        ErrorCode StopSound() override
        {

            return ErrorCode::OK;
        }
    };
}
#undef TAG
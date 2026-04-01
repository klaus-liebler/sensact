//Das ist der Knoten, der logisch im L2_BATH_UP sitzt,
//aber in Wirklichkeiteit im Drempel von L3_WORK sitzt.
#pragma once
#include "hal_ESP32.hh"
#include <audioplayer.hh>
#include <codec_manager.hh>
#include <max98357.hh>
#include <rotenc.hh>
#include <i2c/interfaces.hh>
#include <i2c.hh>
#include <driver/spi_master.h>
#include <driver/ledc.h>
#include <esp_log.h>
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

        
        constexpr gpio_num_t I2C_INT_SDA{GPIO_NUM_38};
        constexpr gpio_num_t I2C_INT_SCL{GPIO_NUM_37};

        
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
        constexpr int AMP_I2S_PORT{I2S_NUM_0};
    }

    namespace INPUTS{
        constexpr u16 GPIO_MAX{255};
        constexpr u16 ROTARY1{256};
        constexpr u16 ROTARY2{257};
        constexpr u16 CHIP_TEMPERATURE{258};
    }

    constexpr std::array<gpio_num_t, 3U> INTERRUPT_LINES{P::I2C_EXT_INT0, P::I2C_EXT_INT1, P::I2C_EXT_INT2,};

    class cHAL : public sensact::hal::cESP32
    {
    protected:
        i2c::iI2CBus_Impl i2c_busses[2]{i2c::iI2CBus_Impl{}, i2c::iI2CBus_Impl{}};
        cRotaryEncoder* rotenc[2]{nullptr};
        uint16_t rotenc_value_with_overflow[2]{0};
        bool rotenc_button[2]{false};
        MAX98357::M *max98357;
        AudioPlayer::Player *mp3player;
        const char* hostname;
        
    public:
        cHAL(temperature_sensor_handle_t tempHandle){
            temp_handle=tempHandle;
        }

        ErrorCode Setup() override
        {
            ESP_LOGI(TAG, "Setup internal I2C bus");
            i2c_busses[(size_t)R::I2C_INT_PORT].Init(R::I2C_INT_PORT, P::I2C_INT_SCL, P::I2C_INT_SDA);
            ESP_LOGI(TAG, "Setup external I2C bus");
            i2c_busses[(size_t)R::I2C_EXT_PORT].Init(R::I2C_EXT_PORT, P::I2C_EXT_SCL, P::I2C_EXT_SDA);
            
            ESP_LOGI(TAG, "Setup MAX98357");
            max98357 = new MAX98357::M(GPIO_NUM_NC, P::AMP_POWERDOWN, 127, 44100);
            MESSAGELOG_ON_ERRORCODE(max98357->Init(P::AMP_I2S_LRCLK, P::AMP_I2S_SCLK, P::AMP_I2S_DATA), messagecodes::C::I2S_INIT);
            mp3player = new AudioPlayer::Player(max98357);
    
            ESP_LOGI(TAG, "Setup CAN");
            this->SetupCAN(P::CAN_TX, P::CAN_RX);
            
            ESP_LOGI(TAG, "Setup Rotary Encoders");
            rotenc[0] = new cRotaryEncoder(P::ROT1_A, P::ROT1_B, P::ROT1_S);
            rotenc[0]->Init(cRotaryEncoder::StepMode::Step1);
            rotenc[0]->Start();
            rotenc[1] = new cRotaryEncoder(P::ROT2_A, P::ROT2_B, P::ROT2_S);
            rotenc[1]->Init(cRotaryEncoder::StepMode::Step1);
            rotenc[1]->Start();

            return ErrorCode::OK;
        }
        ErrorCode HardwareTest() override
        {
            return ErrorCode::OK;
        }

        i2c::iI2CBus* GetI2CBus(uint8_t portIndex) override{
            return &i2c_busses[portIndex % 2];
        }

        ErrorCode BeforeAppLoop() override
        {
            int16_t temp;
            //Wir rufen den aktuellen Wert der Rotary Encoder ab und addieren ihn zum Überlaufzähler.
            //Die Werte dort bewegen sich zwischen 0 und 65535, bevor sie wieder von vorne beginnen.
            rotenc[0]->GetValue(temp, this->rotenc_button[0], true);
            this->rotenc_value_with_overflow[0]+=temp;
            
            rotenc[1]->GetValue(temp, this->rotenc_button[1], true);
            this->rotenc_value_with_overflow[1]+=temp;
            return ErrorCode::OK;
        }
        
        ErrorCode AfterAppLoop() override
        {
            mp3player->Loop();
            return ErrorCode::OK;
        }

        ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed) override
        {
            value=this->rotenc_value_with_overflow[static_cast<size_t>(re)];
            isPressed=this->rotenc_button[static_cast<size_t>(re)];
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

        ErrorCode AppendValidGpioInputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges) override{
            twoMostSignificantBits&=0xC000; //only two MSB are relevant
            ranges.push_back(sensact::Range(twoMostSignificantBits+sensact::magic::INPUT0,twoMostSignificantBits+sensact::magic::INPUT1+1,"CONSTANTS"));
            ranges.push_back(sensact::Range(twoMostSignificantBits+INPUTS::ROTARY1,twoMostSignificantBits+INPUTS::ROTARY2+1,"ROTENC"));
            ranges.push_back(sensact::Range(twoMostSignificantBits+INPUTS::CHIP_TEMPERATURE,twoMostSignificantBits+INPUTS::CHIP_TEMPERATURE+1,"CHIP_TEMPERATURE"));
            return ErrorCode::OK;
        }

        ErrorCode AppendValidGpioOutputRanges(u16 twoMostSignificantBits, std::vector<sensact::Range> &ranges) override{
            return ErrorCode::OK;
        }

        ErrorCode GetU16Input(InOutId id, uint16_t &inputState) override
		{
			if (id == sensact::magic::INPUT0)
			{
				inputState = 0;
				return ErrorCode::OK;
			}
			else if (id == sensact::magic::INPUT1)
			{
				inputState = UINT16_MAX;
				return ErrorCode::OK;
			}
			else if(id == INPUTS::ROTARY1){
                inputState = static_cast<uint16_t>(this->rotenc_value_with_overflow[0]);
                return ErrorCode::OK;
            }
            else if(id == INPUTS::ROTARY2){
                inputState = static_cast<uint16_t>(this->rotenc_value_with_overflow[1]);
                return ErrorCode::OK;
            }
            else if(id == INPUTS::CHIP_TEMPERATURE){
                if(temp_handle==nullptr){
                    return ErrorCode::GENERIC_ERROR;
                }
                float temperature_celcius=0.0f;
                esp_err_t err = temperature_sensor_get_celsius(temp_handle, &temperature_celcius);
                if(err!=ESP_OK){
                    return ErrorCode::GENERIC_ERROR;
                }
                inputState = static_cast<uint16_t>(temperature_celcius*100); //z.B. 2534 = 25.34°C
                return ErrorCode::OK;
            }
            if (id >= GPIO_NUM_MAX)
			{
				return ErrorCode::PIN_NOT_AVAILABLE;
			}

            inputState = gpio_get_level((gpio_num_t)id) ? UINT16_MAX : 0;
            return ErrorCode::OK;
		}
    };
}
#undef TAG
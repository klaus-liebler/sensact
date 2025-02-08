#pragma once
#include "hal_ESP32.hh"
#include <audioplayer.hh>
#include <tas580x.hh>
#include <driver/spi_master.h>
#include <driver/ledc.h>
#include <ethernet.hh>
#include <esp_log.h>
#include <webmanager.hh>
#include <adc_buttons.hh>

#include <ringtoneplayer.hh>
#define TAG "HAL"

lv_group_t *groupHome{nullptr};
lv_group_t *groupSettings{nullptr};
lv_indev_t *encoder_indev{nullptr};
lv_indev_drv_t indev_drv;

extern "C" void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static uint32_t previousKey{false};
    AdcButtons::State nextKey=AdcButtons::UpdateBlocking();
    if(nextKey!=AdcButtons::State::IDLE){
        //wurde gedrückt
        data->key=nextKey==AdcButtons::State::LEFT?LV_KEY_LEFT:nextKey==AdcButtons::State::RIGHT?LV_KEY_RIGHT:LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
        previousKey=data->key;
        
    }else{
        //wurde losgelassen
        data->key=previousKey;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

namespace sensact::hal::SensactHsNano3
{
    constexpr gpio_num_t MON{GPIO_NUM_1};
    constexpr gpio_num_t SW_PIN{GPIO_NUM_2};
    constexpr gpio_num_t RS485_DE{GPIO_NUM_3};

    constexpr i2s_port_t AMP_I2S_PORT{I2S_NUM_0};
    constexpr gpio_num_t AMP_I2S_DATA{GPIO_NUM_4};
    constexpr gpio_num_t AMP_I2S_SCLK{GPIO_NUM_5};
    constexpr gpio_num_t AMP_I2S_LRCLK{GPIO_NUM_6};
    constexpr gpio_num_t AMP_POWERDOWN{GPIO_NUM_39};
    
    constexpr gpio_num_t CAN_TX{GPIO_NUM_7};
    constexpr gpio_num_t CAN_RX{GPIO_NUM_15};
    
    constexpr spi_host_device_t LCD_SPI_HOST{SPI3_HOST};
    constexpr ledc_timer_t LCD_TIMER{LEDC_TIMER_0};
    constexpr int LCD_INTR_FLAGS{ESP_INTR_FLAG_LEVEL3};
    constexpr gpio_num_t LCD_SCLK{GPIO_NUM_8};
    constexpr gpio_num_t LCD_RS{GPIO_NUM_9};
    constexpr gpio_num_t LCD_MOSI{GPIO_NUM_18};
    constexpr gpio_num_t LCD_BACKLIGHT{GPIO_NUM_46};
    constexpr ledc_timer_t BUZZER_TIMER{LEDC_TIMER_2};
    constexpr ledc_channel_t BUZZER_CHANNEL{LEDC_CHANNEL_0}; 
    constexpr gpio_num_t LCD_BUZZER{GPIO_NUM_45};
    constexpr gpio_num_t LCD_CS{GPIO_NUM_48};

    constexpr spi_host_device_t ETH_SPI_HOST{SPI2_HOST};
    constexpr int ETH_INTR_FLAGS{ESP_INTR_FLAG_LEVEL2};
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
        TAS580x::M *tas580x;
        AudioPlayer::Player *mp3player;
        Ringtoneplayer *rtp;
        iI2CPort* i2c_bus[2];
        uint8_t volume_0mute_255max{120};
        const char* hostname;
        
    public:
        cHAL(const char* hostname):hostname(hostname){
            i2c_bus[(uint8_t)I2C_EXTERNAL] = I2C::GetPort_DoNotForgetToDelete(I2C_EXTERNAL_IDF);
            i2c_bus[(uint8_t)I2C_INTERNAL] = I2C::GetPort_DoNotForgetToDelete(I2C_INTERNAL_IDF);
        }

        ErrorCode Setup() override
        {
            ESP_ERROR_CHECK(I2C::Init(I2C_EXTERNAL_IDF, I2C_EXTERNAL_SCL, I2C_EXTERNAL_SDA, ESP_INTR_FLAG_SHARED));
            ESP_ERROR_CHECK(I2C::Init(I2C_INTERNAL_IDF, I2C_INTERNAL_SCL, I2C_INTERNAL_SDA, ESP_INTR_FLAG_SHARED));
            gpio_set_level(LCD_BUZZER, 0);
            gpio_set_direction(LCD_BUZZER, GPIO_MODE_OUTPUT);
            //Hinweis: Das W5500 kommt faktisch auch ohne die Reset-Leitung aus. In einer Zukünftigen Version könnte deshalb ETH_RESET anders verwendet werden
            ETHERNET::initETH_W5500(true, ETH_SPI_HOST, ETH_MISO, ETH_MOSI, ETH_CLK, SPI_MASTER_FREQ_20M, ETH_CS, GPIO_NUM_NC, ETH_INT, 1, ETH_INTR_FLAGS, hostname);
            tas580x = new TAS580x::M(GetI2CPort(I2C_INTERNAL), TAS580x::ADDR7bit::DVDD_4k7, AMP_POWERDOWN, AMP_I2S_SCLK, AMP_I2S_LRCLK, AMP_I2S_DATA, GPIO_NUM_NC, volume_0mute_255max);
            mp3player = new AudioPlayer::Player(tas580x);
            MESSAGELOG_ON_ERRORCODE(mp3player->Init(), messagecodes::C::I2S_INIT);
            //MESSAGELOG_ON_ERRORCODE(tas580x->Init(), messagecodes::C::TAS5805_INIT);//is done inside mp3player->Init()

            rtp = new Ringtoneplayer();
            rtp->Setup(BUZZER_TIMER, BUZZER_CHANNEL, LCD_BUZZER);

            //I2C::Discover(I2C_EXTERNAL_IDF);
            this->SetupCAN(CAN_TX, CAN_RX);
            this->SetupInternalTemperatureSensor();
            LCD::InitLCD(LCD_SPI_HOST, LCD_MOSI, LCD_SCLK, SPI_MASTER_FREQ_20M, LCD_CS, GPIO_NUM_NC, LCD_RS, LCD_BACKLIGHT, 135, 52, 240, 40, LCD_TIMER, LCD_INTR_FLAGS);
            AdcButtons::InitAdcButtons(SW_PIN);
            
            lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
            indev_drv.type = LV_INDEV_TYPE_ENCODER; /*See below.*/
            indev_drv.read_cb = encoder_read;       /*See below.*/
            /*Register the driver in LVGL and save the created input device object*/
            encoder_indev = lv_indev_drv_register(&indev_drv);
            assert(encoder_indev!=NULL);

            groupHome = lv_group_create();
            lv_group_add_obj(groupHome, ui_sldVolume);
            lv_group_add_obj(groupHome, ui_btnNext);
            lv_indev_set_group(encoder_indev, groupHome);
            groupSettings = lv_group_create();
            lv_group_add_obj(groupSettings, ui_btnPrev);
            LCD::StartLVGLTask();
            LCD::SemaphoreTake();
            lv_slider_set_value(ui_sldVolume, this->GetAmplifierVolume()/2.55, LV_ANIM_OFF);
            LCD::SemaphoreGive();
            rtp->PlayNotes(Greeting);
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
            rtp->Loop();
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
        ErrorCode SetAmplifierVolume(uint8_t volume_0mute_255max) override
        {
            this->volume_0mute_255max=volume_0mute_255max;
            ESP_LOGI(TAG, "Set Amplifier Volume to %d", volume_0mute_255max);
            return this->tas580x->SetVolume(255-volume_0mute_255max);
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
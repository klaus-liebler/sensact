#pragma once
#define TAG "MI2MA"

enum class eSliderMode
{
    RGB_WWCW,
    RGB_RGB,
    LED_CC,
};

namespace keycode1
{
    constexpr uint8_t K1_1 = 1;
    constexpr uint8_t K1_0 = 10;
    constexpr uint8_t K2_1 = 2;
    constexpr uint8_t K2_0 = 11;
    constexpr uint8_t K3_1 = 3;
    constexpr uint8_t K3_0 = 12;
    constexpr uint8_t K4_1 = 4;
    constexpr uint8_t K4_0 = 13;
    constexpr uint8_t K5_1 = 5;
    constexpr uint8_t K5_0 = 14;
    constexpr uint8_t K6_1 = 6;
    constexpr uint8_t K6_0 = 15;
    constexpr uint8_t K7_1 = 7;
    constexpr uint8_t K7_0 = 16;
    constexpr uint8_t K8_1 = 8;
    constexpr uint8_t K8_0 = 17;
    constexpr uint8_t KON = 0;
    constexpr uint8_t KW = 20;
    constexpr uint8_t KOFF = 9;
    constexpr uint8_t KSMINUS = 19;
    constexpr uint8_t KSPLUS = 18;
}

namespace command
{
    constexpr uint8_t KEYS = 1;
    constexpr uint8_t WHEEL = 2;
    constexpr uint8_t DIMMER = 5;
    constexpr uint8_t MODE = 6;
    constexpr uint8_t SATURATION = 7;
}

class MilightSensactOutdoor : public MilightCallback
{
public:
    void ReceivedFromMilight(uint8_t milightcmd, uint8_t arg)
    {
        flatbuffers::FlatBufferBuilder builder(128);
        flatbuffers::Offset<tCommand> cmd;
        switch (milightcmd)
        {
        case command::KEYS:
            if (arg == keycode1::KW)
            {
                this->mode = eSliderMode::LED_CC;
                ESP_LOGI(TAG, "mode = eSliderMode::LED_CC");
                return;
            }
            if (arg == keycode1::KSPLUS)
            {
                this->mode = eSliderMode::RGB_RGB;
                ESP_LOGI(TAG, "mode = eSliderMode::RGB_RGB");
                return;
            }
            if (arg == keycode1::KSMINUS)
            {
                this->mode = eSliderMode::RGB_WWCW;
                ESP_LOGI(TAG, "mode = eSliderMode::RGB_WWCW");
                return;
            }
            if (keyCode2command.find(arg) == keyCode2command.end())
            {
                ESP_LOGW("MILIGHT", "Key %d has no associated handler", arg);
                return;
            }
            cmd = (keyCode2command[arg])(&builder, mode);
            break;
        case command::WHEEL:
            if (mode != eSliderMode::RGB_RGB)
                return;
            {
                float x = arg * (360.0 / 255.0);
                ESP_LOGI(TAG, "CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_HUE_0_360, %f", x);
                auto xcmd = CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_HUE_0_360, x);
                cmd = CreatetCommand(builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
            }
            break;
        case command::DIMMER:
            if (mode == eSliderMode::RGB_RGB)
            {
                float x = arg * (1.0 / 100.0);
                ESP_LOGI(TAG, "CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_VALUE_0_1, %f)", x);
                auto xcmd = CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_VALUE_0_1, x);
                cmd = CreatetCommand(builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
            }
            else if (mode == eSliderMode::RGB_WWCW)
            {
                float x = arg * (1.0 / 100.0);
                ESP_LOGI(TAG, "CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_WWCW_BRIGHTNESS_0_1, %f)", x);
                auto xcmd = CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_WWCW_BRIGHTNESS_0_1, x);
                cmd = CreatetCommand(builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
            }
            else
            {
                float x = arg * (0.95 / 100.0) + 0.05;
                ESP_LOGI(TAG, "CreatetSinglePwmCommand(builder, eSinglePwmCommand_CHANGE_INTENSITY %f)", x);
                auto xcmd = CreatetSinglePwmCommand(builder, eSinglePwmCommand_CHANGE_INTENSITY, x);
                cmd = CreatetCommand(builder, 3, uCommand::uCommand_tSinglePwmCommand, xcmd.Union());
            }

            break;
        case command::SATURATION:
            if (mode == eSliderMode::RGB_RGB)
            {
                float x = arg * (1.0 / 100.0);
                ESP_LOGI(TAG, "CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_SATURATION_0_1, %f", x);
                auto xcmd = CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_SATURATION_0_1, x);
                cmd = CreatetCommand(builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
            }
            else if (mode == eSliderMode::RGB_WWCW)
            {
                float x = arg * (1.0 / 100.0);
                ESP_LOGI(TAG, "CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_WWCW_RATIO_0_1, %f", x);
                auto xcmd = CreatetRgbwPwmCommand(builder, eRgbwPwmCommand_CHANGE_WWCW_RATIO_0_1, x);
                cmd = CreatetCommand(builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
            }
            else
            {
                return;
            }
            break;
        default:
            ESP_LOGW(TAG, "Command %d has no associated handler", milightcmd);
            return;
        }
        builder.Finish(cmd);
        auto parsed_cmd = flatbuffers::GetRoot<tCommand>(builder.GetBufferPointer());
        manager->HandleCommand(parsed_cmd);
    }
    MilightSensactOutdoor(Manager *manager) : manager(manager) {}
    virtual ~MilightSensactOutdoor() {}

private:
    eSliderMode mode{eSliderMode::LED_CC};
    Manager *manager;
    std::map<uint8_t, std::function<flatbuffers::Offset<tCommand>(flatbuffers::FlatBufferBuilder *, eSliderMode)>> keyCode2command{
        {keycode1::KON, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             if (mode == eSliderMode::RGB_RGB || mode == eSliderMode::RGB_WWCW)
             {
                 auto xcmd = CreatetRgbwPwmCommand(*builder, eRgbwPwmCommand_ON);
                 return CreatetCommand(*builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
             }
             auto xcmd = CreatetSinglePwmCommand(*builder, eSinglePwmCommand_ON);
             return CreatetCommand(*builder, 3, uCommand::uCommand_tSinglePwmCommand, xcmd.Union());
         }},
        {keycode1::KOFF, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             if (mode == eSliderMode::RGB_RGB || mode == eSliderMode::RGB_WWCW)
             {
                 auto xcmd = CreatetRgbwPwmCommand(*builder, eRgbwPwmCommand_OFF);
                 return CreatetCommand(*builder, 4, uCommand::uCommand_tRgbwPwmCommand, xcmd.Union());
             }
             auto xcmd = CreatetSinglePwmCommand(*builder, eSinglePwmCommand_OFF);
             return CreatetCommand(*builder, 3, uCommand::uCommand_tSinglePwmCommand, xcmd.Union());
         }},
        {keycode1::K1_0, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_UP_OR_STOP);
             return CreatetCommand(*builder, 1, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {keycode1::K1_1, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_DOWN_OR_STOP);
             return CreatetCommand(*builder, 1, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {keycode1::K2_0, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_UP_OR_STOP);
             return CreatetCommand(*builder, 2, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {keycode1::K2_1, [](flatbuffers::FlatBufferBuilder *builder, eSliderMode mode)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_DOWN_OR_STOP);
             return CreatetCommand(*builder, 2, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
    };
};

#undef TAG
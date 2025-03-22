#pragma once
#include <array>
#include "milight.hh"

#define TAG "MI2HO"

namespace milight
{

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
        constexpr uint8_t MAX=21;
    }

    namespace command
    {
        constexpr uint8_t KEYS = 1;
        constexpr uint8_t WHEEL = 2;
        constexpr uint8_t DIMMER = 5;
        constexpr uint8_t MODE = 6;
        constexpr uint8_t SATURATION = 7;
    }

    class Milight2HostAdapter : public iMilightCallback
    {
    public:
        void ReceivedFromMilight(uint8_t milightcmd, uint8_t arg)
        {

            switch (milightcmd)
            {
            case command::KEYS:{
                if(arg>=keycode1::MAX) return;
                if(keyCode2command[arg]==nullptr) return;
                (keyCode2command[arg])(this->ctx, this->canMbp);
                return;
            }
            case command::DIMMER:{
                //arg von 0...100%
                //ab 95% -->voll
                uint16_t val{0};
                if(arg>95){
                    val=UINT16_MAX;
                }else{
                    val=arg*(UINT16_MAX/100.0);
                }
                //float x = arg * (0.95 / 100.0) + 0.05;
                CANMessage m;
                uint8_t payload[2];
                WriteU16(val, payload, 0);
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_PWM___X1_XX1_42, Command::Command_SET_PWM,payload, 2, m);
                ctx->PublishOnMessageBus(m, true);
                return;
            }

            default:
                ESP_LOGW(TAG, "Command %d has no associated handler", milightcmd);
                return;
            }
            
        }
        Milight2HostAdapter(sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp) : ctx(ctx), canMbp(canMbp) {

            this->keyCode2command[keycode1::KON]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_PWM___X1_XX1_42, Command::Command_ON,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
            this->keyCode2command[keycode1::KOFF]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_PWM___X1_XX1_42, Command::Command_OFF,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
            this->keyCode2command[keycode1::K1_0]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_BLIND_X1_XX1_42, Command::Command_UP,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
            this->keyCode2command[keycode1::K1_1]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_BLIND_X1_XX1_42, Command::Command_DOWN,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
            this->keyCode2command[keycode1::K2_0]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_BLIND_X1_XX1_43, Command::Command_UP,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
            this->keyCode2command[keycode1::K2_1]= [](sensact::iHostContext *ctx, aCANMessageBuilderParser *canMbp){
                CANMessage m;
                canMbp->BuildApplicationCommandMessage(ApplicationId::ApplicationId_BLIND_X1_XX1_43, Command::Command_DOWN,nullptr, 0, m);
                ctx->PublishOnMessageBus(m, true);
            };
        }
        virtual ~Milight2HostAdapter() {}

    private:
        sensact::iHostContext *ctx;
        aCANMessageBuilderParser *canMbp;
        std::array<std::function<void(sensact::iHostContext*, aCANMessageBuilderParser*)>, keycode1::MAX> keyCode2command = {0};
    };
}
#undef TAG
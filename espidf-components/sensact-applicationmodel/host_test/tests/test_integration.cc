// Zusammenspiel mehrerer Apps ueber den echten Kommando-Codec: was ein Taster/Drehgeber sendet,
// wird wie im cApplicationHost mit parseCommand.inc an die Ziel-App ausgeliefert.
#include "TestHelpers.hh"
#include "BlindMotorModel.hh"
#include "blind.hh"
#include "onoff.hh"
#include "pushbutton.hh"
#include "rotaryencoder.hh"
#include "singlepwm.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    // Minimaler "Host": mehrere Apps, gleiche Taktung wie cApplicationHost, lokale Zustellung
    // aller gesendeten Kommandos (wie SendApplicationCommandToMessageBus fuer lokale Ziele).
    struct MiniHost
    {
        FakeSensactContext ctx;
        std::vector<cApplication *> apps;

        void Setup()
        {
            ctx.now = 1000;
            for (auto *a : apps)
                a->Setup(&ctx);
        }
        void Run(tms_t ms)
        {
            const tms_t end = ctx.now + ms;
            while (ctx.now < end)
            {
                ctx.now += HOST_TICK_MS;
                for (auto *a : apps)
                    a->Loop(&ctx); // alle jeden Tick -- strengere Taktung als im Host
                Deliver();
            }
        }
        void Deliver()
        {
            auto pending = std::move(ctx.sentCommands);
            ctx.sentCommands.clear();
            for (auto &c : pending)
                for (auto *a : apps)
                    if (a->id == c.target)
                        ctx.Dispatch(*a, c);
        }
    };
}

TEST(Integration, BlindButtonsDriveBlindMotor)
{
    constexpr InOutId BTN_UP = 16388, BTN_DOWN = 16389, R1 = 16392, R2 = 16393;
    MiniHost h;
    apps::cBlind blind(ID::BLIND_X2_XX3_13, R1, R2, eRelayInterlockMode::R1_DOWN__R2_POWER, 8000, 5000);
    apps::cPushbuttonDual2Blind buttons(ID::BLINDB_X2_XX1_7, BTN_UP, BTN_DOWN, ID::BLIND_X2_XX3_13);
    BlindMotorModel motor(eRelayInterlockMode::R1_DOWN__R2_POWER, R1, R2);
    h.apps = {&buttons, &blind};
    h.Setup();
    h.Run(1000);

    // kurzer Druck "runter" -> faehrt ganz zu
    h.ctx.SetInput(BTN_DOWN, 1);
    h.Run(200);
    h.ctx.SetInput(BTN_DOWN, 0);
    h.Run(1000);
    motor.Feed(h.ctx);
    EXPECT_EQ(motor.Current(), Motor::DOWN);
    h.Run(10000);
    motor.Feed(h.ctx);
    EXPECT_EQ(motor.Current(), Motor::OFF);
    EXPECT_EQ(Status(blind, h.ctx)[1], 0);

    // "hoch" gedrueckt halten -> faehrt, solange gehalten; Loslassen stoppt
    h.ctx.SetInput(BTN_UP, 1);
    h.Run(3000);
    motor.Feed(h.ctx);
    EXPECT_EQ(motor.Current(), Motor::UP);
    h.ctx.SetInput(BTN_UP, 0);
    h.Run(300);
    motor.Feed(h.ctx);
    EXPECT_EQ(motor.Current(), Motor::OFF);
    int pos = Status(blind, h.ctx)[1];
    EXPECT_GT(pos, 10);
    EXPECT_LT(pos, 60);
    for (auto &v : motor.violations)
        ADD_FAILURE() << v;
}

TEST(Integration, DimButtonsAndStandbyPowerSupply)
{
    constexpr InOutId BTN_UP = 16390, BTN_DOWN = 16391, PWM = 16394, PSU = 16395;
    MiniHost h;
    apps::cSinglePWM lamp(ID::PWM___X3_XX4_14, {PWM}, 512, 65535, 0, ID::STDBY_X1_XX1_15);
    apps::cOnOff psu(ID::STDBY_X1_XX1_15, PSU, ePowerState::INACTIVE, 10000);
    apps::cPushbuttonDual2PWM buttons(ID::DIM2B_X3_XX2_8, BTN_UP, BTN_DOWN, ID::PWM___X3_XX4_14);
    h.apps = {&buttons, &lamp, &psu};
    h.Setup();
    h.Run(1000);
    EXPECT_EQ(h.ctx.Output(PSU), OFF);

    // kurz druecken -> an, Netzteil kommt per Heartbeat mit
    h.ctx.SetInput(BTN_UP, 1);
    h.Run(100);
    h.ctx.SetInput(BTN_UP, 0);
    h.Run(2000);
    EXPECT_EQ(h.ctx.Output(PWM), 65535);
    EXPECT_EQ(h.ctx.Output(PSU), ON);

    // "runter" halten -> dunkler
    h.ctx.SetInput(BTN_DOWN, 1);
    h.Run(2500);
    h.ctx.SetInput(BTN_DOWN, 0);
    h.Run(1500);
    uint16_t dimmed = h.ctx.Output(PWM);
    EXPECT_LT(dimmed, 40000);
    EXPECT_GT(dimmed, 0);

    // lange an -> Netzteil bleibt an
    h.Run(60000);
    EXPECT_EQ(h.ctx.Output(PSU), ON);

    // kurz druecken -> aus; Netzteil folgt nach seiner Nachlaufzeit
    h.ctx.SetInput(BTN_DOWN, 1);
    h.Run(100);
    h.ctx.SetInput(BTN_DOWN, 0);
    h.Run(2000);
    EXPECT_EQ(h.ctx.Output(PWM), 0);
    h.Run(15000);
    EXPECT_EQ(h.ctx.Output(PSU), OFF);
}

TEST(Integration, RotaryEncoderDimsLamp)
{
    constexpr InOutId PWM = 16394;
    MiniHost h;
    apps::cSinglePWM lamp(ID::PWM___X3_XX4_14, {PWM}, 512, 32768, 0, ID::NO_APPLICATION);
    apps::cRotaryEncoder2PWM enc(ID::ROTAR_X3_XX3_0, eRotaryEncoder::ROTENC0, ID::PWM___X3_XX4_14);
    h.ctx.rotaryEncoders[eRotaryEncoder::ROTENC0] = {100, false};
    h.apps = {&enc, &lamp};
    h.Setup();
    h.Run(1000);

    // Druecken -> an
    h.ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].second = true;
    h.Run(100);
    h.ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].second = false;
    h.Run(2000);
    EXPECT_EQ(Status(lamp, h.ctx)[3], 32768);

    // nach rechts drehen -> heller
    for (uint16_t v = 101; v <= 104; v++)
    {
        h.ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].first = v;
        h.Run(100);
    }
    EXPECT_GT(Status(lamp, h.ctx)[3], 32768);
}

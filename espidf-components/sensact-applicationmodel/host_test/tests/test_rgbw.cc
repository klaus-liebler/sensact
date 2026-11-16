// cRgbw: RGB(W)-LED-Streifen an 3-4 PWM-Ausgaengen mit Liste "bekannter" Farben.
#include "TestHelpers.hh"
#include "rgbw.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId R = 200, G = 201, B = 202, W = 203;

    struct RgbwTest : ::testing::Test
    {
        FakeSensactContext ctx;
        std::vector<apps::Color> colors{{255, 0, 0, 0}, {0, 255, 0, 0}, {0, 0, 255, 0}, {0, 0, 0, 128}};
        std::unique_ptr<apps::cRgbw> app;

        void Make(tms_t autoOff = 0, InOutId w = W, ID standby = ID::STDBY_X1_XX1_15)
        {
            app = std::make_unique<apps::cRgbw>(ID::PWM___X1_XX1_42, R, G, B, w, autoOff, standby, colors);
            ctx.now = 1000;
            app->Setup(&ctx);
            RunFor(*app, ctx, 200);
        }
        std::array<uint16_t, 4> Out() { return {ctx.Output(R), ctx.Output(G), ctx.Output(B), ctx.Output(W)}; }
        bool IsOff() { return Out() == std::array<uint16_t, 4>{0, 0, 0, 0}; }
    };
}

TEST_F(RgbwTest, SetupSwitchesOff)
{
    Make();
    EXPECT_TRUE(ctx.HasOutput(R) && ctx.HasOutput(G) && ctx.HasOutput(B) && ctx.HasOutput(W));
    EXPECT_TRUE(IsOff());
}

TEST_F(RgbwTest, SetRgbwScalesToU16)
{
    Make();
    app->OnSET_RGBWCommand(255, 128, 1, 0, &ctx);
    EXPECT_EQ(Out(), (std::array<uint16_t, 4>{65535, 128 << 8, 1 << 8, 0}));
    auto s = Status(*app, ctx);
    EXPECT_EQ(s, (std::array<uint16_t, 4>{255, 128, 1, 0}));
}

TEST_F(RgbwTest, WhiteChannelNotWrittenWhenNotConnected)
{
    Make(0, sensact::magic::OUTPUT_NULL);
    app->OnSET_RGBWCommand(1, 2, 3, 4, &ctx);
    EXPECT_FALSE(ctx.HasOutput(sensact::magic::OUTPUT_NULL));
}

TEST_F(RgbwTest, ToggleSwitchesBetweenOffAndLastColor)
{
    Make();
    app->OnTOGGLECommand(&ctx);
    EXPECT_EQ(Out()[0], 65535); // Farbe 0 = rot
    app->OnTOGGLECommand(&ctx);
    EXPECT_TRUE(IsOff());
}

TEST_F(RgbwTest, ToggleRestoresCustomColor)
{
    Make();
    app->OnSET_RGBWCommand(10, 20, 30, 40, &ctx);
    app->OnTOGGLECommand(&ctx);
    EXPECT_TRUE(IsOff());
    app->OnTOGGLECommand(&ctx);
    EXPECT_EQ(Out(), (std::array<uint16_t, 4>{10 << 8, 20 << 8, 30 << 8, 40 << 8}));
}

TEST_F(RgbwTest, StepVerticalCyclesThroughWellKnownColorsBothDirections)
{
    Make();
    app->OnSET_SIGNALCommand(1, &ctx); // gruen
    app->OnSTEP_VERTICALCommand(+1, &ctx);
    EXPECT_EQ(Out()[2], 65535); // blau
    app->OnSTEP_VERTICALCommand(-1, &ctx);
    EXPECT_EQ(Out()[1], 65535); // gruen
    // Drehgeber schickt 4096 pro Rastung -- Richtung muss trotzdem stimmen
    app->OnSTEP_VERTICALCommand(-4096, &ctx);
    EXPECT_EQ(Out()[0], 65535) << "grosser negativer Schritt landet nicht auf der vorherigen Farbe";
}

TEST_F(RgbwTest, SetSignalWrapsAroundColorList)
{
    Make();
    app->OnSET_SIGNALCommand(6, &ctx); // 6 % 4 = 2 = blau
    EXPECT_EQ(Out()[2], 65535);
}

TEST_F(RgbwTest, AutoOffAfterConfiguredTime)
{
    Make(5000);
    app->OnSET_RGBWCommand(255, 255, 255, 255, &ctx);
    RunFor(*app, ctx, 4000);
    EXPECT_FALSE(IsOff());
    RunFor(*app, ctx, 2000);
    EXPECT_TRUE(IsOff()) << "konfiguriertes Auto-Off wird nie ausgewertet";
}

TEST_F(RgbwTest, LoopReportsChange)
{
    Make();
    app->OnSET_RGBWCommand(1, 2, 3, 4, &ctx);
    EXPECT_EQ(Tick(*app, ctx), eAppCallResult::OK_CHANGED);
    EXPECT_EQ(Tick(*app, ctx), eAppCallResult::OK);
}

TEST_F(RgbwTest, SendsHeartbeatWhileOn)
{
    Make();
    app->OnSET_RGBWCommand(1, 2, 3, 4, &ctx);
    RunFor(*app, ctx, 25000);
    EXPECT_GE(CommandsTo(ctx, ID::STDBY_X1_XX1_15).size(), 2u);
    app->OnOFFCommand(0, &ctx);
    ctx.ClearRecordings();
    RunFor(*app, ctx, 25000);
    EXPECT_TRUE(ctx.sentCommands.empty());
}

TEST_F(RgbwTest, HeartbeatIntervalIsShorterThanStandbyReceiverTimeout)
{
    // Empfaenger (cOnOff ohne autoOff) schaltet HEARTBEAT_STANDBY_RECEIVER nach dem letzten
    // Heartbeat ab -- der Sender muss deutlich oefter senden.
    Make();
    app->OnSET_RGBWCommand(1, 2, 3, 4, &ctx);
    RunFor(*app, ctx, 120000);
    size_t n = CommandsTo(ctx, ID::STDBY_X1_XX1_15).size();
    EXPECT_GE(n, (size_t)(120000 / sensact::magic::HEARTBEAT_STANDBY_RECEIVER) * 2);
}

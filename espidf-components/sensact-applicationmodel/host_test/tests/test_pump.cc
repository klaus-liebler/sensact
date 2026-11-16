// cPump: Relais zyklisch onInterval an, offInterval aus (z.B. Zirkulationspumpe).
#include "TestHelpers.hh"
#include "pump.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId RELAY = 300;
}

TEST(Pump, SetupSwitchesRelayOff)
{
    FakeSensactContext ctx;
    apps::cPump app(ID::POWIT_X1_XX1_42, RELAY, 2000, 8000);
    app.Setup(&ctx);
    EXPECT_TRUE(ctx.HasOutput(RELAY));
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST(Pump, AlternatesBetweenOnAndOffIntervals)
{
    FakeSensactContext ctx;
    apps::cPump app(ID::POWIT_X1_XX1_42, RELAY, 2000, 8000);
    ctx.now = 1000;
    app.Setup(&ctx);
    int onPhases = 0;
    bool lastOn = false;
    tms_t onTime = 0;
    for (int i = 0; i < 1000; i++) // 100 s
    {
        RunFor(app, ctx, 100);
        bool on = ctx.Output(RELAY) != 0;
        if (on && !lastOn)
            onPhases++;
        if (on)
            onTime += 100;
        lastOn = on;
    }
    EXPECT_GE(onPhases, 9);
    EXPECT_LE(onPhases, 11);
    EXPECT_NEAR((double)onTime / 100000.0, 0.2, 0.03) << "Relais bleibt dauerhaft an";
}

TEST(Pump, RelayIsDrivenWithFullActiveValue)
{
    FakeSensactContext ctx;
    apps::cPump app(ID::POWIT_X1_XX1_42, RELAY, 2000, 100);
    ctx.now = 1000;
    app.Setup(&ctx);
    RunFor(app, ctx, 500);
    EXPECT_EQ(ctx.Output(RELAY), ON) << "SetU16Output(relay, true) ergibt 1 statt ACTIVE";
}

TEST(Pump, StatusReportsState)
{
    FakeSensactContext ctx;
    apps::cPump app(ID::POWIT_X1_XX1_42, RELAY, 2000, 100);
    ctx.now = 1000;
    app.Setup(&ctx);
    RunFor(app, ctx, 500);
    EXPECT_EQ(Status(app, ctx)[1], 1);
}

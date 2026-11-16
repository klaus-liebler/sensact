// cOnOff: Relais (Licht, Steckdose, Standby-Netzteil) mit ON/OFF/TOGGLE und Auto-Off.
#include "TestHelpers.hh"
#include "onoff.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId RELAY = 16384;

    struct OnOffTest : ::testing::Test
    {
        FakeSensactContext ctx;
        void Boot(apps::cOnOff &app)
        {
            ctx.now = 1000;
            app.Setup(&ctx);
            Tick(app, ctx);
        }
    };
}

TEST_F(OnOffTest, SetupSwitchesRelayOff)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 5000);
    app.Setup(&ctx);
    EXPECT_TRUE(ctx.HasOutput(RELAY));
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST_F(OnOffTest, OnWithConfiguredAutoOffSwitchesOffAfterConfiguredTime)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 5000);
    Boot(app);
    app.OnONCommand(0, &ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    RunFor(app, ctx, 4800);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    RunFor(app, ctx, 200);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST_F(OnOffTest, OnWithExplicitTimeOverridesConfiguredAutoOff)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 5000);
    Boot(app);
    app.OnONCommand(20000, &ctx);
    RunFor(app, ctx, 19000);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    RunFor(app, ctx, 1200);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST_F(OnOffTest, OnWithoutAnyAutoOffStaysOnUntilOff)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 0);
    Boot(app);
    app.OnONCommand(0, &ctx);
    RunFor(app, ctx, sensact::time::H12_ms);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    app.OnOFFCommand(0, &ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST_F(OnOffTest, ToggleAlternates)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 0);
    Boot(app);
    app.OnTOGGLECommand(&ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    app.OnTOGGLECommand(&ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
    app.OnTOGGLE_SPECIALCommand(&ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), ON);
}

TEST_F(OnOffTest, InitialStateActiveWithoutAutoOffIsOnAfterBoot)
{
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::ACTIVE, 0);
    Boot(app);
    EXPECT_EQ(ctx.Output(RELAY), ON);
}

TEST_F(OnOffTest, StatusReportsOnOffInSlot1)
{
    // Konvention FillStatus (cApplication.hh): buf[1] = CurrentValue (On=1, Off=0)
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 0);
    Boot(app);
    EXPECT_EQ(Status(app, ctx)[1], 0);
    app.OnONCommand(0, &ctx);
    EXPECT_EQ(Status(app, ctx)[1], 1);
}

TEST_F(OnOffTest, LoopReportsChangeWhenRelaySwitches)
{
    // Ohne OK_CHANGED veroeffentlicht cApplicationHost den neuen Zustand erst beim naechsten
    // reihum faelligen REGULAR_STATUS (bei vielen Apps pro Knoten: zig Sekunden) -- Web-UI und
    // andere Knoten sehen ein- und ausgeschaltete Relais dann erst stark verzoegert.
    apps::cOnOff app(ID::POWIT_X1_XX3_9, RELAY, ePowerState::INACTIVE, 5000);
    Boot(app);
    EXPECT_EQ(Tick(app, ctx), eAppCallResult::OK);
    app.OnONCommand(0, &ctx);
    EXPECT_EQ(Tick(app, ctx), eAppCallResult::OK_CHANGED);
    EXPECT_EQ(Tick(app, ctx), eAppCallResult::OK);
    ctx.now += 5000; // Auto-Off faellig
    EXPECT_EQ(Tick(app, ctx), eAppCallResult::OK_CHANGED);
    EXPECT_EQ(Tick(app, ctx), eAppCallResult::OK);
}

TEST_F(OnOffTest, HeartbeatKeepsStandbyRelayOnWhileHeartbeatsArrive)
{
    // Standby-Netzteil einer PWM-Lampe: autoOffCfg=10000, die PWM-App schickt alle ~5.5 s ein
    // HEARTBEAT, solange die Lampe an ist.
    apps::cOnOff app(ID::STDBY_X1_XX1_15, RELAY, ePowerState::INACTIVE, 10000);
    Boot(app);
    for (int i = 0; i < 10; i++)
    {
        app.OnHEARTBEATCommand((uint32_t)ID::PWM___X3_XX4_14, &ctx);
        RunFor(app, ctx, sensact::magic::HEARTBEAT_STANDBY_SENDER);
        EXPECT_EQ(ctx.Output(RELAY), ON);
    }
    RunFor(app, ctx, 10000);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

TEST_F(OnOffTest, HeartbeatDoesNotShortenAManuallyRequestedOnTime)
{
    // Relais wurde manuell dauerhaft eingeschaltet (kein Auto-Off). Ein Heartbeat einer
    // angeschlossenen PWM-App darf daraus kein "aus in 30 s" machen.
    apps::cOnOff app(ID::STDBY_X1_XX1_15, RELAY, ePowerState::INACTIVE, 0);
    Boot(app);
    app.OnONCommand(0, &ctx);
    app.OnHEARTBEATCommand((uint32_t)ID::PWM___X3_XX4_14, &ctx);
    RunFor(app, ctx, sensact::time::H1_ms);
    EXPECT_EQ(ctx.Output(RELAY), ON);
}

TEST_F(OnOffTest, HeartbeatSwitchesOnAnOffRelay)
{
    apps::cOnOff app(ID::STDBY_X1_XX1_15, RELAY, ePowerState::INACTIVE, 0);
    Boot(app);
    app.OnHEARTBEATCommand((uint32_t)ID::PWM___X3_XX4_14, &ctx);
    RunFor(app, ctx, 100);
    EXPECT_EQ(ctx.Output(RELAY), ON);
    RunFor(app, ctx, sensact::magic::HEARTBEAT_STANDBY_RECEIVER);
    EXPECT_EQ(ctx.Output(RELAY), OFF);
}

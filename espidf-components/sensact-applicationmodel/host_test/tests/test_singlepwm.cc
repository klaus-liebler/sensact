// cSinglePWM: dimmbare Lampe(n) an einem oder mehreren PWM-Ausgaengen.
//
// Helligkeitsskala: 0..65535 (targetLevel/storedLevel/SET_VERTICAL_TARGET); am Ausgang wird ueber
// level2pwm[level >> 8] eine wahrnehmungsangepasste Kennlinie ausgegeben.
// Status: [0] an/aus, [1] storedLevel, [2] autoDim-Richtung, [3] targetLevel.
#include "TestHelpers.hh"
#include "singlepwm.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId PWM_A = 100;
    constexpr InOutId PWM_B = 101;
    constexpr uint16_t MIN_LEVEL = 512;
    constexpr uint16_t PWM_MAX = 65535;

    struct PwmTest : ::testing::Test
    {
        FakeSensactContext ctx;
        std::unique_ptr<apps::cSinglePWM> app;

        void Make(tms_t autoOff = 0, uint16_t minimal = MIN_LEVEL, uint16_t initial = 65535, ID standby = ID::STDBY_X1_XX1_15)
        {
            app = std::make_unique<apps::cSinglePWM>(ID::PWM___X3_XX4_14, std::vector<InOutId>{PWM_A, PWM_B}, minimal, initial, autoOff, standby);
            ctx.now = 1000;
            app->Setup(&ctx);
            RunFor(*app, ctx, 200);
        }
        void Run(tms_t ms) { RunFor(*app, ctx, ms); }
        uint16_t Out() { return ctx.Output(PWM_A); }
        std::array<uint16_t, 4> St() { return Status(*app, ctx); }
        void Deliver()
        {
            for (auto &c : ctx.sentCommands)
                if (c.target == app->id)
                    ctx.Dispatch(*app, c);
            ctx.sentCommands.clear();
        }
        int HeartbeatsSent()
        {
            int n = 0;
            for (auto &c : ctx.sentCommands)
                if (c.command == CMD::HEARTBEAT && c.target == ID::STDBY_X1_XX1_15)
                    n++;
            return n;
        }
    };

}

TEST_F(PwmTest, SetupWritesZeroToAllOutputs)
{
    Make();
    EXPECT_TRUE(ctx.HasOutput(PWM_A));
    EXPECT_TRUE(ctx.HasOutput(PWM_B));
    EXPECT_EQ(ctx.Output(PWM_A), 0);
    EXPECT_EQ(ctx.Output(PWM_B), 0);
}

TEST_F(PwmTest, OnFadesToStoredLevelOnAllOutputs)
{
    Make();
    app->OnONCommand(0, &ctx);
    Run(100);
    EXPECT_GT(Out(), 0);
    EXPECT_LT(Out(), PWM_MAX) << "Einschalten soll weich hochfaden, nicht springen";
    Run(1500);
    EXPECT_EQ(ctx.Output(PWM_A), PWM_MAX);
    EXPECT_EQ(ctx.Output(PWM_B), PWM_MAX);
    EXPECT_EQ(St()[0], 1);
}

TEST_F(PwmTest, OffFadesOutAndOnRestoresPreviousLevel)
{
    Make();
    app->OnSET_VERTICAL_TARGETCommand(128 * 256, &ctx);
    Run(1500);
    uint16_t half = Out();
    EXPECT_GT(half, 0);
    EXPECT_LT(half, PWM_MAX);
    app->OnOFFCommand(0, &ctx);
    Run(1500);
    EXPECT_EQ(Out(), 0);
    EXPECT_EQ(St()[0], 0);
    app->OnONCommand(0, &ctx);
    Run(1500);
    EXPECT_EQ(Out(), half);
}

TEST_F(PwmTest, ToggleSwitchesOnAndOff)
{
    Make();
    app->OnTOGGLECommand(&ctx);
    Run(1500);
    EXPECT_EQ(Out(), PWM_MAX);
    app->OnTOGGLECommand(&ctx);
    Run(1500);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, SetTargetBelowMinimalLevelClampsToMinimal)
{
    Make();
    app->OnSET_VERTICAL_TARGETCommand(1, &ctx);
    Run(1500);
    EXPECT_GT(Out(), 0);
    EXPECT_EQ(St()[3], MIN_LEVEL);
}

TEST_F(PwmTest, SetTargetZeroSwitchesOffAndKeepsStoredLevel)
{
    // Slider der Web-UI ganz links bzw. Milight-Dimmer auf 0 % = aus
    Make();
    app->OnSET_VERTICAL_TARGETCommand(40000, &ctx);
    Run(1500);
    app->OnSET_VERTICAL_TARGETCommand(0, &ctx);
    Run(1500);
    EXPECT_EQ(Out(), 0);
    app->OnONCommand(0, &ctx);
    Run(1500);
    EXPECT_EQ(St()[3], 40000);
}

TEST_F(PwmTest, WithMinimalLevelZeroOnAfterSetTargetZeroStillSwitchesOn)
{
    // Konfiguration wie "Spots Terrassendach" (minimalOnLevel 0): frueher wurde dabei storedLevel=0
    // gespeichert -- ON/TOGGLE liessen die Lampe danach dauerhaft aus.
    Make(0, 0, 65535);
    app->OnSET_VERTICAL_TARGETCommand(0, &ctx);
    Run(1500);
    app->OnONCommand(0, &ctx);
    Run(1500);
    EXPECT_GT(Out(), 0);
    app->OnTOGGLECommand(&ctx);
    Run(1500);
    app->OnTOGGLECommand(&ctx);
    Run(1500);
    EXPECT_GT(Out(), 0);
}

TEST_F(PwmTest, AutoOffAfterConfiguredTime)
{
    Make(5000);
    app->OnONCommand(0, &ctx);
    Run(4500);
    EXPECT_EQ(Out(), PWM_MAX);
    Run(2500);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, OperatingTheLampRestartsAutoOffTimer)
{
    Make(5000);
    app->OnONCommand(0, &ctx);
    Run(4000);
    app->OnSET_VERTICAL_TARGETCommand(30000, &ctx);
    Run(4000);
    EXPECT_GT(Out(), 0);
    Run(2500);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, OnWithExplicitAutoOffTime)
{
    Make();
    app->OnONCommand(3000, &ctx);
    Run(2500);
    EXPECT_EQ(Out(), PWM_MAX);
    Run(2000);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, OffWithAutoReturnToOn)
{
    Make();
    app->OnONCommand(0, &ctx);
    Run(1500);
    app->OnOFFCommand(3000, &ctx);
    Run(2000);
    EXPECT_EQ(Out(), 0);
    Run(2500);
    EXPECT_EQ(Out(), PWM_MAX);
}

TEST_F(PwmTest, PendingAutoOnIsCancelledByLaterManualOff)
{
    // OFF(5 s) -> sofort wieder ON -> manuell OFF: die Lampe darf sich danach nicht wegen des
    // alten, laengst ueberholten Auto-On selbst wieder einschalten.
    Make();
    app->OnONCommand(0, &ctx);
    Run(1500);
    app->OnOFFCommand(5000, &ctx);
    Run(500);
    app->OnONCommand(0, &ctx);
    Run(500);
    app->OnOFFCommand(0, &ctx);
    Run(10000);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, HoldToDimUpReachesFullBrightnessWithinSeconds)
{
    // PushbuttonDual2PWM: gedrueckt halten -> UP(0) ... loslassen -> STOP
    Make();
    app->OnSET_VERTICAL_TARGETCommand(1, &ctx); // minimal
    Run(1500);
    uint16_t start = Out();
    app->OnUPCommand(0, &ctx);
    Run(2000);
    EXPECT_GT(Out(), start + 5000) << "Dimmen per Gedrueckthalten ist praktisch nicht wahrnehmbar";
    Run(8000);
    EXPECT_EQ(Out(), PWM_MAX);
    app->OnSTOPCommand(&ctx);
    Run(1000);
    EXPECT_EQ(Out(), PWM_MAX);
}

TEST_F(PwmTest, HoldToDimDownStopsAtMinimalLevel)
{
    Make();
    app->OnONCommand(0, &ctx);
    Run(1500);
    app->OnDOWNCommand(0, &ctx);
    Run(2000);
    EXPECT_LT(Out(), PWM_MAX - 5000);
    Run(10000);
    app->OnSTOPCommand(&ctx);
    EXPECT_EQ(St()[3], MIN_LEVEL);
    Run(1000);
    EXPECT_GT(Out(), 0);
}

TEST_F(PwmTest, ReleaseStopsDimmingAtCurrentLevel)
{
    Make();
    app->OnSET_VERTICAL_TARGETCommand(1, &ctx);
    Run(1500);
    app->OnUPCommand(0, &ctx);
    Run(1000);
    app->OnSTOPCommand(&ctx);
    Run(500);
    uint16_t level = Out();
    Run(5000);
    EXPECT_EQ(Out(), level);
    EXPECT_GT(level, 0);
    EXPECT_LT(level, PWM_MAX);
}

TEST_F(PwmTest, DimmingAtLimitDoesNotFloodStatusUpdates)
{
    // Jedes OK_CHANGED fuehrt im Host zu einer Statusnachricht auf CAN-Bus und Websocket.
    Make();
    app->OnONCommand(0, &ctx);
    Run(1500);
    app->OnUPCommand(0, &ctx); // schon am Maximum
    Run(2000);
    int changes = RunFor(*app, ctx, 5000);
    EXPECT_EQ(changes, 0);
}

TEST_F(PwmTest, AutoOffStillWorksIfStopAfterDimmingIsLost)
{
    // STOP-Kommando (Taster losgelassen) geht verloren, z.B. CAN-Fehler: die Lampe darf deshalb
    // nicht fuer immer anbleiben.
    Make(20000);
    app->OnONCommand(0, &ctx);
    app->OnUPCommand(0, &ctx);
    Run(60000);
    EXPECT_EQ(Out(), 0);
}

TEST_F(PwmTest, SingleButtonStartAlternatesDimDirection)
{
    // PushbuttonSingle2PwmSingle: Halten -> START, Loslassen -> STOP. Innerhalb von 5 s erneut
    // gehalten -> Gegenrichtung.
    Make();
    app->OnSET_VERTICAL_TARGETCommand(30000, &ctx);
    Run(1500);
    app->OnSTARTCommand(&ctx);
    Run(1000);
    app->OnSTOPCommand(&ctx);
    uint16_t afterFirst = St()[3];
    EXPECT_GT(afterFirst, 30000);
    Run(500);
    app->OnSTARTCommand(&ctx);
    Run(1000);
    app->OnSTOPCommand(&ctx);
    EXPECT_LT(St()[3], afterFirst);
    Run(6000);
    uint16_t before = St()[3];
    app->OnSTARTCommand(&ctx);
    Run(500);
    app->OnSTOPCommand(&ctx);
    EXPECT_GT(St()[3], before) << "nach 5 s Pause beginnt das Dimmen wieder aufwaerts";
}

TEST_F(PwmTest, StepVerticalChangesLevelRelatively)
{
    Make();
    app->OnSET_VERTICAL_TARGETCommand(30000, &ctx);
    app->OnSTEP_VERTICALCommand(4096, &ctx);
    EXPECT_EQ(St()[3], 30000 + 2 * 4096);
    app->OnSTEP_VERTICALCommand(-4096, &ctx);
    EXPECT_EQ(St()[3], 30000);
    app->OnSTEP_VERTICALCommand(INT16_MAX, &ctx);
    EXPECT_EQ(St()[3], 65535);
    app->OnSTEP_VERTICALCommand(INT16_MIN, &ctx);
    app->OnSTEP_VERTICALCommand(INT16_MIN, &ctx);
    EXPECT_EQ(St()[3], MIN_LEVEL);
}

TEST_F(PwmTest, SendsHeartbeatsToStandbyControllerOnlyWhileLit)
{
    Make();
    ctx.ClearRecordings();
    Run(20000);
    EXPECT_EQ(HeartbeatsSent(), 0);
    app->OnONCommand(0, &ctx);
    Run(20000);
    int hb = HeartbeatsSent();
    EXPECT_GE(hb, 3);
    EXPECT_LE(hb, 5);
    for (auto &c : ctx.sentCommands)
        if (c.command == CMD::HEARTBEAT)
            EXPECT_EQ(PayloadU32(c), (uint32_t)ID::PWM___X3_XX4_14);
    app->OnOFFCommand(0, &ctx);
    Run(2000);
    ctx.ClearRecordings();
    Run(20000);
    EXPECT_EQ(HeartbeatsSent(), 0);
}

TEST_F(PwmTest, NoHeartbeatsWithoutStandbyController)
{
    Make(0, MIN_LEVEL, 65535, ID::NO_APPLICATION);
    app->OnONCommand(0, &ctx);
    Run(20000);
    EXPECT_TRUE(ctx.sentCommands.empty());
}

TEST_F(PwmTest, NeedsFastLoopOnlyWhileFadingOrDimming)
{
    Make();
    EXPECT_FALSE(app->NeedsFastLoop());
    app->OnONCommand(0, &ctx);
    Tick(*app, ctx, 20);
    EXPECT_TRUE(app->NeedsFastLoop());
    Run(2000);
    EXPECT_FALSE(app->NeedsFastLoop());
    app->OnDOWNCommand(0, &ctx);
    Tick(*app, ctx, 20);
    EXPECT_TRUE(app->NeedsFastLoop());
}

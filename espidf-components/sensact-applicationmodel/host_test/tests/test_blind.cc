// cBlind: Rollladen/Markise an zwei Relais, Positionsschaetzung ueber die Fahrzeit.
//
// Status-Konvention (FillStatus, s. auch web-components-sensact/sensactapps_base.ts):
//   buf[0] = eCurrentBlindState (0 ENERGY_SAVE, 1 STOP, 2 PREPARE_UP, 3 PREPARE_DOWN, 4 UP, 5 DOWN)
//   buf[1] = aktuelle Position in % (0 = ganz unten/zu, 100 = ganz oben/offen)
//   buf[2] = Bewegung (4 UP, 5 DOWN, sonst 0)
//   buf[3] = Zielposition in %
// SET_VERTICAL_TARGET (target << 15 = interne Position): 16384..49151 = 0..100 %, darunter/darueber
// sicher ganz zu/auf (Ueberfahren), 0/65535 = Dauerbetrieb ab/auf bis zum naechsten Kommando.
#include "TestHelpers.hh"
#include "BlindMotorModel.hh"
#include "blind.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId R1 = 0;
    constexpr InOutId R2 = 1;
    constexpr uint32_t TIME_UP = 20000;
    constexpr uint32_t TIME_DOWN = 15000;

    // SET_VERTICAL_TARGET-Wert fuer eine Teiloeffnung in %
    constexpr uint16_t Pct(int percent) { return (uint16_t)(16384 + percent * 32767 / 100); }

    enum BlindState : uint16_t
    {
        ENERGY_SAVE = 0,
        STOP = 1,
        PREPARE_UP = 2,
        PREPARE_DOWN = 3,
        UP = 4,
        DOWN = 5
    };

    struct BlindFixture
    {
        FakeSensactContext ctx;
        apps::cBlind app;
        BlindMotorModel motor;

        explicit BlindFixture(eRelayInterlockMode mode)
            : app(ID::BLIND_X1_XX1_42, R1, R2, mode, TIME_UP, TIME_DOWN), motor(mode, R1, R2)
        {
            ctx.now = 1000;
            app.Setup(&ctx);
            Run(1000);
        }

        void Run(tms_t ms)
        {
            // in kleinen Haeppchen, damit motor.Feed die Schaltflanken zeitnah sieht
            for (tms_t t = 0; t < ms; t += 100)
            {
                RunFor(app, ctx, 100);
                motor.Feed(ctx);
            }
        }
        // Laeuft, bis der Motor steht und die App im Energiesparzustand ist (max. maxMs)
        void RunUntilIdle(tms_t maxMs = 120000)
        {
            for (tms_t t = 0; t < maxMs; t += 100)
            {
                Run(100);
                if (State() == ENERGY_SAVE && motor.Current() == Motor::OFF)
                    return;
            }
            ADD_FAILURE() << "Rollladen kommt nicht zur Ruhe";
        }
        uint16_t State() { return Status(app, ctx)[0]; }
        uint16_t Percent() { return Status(app, ctx)[1]; }

        void Up(bool forced = false) { ctx.SendUPCommand(app.id, forced); Deliver(); }
        void Down(bool forced = false) { ctx.SendDOWNCommand(app.id, forced); Deliver(); }
        void Stop() { ctx.SendSTOPCommand(app.id); Deliver(); }
        void Target(uint16_t t) { ctx.SendSET_VERTICAL_TARGETCommand(app.id, t); Deliver(); }
        void Deliver()
        {
            for (auto &c : ctx.sentCommands)
                ctx.Dispatch(app, c);
            ctx.sentCommands.clear();
        }
        void ExpectNoViolations()
        {
            for (auto &v : motor.violations)
                ADD_FAILURE() << ModeName(eRelayInterlockMode::R1_DOWN__R2_UP) << ": " << v;
        }
    };

    struct BlindTest : ::testing::Test
    {
        BlindFixture f{eRelayInterlockMode::R1_DOWN__R2_UP};
        void TearDown() override { f.ExpectNoViolations(); }
    };
}

TEST_F(BlindTest, AfterBootRelaysAreOffAndPositionIsAssumedOpen)
{
    EXPECT_EQ(f.ctx.Output(R1), OFF);
    EXPECT_EQ(f.ctx.Output(R2), OFF);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    EXPECT_EQ(f.Percent(), 100);
}

TEST_F(BlindTest, DownFromOpenRunsDownUntilFullyClosed)
{
    f.Down();
    f.Run(300);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
    EXPECT_EQ(f.State(), DOWN);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 0);
    // mindestens die volle Fahrzeit, aber nicht endlos (Ueberfahren bis SAFE_DOWN: 1.3 x Fahrzeit)
    EXPECT_GE(f.motor.DownMs(), (tms_t)TIME_DOWN);
    EXPECT_LE(f.motor.DownMs(), (tms_t)(1.3 * TIME_DOWN) + 300);
    EXPECT_EQ(f.ctx.Output(R1), OFF);
    EXPECT_EQ(f.ctx.Output(R2), OFF);
}

TEST_F(BlindTest, SecondPressWhileMovingStops)
{
    f.Down();
    f.Run(5000);
    ASSERT_EQ(f.motor.Current(), Motor::DOWN);
    f.Down();
    f.Run(200);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    f.RunUntilIdle();
    EXPECT_GT(f.Percent(), 0);
    EXPECT_LT(f.Percent(), 100);
}

TEST_F(BlindTest, StopCommandStopsImmediately)
{
    f.Down();
    f.Run(4000);
    f.Stop();
    f.Run(200);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
}

TEST_F(BlindTest, PositionIsTrackedFromRuntime)
{
    // 0.5 * Fahrzeit nach unten ~ 50 % (Start bei 100 %)
    f.Down();
    f.Run(200 + TIME_DOWN / 2); // 200 ms PREPARE-Wartezeit
    f.Stop();
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 50, 2);
}

TEST_F(BlindTest, ComfortFunctionUpAtFullyOpenGoesDown)
{
    // Komfortfunktion (s. cBlind::OnUPCommand): ganz oben kann es nicht weiter hoch -> runter.
    // Gilt erst, wenn die Position bekannt ist (eine volle Fahrt hinter sich).
    f.Up();
    f.RunUntilIdle();
    ASSERT_EQ(f.Percent(), 100);
    f.Up();
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
}

TEST_F(BlindTest, AfterBootUpMovesUpOverFullTravel)
{
    // Nach dem Booten ist die Position unbekannt (intern "offen" angenommen). "Hoch" darf deshalb
    // nicht per Komfortfunktion runterfahren, sondern muss sicher ganz nach oben -- auch wenn der
    // Rollladen tatsaechlich ganz unten steht.
    f.Up();
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.RunUntilIdle();
    EXPECT_GE(f.motor.UpMs(), (tms_t)TIME_UP);
    EXPECT_EQ(f.Percent(), 100);
}

TEST_F(BlindTest, ShortMovesAfterBootDoNotEnableComfortFunction)
{
    // Kurze Fahrt nach dem Booten -> Position weiterhin unbekannt
    f.Up();
    f.Run(3000);
    f.Stop();
    f.RunUntilIdle();
    f.Up();
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.RunUntilIdle();
    // ... diese Fahrt lief ueber den vollen Weg -> ab jetzt bekannt, Komfortfunktion aktiv
    f.Up();
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
}

TEST_F(BlindTest, ComfortFunctionDownAtFullyClosedGoesUp)
{
    f.Down();
    f.RunUntilIdle();
    ASSERT_EQ(f.Percent(), 0);
    f.Down();
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
}

TEST_F(BlindTest, SetVerticalTargetDrivesToIntermediatePosition)
{
    f.Target(Pct(50));
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 50, 2);
    f.Target(Pct(25));
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 25, 2);
    f.Target(Pct(75)); // Richtung hoch
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 75, 2);
}

TEST_F(BlindTest, TargetScaleIsInternalPositionShiftedBy15Bits)
{
    // 16384 = "ganz zu" (0 %), 49151 = "ganz offen" (100 %), 32768 = Mitte
    f.Target(32768);
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 50, 2);
    f.Target(49151);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);
}

TEST_F(BlindTest, TargetBelowPartialRangeClosesSafelyAndStops)
{
    // Wert unterhalb 16384: ueber "ganz zu" hinaus fahren (mech. Endschalter), dann selbst anhalten
    f.Target(3277); // ~ SAFE_DOWN
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 0);
    EXPECT_GE(f.motor.DownMs(), (tms_t)TIME_DOWN) << "faehrt nicht sicher bis in die Endlage";
}

TEST_F(BlindTest, TargetAbovePartialRangeOpensSafelyAndStops)
{
    f.Target(Pct(0));
    f.RunUntilIdle();
    f.Target(60000);
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);
    EXPECT_GE(f.motor.UpMs(), (tms_t)TIME_UP);
}

TEST_F(BlindTest, TargetZeroRunsDownContinuouslyUntilNextCommand)
{
    f.Target(0);
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
    f.Run(9 * TIME_DOWN);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN) << "Dauerbetrieb endet zu frueh von selbst";
    f.Stop();
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 0);
}

TEST_F(BlindTest, TargetMaxRunsUpContinuouslyUntilNextCommand)
{
    f.Down();
    f.RunUntilIdle();
    f.Target(65535);
    f.Run(500);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.Run(9 * TIME_UP);
    EXPECT_EQ(f.motor.Current(), Motor::UP) << "Dauerbetrieb endet zu frueh von selbst";
    f.Target(Pct(50)); // anderes Kommando beendet den Dauerbetrieb
    f.RunUntilIdle();
    EXPECT_NEAR(f.Percent(), 50, 2);
}

TEST_F(BlindTest, SetVerticalTargetInOppositeDirectionReversesAfterDeadTime)
{
    f.Target(1000);
    f.Run(5000);
    ASSERT_EQ(f.motor.Current(), Motor::DOWN);
    f.Target(64000);
    f.Run(200);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    f.Run(2000);
    EXPECT_EQ(f.motor.Current(), Motor::UP) << "neues Ziel geht beim Richtungswechsel verloren";
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);
}

TEST_F(BlindTest, ContinuousRunReversesOnTargetInOppositeDirection)
{
    f.Target(0);
    f.Run(30000);
    f.Target(65535);
    f.Run(2000);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
}

TEST_F(BlindTest, ForcedUpRunsUntilStopAndKeepsPositionConsistent)
{
    // PushbuttonDual2Blind sendet beim Gedrueckthalten UP(forced=1) = Dauerbetrieb bis STOP
    // (Loslassen). Auch sehr langes Halten darf die Positionsrechnung nicht ueberlaufen lassen.
    f.Down();
    f.RunUntilIdle();
    f.Up(true);
    f.Run(9 * TIME_UP);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.Stop();
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);
}

TEST_F(BlindTest, ForcedDownRunsUntilStopAndKeepsPositionConsistent)
{
    f.Down(true);
    f.Run(9 * TIME_DOWN);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
    f.Stop();
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 0);
}

TEST_F(BlindTest, ContinuousRunStopsAtSafetyLimitIfStopIsLost)
{
    // STOP beim Loslassen des Tasters geht verloren -> spaetestens nach 10 Fahrzeiten Schluss
    f.Up(true);
    f.Run(9 * TIME_UP);
    ASSERT_EQ(f.motor.Current(), Motor::UP);
    f.Run(TIME_UP + 1000);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);

    f.Target(0); // Dauerbetrieb abwaerts per SET_VERTICAL_TARGET
    f.Run(9 * TIME_DOWN);
    ASSERT_EQ(f.motor.Current(), Motor::DOWN);
    f.Run(TIME_DOWN + 1000);
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 0);
}

TEST_F(BlindTest, LoopReportsChangeOnStartAndOnStop)
{
    // Ohne OK_CHANGED bei Start/Stopp sieht die Web-UI den Endzustand erst beim naechsten
    // regulaeren Status-Durchlauf.
    f.Down();
    bool changedOnStart = false;
    for (int i = 0; i < 5; i++)
        changedOnStart |= Tick(f.app, f.ctx) == eAppCallResult::OK_CHANGED;
    EXPECT_TRUE(changedOnStart);
    f.Run(3000);
    f.Stop();
    bool changedOnStop = false;
    for (int i = 0; i < 3; i++)
        changedOnStop |= Tick(f.app, f.ctx) == eAppCallResult::OK_CHANGED;
    EXPECT_TRUE(changedOnStop);
    f.motor.Feed(f.ctx);
}

TEST_F(BlindTest, StatusTargetEqualsPositionWhenIdle)
{
    f.Target(Pct(50));
    f.RunUntilIdle();
    auto s = Status(f.app, f.ctx);
    EXPECT_EQ(s[3], s[1]);
}

// ---------------------------------------------------------------------------------------------
// Fuer alle sechs Relais-Verschaltungen
// ---------------------------------------------------------------------------------------------
class BlindAllModes : public ::testing::TestWithParam<eRelayInterlockMode>
{
protected:
    BlindFixture f{GetParam()};
    void TearDown() override
    {
        for (auto &v : f.motor.violations)
            ADD_FAILURE() << ModeName(GetParam()) << ": " << v;
    }
};

TEST_P(BlindAllModes, DownAndUpMoveTheMotorInTheRightDirection)
{
    f.Down();
    f.Run(1000);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN);
    f.RunUntilIdle();
    EXPECT_EQ(f.motor.Current(), Motor::OFF);
    f.Up();
    f.Run(1000);
    EXPECT_EQ(f.motor.Current(), Motor::UP);
    f.RunUntilIdle();
    EXPECT_EQ(f.Percent(), 100);
}

TEST_P(BlindAllModes, QuickDoublePressReversesDirectionCorrectly)
{
    // Rollladen faehrt hoch, Benutzer drueckt zweimal schnell "runter": erster Druck = Stopp,
    // zweiter = runter. Der zweite kommt, bevor die App nach 500 ms in ENERGY_SAVE geht.
    f.Down();
    f.RunUntilIdle();
    f.Up();
    f.Run(3000);
    ASSERT_EQ(f.motor.Current(), Motor::UP);
    f.Down();
    f.Run(100);
    f.Down();
    f.Run(3000);
    EXPECT_EQ(f.motor.Current(), Motor::DOWN) << "Motor faehrt in die falsche Richtung (Richtungsrelais nicht umgeschaltet)";
    EXPECT_EQ(f.State(), DOWN);
}

INSTANTIATE_TEST_SUITE_P(Modes, BlindAllModes,
                         ::testing::Values(eRelayInterlockMode::R1_POWER__R2_UP, eRelayInterlockMode::R1_POWER__R2_DOWN,
                                           eRelayInterlockMode::R1_UP__R2_POWER, eRelayInterlockMode::R1_DOWN__R2_POWER,
                                           eRelayInterlockMode::R1_DOWN__R2_UP, eRelayInterlockMode::R1_UP__R2_DOWN),
                         [](const ::testing::TestParamInfo<eRelayInterlockMode> &i) { return std::string(ModeName(i.param)); });

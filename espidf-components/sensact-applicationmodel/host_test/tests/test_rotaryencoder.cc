// cRotaryEncoder2PWM: Drehgeber mit Taster -> TOGGLE / STEP_VERTICAL an eine Lampe.
#include "TestHelpers.hh"
#include "rotaryencoder.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    struct RotaryTest : ::testing::Test
    {
        FakeSensactContext ctx;
        apps::cRotaryEncoder2PWM app{ID::ROTAR_X3_XX3_0, eRotaryEncoder::ROTENC0, ID::PWM___X3_XX4_14};
        uint16_t value{1000};

        void SetUp() override
        {
            ctx.rotaryEncoders[eRotaryEncoder::ROTENC0] = {value, false};
            ctx.now = 10000;
            app.Setup(&ctx);
            RunFor(app, ctx, 1000);
        }
        void Turn(int detents)
        {
            value = (uint16_t)(value + detents);
            ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].first = value;
            RunFor(app, ctx, 100);
        }
        std::vector<int16_t> Steps()
        {
            std::vector<int16_t> r;
            for (auto &c : ctx.sentCommands)
                if (c.command == CMD::STEP_VERTICAL)
                    r.push_back(PayloadI16(c));
            return r;
        }
    };
}

TEST_F(RotaryTest, PressSendsToggle)
{
    ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].second = true;
    RunFor(app, ctx, 100);
    EXPECT_EQ(CommandsTo(ctx, ID::PWM___X3_XX4_14), std::vector<CMD>{CMD::TOGGLE});
}

TEST_F(RotaryTest, FirstDetentAfterIdleIsSwallowedThenStepsAreSent)
{
    // Dokumentiertes Verhalten: die erste Rastung nach >800 ms Pause "weckt" nur auf.
    Turn(+1);
    EXPECT_TRUE(Steps().empty());
    Turn(+1);
    Turn(-1);
    EXPECT_EQ(Steps(), (std::vector<int16_t>{4096, -4096}));
}

TEST_F(RotaryTest, TurnRightAfterPressIsIgnoredDuringDeadTime)
{
    Turn(+1); // aufwecken
    ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].second = true;
    RunFor(app, ctx, 100);
    Turn(+1); // < 200 ms nach dem Drueckflanke
    EXPECT_TRUE(Steps().empty());
}

TEST_F(RotaryTest, CounterWrapAroundIsASingleDetent)
{
    value = 65535;
    ctx.rotaryEncoders[eRotaryEncoder::ROTENC0].first = value;
    RunFor(app, ctx, 2000);
    Turn(-1); // aufwecken (65534)
    Turn(+1); // 65535
    Turn(+1); // 0 -- Zaehler laeuft ueber
    auto s = Steps();
    ASSERT_EQ(s.size(), 2u);
    EXPECT_EQ(s[1], 4096) << "Ueberlauf des Zaehlers wird als riesiger Rueckwaertsschritt interpretiert";
}

TEST_F(RotaryTest, FastTurnDoesNotOverflowStepIntoOppositeDirection)
{
    Turn(+1);    // aufwecken
    Turn(+10);   // 10 Rastungen innerhalb eines Loop-Takts -> 40960 passt nicht in int16
    auto s = Steps();
    ASSERT_EQ(s.size(), 1u);
    EXPECT_GT(s[0], 0) << "schnelles Rechtsdrehen dunkelt ab";
}

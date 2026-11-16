// Taster-Apps (pushbutton.hh): Flankenerkennung kurz/lang/gehalten/Doppelklick und die daraus
// gesendeten Kommandos.
#include "TestHelpers.hh"
#include "pushbutton.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId IN_A = 16384;
    constexpr InOutId IN_B = 16385;

    struct ButtonTest : ::testing::Test
    {
        FakeSensactContext ctx;
        void Boot(cApplication &app)
        {
            ctx.now = 10000;
            app.Setup(&ctx);
            RunFor(app, ctx, 1000);
            ctx.ClearRecordings();
        }
        void Press(cApplication &app, InOutId in, tms_t holdMs)
        {
            ctx.SetInput(in, 1);
            RunFor(app, ctx, holdMs);
            ctx.SetInput(in, 0);
            RunFor(app, ctx, 100);
        }
    };
}

TEST_F(ButtonTest, Single2ToggleSendsToggleToAllTargetsOnPress)
{
    apps::cPushbuttonSingle2Toggle app(ID::PUSHB_X3_XX3_5, IN_A, {ID::POWIT_X1_XX3_9, ID::POWIT_X2_XX4_10});
    Boot(app);
    ctx.SetInput(IN_A, 1);
    RunFor(app, ctx, 100);
    EXPECT_EQ(CommandsTo(ctx, ID::POWIT_X1_XX3_9), std::vector<CMD>{CMD::TOGGLE});
    EXPECT_EQ(CommandsTo(ctx, ID::POWIT_X2_XX4_10), std::vector<CMD>{CMD::TOGGLE});
    RunFor(app, ctx, 3000); // gehalten: nichts weiter
    ctx.SetInput(IN_A, 0);
    RunFor(app, ctx, 300);
    EXPECT_EQ(ctx.sentCommands.size(), 2u);
}

TEST_F(ButtonTest, Single2ToggleStatusShowsPressedState)
{
    apps::cPushbuttonSingle2Toggle app(ID::PUSHB_X1_XX1_3, IN_A, {ID::POWIT_X1_XX3_9});
    Boot(app);
    EXPECT_EQ(Status(app, ctx)[1], 0);
    ctx.SetInput(IN_A, 1);
    RunFor(app, ctx, 100);
    EXPECT_EQ(Status(app, ctx)[1], 1);
}

TEST_F(ButtonTest, Single2PwmShortPressTogglesOnRelease)
{
    apps::cPushbuttonSingle2PwmSingle app(ID::PUSHB_X1_XX1_3, IN_A, ID::PWM___X3_XX4_14);
    Boot(app);
    ctx.SetInput(IN_A, 1);
    RunFor(app, ctx, 200);
    EXPECT_TRUE(ctx.sentCommands.empty()) << "kurzer Druck wirkt erst beim Loslassen";
    ctx.SetInput(IN_A, 0);
    RunFor(app, ctx, 100);
    EXPECT_EQ(CommandsTo(ctx, ID::PWM___X3_XX4_14), std::vector<CMD>{CMD::TOGGLE});
}

TEST_F(ButtonTest, Single2PwmHoldStartsAndReleaseStopsDimming)
{
    apps::cPushbuttonSingle2PwmSingle app(ID::PUSHB_X1_XX1_3, IN_A, ID::PWM___X3_XX4_14);
    Boot(app);
    Press(app, IN_A, 2000);
    EXPECT_EQ(CommandsTo(ctx, ID::PWM___X3_XX4_14), (std::vector<CMD>{CMD::START, CMD::STOP}));
}

TEST_F(ButtonTest, Single2PwmDoubleClickEndsSwitchedOn)
{
    apps::cPushbuttonSingle2PwmSingle app(ID::PUSHB_X1_XX1_3, IN_A, ID::PWM___X3_XX4_14);
    Boot(app);
    Press(app, IN_A, 100);
    Press(app, IN_A, 100);
    // zweimal TOGGLE heben sich auf, das ON vom Doppelklick bleibt
    EXPECT_EQ(CommandsTo(ctx, ID::PWM___X3_XX4_14), (std::vector<CMD>{CMD::TOGGLE, CMD::TOGGLE, CMD::ON}));
}

TEST_F(ButtonTest, Dual2BlindShortPressSendsNonForcedUpDown)
{
    apps::cPushbuttonDual2Blind app(ID::BLINDB_X2_XX1_7, IN_A, IN_B, ID::BLIND_X2_XX3_13);
    Boot(app);
    Press(app, IN_A, 100);
    Press(app, IN_B, 100);
    ASSERT_EQ(ctx.sentCommands.size(), 2u);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::UP);
    EXPECT_EQ(ctx.sentCommands[0].payload, std::vector<uint8_t>{0});
    EXPECT_EQ(ctx.sentCommands[1].command, CMD::DOWN);
    EXPECT_EQ(ctx.sentCommands[1].payload, std::vector<uint8_t>{0});
}

TEST_F(ButtonTest, Dual2BlindHoldSendsForcedMoveAndStopOnRelease)
{
    apps::cPushbuttonDual2Blind app(ID::BLINDB_X2_XX1_7, IN_A, IN_B, ID::BLIND_X2_XX3_13);
    Boot(app);
    Press(app, IN_B, 3000);
    ASSERT_EQ(ctx.sentCommands.size(), 2u);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::DOWN);
    EXPECT_EQ(ctx.sentCommands[0].payload, std::vector<uint8_t>{1});
    EXPECT_EQ(ctx.sentCommands[1].command, CMD::STOP);
}

TEST_F(ButtonTest, Dual2BlindStatusShowsBothButtons)
{
    apps::cPushbuttonDual2Blind app(ID::BLINDB_X2_XX1_7, IN_A, IN_B, ID::BLIND_X2_XX3_13);
    Boot(app);
    ctx.SetInput(IN_A, 1);
    RunFor(app, ctx, 100);
    auto s = Status(app, ctx);
    EXPECT_EQ(s[1], 0); // down
    EXPECT_EQ(s[2], 1); // up
}

TEST_F(ButtonTest, Dual2PwmShortTogglesHoldDimsReleaseStops)
{
    apps::cPushbuttonDual2PWM app(ID::DIM2B_X3_XX2_8, IN_A, IN_B, ID::PWM___X3_XX4_14);
    Boot(app);
    Press(app, IN_A, 100);
    Press(app, IN_A, 1500);
    Press(app, IN_B, 1500);
    EXPECT_EQ(CommandsTo(ctx, ID::PWM___X3_XX4_14), (std::vector<CMD>{CMD::TOGGLE, CMD::UP, CMD::STOP, CMD::DOWN, CMD::STOP}));
}

TEST_F(ButtonTest, InputHeldAtBootTriggersOneToggle_KnownIssueE7)
{
    // Ein beim Booten bereits gedrueckter (oder dauerhaft auf 1 klemmender) Eingang ...
    apps::cPushbuttonSingle2Toggle app(ID::PUSHB_X1_XX1_3, IN_A, {ID::POWIT_X1_XX3_9});
    ctx.SetInput(IN_A, 1);
    ctx.now = 10000;
    app.Setup(&ctx);
    RunFor(app, ctx, 1000);
    // ... loest derzeit einmalig ein TOGGLE aus (bekanntes Verhalten, s. docs/known-issues.md E7)
    EXPECT_EQ(ctx.sentCommands.size(), 1u);
}

// cRgbIndicator: Signal-/Orientierungslicht aus RGB-LEDs (sensactUp: die vier Eck-LEDs) und
// IDENTIFY an die Knoten-App.
#include "TestHelpers.hh"
#include "node.hh"
#include "rgbindicator.hh"

using namespace sensact;
using namespace sensact::test;
using apps::cRgbIndicator;

namespace
{
    constexpr uint32_t WARM = 0xFFB060;

    struct RgbIndicatorTest : ::testing::Test
    {
        FakeSensactContext ctx;
        cRgbIndicator app{ID::POWIT_X1_XX3_9, {1, 2, 3, 4}, WARM, cRgbIndicator::SIG_NONE, 0};
        void Boot()
        {
            ctx.now = 1000;
            app.Setup(&ctx);
            RunFor(app, ctx, 100);
        }
        void ExpectAll(uint32_t rgb)
        {
            for (uint8_t i = 1; i <= 4; i++) EXPECT_EQ(ctx.RgbLed(i), rgb) << "LED " << (int)i;
            EXPECT_EQ(ctx.rgbLeds.count(0), 0u) << "LED 0 gehoert der App nicht";
        }
    };
}

TEST_F(RgbIndicatorTest, StartsDarkAndTogglesTheSteadyLight)
{
    Boot();
    ExpectAll(0);
    app.OnTOGGLECommand(&ctx);
    RunFor(app, ctx, 100);
    ExpectAll(WARM);
    EXPECT_EQ(Status(app, ctx)[0], 1);
    app.OnTOGGLECommand(&ctx);
    RunFor(app, ctx, 100);
    ExpectAll(0);
}

TEST_F(RgbIndicatorTest, BrightnessScalesTheColourAndZeroSwitchesOff)
{
    Boot();
    app.OnSET_VERTICAL_TARGETCommand(0x8000, &ctx);
    RunFor(app, ctx, 100);
    ExpectAll(cRgbIndicator::Scale(WARM, 0x8000));
    EXPECT_EQ(ctx.RgbLed(1), 0x7F5830u);
    app.OnSET_VERTICAL_TARGETCommand(0, &ctx);
    RunFor(app, ctx, 100);
    ExpectAll(0);
}

TEST_F(RgbIndicatorTest, SetRgbwMixesWhiteIntoAllChannels)
{
    Boot();
    app.OnSET_RGBWCommand(0x10, 0x00, 0xF0, 0x20, &ctx);
    RunFor(app, ctx, 100);
    ExpectAll(0x3020FF);
    app.OnSET_RGBWCommand(0, 0, 0, 0, &ctx);
    RunFor(app, ctx, 100);
    ExpectAll(0);
}

TEST_F(RgbIndicatorTest, DoorbellFlashesThreeTimesThenReturnsToTheSteadyLight)
{
    Boot();
    app.OnONCommand(0, &ctx);
    RunFor(app, ctx, 100);
    app.OnSET_SIGNALCommand(cRgbIndicator::SIG_DOORBELL, &ctx);
    int flashes = 0;
    bool lastWhite = false;
    for (int i = 0; i < 70; i++) // 1,4 s im 20-ms-Takt (das Muster braucht den schnellen Takt)
    {
        Tick(app, ctx, 20);
        const bool white = ctx.RgbLed(1) == 0xFFFFFF;
        if (white && !lastWhite) flashes++;
        lastWhite = white;
    }
    EXPECT_EQ(flashes, 3);
    ExpectAll(WARM);
    EXPECT_EQ(Status(app, ctx)[2], cRgbIndicator::SIG_NONE);
}

TEST_F(RgbIndicatorTest, ToggleSignalMakesTheAppADoorbellIndicator)
{
    cRgbIndicator bell{ID::POWIT_X1_XX3_9, {1}, WARM, cRgbIndicator::SIG_DOORBELL, 0};
    ctx.now = 1000;
    bell.Setup(&ctx);
    bell.OnTOGGLECommand(&ctx);
    EXPECT_TRUE(bell.NeedsFastLoop());
    Tick(bell, ctx, 20);
    EXPECT_EQ(ctx.RgbLed(1), 0xFFFFFFu);
    RunFor(bell, ctx, 2000);
    EXPECT_EQ(ctx.RgbLed(1), 0u) << "Tuerklingel ohne Dauerlicht: danach dunkel";
    EXPECT_FALSE(bell.NeedsFastLoop());
}

TEST_F(RgbIndicatorTest, AlarmBlinksUntilOff)
{
    Boot();
    app.OnSET_SIGNALCommand(cRgbIndicator::SIG_ALARM, &ctx);
    RunFor(app, ctx, 10'000);
    EXPECT_EQ(Status(app, ctx)[2], cRgbIndicator::SIG_ALARM);
    app.OnOFFCommand(0, &ctx);
    RunFor(app, ctx, 100);
    ExpectAll(0);
    EXPECT_FALSE(app.NeedsFastLoop());
}

TEST_F(RgbIndicatorTest, UnknownSignalIsIgnored)
{
    Boot();
    app.OnSET_SIGNALCommand(99, &ctx);
    EXPECT_FALSE(app.NeedsFastLoop());
}

TEST_F(RgbIndicatorTest, AutoOffAfterTheGivenTime)
{
    Boot();
    app.OnONCommand(2000, &ctx);
    RunFor(app, ctx, 1900);
    ExpectAll(WARM);
    RunFor(app, ctx, 200);
    ExpectAll(0);
}

TEST(SensactNode, IdentifyAsksTheStationToShowItself)
{
    FakeSensactContext ctx;
    apps::cSensactNode node(ID::POWIT_X1_XX3_9);
    node.OnIDENTIFYCommand(0, &ctx);
    node.OnIDENTIFYCommand(5000, &ctx);
    node.OnIDENTIFYCommand(600'000, &ctx);
    ASSERT_EQ(ctx.identifyCalls.size(), 3u);
    EXPECT_EQ(ctx.identifyCalls[0], 10'000) << "0 = Standard 10 s";
    EXPECT_EQ(ctx.identifyCalls[1], 5'000);
    EXPECT_EQ(ctx.identifyCalls[2], 60'000) << "hoechstens 60 s";
}

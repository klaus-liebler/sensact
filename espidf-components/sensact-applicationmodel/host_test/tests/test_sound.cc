// cSound: Klingel/Signalgeber (MP3 aus dem Flash) mit Tag-/Nacht-Lautstaerke.
#include "TestHelpers.hh"
#include "sound.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    struct SoundTest : ::testing::Test
    {
        FakeSensactContext ctx;
        apps::cSound app{ID::SOUND_X2_XX2_0, 0x8000, 1};
        void SetUp() override
        {
            ctx.now = 1000;
            app.Setup(&ctx);
        }
    };
}

TEST_F(SoundTest, SetSignalPlaysSelectedSoundWithManualVolume)
{
    app.OnSET_SIGNALCommand(2, &ctx);
    ASSERT_EQ(ctx.mp3Plays.size(), 1u);
    EXPECT_EQ(ctx.mp3Plays[0].volume, 0x80);
    EXPECT_NE(ctx.mp3Plays[0].buf, nullptr);
    EXPECT_GT(ctx.mp3Plays[0].len, 0u);
    EXPECT_EQ(Status(app, ctx)[1], 2);
}

TEST_F(SoundTest, DifferentSignalsPlayDifferentFiles)
{
    app.OnSET_SIGNALCommand(1, &ctx);
    app.OnSET_SIGNALCommand(2, &ctx);
    ASSERT_EQ(ctx.mp3Plays.size(), 2u);
    EXPECT_NE(ctx.mp3Plays[0].buf, ctx.mp3Plays[1].buf);
}

TEST_F(SoundTest, InvalidSignalDoesNotPlayANullBuffer)
{
    app.OnSET_SIGNALCommand(99, &ctx);
    for (auto &p : ctx.mp3Plays)
        EXPECT_NE(p.buf, nullptr) << "PlayMP3 mit nullptr/0 Bytes an die HAL";
}

TEST_F(SoundTest, MutedPlaysSilently)
{
    app.OnOFFCommand(0, &ctx);
    app.OnSET_SIGNALCommand(1, &ctx);
    app.OnTOGGLECommand(&ctx);
    ASSERT_EQ(ctx.mp3Plays.size(), 2u);
    EXPECT_EQ(ctx.mp3Plays[0].volume, 0);
    EXPECT_EQ(ctx.mp3Plays[1].volume, 0);
    app.OnONCommand(0, &ctx);
    app.OnSET_SIGNALCommand(1, &ctx);
    EXPECT_GT(ctx.mp3Plays.back().volume, 0);
}

TEST_F(SoundTest, ToggleRingsDefaultSignal)
{
    app.OnTOGGLECommand(&ctx);
    ASSERT_EQ(ctx.mp3Plays.size(), 1u);
    EXPECT_EQ(Status(app, ctx)[1], 1);
}

TEST_F(SoundTest, SetVerticalTargetSetsManualVolume)
{
    app.OnSET_VERTICAL_TARGETCommand(0x4000, &ctx);
    app.OnSET_SIGNALCommand(1, &ctx);
    EXPECT_EQ(ctx.mp3Plays.back().volume, 0x40);
    EXPECT_EQ(Status(app, ctx)[2], 0x4000);
}

TEST_F(SoundTest, StopStopsPlayback)
{
    app.OnSTOPCommand(&ctx);
    EXPECT_EQ(ctx.stopSoundCalls, 1);
}

TEST_F(SoundTest, ParamsArePersistedAndRestoredOnSetup)
{
    app.OnSET_PARAMCommand(1, 1000, &ctx);
    app.OnSET_PARAMCommand(2, 2000, &ctx);
    app.OnSET_PARAMCommand(3, 28, &ctx);
    app.OnSET_PARAMCommand(4, 88, &ctx);
    app.OnSET_PARAMCommand(99, 5, &ctx); // unbekannt -> ignoriert
    EXPECT_EQ(ctx.saveParamCalls, 0) << "SET_PARAM allein persistiert nicht";
    app.OnSAVE_PARAMCommand(&ctx);
    EXPECT_EQ(ctx.persistedParams[(uint16_t)app.id], (std::array<uint16_t, 4>{1000, 2000, 28, 88}));

    apps::cSound fresh{ID::SOUND_X2_XX2_0, 0x8000, 1};
    fresh.Setup(&ctx);
    fresh.OnSAVE_PARAMCommand(&ctx);
    EXPECT_EQ(ctx.persistedParams[(uint16_t)app.id], (std::array<uint16_t, 4>{1000, 2000, 28, 88}));
}

TEST(SoundVolumeWindow, NormalWindow)
{
    // "laut" von 07:00 (28) bis 22:00 (88)
    EXPECT_FALSE(apps::cSound::IsHighVolumePeriod(27, 28, 88));
    EXPECT_TRUE(apps::cSound::IsHighVolumePeriod(28, 28, 88));
    EXPECT_TRUE(apps::cSound::IsHighVolumePeriod(87, 28, 88));
    EXPECT_FALSE(apps::cSound::IsHighVolumePeriod(88, 28, 88));
}

TEST(SoundVolumeWindow, WindowAcrossMidnight)
{
    // "laut" von 22:00 (88) bis 06:00 (24)
    EXPECT_TRUE(apps::cSound::IsHighVolumePeriod(90, 88, 24));
    EXPECT_TRUE(apps::cSound::IsHighVolumePeriod(0, 88, 24));
    EXPECT_TRUE(apps::cSound::IsHighVolumePeriod(23, 88, 24));
    EXPECT_FALSE(apps::cSound::IsHighVolumePeriod(24, 88, 24));
    EXPECT_FALSE(apps::cSound::IsHighVolumePeriod(50, 88, 24));
}

TEST(SoundVolumeWindow, EmptyWindowMeansNeverHigh)
{
    // Default (Start == Ende == 0): nie "laut" -> Klingel laeuft immer mit lowVolume
    for (uint16_t q = 0; q < 96; q++)
        EXPECT_FALSE(apps::cSound::IsHighVolumePeriod(q, 0, 0));
}

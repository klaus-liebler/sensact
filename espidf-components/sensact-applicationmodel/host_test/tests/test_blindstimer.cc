// cBlindsTimer: Rolllaeden zur Morgen-/Abenddaemmerung oeffnen/schliessen.
#include "TestHelpers.hh"
#include "blindstimer.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr int64_t JUN21_2024 = 1718928000; // 2024-06-21T00:00:00Z
    constexpr int OPEN_OFFSET = 30, CLOSE_OFFSET = -15, RANDOM = 20;
    const std::vector<ID> BLINDS{ID::BLIND_X1_XX1_42, ID::BLIND_X1_XX1_43};

    std::unique_ptr<apps::cBlindsTimer> MakeTimer()
    {
        return std::make_unique<apps::cBlindsTimer>(ID::REMOT_X1_XX1_42, BLINDS, 52.0965, 7.6171, sunsetsunrise::eDawn::CIVIL,
                                                    OPEN_OFFSET, CLOSE_OFFSET, (uint16_t)RANDOM);
    }

    struct TimerTest : ::testing::Test
    {
        FakeSensactContext ctx;
        std::unique_ptr<apps::cBlindsTimer> app = MakeTimer();

        // Laeuft minutenweise und merkt sich, wann (Unix-Sekunden) welches Kommando an wen ging
        struct Sent
        {
            int64_t when;
            ID target;
            uint16_t value;
        };
        std::vector<Sent> RunMinutes(int minutes)
        {
            std::vector<Sent> r;
            for (int m = 0; m < minutes; m++)
            {
                ctx.sentCommands.clear();
                RunFor(*app, ctx, 60'000);
                for (auto &c : ctx.sentCommands)
                {
                    EXPECT_EQ(c.command, CMD::SET_VERTICAL_TARGET);
                    r.push_back({ctx.UnixTimeNow(), c.target, (uint16_t)(c.payload[0] | (c.payload[1] << 8))});
                }
            }
            return r;
        }
    };
}

TEST_F(TimerTest, EventTimesFollowCivilDawnWithOffsetAndRandom)
{
    time_t rise, set;
    ASSERT_TRUE(sunsetsunrise::SunriseAndSunsetOfDay<double>(JUN21_2024, 52.0965 * sunsetsunrise::DEG, 7.6171 * sunsetsunrise::DEG,
                                                             sunsetsunrise::eDawn::CIVIL, rise, set));
    for (size_t i = 0; i < BLINDS.size(); i++)
    {
        const int64_t open = app->NextEvent(i, JUN21_2024, true);
        const int64_t close = app->NextEvent(i, JUN21_2024, false);
        EXPECT_GE(open, rise + OPEN_OFFSET * 60);
        EXPECT_LE(open, rise + OPEN_OFFSET * 60 + RANDOM * 60);
        EXPECT_GE(close, set + CLOSE_OFFSET * 60);
        EXPECT_LE(close, set + CLOSE_OFFSET * 60 + RANDOM * 60);
    }
    // Zufallsanteil unterscheidet sich zwischen den Rolllaeden und den Tagen
    EXPECT_NE(app->NextEvent(0, JUN21_2024, true), app->NextEvent(1, JUN21_2024, true));
    EXPECT_NE(app->NextEvent(0, JUN21_2024, true) - JUN21_2024,
              app->NextEvent(0, JUN21_2024 + sunsetsunrise::ONE_DAY, true) - JUN21_2024 - sunsetsunrise::ONE_DAY);
}

TEST_F(TimerTest, InactiveByDefaultSendsNothing)
{
    app->Setup(&ctx);
    ctx.SetClock(JUN21_2024);
    EXPECT_TRUE(RunMinutes(24 * 60).empty());
}

TEST_F(TimerTest, ActiveTimerOpensAndClosesEachBlindOncePerDay)
{
    app->Setup(&ctx);
    app->OnONCommand(0, &ctx);
    ctx.SetClock(JUN21_2024);
    auto sent = RunMinutes(24 * 60);
    ASSERT_EQ(sent.size(), 4u);
    for (size_t i = 0; i < BLINDS.size(); i++)
    {
        int opens = 0, closes = 0;
        for (auto &s : sent)
        {
            if (s.target != BLINDS[i])
                continue;
            if (s.value == apps::cBlindsTimer::TARGET_OPEN)
            {
                opens++;
                EXPECT_NEAR(s.when, app->NextEvent(i, JUN21_2024, true), 60);
            }
            if (s.value == apps::cBlindsTimer::TARGET_CLOSE)
            {
                closes++;
                EXPECT_NEAR(s.when, app->NextEvent(i, JUN21_2024, false), 60);
            }
        }
        EXPECT_EQ(opens, 1);
        EXPECT_EQ(closes, 1);
    }
    // Werte aus dem Ueberfahrbereich von cBlind: sicher ganz auf/zu, dann Stopp
    EXPECT_GT(apps::cBlindsTimer::TARGET_OPEN, 49151);
    EXPECT_LT(apps::cBlindsTimer::TARGET_CLOSE, 16384);
}

TEST_F(TimerTest, ActiveStateSurvivesRestart)
{
    app->Setup(&ctx);
    app->OnONCommand(0, &ctx);
    auto restarted = MakeTimer();
    restarted->Setup(&ctx);
    EXPECT_EQ(Status(*restarted, ctx)[1], 1);
    restarted->OnOFFCommand(0, &ctx);
    auto again = MakeTimer();
    again->Setup(&ctx);
    EXPECT_EQ(Status(*again, ctx)[1], 0);
}

TEST_F(TimerTest, NothingHappensWhileClockIsNotSet)
{
    app->Setup(&ctx);
    app->OnONCommand(0, &ctx);
    EXPECT_TRUE(RunMinutes(24 * 60).empty());
    EXPECT_EQ(Status(*app, ctx)[2], 0xFFFF);
}

TEST_F(TimerTest, MissedEventsAreNotCaughtUp)
{
    // Knoten kommt erst mittags hoch -- das Oeffnen am Morgen wird nicht nachgeholt
    app->Setup(&ctx);
    app->OnONCommand(0, &ctx);
    ctx.SetClock(JUN21_2024 + 12 * 3600);
    auto sent = RunMinutes(60);
    EXPECT_TRUE(sent.empty());
    // Uhr springt ueber einen Schliesszeitpunkt hinweg (z.B. Korrektur um Stunden): nicht nachholen
    ctx.SetClock(JUN21_2024 + 23 * 3600);
    sent = RunMinutes(30);
    EXPECT_TRUE(sent.empty());
}

TEST_F(TimerTest, StatusShowsMinutesUntilNextEvents)
{
    app->Setup(&ctx);
    ctx.SetClock(JUN21_2024);
    RunMinutes(1);
    auto s = Status(*app, ctx);
    const int64_t nowUnix = ctx.UnixTimeNow();
    int64_t earliestOpen = std::min(app->NextEvent(0, nowUnix, true), app->NextEvent(1, nowUnix, true));
    EXPECT_NEAR(s[2], (earliestOpen - nowUnix) / 60, 2);
    EXPECT_GT(s[3], s[2]);
}

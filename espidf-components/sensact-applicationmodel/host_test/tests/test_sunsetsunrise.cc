// sunsetsunrise.hh (espidf-components/common): Sonnenauf- und -untergang. Referenzwerte fuer
// Greven (52.0965 N, 7.6171 E, s. Webmanager-Scheduler) aus den bekannten Tabellen fuer das
// Muensterland; die Naeherung ist auf wenige Minuten genau.
#include <gtest/gtest.h>
#include "sunsetsunrise.hh"

namespace
{
    using namespace sunsetsunrise;
    constexpr double LAT = 52.0965 * DEG;
    constexpr double LON = 7.6171 * DEG;
    constexpr time_t JUN21_2024 = 1718928000; // 2024-06-21T00:00:00Z
    constexpr time_t DEC21_2024 = 1734739200; // 2024-12-21T00:00:00Z
    constexpr int64_t TOL = 8 * 60;           // Sekunden

    int64_t hm(int h, int m) { return h * 3600 + m * 60; }
}

TEST(SunsetSunrise, MidsummerInGreven)
{
    time_t rise, set;
    ASSERT_TRUE(SunriseAndSunsetOfDay<double>(JUN21_2024 + 12 * 3600, LAT, LON, eDawn::NORMAL, rise, set));
    EXPECT_NEAR(rise - JUN21_2024, hm(3, 12), TOL); // 05:12 MESZ
    EXPECT_NEAR(set - JUN21_2024, hm(19, 54), TOL); // 21:54 MESZ
}

TEST(SunsetSunrise, MidwinterInGreven)
{
    time_t rise, set;
    ASSERT_TRUE(SunriseAndSunsetOfDay<double>(DEC21_2024, LAT, LON, eDawn::NORMAL, rise, set));
    EXPECT_NEAR(rise - DEC21_2024, hm(7, 35), TOL); // 08:35 MEZ
    EXPECT_NEAR(set - DEC21_2024, hm(15, 16), TOL); // 16:16 MEZ
}

TEST(SunsetSunrise, CivilDawnIsBeforeSunriseAndAfterSunset)
{
    time_t rise, set, civilRise, civilSet;
    ASSERT_TRUE(SunriseAndSunsetOfDay<double>(DEC21_2024, LAT, LON, eDawn::NORMAL, rise, set));
    ASSERT_TRUE(SunriseAndSunsetOfDay<double>(DEC21_2024, LAT, LON, eDawn::CIVIL, civilRise, civilSet));
    EXPECT_GT(rise - civilRise, 30 * 60);
    EXPECT_LT(rise - civilRise, 60 * 60);
    EXPECT_GT(civilSet - set, 30 * 60);
}

TEST(SunsetSunrise, NextEventsAreInTheFuture)
{
    time_t todayRise, todaySet, rise, set;
    ASSERT_TRUE(SunriseAndSunsetOfDay<double>(JUN21_2024, LAT, LON, eDawn::NORMAL, todayRise, todaySet));
    // 01:00 UTC: Auf- und Untergang von heute
    ASSERT_TRUE(NextSunriseAndSunset<double>(JUN21_2024 + 3600, LAT, LON, eDawn::NORMAL, rise, set));
    EXPECT_EQ(rise, todayRise);
    EXPECT_EQ(set, todaySet);
    // 12:00 UTC: Untergang heute, Aufgang morgen
    ASSERT_TRUE(NextSunriseAndSunset<double>(JUN21_2024 + 12 * 3600, LAT, LON, eDawn::NORMAL, rise, set));
    EXPECT_EQ(set, todaySet);
    EXPECT_GT(rise, JUN21_2024 + ONE_DAY);
    EXPECT_LT(rise, JUN21_2024 + ONE_DAY + 6 * 3600);
}

TEST(SunsetSunrise, PolarDayHasNoSunset)
{
    time_t rise = 0, set = 0;
    EXPECT_FALSE(SunriseAndSunsetOfDay<double>(JUN21_2024, 78.0 * DEG, 15.0 * DEG, eDawn::NORMAL, rise, set));
    EXPECT_EQ(rise, 0);
}

TEST(SunsetSunrise, DaysSince2000)
{
    EXPECT_EQ(DaysSinceJan1_2000UTC(UNIX_TIME_SECS_1_1_2000), 0);
    EXPECT_EQ(DaysSinceJan1_2000UTC(UNIX_TIME_SECS_1_1_2000 + 3 * ONE_DAY + 5), 3);
}

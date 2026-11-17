// cMilightController: Milight-Fernbedienung (FUT089) -> Kommandos an Lampe und Rolllaeden.
#include "TestHelpers.hh"
#include <atomic>
#include <thread>
#include "milightcontroller.hh"

using namespace sensact;
using namespace sensact::test;
namespace keys = sensact::apps::milight::keycodesFUT89;
namespace cmds = sensact::apps::milight::commandsFUT89;

namespace
{
    struct MilightTest : ::testing::Test
    {
        FakeSensactContext ctx;
        apps::cMilightController app{ID::REMOT_X1_XX1_42, ID::PWM___X1_XX1_42, {ID::BLIND_X1_XX1_42, ID::BLIND_X1_XX1_43}, 43'200'000};
        int64_t micros{1'000'000};

        void Receive(uint8_t cmd, uint8_t arg)
        {
            micros += 1'000'000; // 1 s Abstand: keine Entprellung
            app.ReceivedFromMilight(cmd, arg, micros);
        }
    };
}

TEST_F(MilightTest, OnAndOffKeysSwitchTheLight)
{
    Receive(cmds::KEYS, keys::KON);
    Receive(cmds::KEYS, keys::KOFF);
    app.Loop(&ctx);
    ASSERT_EQ(ctx.sentCommands.size(), 2u);
    EXPECT_EQ(ctx.sentCommands[0].target, ID::PWM___X1_XX1_42);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::ON);
    EXPECT_EQ(PayloadU32(ctx.sentCommands[0]), 43'200'000u);
    EXPECT_EQ(ctx.sentCommands[1].command, CMD::OFF);
}

TEST_F(MilightTest, GroupKeysMoveTheBlinds)
{
    Receive(cmds::KEYS, keys::K1_0); // Gruppe 1 hoch
    Receive(cmds::KEYS, keys::K1_1); // Gruppe 1 runter
    Receive(cmds::KEYS, keys::K2_0); // Gruppe 2 hoch
    Receive(cmds::KEYS, keys::K3_1); // Gruppe 3: nicht belegt
    app.Loop(&ctx);
    ASSERT_EQ(ctx.sentCommands.size(), 3u);
    EXPECT_EQ(ctx.sentCommands[0].target, ID::BLIND_X1_XX1_42);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::UP);
    EXPECT_EQ(ctx.sentCommands[0].payload, std::vector<uint8_t>{0}); // nicht "forced"
    EXPECT_EQ(ctx.sentCommands[1].target, ID::BLIND_X1_XX1_42);
    EXPECT_EQ(ctx.sentCommands[1].command, CMD::DOWN);
    EXPECT_EQ(ctx.sentCommands[2].target, ID::BLIND_X1_XX1_43);
    EXPECT_EQ(ctx.sentCommands[2].command, CMD::UP);
}

TEST_F(MilightTest, DimmerWheelSetsBrightness)
{
    Receive(cmds::DIMMER, 50);
    Receive(cmds::DIMMER, 100);
    app.Loop(&ctx);
    ASSERT_EQ(ctx.sentCommands.size(), 2u);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::SET_VERTICAL_TARGET);
    EXPECT_NEAR(ctx.sentCommands[0].payload[0] | (ctx.sentCommands[0].payload[1] << 8), 32767, 2);
    EXPECT_EQ(ctx.sentCommands[1].payload, (std::vector<uint8_t>{0xFF, 0xFF}));
}

TEST_F(MilightTest, RepeatedTransmissionsWithin500msAreIgnored)
{
    // die Fernbedienung sendet jeden Tastendruck mehrfach
    app.ReceivedFromMilight(cmds::KEYS, keys::KON, 1'000'000);
    app.ReceivedFromMilight(cmds::KEYS, keys::KON, 1'100'000);
    app.ReceivedFromMilight(cmds::KEYS, keys::KON, 1'400'000);
    app.ReceivedFromMilight(cmds::KEYS, keys::KON, 1'600'000); // > 500 ms nach dem ersten
    app.Loop(&ctx);
    EXPECT_EQ(ctx.sentCommands.size(), 2u);
}

TEST_F(MilightTest, FullQueueDropsInsteadOfOverwriting)
{
    for (int i = 0; i < 40; i++)
        Receive(cmds::DIMMER, (uint8_t)(i % 90)); // abwechselnde Werte, keine Entprellung
    app.Loop(&ctx);
    EXPECT_EQ(ctx.sentCommands.size(), 16u); // Puffergroesse
    // die ersten 16 kamen in Reihenfolge an
    EXPECT_NEAR(ctx.sentCommands[1].payload[0] | (ctx.sentCommands[1].payload[1] << 8), 1 * 655, 2);
}

TEST_F(MilightTest, ProducerAndConsumerOnDifferentThreads)
{
    // Funk-Task und App-Loop laufen nebenlaeufig: nichts darf verloren gehen oder doppelt kommen,
    // solange der Puffer nicht ueberlaeuft. ctx gehoert allein dem Verbraucher; der Erzeuger sieht
    // nur den atomaren Zaehler "consumed".
    constexpr int N = 2000;
    std::atomic<int> consumed{0};
    std::atomic<bool> done{false};
    std::thread radio([&] {
        for (int i = 0; i < N; i++)
        {
            while (i - consumed.load() >= 16)
                std::this_thread::yield(); // Puffer voll: warten
            app.ReceivedFromMilight(cmds::DIMMER, (uint8_t)(i % 2 ? 10 : 20), (int64_t)i * 1'000'000);
        }
        done = true;
    });
    while (!done || consumed.load() < N)
    {
        app.Loop(&ctx);
        consumed.store((int)ctx.sentCommands.size());
        if (done && consumed.load() < N && ctx.sentCommands.size() == (size_t)consumed.load())
        {
            app.Loop(&ctx);
            consumed.store((int)ctx.sentCommands.size());
            if (consumed.load() < N)
                break; // nichts mehr da: Verlust -- unten ausgewertet
        }
    }
    radio.join();
    ASSERT_EQ(ctx.sentCommands.size(), (size_t)N);
    for (size_t i = 0; i < ctx.sentCommands.size(); i++)
    {
        const int v = ctx.sentCommands[i].payload[0] | (ctx.sentCommands[i].payload[1] << 8);
        ASSERT_NEAR(v, (int)((i % 2 ? 10 : 20) * (65535 / 100.0)), 1) << "Reihenfolge verletzt bei " << i;
    }
}

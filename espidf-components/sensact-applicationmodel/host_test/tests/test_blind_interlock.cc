// Randomisierter Dauertest fuer cBlind: viele zufaellige Kommandofolgen in zufaelligen Abstaenden,
// fuer jede Relais-Verschaltung. Geprueft wird nicht ein bestimmtes Ergebnis, sondern dass
// bestimmte Dinge NIE passieren (s. BlindMotorModel.hh) und dass Statusmeldung und
// Positionsschaetzung zur tatsaechlichen Motorbewegung passen.
#include <random>
#include "TestHelpers.hh"
#include "BlindMotorModel.hh"
#include "blind.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    constexpr InOutId R1 = 10;
    constexpr InOutId R2 = 11;
    constexpr uint32_t TIME_UP = 20000;
    constexpr uint32_t TIME_DOWN = 15000;

    class BlindRandomized : public ::testing::TestWithParam<eRelayInterlockMode>
    {
    };
}

TEST_P(BlindRandomized, InvariantsHoldForRandomCommandSequences)
{
    const auto mode = GetParam();
    FakeSensactContext ctx;
    apps::cBlind app(ID::BLIND_X1_XX1_42, R1, R2, mode, TIME_UP, TIME_DOWN);
    BlindMotorModel motor(mode, R1, R2);
    ctx.now = 1000;
    app.Setup(&ctx);

    std::mt19937 rng(0xB11D + (int)mode);
    std::uniform_int_distribution<int> cmdDist(0, 6);
    std::uniform_int_distribution<int> waitDist(0, 4000);
    std::uniform_int_distribution<int> targetDist(0, 65535);

    // Physikalische Position (0..1 = zu..offen), nur aus der tatsaechlichen Motorlaufzeit
    double physical = 1.0; // App nimmt nach dem Booten "offen" an -- Test startet genauso
    tms_t lastMotorEval = ctx.now;
    Motor lastMotor = Motor::OFF;
    auto integrate = [&](tms_t until) {
        double dt = (double)(until - lastMotorEval);
        if (lastMotor == Motor::UP)
            physical = std::min(1.0, physical + dt / TIME_UP);
        else if (lastMotor == Motor::DOWN)
            physical = std::max(0.0, physical - dt / TIME_DOWN);
        lastMotorEval = until;
    };

    int consistencyErrors = 0;
    for (int step = 0; step < 1500 && consistencyErrors < 5; step++)
    {
        // Schritt 0: Referenzfahrt nach unten -- erst danach ist die Position der App bekannt
        // (nach dem Booten faehrt sie vorsichtshalber ueber den vollen Weg).
        int cmd = step == 0 ? 1 : cmdDist(rng);
        switch (cmd)
        {
        case 0: ctx.SendUPCommand(app.id, 0); break;
        case 1: ctx.SendDOWNCommand(app.id, 0); break;
        case 2: ctx.SendUPCommand(app.id, 1); break;
        case 3: ctx.SendDOWNCommand(app.id, 1); break;
        case 4: ctx.SendSTOPCommand(app.id); break;
        default: ctx.SendSET_VERTICAL_TARGETCommand(app.id, (uint16_t)targetDist(rng)); break;
        }
        for (auto &c : ctx.sentCommands)
            ctx.Dispatch(app, c);
        ctx.sentCommands.clear();

        tms_t wait = waitDist(rng);
        if (step % 50 == 0)
            wait = 60000; // ab und zu lange laufen lassen (Endlagen erreichen)
        for (tms_t t = 0; t < wait; t += 100)
        {
            RunFor(app, ctx, 100);
            // Flanken einzeln integrieren, damit die physikalische Position stimmt
            auto before = motor.transitions.size();
            motor.Feed(ctx);
            for (size_t i = before; i < motor.transitions.size(); i++)
            {
                integrate(motor.transitions[i].time);
                lastMotor = motor.transitions[i].motor;
            }
            integrate(ctx.now);

            // Statusmeldung muss zur tatsaechlichen Bewegung passen
            auto s = Status(app, ctx);
            Motor reported = s[0] == 4 ? Motor::UP : s[0] == 5 ? Motor::DOWN : Motor::OFF;
            if (reported != motor.Current())
            {
                ADD_FAILURE() << ModeName(mode) << " step " << step << " t=" << ctx.now << ": App meldet "
                              << MotorName(reported) << ", Motor ist aber " << MotorName(motor.Current());
                consistencyErrors++;
                break;
            }
        }
        // Im Stillstand muss die geschaetzte Position zur physikalischen passen
        auto s = Status(app, ctx);
        if (motor.Current() == Motor::OFF && (s[0] == 0 || s[0] == 1))
        {
            if (std::abs((int)s[1] - (int)std::lround(physical * 100)) > 3)
            {
                ADD_FAILURE() << ModeName(mode) << " step " << step << ": App schaetzt " << s[1] << " %, physikalisch "
                              << physical * 100 << " %";
                consistencyErrors++;
            }
        }
    }
    for (auto &v : motor.violations)
        ADD_FAILURE() << ModeName(mode) << ": " << v;
}

INSTANTIATE_TEST_SUITE_P(Modes, BlindRandomized,
                         ::testing::Values(eRelayInterlockMode::R1_POWER__R2_UP, eRelayInterlockMode::R1_POWER__R2_DOWN,
                                           eRelayInterlockMode::R1_UP__R2_POWER, eRelayInterlockMode::R1_DOWN__R2_POWER,
                                           eRelayInterlockMode::R1_DOWN__R2_UP, eRelayInterlockMode::R1_UP__R2_DOWN),
                         [](const ::testing::TestParamInfo<eRelayInterlockMode> &i) { return std::string(ModeName(i.param)); });

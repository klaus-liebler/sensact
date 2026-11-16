#pragma once
// Elektrisches Modell eines Rollladenmotors an zwei Relais -- unabhaengig von cBlind
// implementiert, damit die Tests die *tatsaechliche* Motorbewegung aus den geschalteten
// Ausgaengen ableiten, statt der App ihre eigene Zustandsmeldung zu glauben.
//
// Zusaetzlich prueft Feed() bei JEDER einzelnen Relais-Schaltflanke die
// Sicherheitsinvarianten:
//  1. Bei direkter Ansteuerung (R1_DOWN__R2_UP / R1_UP__R2_DOWN) sind nie beide
//     Richtungsrelais gleichzeitig an (Kurzschluss der Motorwicklungen).
//  2. Bei Leistungs-/Richtungsrelais wird das Richtungsrelais nie umgeschaltet, waehrend das
//     Leistungsrelais an ist (Schalten unter Last, Kontaktabbrand, Motor-Reversierschlag).
//  3. Zwischen zwei entgegengesetzten Fahrten steht der Motor mindestens minReverseDeadTimeMs.
#include <string>
#include <vector>
#include "FakeSensactContext.hh"

namespace sensact::test
{
    enum class Motor
    {
        OFF,
        UP,
        DOWN,
        SHORT_CIRCUIT
    };

    inline const char *MotorName(Motor m)
    {
        switch (m)
        {
        case Motor::OFF: return "OFF";
        case Motor::UP: return "UP";
        case Motor::DOWN: return "DOWN";
        default: return "SHORT_CIRCUIT";
        }
    }

    class BlindMotorModel
    {
    public:
        BlindMotorModel(eRelayInterlockMode mode, InOutId r1, InOutId r2, tms_t minReverseDeadTimeMs = 500)
            : mode(mode), r1(r1), r2(r2), minReverseDeadTimeMs(minReverseDeadTimeMs) {}

        // Alle seit dem letzten Aufruf geschriebenen Ausgaenge (ctx.outputLog) in Reihenfolge
        // nachspielen und dabei die Invarianten pruefen. Leert ctx.outputLog.
        void Feed(FakeSensactContext &ctx)
        {
            for (auto &w : ctx.outputLog)
            {
                if (w.id != r1 && w.id != r2)
                    continue;
                bool &target = (w.id == r1) ? r1On : r2On;
                bool newVal = w.value != 0;
                if (newVal == target)
                    continue;
                // Invariante 2: Richtungsrelais schaltet unter Last?
                if (IsPowerMode() && w.id == DirectionRelay() && PowerOn())
                    Violation(w.time, "Richtungsrelais schaltet, waehrend das Leistungsrelais an ist");
                target = newVal;
                Motor m = Direction();
                if (m == Motor::SHORT_CIRCUIT)
                    Violation(w.time, "beide Richtungsrelais gleichzeitig an");
                if (m != current)
                {
                    if (current != Motor::OFF && m != Motor::OFF)
                        Violation(w.time, "Richtungswechsel ohne Stillstand");
                    if (m != Motor::OFF && lastMoving != Motor::OFF && m != lastMoving && w.time - offSince < minReverseDeadTimeMs)
                        Violation(w.time, "Reversieren nach nur " + std::to_string(w.time - offSince) + " ms Stillstand");
                    if (current != Motor::OFF)
                    {
                        // Fahrt beendet -> Fahrstrecke aufaddieren
                        tms_t dt = w.time - movingSince;
                        if (current == Motor::UP)
                            upMs += dt;
                        else if (current == Motor::DOWN)
                            downMs += dt;
                        lastMoving = current;
                        offSince = w.time;
                    }
                    if (m != Motor::OFF)
                        movingSince = w.time;
                    current = m;
                    transitions.push_back({w.time, m});
                }
            }
            ctx.outputLog.clear();
        }

        Motor Current() const { return current; }

        // Netto-Fahrzeit nach oben (positiv) bzw. unten (negativ), nur abgeschlossene Fahrten
        tms_t NetUpMs() const { return upMs - downMs; }
        tms_t UpMs() const { return upMs; }
        tms_t DownMs() const { return downMs; }

        struct Transition
        {
            tms_t time;
            Motor motor;
        };
        std::vector<Transition> transitions;
        std::vector<std::string> violations;

    private:
        eRelayInterlockMode mode;
        InOutId r1, r2;
        tms_t minReverseDeadTimeMs;
        bool r1On{false}, r2On{false};
        Motor current{Motor::OFF};
        Motor lastMoving{Motor::OFF};
        tms_t offSince{-1000000};
        tms_t movingSince{0};
        tms_t upMs{0}, downMs{0};

        void Violation(tms_t t, const std::string &what)
        {
            violations.push_back("t=" + std::to_string(t) + "ms: " + what);
        }

        bool IsPowerMode() const
        {
            return mode != eRelayInterlockMode::R1_DOWN__R2_UP && mode != eRelayInterlockMode::R1_UP__R2_DOWN;
        }
        InOutId DirectionRelay() const
        {
            switch (mode)
            {
            case eRelayInterlockMode::R1_POWER__R2_UP:
            case eRelayInterlockMode::R1_POWER__R2_DOWN:
                return r2;
            default:
                return r1;
            }
        }
        bool PowerOn() const
        {
            switch (mode)
            {
            case eRelayInterlockMode::R1_POWER__R2_UP:
            case eRelayInterlockMode::R1_POWER__R2_DOWN:
                return r1On;
            default:
                return r2On;
            }
        }
        Motor Direction() const
        {
            switch (mode)
            {
            case eRelayInterlockMode::R1_POWER__R2_UP:
                return !r1On ? Motor::OFF : (r2On ? Motor::UP : Motor::DOWN);
            case eRelayInterlockMode::R1_POWER__R2_DOWN:
                return !r1On ? Motor::OFF : (r2On ? Motor::DOWN : Motor::UP);
            case eRelayInterlockMode::R1_UP__R2_POWER:
                return !r2On ? Motor::OFF : (r1On ? Motor::UP : Motor::DOWN);
            case eRelayInterlockMode::R1_DOWN__R2_POWER:
                return !r2On ? Motor::OFF : (r1On ? Motor::DOWN : Motor::UP);
            case eRelayInterlockMode::R1_DOWN__R2_UP:
                return r1On && r2On ? Motor::SHORT_CIRCUIT : r1On ? Motor::DOWN : r2On ? Motor::UP : Motor::OFF;
            case eRelayInterlockMode::R1_UP__R2_DOWN:
                return r1On && r2On ? Motor::SHORT_CIRCUIT : r1On ? Motor::UP : r2On ? Motor::DOWN : Motor::OFF;
            }
            return Motor::OFF;
        }
    };

    inline const char *ModeName(eRelayInterlockMode m)
    {
        switch (m)
        {
        case eRelayInterlockMode::R1_POWER__R2_UP: return "R1_POWER__R2_UP";
        case eRelayInterlockMode::R1_POWER__R2_DOWN: return "R1_POWER__R2_DOWN";
        case eRelayInterlockMode::R1_UP__R2_POWER: return "R1_UP__R2_POWER";
        case eRelayInterlockMode::R1_DOWN__R2_POWER: return "R1_DOWN__R2_POWER";
        case eRelayInterlockMode::R1_DOWN__R2_UP: return "R1_DOWN__R2_UP";
        case eRelayInterlockMode::R1_UP__R2_DOWN: return "R1_UP__R2_DOWN";
        }
        return "?";
    }
}

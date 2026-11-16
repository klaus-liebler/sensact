#pragma once
#include <gtest/gtest.h>
#include "FakeSensactContext.hh"

namespace sensact::test
{
    using sensact::apps::cApplication;
    using ID = sensact::eApplicationID;
    using CMD = sensact::eCommandType;

    constexpr uint16_t ON = sensact::magic::ACTIVE;
    constexpr uint16_t OFF = sensact::magic::INACTIVE;

    // Taktung wie cApplicationHost::Loop: NodemasterTask tickt alle 20 ms, eine "kalte" App ist
    // per Bucket-Rotation nur jeden 5. Tick (= alle 100 ms) dran, eine App mit NeedsFastLoop()
    // jeden Tick.
    constexpr tms_t HOST_TICK_MS = 20;
    constexpr tms_t HOST_BUCKETS = 5;

    // Fuehrt eine App wie der echte Host aus, bis ctx.now um durationMs weitergelaufen ist.
    // Rueckgabe: Anzahl der Loop()-Aufrufe, die OK_CHANGED geliefert haben.
    inline int RunFor(cApplication &app, FakeSensactContext &ctx, tms_t durationMs)
    {
        int changed = 0;
        bool fast = false;
        const tms_t end = ctx.now + durationMs;
        while (ctx.now < end)
        {
            ctx.now += HOST_TICK_MS;
            bool inBucket = (ctx.now / HOST_TICK_MS) % HOST_BUCKETS == 0;
            if (!inBucket && !fast)
                continue;
            if (app.Loop(&ctx) == sensact::eAppCallResult::OK_CHANGED)
                changed++;
            fast = app.NeedsFastLoop();
        }
        return changed;
    }

    // Ein einzelner Loop()-Aufruf, nachdem die Zeit um dtMs weitergelaufen ist.
    inline sensact::eAppCallResult Tick(cApplication &app, FakeSensactContext &ctx, tms_t dtMs = 100)
    {
        ctx.now += dtMs;
        return app.Loop(&ctx);
    }

    inline std::array<uint16_t, 4> Status(cApplication &app, FakeSensactContext &ctx)
    {
        std::array<uint16_t, 4> buf{0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD};
        app.FillStatus(ctx, buf);
        return buf;
    }

    inline uint32_t PayloadU32(const SentCommand &c, size_t offset = 0)
    {
        return (uint32_t)c.payload.at(offset) | ((uint32_t)c.payload.at(offset + 1) << 8) |
               ((uint32_t)c.payload.at(offset + 2) << 16) | ((uint32_t)c.payload.at(offset + 3) << 24);
    }
    inline int16_t PayloadI16(const SentCommand &c, size_t offset = 0)
    {
        return (int16_t)((uint16_t)c.payload.at(offset) | ((uint16_t)c.payload.at(offset + 1) << 8));
    }

    // Liefert alle seit dem letzten ClearRecordings() an target gesendeten Kommandotypen.
    inline std::vector<CMD> CommandsTo(const FakeSensactContext &ctx, ID target)
    {
        std::vector<CMD> r;
        for (auto &c : ctx.sentCommands)
            if (c.target == target)
                r.push_back(c.command);
        return r;
    }
}

// Lesbare Ausgabe von Enums in gtest-Fehlermeldungen
namespace sensact
{
    inline void PrintTo(eCommandType c, std::ostream *os) { *os << CommandNames[(uint16_t)c]; }
    inline void PrintTo(eApplicationID a, std::ostream *os)
    {
        if ((uint16_t)a < (uint16_t)eApplicationID::CNT)
            *os << ApplicationNames[(uint16_t)a];
        else
            *os << "AppId(" << (uint16_t)a << ")";
    }
}

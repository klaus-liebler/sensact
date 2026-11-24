#include "rgbindicator.hh"

#include <sensact_logger.hh>

#include "cApplications.hh"

#define TAG "SIGNL"

namespace sensact::apps
{
    namespace
    {
        constexpr uint32_t WHITE = 0xFFFFFF, RED = 0xFF0000, GREEN = 0x00FF00, BLUE = 0x0040FF;
        constexpr tms_t ALARM_MAX_MS = 60'000;
    }

    cRgbIndicator::cRgbIndicator(eApplicationID id, std::vector<uint8_t> leds, uint32_t color, uint16_t toggleSignal,
                                 uint32_t autoOffMs)
        : cApplication(id), leds(std::move(leds)), color(color & 0xFFFFFF), toggleSignal(toggleSignal), autoOffCfg(autoOffMs)
    {
    }

    eAppCallResult cRgbIndicator::Setup(iSensactContext *ctx)
    {
        Show(ctx, 0);
        return eAppCallResult::OK;
    }

    uint32_t cRgbIndicator::Scale(uint32_t rgb, uint16_t level)
    {
        const auto ch = [&](unsigned shift) { return (((rgb >> shift) & 0xFF) * (uint32_t)level / 0xFFFF) << shift; };
        return ch(16) | ch(8) | ch(0);
    }

    bool cRgbIndicator::PatternColor(uint16_t signal, tms_t t, uint32_t &rgb)
    {
        switch (signal)
        {
        case SIG_DOORBELL:
            if (t >= 1200) return false;
            rgb = (t % 400) < 200 ? WHITE : 0;
            return true;
        case SIG_ALARM:
            if (t >= ALARM_MAX_MS) return false;
            rgb = (t % 500) < 250 ? RED : 0;
            return true;
        case SIG_NOTIFY:
        {
            if (t >= 6000) return false;
            const tms_t p = t % 2000; // 1 s auf, 1 s ab
            const uint32_t level = (uint32_t)(p < 1000 ? p : 2000 - p) * 0xFFFF / 1000;
            rgb = Scale(BLUE, (uint16_t)level);
            return true;
        }
        case SIG_OK:
            if (t >= 1000) return false;
            rgb = GREEN;
            return true;
        case SIG_ERROR:
            if (t >= 1000) return false;
            rgb = RED;
            return true;
        default:
            return false;
        }
    }

    void cRgbIndicator::Show(iSensactContext *ctx, uint32_t rgb)
    {
        if (rgb == shown) return;
        shown = rgb;
        const CRGB c((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
        for (const uint8_t i : leds)
        {
            ctx->SetRGBLed(i, c);
        }
    }

    eAppCallResult cRgbIndicator::Loop(iSensactContext *ctx)
    {
        const tms_t now = ctx->Now();
        if (on && autoOffAt != 0 && now >= autoOffAt)
        {
            LOGI(TAG, "%s: auto off", N());
            on = false;
            autoOffAt = 0;
            changed = true;
        }
        uint32_t rgb = on ? Scale(color, level) : 0;
        if (signal != SIG_NONE)
        {
            uint32_t p;
            if (PatternColor(signal, now - signalStart, p))
            {
                rgb = p;
            }
            else
            {
                signal = SIG_NONE; // vorbei: zurueck zum Dauerlicht
                changed = true;
            }
        }
        Show(ctx, rgb);
        if (changed)
        {
            changed = false;
            return eAppCallResult::OK_CHANGED;
        }
        return eAppCallResult::OK;
    }

    eFillStatusResult cRgbIndicator::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4> &buf)
    {
        buf[0] = on ? 1 : 0;
        buf[1] = level;
        buf[2] = signal;
        // Farbe als RGB565
        buf[3] = (uint16_t)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
        return eFillStatusResult::OK;
    }

    void cRgbIndicator::OnONCommand(uint32_t autoReturnToOffMsecs, iSensactContext *ctx)
    {
        on = true;
        if (level == 0) level = 0xFFFF;
        const tms_t d = autoReturnToOffMsecs != 0 ? autoReturnToOffMsecs : autoOffCfg;
        autoOffAt = d != 0 ? ctx->Now() + d : 0;
        changed = true;
    }

    void cRgbIndicator::OnOFFCommand(uint32_t autoReturnToOnMsecs, iSensactContext *ctx)
    {
        on = false;
        signal = SIG_NONE;
        autoOffAt = 0;
        changed = true;
    }

    void cRgbIndicator::OnTOGGLECommand(iSensactContext *ctx)
    {
        if (toggleSignal != SIG_NONE)
        {
            OnSET_SIGNALCommand(toggleSignal, ctx);
        }
        else if (on)
        {
            OnOFFCommand(0, ctx);
        }
        else
        {
            OnONCommand(0, ctx);
        }
    }

    void cRgbIndicator::OnSET_RGBWCommand(uint8_t R, uint8_t G, uint8_t B, uint8_t W, iSensactContext *ctx)
    {
        // Weiss mischt sich in alle drei Kanaele (die WS2812 haben keinen eigenen Weiss-Kanal)
        const auto mix = [W](uint8_t c) { return (uint32_t)(c + W > 255 ? 255 : c + W); };
        color = (mix(R) << 16) | (mix(G) << 8) | mix(B);
        if (color == 0)
        {
            OnOFFCommand(0, ctx);
            return;
        }
        OnONCommand(0, ctx);
    }

    void cRgbIndicator::OnSET_VERTICAL_TARGETCommand(uint16_t target, iSensactContext *ctx)
    {
        level = target;
        if (target == 0)
        {
            OnOFFCommand(0, ctx);
            return;
        }
        OnONCommand(0, ctx);
    }

    void cRgbIndicator::OnSET_SIGNALCommand(uint16_t s, iSensactContext *ctx)
    {
        uint32_t ignored;
        if (s != SIG_NONE && !PatternColor(s, 0, ignored))
        {
            LOGW(TAG, "%s: unknown signal %u", N(), s);
            return;
        }
        signal = s;
        signalStart = ctx->Now();
        changed = true;
    }
}

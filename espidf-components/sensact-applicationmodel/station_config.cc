#include "station_config.hh"
#include <type_traits>
#include <wsprotocol_cpp/ws_protocol.hh>
#include <sensact_commons.hh>
#include "cApplications.hh"
#include "apps/blind.hh"
#include "apps/lightbarrier.hh"
#include "apps/node.hh"
#include "apps/onoff.hh"
#include "apps/pump.hh"
#include "apps/pushbutton.hh"
#include "apps/rgbw.hh"
#include "apps/rgbindicator.hh"
#include "apps/rotaryencoder.hh"
#include "apps/singlepwm.hh"

// App-Typen, die eine kleine Firmware weglassen kann (STM32F103: kein Audio, kein Fingerprint, kein
// Milight-Empfaenger, keine Gleitkomma-Sonnenstandsrechnung in 128 KB Flash). Ohne Schalter sind alle
// dabei (ESP32, Host-Tests). Eine Konfiguration mit einem weggelassenen Typ wird abgelehnt
// ("app type not supported by this firmware"), die Station startet dann ohne Apps.
#ifndef SENSACT_WITHOUT_SOUND
#include "apps/sound.hh"
#endif
#ifndef SENSACT_WITHOUT_FINGERPRINT
#include "apps/fingerprint.hh"
#endif
#ifndef SENSACT_WITHOUT_MILIGHT
#include "apps/milightcontroller.hh"
#endif
#ifndef SENSACT_WITHOUT_BLINDSTIMER
#include <sunsetsunrise.hh>
#include "apps/blindstimer.hh"
#endif

namespace sensact::apps
{
#include "wellKnownColors.inc"
}

namespace sensact::config
{
    StationInfo Station;

    namespace cfg = WsProtocol::sensactcfg;

    // Die Werte des Schemas (sensactcfg.cs) muessen zu den C++-Enums passen
    static_assert((int)cfg::RelayInterlockMode::R1_POWER__R2_UP == (int)eRelayInterlockMode::R1_POWER__R2_UP);
    static_assert((int)cfg::RelayInterlockMode::R1_POWER__R2_DOWN == (int)eRelayInterlockMode::R1_POWER__R2_DOWN);
    static_assert((int)cfg::RelayInterlockMode::R1_UP__R2_POWER == (int)eRelayInterlockMode::R1_UP__R2_POWER);
    static_assert((int)cfg::RelayInterlockMode::R1_DOWN__R2_POWER == (int)eRelayInterlockMode::R1_DOWN__R2_POWER);
    static_assert((int)cfg::RelayInterlockMode::R1_DOWN__R2_UP == (int)eRelayInterlockMode::R1_DOWN__R2_UP);
    static_assert((int)cfg::RelayInterlockMode::R1_UP__R2_DOWN == (int)eRelayInterlockMode::R1_UP__R2_DOWN);

    static eApplicationID Id(uint16_t v) { return (eApplicationID)v; }

    // Liest eine u16-Liste eines Klassenfeldes (generierte Decode<Klasse><Feld>ElementAt-Funktion)
    template <typename DecodeAt>
    static std::vector<eApplicationID> AppIds(const uint8_t *data, size_t count, DecodeAt decodeAt)
    {
        std::vector<eApplicationID> result;
        for (size_t i = 0; i < count; i++)
        {
            uint16_t v{};
            if (decodeAt(data, i, v)) result.push_back(Id(v));
        }
        return result;
    }

#ifndef SENSACT_WITHOUT_BLINDSTIMER
    static sunsetsunrise::eDawn ToDawn(cfg::Dawn d)
    {
        switch (d)
        {
        case cfg::Dawn::NORMAL: return sunsetsunrise::eDawn::NORMAL;
        case cfg::Dawn::NAUTIC: return sunsetsunrise::eDawn::NAUTIC;
        case cfg::Dawn::ASTRONOMIC: return sunsetsunrise::eDawn::ASTRONOMIC;
        default: return sunsetsunrise::eDawn::CIVIL;
        }
    }
#endif

    // Ein App-Typ, den diese Firmware weggelassen hat (SENSACT_WITHOUT_...)
    [[maybe_unused]] static apps::cApplication *NotInThisFirmware(std::string &error)
    {
        error = "app type not supported by this firmware";
        return nullptr;
    }

    // Legt die App zu einem Konfigurationselement an (nullptr bei unbekanntem/ungueltigem Element)
    template <typename T>
    static apps::cApplication *Create(const T &a, std::string &error)
    {
        using namespace apps;
        if constexpr (std::is_same_v<T, cfg::NodeApp::Payload>)
            return new cSensactNode(Id(a.id));
        else if constexpr (std::is_same_v<T, cfg::BlindApp::Payload>)
            return new cBlind(Id(a.id), a.relay1, a.relay2, (eRelayInterlockMode)a.mode, a.upTimeMs, a.downTimeMs);
        else if constexpr (std::is_same_v<T, cfg::OnOffApp::Payload>)
            return new cOnOff(Id(a.id), a.output, a.initiallyOn ? ePowerState::ACTIVE : ePowerState::INACTIVE, a.autoOffMs);
        else if constexpr (std::is_same_v<T, cfg::SinglePwmApp::Payload>)
        {
            std::vector<InOutId> outputs;
            for (size_t i = 0; i < a.outputsCount; i++)
            {
                uint16_t v{};
                if (cfg::DecodeSinglePwmAppOutputsElementAt(a.outputsData, i, v)) outputs.push_back(v);
            }
            return new cSinglePWM(Id(a.id), outputs, a.minimalLevel, a.initialLevel, a.autoOffMs, Id(a.standbyController));
        }
        else if constexpr (std::is_same_v<T, cfg::RgbwApp::Payload>)
            return new cRgbw(Id(a.id), a.outputR, a.outputG, a.outputB, a.outputW, a.autoOffMs, Id(a.standbyController), WELL_KNOWN_COLORS);
        else if constexpr (std::is_same_v<T, cfg::RgbIndicatorApp::Payload>)
        {
            std::vector<uint8_t> leds;
            for (size_t i = 0; i < a.ledsCount; i++)
            {
                uint8_t v{};
                if (cfg::DecodeRgbIndicatorAppLedsElementAt(a.ledsData, i, v)) leds.push_back(v);
            }
            return new cRgbIndicator(Id(a.id), std::move(leds), a.color, a.toggleSignal, a.autoOffMs);
        }
        else if constexpr (std::is_same_v<T, cfg::SoundApp::Payload>)
#ifndef SENSACT_WITHOUT_SOUND
            return new cSound(Id(a.id), a.initialVolume, a.defaultSignalOnToggle);
#else
            return NotInThisFirmware(error);
#endif
        else if constexpr (std::is_same_v<T, cfg::PumpApp::Payload>)
            return new cPump(Id(a.id), a.output, a.onIntervalMs, a.offIntervalMs);
        else if constexpr (std::is_same_v<T, cfg::ToggleButtonApp::Payload>)
            return new cPushbuttonSingle2Toggle(Id(a.id), a.input, AppIds(a.targetsData, a.targetsCount, cfg::DecodeToggleButtonAppTargetsElementAt));
        else if constexpr (std::is_same_v<T, cfg::DimButtonApp::Payload>)
            return new cPushbuttonSingle2PwmSingle(Id(a.id), a.input, Id(a.target));
        else if constexpr (std::is_same_v<T, cfg::DualDimButtonsApp::Payload>)
            return new cPushbuttonDual2PWM(Id(a.id), a.inputUp, a.inputDown, Id(a.target));
        else if constexpr (std::is_same_v<T, cfg::BlindButtonsApp::Payload>)
            return new cPushbuttonDual2Blind(Id(a.id), a.inputUp, a.inputDown, Id(a.target));
        else if constexpr (std::is_same_v<T, cfg::RotaryEncoderApp::Payload>)
        {
            if (a.encoder > 1)
            {
                error = "rotary encoder index > 1";
                return nullptr;
            }
            return new cRotaryEncoder2PWM(Id(a.id), (eRotaryEncoder)a.encoder, Id(a.target));
        }
        else if constexpr (std::is_same_v<T, cfg::LightbarrierApp::Payload>)
            return new cLightbarrier(Id(a.id), a.input, a.activeHigh, Id(a.brightnessSensor), Id(a.target));
        else if constexpr (std::is_same_v<T, cfg::FingerprintApp::Payload>)
#ifndef SENSACT_WITHOUT_FINGERPRINT
            return new cFingerprint(Id(a.id), Id(a.target1), Id(a.target2), Id(a.target3));
#else
            return NotInThisFirmware(error);
#endif
        else if constexpr (std::is_same_v<T, cfg::MilightControllerApp::Payload>)
#ifndef SENSACT_WITHOUT_MILIGHT
            return new cMilightController(Id(a.id), Id(a.light), AppIds(a.blindsData, a.blindsCount, cfg::DecodeMilightControllerAppBlindsElementAt), a.lightOnMs);
#else
            return NotInThisFirmware(error);
#endif
        else if constexpr (std::is_same_v<T, cfg::BlindsTimerApp::Payload>)
#ifndef SENSACT_WITHOUT_BLINDSTIMER
            return new cBlindsTimer(Id(a.id), AppIds(a.blindsData, a.blindsCount, cfg::DecodeBlindsTimerAppBlindsElementAt),
                                    a.latitudeMicroDeg / 1e6, a.longitudeMicroDeg / 1e6, ToDawn(a.dawn), a.openOffsetMin, a.closeOffsetMin, a.maxRandomMin);
#else
            return NotInThisFirmware(error);
#endif
        else
        {
            error = "unsupported app type in configuration";
            return nullptr;
        }
    }

    bool LoadStationConfig(const uint8_t *data, size_t len, std::string &error)
    {
        Station = StationInfo{};
        apps::cApplications::Reset(0);

        cfg::StationConfig::Payload c{};
        if (!cfg::StationConfig::Decode(data, len, c))
        {
            error = "station configuration cannot be decoded";
            return false;
        }
        if (c.formatVersion != FORMAT_VERSION)
        {
            error = "station configuration has format version " + std::to_string(c.formatVersion) + ", expected " + std::to_string(FORMAT_VERSION);
            return false;
        }

        std::vector<BusConfig> buses;
        bool ok = cfg::DecodeStationConfigBusesElements(c.busesData, c.busesDataSize, c.busesCount, [&](const auto &b) {
            using T = std::decay_t<decltype(b)>;
            BusConfig bc{b.index, std::is_same_v<T, cfg::I2cBus::Payload>, {}, {}};
            if constexpr (std::is_same_v<T, cfg::I2cBus::Payload>)
            {
                for (size_t i = 0; i < b.pca9555Count; i++)
                {
                    uint8_t v{};
                    if (cfg::DecodeI2cBusPca9555ElementAt(b.pca9555Data, i, v)) bc.pca9555.push_back(v);
                }
                for (size_t i = 0; i < b.pca9685Count; i++)
                {
                    uint8_t v{};
                    if (cfg::DecodeI2cBusPca9685ElementAt(b.pca9685Data, i, v)) bc.pca9685.push_back(v);
                }
            }
            buses.push_back(std::move(bc));
        });
        if (!ok)
        {
            error = "bus list cannot be decoded";
            return false;
        }

        apps::cApplications::Reset(c.appIdCount);
        ok = cfg::DecodeStationConfigNamesElements(c.namesData, c.namesDataSize, c.namesCount, [&](const auto &n) {
            apps::cApplications::SetName(n.id, n.name);
        });
        if (!ok)
        {
            error = "name list cannot be decoded";
            apps::cApplications::Reset(0);
            return false;
        }

        std::string appError;
        ok = cfg::DecodeStationConfigAppsElements(c.appsData, c.appsDataSize, c.appsCount, [&](const auto &a) {
            if (!appError.empty()) return;
            apps::cApplication *app = Create(a, appError);
            if (app == nullptr)
            {
                if (appError.empty()) appError = "app could not be created";
                appError += " (app " + std::to_string(a.id) + ")";
                return;
            }
            if (!apps::cApplications::Register(app))
            {
                appError = "app number " + std::to_string(a.id) + " outside 0.." + std::to_string(c.appIdCount - 1) + " or used twice";
                delete app;
            }
        });
        if (!ok || !appError.empty())
        {
            error = !appError.empty() ? appError : "app list cannot be decoded";
            apps::cApplications::Reset(0);
            return false;
        }

        apps::cApplications::NodeMasterApplication = Id(c.nodeAppId);
        Station.loaded = true;
        Station.nodeId = (eNodeID)c.nodeId;
        Station.nodeAppId = Id(c.nodeAppId);
        Station.nodeName = c.nodeName;
        Station.houseName = c.houseName;
        Station.created = c.created;
        Station.buses = std::move(buses);
        return true;
    }
}

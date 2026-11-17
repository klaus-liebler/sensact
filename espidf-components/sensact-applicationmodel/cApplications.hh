#pragma once
#include <stdint.h>
#include <vector>

#include "cApplication.hh"
#include "applicationmodel_enums_and_structs.hh"
#define N() (sensact::AppName(this->id))
#define NID(otherId) (sensact::AppName(otherId))

namespace sensact::apps
{
    // Register der Apps dieser Station -- zur Laufzeit gefuellt aus der Stationskonfiguration
    // (s. station_config.hh). Frueher generierte Tabellen (Glo2locCmd, ApplicationNames,
    // NodeMasterApplication) aus dem einkompilierten Hausmodell.
    class cApplications
    {
    public:
        // Nummer der Node-App dieser Station (NO_APPLICATION, solange keine Konfiguration geladen ist)
        static eApplicationID NodeMasterApplication;

        // Groesste App-Nummer des Hauses + 1; App-Nummern darunter sind gueltig (lokal oder auf anderen Stationen)
        static uint16_t AppIdCount() { return (uint16_t)byId.size(); }

        // Lokale App mit dieser Nummer oder nullptr (unbekannt, auf einer anderen Station, ausserhalb)
        static cApplication *Get(uint16_t id) { return id < byId.size() ? byId[id] : nullptr; }
        static cApplication *Get(eApplicationID id) { return Get((uint16_t)id); }

        // Alle lokalen Apps in der Reihenfolge der Konfiguration
        static const std::vector<cApplication *> &Local() { return local; }

        // Erste lokale App eines Typs (fuer Board-Adapter, z. B. Milight-Empfaenger -> cMilightController), sonst nullptr
        static cApplication *FindFirst(eAppType type)
        {
            for (auto *a : local)
                if (a->GetAppType() == type) return a;
            return nullptr;
        }

        // --- fuer die Fabrik (station_config.cc) und Tests
        static void Reset(uint16_t appIdCount);
        // false, wenn die Nummer ausserhalb liegt oder schon belegt ist
        static bool Register(cApplication *app);
        static void SetName(uint16_t id, const char *name);
        static const char *Name(uint16_t id);

    private:
        static std::vector<cApplication *> byId;
        static std::vector<cApplication *> local;
        static std::vector<const char *> names;
    };
}

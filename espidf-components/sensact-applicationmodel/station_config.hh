#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include "applicationmodel_enums_and_structs.hh"

// App- und Bus-Fabrik der datengetriebenen Basis (s. docs/concept-modular-firmware.md, Schritt 2):
// liest die Stationskonfiguration (BestBinaryBuffers-Nachricht sensactcfg::StationConfig, Abschnitt 2
// des Stationspakets, erzeugt vom Builder aus dem XML-Hausmodell), legt alle Apps der Station an und
// traegt sie in cApplications ein. Die Busbeschreibung wertet die Board-Initialisierung aus.
namespace sensact::config
{
    struct BusConfig
    {
        uint8_t index;
        bool isI2c;
        std::vector<uint8_t> pca9555; // 7-Bit-Adressen, Reihenfolge = Chip-Index
        std::vector<uint8_t> pca9685;
    };

    struct StationInfo
    {
        bool loaded{false};
        eNodeID nodeId{eNodeID::UNKNOWN};
        eApplicationID nodeAppId{eApplicationID::NO_APPLICATION};
        std::string nodeName;
        std::string houseName;
        std::string created;
        std::vector<BusConfig> buses;
    };

    // Die geladene Station (loaded == false: keine/ungueltige Konfiguration -> Sicherheitsmodus, keine Apps)
    extern StationInfo Station;

    constexpr uint16_t FORMAT_VERSION = 1;

    // Dekodiert die Konfiguration, legt die Apps an und registriert sie. Bei einem Fehler wird nichts
    // angelegt (cApplications bleibt leer), error beschreibt die Ursache. data muss gueltig bleiben,
    // solange Namen benutzt werden (die Namen zeigen in den Puffer).
    bool LoadStationConfig(const uint8_t *data, size_t len, std::string &error);
}

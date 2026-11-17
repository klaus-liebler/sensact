#pragma once
#include <hal.hh>
#include "busmaster.hh"
#include "interfaces.hh"
#include <webmanager.hh>
#include <webmanager_plugins/usersettings_plugin.hh>
#include <can_message_builder_parser.hh>
#include <driver/temperature_sensor.h>
#include <vector>
#include <station_config.hh>

// Ersetzt die drei per-Board .inc-Dateien (hal.inc/station_config_hardware.inc/
// station_config_hosts.inc), die bisher per Praeprozessor mitten in main.ccs Scope gespleisst
// wurden -- funktionierte nur, weil app_main() nie zurueckkehrt (Busmaster/PCA-Objekte lagen
// stack-lokal an der Splice-Stelle). Jetzt: main/hal/<NODE_ID>/board_init.hh definiert eine
// Klasse sensact::BoardInit : IBoardInit (header-only, ueber denselben hal/${NODE_ID}-
// Include-Mechanismus wie bisher), main.cc konstruiert genau eine Instanz und ruft ihre Methoden
// an den passenden Stellen auf. Ownership von Busmastern/Peripherie-Objekten ist jetzt explizit
// (heap-allokiert), nicht mehr implizit ueber "Funktion kehrt nie zurueck".
namespace sensact {

// Buendelt alles, was main.cc dem board-spezifischen Init-Code bisher implizit ueber gemeinsamen
// Scope zur Verfuegung gestellt hat (TempHandle/CanMBP/NVS-Partitionsname/Hostname/Plugins-Vektor).
struct IInitializationContext {
    temperature_sensor_handle_t TempHandle;
    aCANMessageBuilderParser* CanMBP;
    const char* NvsPartitionName;
    // aus cfg::HOSTNAME (zur Boot-Zeit verfuegbar) -- NICHT wm->GetHostname() (erst nach
    // webmanager::M::Begin() gueltig, also lange nach der Board-Init).
    const char* Hostname;
    // Hook-Punkt fuer board-eigene Zusatz-Plugins (Fingerprint/Journal/etc., bisher SIDEDOORs
    // hal.inc-Sonderweg). Board-Init darf hier push_back()en.
    std::vector<webmanager::iWebmanagerPlugin*>* Plugins;
    // Fuer persistente, pro-App-ID gespeicherte Parameter (s. iSensactContext::
    // LoadPersistedParams/SavePersistedParams) -- CreateHosts() reicht das an den
    // cApplicationHost-Konstruktor durch.
    UsersettingsPlugin* Usersettings;
};

class IBoardInit {
public:
    virtual ~IBoardInit() = default;

    // Wird VOR allem anderen aufgerufen (noch vor TinyUSB-Konfiguration) -- Pin fuer die
    // VBUS-Erkennung des USB-Treibers. Default GPIO_NUM_NC = "nicht verdrahtet"/selbstversorgt,
    // wie es die meisten Boards ohnehin sind.
    virtual gpio_num_t VbusSensePin() { return GPIO_NUM_NC; }

    // Ersetzt hal.inc + den HAL/Busmaster-Teil von station_config_hardware.inc. Gibt den
    // erzeugten (eigentuemerschaftsmaessig an den Aufrufer uebergehenden) iHAL* zurueck; haengt
    // die von ihr erzeugten Busmaster-Instanzen an outBusmasters an.
    virtual sensact::hal::iHAL* CreateHalAndBusmasters(
        IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) = 0;

    // Ersetzt station_config_hosts.inc. Muss einen eigenen cApplicationHost (falls vorhanden)
    // SOWOHL in outHosts ALS AUCH in ctx.Plugins eintragen -- macht "als Webmanager-Plugin
    // registrieren" zur expliziten Pflicht der Implementierung statt eines fehleranfaelligen
    // impliziten Kontrakts (kein aeusserer `hosts`-Local mehr, das man aus Versehen shadowen kann).
    virtual void CreateHosts(
        IInitializationContext& ctx, sensact::iHostContext* hostCtx,
        std::vector<sensact::iHost*>& outHosts) = 0;
};

// Busmaster aus der Stationskonfiguration (sensact::config::Station.buses, s. station_config.hh):
// GPIO-Bus der HAL und ein I2C-Bus mit genau den PCA9555/PCA9685, die das Hausmodell fuer diese
// Station nennt (Reihenfolge = Chip-Index). Die Position im Vektor ist der Bus-Index der InOutId.
// Ohne Konfiguration (Sicherheitsmodus) die Busse des Boards ohne Erweiterungen -- weitere Chips
// findet I2CBusmaster::DiscoverAdditionalDevices ohnehin, nutzbar ueber die Nodemaster-Seite.
inline void CreateBusmastersFromStationConfig(sensact::hal::iHAL* h, uint8_t i2cPort, std::array<gpio_num_t, 3> interruptLines,
                                             std::vector<AbstractBusmaster*>& out, bool boardHasI2cBus = true)
{
    std::vector<config::BusConfig> buses = config::Station.buses;
    if (!config::Station.loaded)
    {
        buses = {config::BusConfig{0, false, {}, {}}};
        if (boardHasI2cBus) buses.push_back(config::BusConfig{1, true, {}, {}});
    }
    for (auto& b : buses)
    {
        if (b.index != out.size())
        {
            ESP_LOGE("BoardInit", "Bus index %u out of order (expected %u) -- bus ignored", b.index, (unsigned)out.size());
            continue;
        }
        if (!b.isI2c)
        {
            out.push_back(new DirectGPIOBusmaster("GPIO", h));
            continue;
        }
        std::vector<PCA9555::M*> pca9555_vec;
        for (uint8_t addr : b.pca9555) pca9555_vec.push_back(new PCA9555::M((PCA9555::Device)addr, 0x0000, 0xFFFF, 0xFFFF));
        std::vector<PCA9685::M*> pca9685_vec;
        for (uint8_t addr : b.pca9685)
            pca9685_vec.push_back(new PCA9685::M((PCA9685::Device)addr, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::TotemPole, PCA9685::OutputNotEn::OutputNotEn_0, PCA9685::Frequency::Frequency_400Hz));
        ESP_LOGI("BoardInit", "I2C bus %u: %u PCA9555, %u PCA9685 from station configuration", b.index, (unsigned)pca9555_vec.size(), (unsigned)pca9685_vec.size());
        out.push_back(new I2CBusmaster("SensactBus", i2cPort, interruptLines, pca9555_vec, pca9685_vec));
    }
}

}

#pragma once
#include <hal.hh>
#include "busmaster.hh"
#include "interfaces.hh"
#include <webmanager.hh>
#include <can_message_builder_parser.hh>
#include <driver/temperature_sensor.h>
#include <vector>

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

}

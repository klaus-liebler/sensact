# Architektur

## Grobe Schichten

```
main.cc (app_main)
  – USB/CDC, LittleFS, NVS, Temperatursensor, CAN-Message-Builder (alt/neu) initialisieren
  – sensact::BoardInit erzeugt HAL, Busmaster und Hosts (s. "Board-Init" unten)
  └─ webmanager::M (eigenes Repo: espidf-component-webmanager)
       – Wifi-Statemachine, HTTPS-Server, OTA, Login, Plugin-Dispatch
  └─ cNodemaster (nodemaster.hh)
       – 20-ms-Tick: CAN-RX -> OfferMessage(hosts) -> Loop(hosts) -> AfterAppLoop -> Health-Check
       – cApplicationHost verteilt die Apps per Bucket-Rotation: jede App spätestens alle 100 ms,
         Apps mit NeedsFastLoop() (z.B. PWM-Fade) in jedem Tick
       – implementiert iHostContext (Services für Hosts) und webmanager::iWebmanagerPlugin
       ├─ AbstractBusmaster[] (busmaster.hh)
       │    ├─ I2CBusmaster       – PCA9555/PCA9685 über I2C, eigener FreeRTOS-Task
       │    ├─ DirectGPIOBusmaster – direkt über iHAL
       │    └─ CANBusmaster        – Stub
       └─ iHost[] (interfaces.hh)
            ├─ cApplicationHost (node_applicationhost.cc/hh)
            │    – implementiert iHost + sensact::apps::iSensactContext + webmanager::iWebmanagerPlugin
            │    – Setup/Loop/Status-Publishing aller sensact-Apps
            │    └─ cApplication[] (sensact/espidf-components/sensact-applicationmodel/apps/)
            │         blind, blindstimer, lightbarrier, milightcontroller, node, onoff, pump,
            │         pushbutton, rgbw, rotaryencoder, singlepwm, sound, fingerprint
            ├─ cGatewayHost (node_gatewayhost.hh) – aktuell No-Op-Stub
            └─ cIoHost (node_iohost.hh)          – aktuell No-Op-Stub
```

## Board-Init

Definiert in `main/hal/board_init_interface.hh` (`IBoardInit`) und je **Board-Typ**
`main/hal/<BoardType>/board_init.hh` (`sensact::BoardInit`), z. B. `main/hal/SensactHsNano3/`.
Welcher Board-Typ eine Station hat, steht im Hausmodell (`<station board="…">`); der Builder
schreibt ihn als `HAL_DIR` in `generated/cmake/config.json`, `main/CMakeLists.txt` nimmt
`hal/${HAL_DIR}` in den Include-Pfad. Ausnahme `MONITORING` (keine Station eines Hauses, eigenes
Verzeichnis). `app_main()` erzeugt genau eine Instanz von `sensact::BoardInit : IBoardInit`.
`IBoardInit` definiert drei Methoden:

- `VbusSensePin()` – GPIO für die USB-VBUS-Erkennung (Default: nicht verdrahtet).
- `CreateHalAndBusmasters(IInitializationContext&, std::vector<AbstractBusmaster*>&)`
  – erzeugt den konkreten `iHAL*` (z.B. `SensactHsNano3::cHAL`) und die Busmaster. Welche
  PCA9555/PCA9685 an der Station hängen, kommt aus der Stationskonfiguration
  (`CreateBusmastersFromStationConfig`, s. "Stationskonfiguration" unten).
- `CreateHosts(IInitializationContext&, iHostContext*, std::vector<iHost*>&)`
  – erzeugt die für diesen Node aktiven `iHost`-Instanzen (typischerweise
  `cApplicationHost`) und trägt sie sowohl in `outHosts` als auch – falls sie
  gleichzeitig Webmanager-Plugin sind – in `ctx.Plugins` ein.

`IInitializationContext` bündelt, was `main.cc` dem Board-Init zur Verfügung
stellt: Temperatursensor-Handle, `aCANMessageBuilderParser*`, NVS-Partitionsname,
Hostname sowie den Plugin-Vektor (für zusätzliche board-eigene Webmanager-Plugins,
z.B. Fingerprint/Journal).

`main/hal/<BoardType>/` existiert für die Board-Typen des Katalogs (`model/catalog.xml`):
`SensactHsNano3`, `SensactUpHs`, `SensactUp3`, `SensactUpControl` (verbindet den
Fingerabdruckleser mit der ersten Fingerprint-App der Konfiguration), `SensactOutdoor`
(Milight-Empfänger → erste Milight-App), dazu `MONITORING`. Eine neue Station mit einem
vorhandenen Board-Typ braucht keinen neuen Code.

## HAL-Board-Varianten (`main/hal/hal_sensact*.hh`)

Gemeinsame Basis `hal_esp32.hh` (`sensact::hal::cESP32`, TWAI/CAN, GPIO, UART,
MCPWM, Temperatursensor). Board-spezifische Ableitungen:

| Datei | Board-Variante | Besonderheiten |
|---|---|---|
| `hal_sensactHs_modular.hh` | HS Modular | internes I2C, I2S-Audio (MAX98357), Ethernet, CAN |
| `hal_sensactHsNano3.hh` | HS Nano3 | TAS580x-Verstärker, Ethernet, ADC-Buttons, Buzzer |
| `hal_sensactOutdoor.hh` | Outdoor | Relais K1–K4, LEDs, NRF-Funk, MAX98357-Audio |
| `hal_sensactUp3.hh` | Up3 | SPI-LCD, 1-Wire, Drehencoder |
| `hal_sensactUpControl.hh` | Up Control | NRF-Funk, Fingerprint-Sensor, Buzzer |
| `hal_sensactUpHs.hh` | Up HS | 2 Drehencoder, Codec/MAX98357-Audio, USB-VBUS-Sense |

Ein `BoardInit::CreateHalAndBusmasters()` instanziiert genau eine dieser HAL-Klassen
und die Busmaster: `DirectGPIOBusmaster` (Bus 0) und – falls das Board einen I2C-Bus hat –
einen `I2CBusmaster` mit den PCA9555/PCA9685 aus der Stationskonfiguration (z. B. für
`SNSCT_L0_TECH_HS_1` vier `PCA9555` + vier `PCA9685`).

## Sensact-Apps

Jede App erbt von `sensact::apps::cApplication`
(`sensact-applicationmodel/cApplication.hh`): `Setup(iSensactContext*)`,
`Loop(iSensactContext*)`, `FillStatus(iSensactContext&, array<uint16_t,4>&)`, plus
generierte Command-Handler. Alle App-Methoden hängen nur am Interface
`iSensactContext`, nicht an ESP-IDF-Typen direkt.

### Stationskonfiguration (datengetriebene Basis)

Die Firmware ist **stationsneutral**: welche Apps eine Station hat und mit welchen Parametern,
steht nicht im Code, sondern in der Stationskonfiguration (Abschnitt 2 des Stationspakets
`/spiffs/station.pkg`, BestBinaryBuffers-Nachricht `sensactcfg::StationConfig`, Schema
`best_binary_buffers_schema/sensactcfg.cs`). Beim Start liest `main.cc` sie
(`station_package::ReadSection`, prüft Station, Basis-Kennung und Prüfsumme), die App-Fabrik
`sensact::config::LoadStationConfig` (`sensact-applicationmodel/station_config.cc`) legt alle
Apps an und trägt sie in `cApplications` ein: `Get(id)` (lokale App oder `nullptr`, wenn die App
auf einer anderen Station läuft), `AppIdCount()`, `AppName(id)`, `FindFirst(eAppType)`.
`eApplicationID` kennt in der Firmware nur `MASTER`/`NO_APPLICATION`, alle anderen Nummern sind
Werte ohne Namen; die Namen kommen aus der Konfiguration. Ohne gültige Konfiguration läuft die
Station im **Sicherheitsmodus**: keine Apps, eingebaute Wartungsseite (`main/maintenance/`),
Paket- und Firmware-Upload funktionieren.

- Kommandos und Statusbelegung jeder App:
  [sensact-applicationmodel/README.md](../../../espidf-components/sensact-applicationmodel/README.md)
  (verbindliche Referenz, auch für die Web-UI).
- Host-Unit-Tests (GoogleTest, ohne ESP32):
  [sensact-applicationmodel/host_test/README.md](../../../espidf-components/sensact-applicationmodel/host_test/README.md).
  Keine App bindet ESP-IDF ein; Hardware-nahe Anbindungen (z. B. der Milight-Funkempfänger)
  liegen als Adapter im Board-Init (`main/hal/<BoardType>/board_init.hh`).

## Cross-Cutting

- `messagecodes.hh` – X-Macro-generiertes `enum class messagecodes::C` +
  String-Tabelle, aus `messagecodes_default.inc` + optionalem `messagecodes_user.inc`.
- `logger.hh` – `LOGGER::Journal(code, data)`, leitet an
  `webmanager::JournalPlugin` weiter.
- `journal_plugin.cc/hh` – Ringpuffer (16 Einträge) in `__NOINIT_ATTR`-RAM (überlebt
  Neustarts außer Kaltstart), exponiert über Websocket (`ns10journal`).

## Bekannt tote/veraltete Teile

`main/__archive/` – nicht im Build referenziert (`.ccold`/`.hhold`-Endungen,
veraltete APIs).

## Wifimanager

Server-seitig die Klasse `webmanager::M` (`espidf-component-webmanager/cpp/webmanager.hh`,
eigenes Repo): WLAN-Statemachine, HTTP(S)-Server, OTA, Login/Session, Plugin-Dispatch.
Boards mit Ethernet können per `WIFI_ENABLED=false` (Board-Settings) stattdessen
`webmanager_eth_only::M` verwenden (s. `main.cc`).

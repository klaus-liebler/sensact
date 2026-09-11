# Architektur

## Grobe Schichten

```
main.cc (app_main)
  – USB/CDC, LittleFS, NVS, Temperatursensor, CAN-Message-Builder (alt/neu) initialisieren
  – sensact::BoardInit erzeugt HAL, Busmaster und Hosts (s. "Board-Init" unten)
  └─ webmanager::M (eigenes Repo: espidf-component-webmanager)
       – Wifi-Statemachine, HTTPS-Server, OTA, Login, Plugin-Dispatch
  └─ cNodemaster (nodemaster.hh)
       – 100ms-Zyklus: CAN-RX -> OfferMessage(hosts) -> Loop(hosts) -> AfterAppLoop -> Health-Check
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

Definiert in `main/hal/board_init_interface.hh` (`IBoardInit`) und je
`main/hal/<NODE_ID>/board_init.hh` (`sensact::BoardInit`). `app_main()` erzeugt
genau eine Instanz von `sensact::BoardInit : IBoardInit`
(header-only, board-spezifisch über denselben `hal/${NODE_ID}`-Include-Mechanismus
wie die restliche HAL-Auswahl). `IBoardInit` definiert drei Methoden:

- `VbusSensePin()` – GPIO für die USB-VBUS-Erkennung (Default: nicht verdrahtet).
- `CreateHalAndBusmasters(IInitializationContext&, std::vector<AbstractBusmaster*>&)`
  – erzeugt den konkreten `iHAL*` (z.B. `SensactHsNano3::cHAL`) und die für dieses
  Board passenden `AbstractBusmaster`-Instanzen (I2C-Peripherie wie PCA9555/PCA9685,
  GPIO-Busmaster, …).
- `CreateHosts(IInitializationContext&, iHostContext*, std::vector<iHost*>&)`
  – erzeugt die für diesen Node aktiven `iHost`-Instanzen (typischerweise
  `cApplicationHost`) und trägt sie sowohl in `outHosts` als auch – falls sie
  gleichzeitig Webmanager-Plugin sind – in `ctx.Plugins` ein.

`IInitializationContext` bündelt, was `main.cc` dem Board-Init zur Verfügung
stellt: Temperatursensor-Handle, `aCANMessageBuilderParser*`, NVS-Partitionsname,
Hostname sowie den Plugin-Vektor (für zusätzliche board-eigene Webmanager-Plugins,
z.B. Fingerprint/Journal).

`main/hal/<NODE_ID>/` existiert für: `MONITORING`, `SNSCT_L0_TECH_HS_1`,
`SNSCT_L3_WORK_HS`, `SNSCT_NODE_HS`, `SNSCT_NODE_SIDEDOOR`, `SNSCT_NODE_TERRASSE`,
`SNSCT_NODE_UP`. Jedes dieser Verzeichnisse enthält ausschließlich `board_init.hh`
(keine weiteren Dateien) und wird von Hand angelegt/gepflegt – es gibt keinen
Generator dafür (s. [board-provisioning.md](board-provisioning.md)). Welcher
physische Board-Typ (`hal_sensact*.hh`) hinter einer `NODE_ID` steht, ist zusätzlich
in `builder/NodeIdBoardTypeCatalog.cs` hinterlegt (muss manuell synchron gehalten
werden).

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
und verdrahtet dazu passend die Busmaster/Peripherie (z.B. für `SensactHsNano3`:
vier `PCA9555` + vier `PCA9685` über einen `I2CBusmaster` plus ein
`DirectGPIOBusmaster`).

## Sensact-Apps

Jede App erbt von `sensact::apps::cApplication`
(`sensact-applicationmodel/cApplication.hh`): `Setup(iSensactContext*)`,
`Loop(iSensactContext*)`, `FillStatus(iSensactContext&, array<uint16_t,4>&)`, plus
generierte Command-Handler. Die Registry `cApplications::Glo2locCmd[]`/`Glo2locEvt[]`
bildet `eApplicationID` auf konkrete Instanzen ab (oder `nullptr`, falls die App auf
einem anderen Node gehostet wird). Alle App-Methoden hängen nur am Interface
`iSensactContext`, nicht an ESP-IDF-Typen direkt.

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

Server-seitig eine einzelne ~2000-Zeilen-Klasse `webmanager::M`
(`espidf-component-webmanager/cpp/webmanager.hh`), die WLAN-Statemachine,
HTTP-Fileserver, OTA, Login/Session und Wifimanager-Protokoll in einer Klasse
vereint.

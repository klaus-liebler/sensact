# Architektur

Lebendes Dokument – wird im Zuge von
[plan_v2/04-architektur-review.md](plan_v2/04-architektur-review.md) laufend
erweitert/korrigiert. Der aktuelle Stand ist eine erste, aus einer Code-Analyse
gewonnene Bestandsaufnahme (2026-07-31), noch nicht vollständig verifiziert.

## Grobe Schichten

```
main.cc (app_main)
  └─ webmanager::M (eigenes Repo: espidf-component-webmanager)
       – Wifi-Statemachine, HTTP/OTA/Login, Plugin-Dispatch
  └─ cNodemaster (nodemaster.hh)
       – 100ms-Zyklus: CAN-RX -> OfferMessage(hosts) -> Loop(hosts) -> CAN/Health
       – implementiert iHostContext (Services für Hosts) und iWebmanagerPlugin
       ├─ AbstractBusmaster[] (busmaster.hh)
       │    ├─ I2CBusmaster       – PCA9555/PCA9685 über I2C, eigener Task
       │    ├─ DirectGPIOBusmaster – direkt über iHAL
       │    └─ CANBusmaster        – Stub
       └─ iHost[] (interfaces.hh)
            ├─ cApplicationHost (node_applicationhost.cc/hh)
            │    – implementiert iHost + sensact::apps::iSensactContext + iWebmanagerPlugin
            │    – Setup/Loop/Status-Publishing aller sensact-Apps
            │    └─ cApplication[] (sensact/espidf-components/sensact-applicationmodel/apps/)
            │         blind, blindstimer, onoff, pump, singlepwm, rgbw, sound,
            │         milightcontroller, pushbutton, rotaryencoder, node, fingerprint
            ├─ cGatewayHost (node_gatewayhost.hh) – aktuell No-Op-Stub
            └─ cIoHost (node_iohost.hh)          – aktuell No-Op-Stub
```

## HAL-Board-Varianten (`main/hal/`)

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

Grad der Code-Duplizierung zwischen diesen 6 Dateien noch nicht im Detail geprüft –
siehe Backlog-Punkt in
[plan_v2/04-architektur-review.md](plan_v2/04-architektur-review.md).

Pro Node (`main/hal/<NODE_ID>/`) liegen zusätzlich drei **von Hand gepflegte**
`.inc`-Dateien (`hal.inc`, `station_config_hardware.inc`,
`station_config_hosts.inc`), die auswählen, welche HAL/Busmaster/Hosts für dieses
konkrete Board aktiv sind – siehe [board-provisioning.md](board-provisioning.md).

## Sensact-Apps

Jede App erbt von `sensact::apps::cApplication`
(`sensact-applicationmodel/cApplication.hh`): `Setup(iSensactContext*)`,
`Loop(iSensactContext*)`, `FillStatus(iSensactContext&, array<uint16_t,4>&)`, plus
generierte Command-Handler. Die Registry `cApplications::Glo2locCmd[]`/`Glo2locEvt[]`
bildet `eApplicationID` auf konkrete Instanzen ab (oder `nullptr`, falls die App auf
einem anderen Node gehostet wird). Alle App-Methoden hängen nur am Interface
`iSensactContext`, nicht an ESP-IDF-Typen direkt – das ist die Grundlage für die
Host-Testbarkeit in [plan_v2/05-unit-tests-sensact-apps.md](plan_v2/05-unit-tests-sensact-apps.md).

## Cross-Cutting

- `messagecodes.hh` – X-Macro-generiertes `enum class messagecodes::C` +
  String-Tabelle, aus `messagecodes_default.inc` + optionalem `messagecodes_user.inc`.
- `logger.hh` – `LOGGER::Journal(code, data)`, leitet an
  `webmanager::JournalPlugin` weiter.
- `journal_plugin.cc/hh` – Ringpuffer (16 Einträge) in `__NOINIT_ATTR`-RAM (überlebt
  Neustarts außer Kaltstart), exponiert über Websocket (`ns10journal`).

## Bekannt tote/veraltete Teile

`main/__archive/` – nicht im Build referenziert (`.ccold`/`.hhold`-Endungen,
veraltete APIs). Aufräum-Kandidat, siehe
[plan_v2/04-architektur-review.md](plan_v2/04-architektur-review.md).

## Wifimanager

Server-seitig eine einzelne ~1430-Zeilen-Klasse `webmanager::M`
(`espidf-component-webmanager/cpp/webmanager.hh`), die WLAN-Statemachine,
HTTP-Fileserver, OTA, Login/Session und Wifimanager-Protokoll in einer Klasse
vereint. Details, bekannte Bugs und das geplante konfigurierbare AP-Fallback siehe
[plan_v2/03-wifimanager-review.md](plan_v2/03-wifimanager-review.md).

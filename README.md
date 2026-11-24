# sensact

Verteilte Hausautomatisierung – Hardware, Firmware, Werkzeuge und Weboberfläche.

Stationen an einem gemeinsamen CAN-Bus lesen Taster und Drehgeber und schalten Licht, Rollläden, Pumpen
und Signale. Die kleinste Einheit ist die **App** (Taster, Lampe, Rollladen …) mit einer hausweit
eindeutigen Nummer; Apps schicken einander Kommandos, egal auf welcher Station sie laufen. Was wo
verdrahtet ist, beschreibt ein **XML-Hausmodell**, aus dem Konfiguration und Weboberfläche jeder Station
erzeugt werden.

**Dokumentation: [docs/README.md](docs/README.md)** – Fachkonzepte, Bedienung im Haus,
Weboberfläche, Betrieb und Änderungen.

## Inhalt des Repos

| Ordner | Inhalt |
|---|---|
| [`docs/`](docs/README.md) | Fachkonzepte und Nutzerdokumentation |
| [`firmware/sensact_firmware/`](firmware/sensact_firmware/README.md) | ESP32-S3-Basis-Firmware, Builder, Editor, Hausmodell (`model/`), Weboberfläche – mit Entwicklerdokumentation |
| [`firmware/sensact_firmware_f103/`](firmware/sensact_firmware_f103/README.md) | Firmware der sensactUp-Stationen (STM32F103) auf rtoos |
| [`espidf-components/sensact-applicationmodel/`](espidf-components/sensact-applicationmodel/README.md) | die Apps (gemeinsam für ESP32 und STM32F103) mit Host-Tests |
| `espidf-components/sensact-middleware/`, `sensact-hal/`, `sensactcommon/` | CAN-Codec, HAL-Basis, gemeinsame Typen |
| `firmware/sensact_firmware_stm32/`, `firmware/firmwareCubeIDE/` | Altfirmware der STM32F407-Station |
| `pcb/` | Platinen (KiCad) |
| `doc/` | Datenblätter |
| `runware/` | PC-Werkzeuge (UDP-Monitor) |

## Hardware

- **SensactHS** – Hutschienenmontage im Verteiler (Technikstationen mit I2C-Erweiterungen für Ein- und Ausgänge)
- **SensactUP** – Unterputzmontage im Raum (Taster, Drehgeber, Signal-LEDs)
- Spezialstationen für Fingerabdruckleser (Seitentür) und Außenbereich (Terrasse)

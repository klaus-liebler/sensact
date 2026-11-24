# sensact – Dokumentation

sensact ist eine verteilte Hausautomatisierung: Stationen an einem gemeinsamen CAN-Bus lesen
Taster und Drehgeber, schalten Lampen, Rollläden, Pumpen und Signale und sprechen dabei über
kleine, einzeln adressierbare Einheiten, die **Apps**. Was wo verdrahtet ist und wie es
zusammenarbeitet, steht in einem einzigen **Hausmodell** (XML); aus ihm entstehen die
Konfigurationen und Weboberflächen der Stationen.

## Für wen ist was?

| Ich möchte … | Lesen |
|---|---|
| im Haus Licht, Rollläden und Klingel bedienen | [Nutzer: Bedienung im Haus](nutzer/bedienung-im-haus.md) |
| eine Station über den Browser bedienen oder prüfen | [Nutzer: Weboberfläche](nutzer/weboberflaeche.md) |
| etwas an der Anlage ändern (Taster umbelegen, Lampe ergänzen, Firmware aktualisieren) | [Nutzer: Betrieb und Änderungen](nutzer/betrieb-und-aenderungen.md) |
| verstehen, wie sensact aufgebaut ist und warum | [Fachkonzepte](#fachkonzepte) |
| am Code arbeiten, bauen, flashen | [Entwicklerdokumentation](../firmware/sensact_firmware/README.md) |

## Fachkonzepte

1. [Systemüberblick](konzepte/01-systemueberblick.md) – Stationen, Bus, Apps, Werkzeuge auf einer Seite
2. [Hausmodell und Katalog](konzepte/02-hausmodell.md) – die Quelle der Wahrheit: Stationen, Hardware, Apps, Nummern
3. [Apps, Kommandos und Status](konzepte/03-apps.md) – das Verhaltensmodell: wer schickt wem was
4. [Kommunikation](konzepte/04-kommunikation.md) – CAN-Bus, Nachrichtenformat, Weboberfläche
5. [Stationen und Firmware](konzepte/05-stationen-und-firmware.md) – ESP32-Basis, STM32F103 auf rtoos, Altknoten
6. [Konfiguration und Auslieferung](konzepte/06-konfiguration-und-auslieferung.md) – Builder, Stationspaket, Updates
7. [Weboberfläche](konzepte/07-weboberflaeche.md) – Anwendungs-Shell, Arbeitsbereiche, digitaler Zwilling
8. [Qualitätssicherung](konzepte/08-qualitaetssicherung.md) – Host-Tests, Modellprüfung, Emulatoren

## Weitere Quellen

- [Referenz aller Apps: Kommandos und Statusbelegung](../espidf-components/sensact-applicationmodel/README.md) – verbindlich
- [Hausmodell bearbeiten](../firmware/sensact_firmware/model/README.md), [Editor](../firmware/sensact_firmware/editor/README.md)
- [F103-Firmware (sensactUp)](../firmware/sensact_firmware_f103/README.md)
- [Offene Punkte und Entscheidungen](../firmware/sensact_firmware/docs/known-issues.md) – lebende Liste
- [Entwurfsentscheidungen zur datengetriebenen Firmware](../firmware/sensact_firmware/docs/concept-modular-firmware.md) – Entscheidungsprotokoll mit Varianten und Begründungen

## Begriffe

| Begriff | Bedeutung |
|---|---|
| **Station** (auch Knoten, Node) | ein Gerät am CAN-Bus, z. B. `SNSCT_L0_TECH_HS_1`; hat eine feste Stationsnummer |
| **Board-Typ** | die Platine einer Station (`SensactHsNano3`, `SensactUp02` …), beschrieben im Katalog |
| **Erweiterung** | ein Chip an einem Bus der Station, z. B. PCA9555 (16 Eingänge) oder PCA9685 (16 PWM-Ausgänge) |
| **App** | kleinste adressierbare Einheit: ein Taster, eine Lampe, ein Rollladen; feste Nummer im ganzen Haus |
| **Kommando** | Auftrag an eine App, z. B. `TOGGLE`, `SET_VERTICAL_TARGET(level)` |
| **Hausmodell** | `model/Sattlerstrasse16.xml`: alle Stationen, Erweiterungen und Apps des Hauses |
| **Katalog** | `model/catalog.xml`: Board-Typen (Busse, Pins, Features) und Erweiterungs-Chips |
| **Basis-Firmware** | die stationsneutrale Firmware der ESP32-Stationen |
| **Stationspaket** | eine Datei mit Konfiguration und Weboberfläche einer ESP32-Station, per Browser hochgeladen |
| **Builder** | Kommandozeilenwerkzeug (C#), das generiert, baut, verpackt und flasht |
| **Editor** | Browser-Werkzeug (Blazor) zum Bearbeiten des Hausmodells |
| **Sicherheitsmodus** | Zustand einer ESP32-Station ohne gültiges Paket: keine Apps, nur Wartungsseite |

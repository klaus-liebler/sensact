# sensact_firmware – Entwicklerdokumentation

Einstieg für alle, die am Code arbeiten: ESP32-Basis-Firmware, Builder, Editor, Hausmodell.
Was sensact fachlich ist und wie es aufgebaut ist, steht in der
**[Gesamtdokumentation](../../docs/README.md)** (Fachkonzepte und Nutzerdokumentation) – dort anfangen,
wer das System noch nicht kennt.

## Kurzfassung

`sensact_firmware` ist die ESP-IDF-Firmware der ESP32-S3-Stationen und das Werkzeug drumherum:

- `main/` – Basis-Firmware (stationsneutral: Apps und Erweiterungen kommen aus dem Stationspaket)
- `builder/` – C#-Builder: Generierung, Web-Build, Stationspaket, Firmware, Flashen
- `editor/` – Blazor-Editor für das Hausmodell
- `modellib/` – Import, Prüfung und Codegenerierung des Hausmodells (von Builder und Editor genutzt)
- `model/` – XML-Hausmodell, Katalog, XSDs
- `best_binary_buffers_schema/` – Schemas für ws-Protokoll und Stationskonfiguration
- `web/` – Weboberfläche (Vite), `testserver/` – Station-Simulation für die Oberfläche

Die Apps selbst liegen in `../../espidf-components/sensact-applicationmodel`, die Firmware der
STM32F103-Stationen in [`../sensact_firmware_f103`](../sensact_firmware_f103/README.md).

Erstes Flashen per USB: `dotnet run --project builder -- Pipeline --model <Name>`; danach nach
Modelländerungen nur `BuildPackage --model <Name>` und das Paket über die Weboberfläche hochladen. Vorher
die einmaligen Vorbereitungen in [docs/build-process.md](docs/build-process.md) erledigen.

## Dokumente

- [docs/repo-map.md](docs/repo-map.md) – aus welchen Repos das Gesamtsystem besteht und wie sie zusammenhängen
- [docs/build-process.md](docs/build-process.md) – Voraussetzungen, Builder-Phasen, generierte Dateien, Troubleshooting
- [docs/board-provisioning.md](docs/board-provisioning.md) – neues Board anlegen und in Betrieb nehmen
- [docs/architecture.md](docs/architecture.md) – Node-/HAL-/App-Architektur der ESP32-Firmware im Code
- [Sensact-Apps: Kommandos & Status](../../espidf-components/sensact-applicationmodel/README.md)
  – verbindliche Referenz aller Apps (auch für die Web-UI)
- [Host-Unit-Tests der Apps](../../espidf-components/sensact-applicationmodel/host_test/README.md)
  – GoogleTest auf dem PC, kein Board nötig
- [model/README.md](model/README.md) – XML-Hausmodell: bearbeiten, Regeln, neuer App-Typ
- [editor/README.md](editor/README.md) – Editor für das Hausmodell
- [docs/concept-modular-firmware.md](docs/concept-modular-firmware.md) – Entscheidungsprotokoll zur
  datengetriebenen Basis (Varianten, Begründungen, Etappen, F103-Konfiguration über CAN)
- [docs/known-issues.md](docs/known-issues.md) – offene Punkte, Entscheidungsfragen und Erledigtes

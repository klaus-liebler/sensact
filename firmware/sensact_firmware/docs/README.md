# sensact_firmware – Entwicklerdokumentation

Einstiegspunkt für alles, was zum Verständnis und zur Weiterentwicklung dieses
Projekts gebraucht wird.

- [repo-map.md](repo-map.md) – Aus welchen Repos das Gesamtsystem besteht und wie sie
  zusammenhängen
- [build-process.md](build-process.md) – Wie ein Build/Flash-Durchlauf abläuft
- [board-provisioning.md](board-provisioning.md) – Wie ein neues Board angelegt und
  in Betrieb genommen wird
- [architecture.md](architecture.md) – Node-/HAL-/App-Architektur der Firmware
- [plan_v2/00-overview.md](plan_v2/00-overview.md) – Archiviertes Planungsdokument
  einer früheren Überarbeitungsphase, falls historischer Kontext gebraucht wird

## Kurzfassung für Eilige

`sensact_firmware` ist die ESP-IDF-Firmware für die "sensact"-Hausautomatisierungs-
Boards (Rollladen, Relais, Dimmer, Fingerprint-Türsteuerung u.a.). Sie ist Teil des
größeren `sensact`-Repos, bindet aber mehrere eigenständige Repos ein (Wifimanager,
Web-UI-Bausteine, Build-Tools) – siehe [repo-map.md](repo-map.md) für Details, bevor
du an mehreren Stellen gleichzeitig etwas änderst.

Ein Build läuft über den C#-Builder in `builder/` (`dotnet run --project builder --
Pipeline --model <Name>`) – Details in [build-process.md](build-process.md).

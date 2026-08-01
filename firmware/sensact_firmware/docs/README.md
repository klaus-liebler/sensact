# sensact_firmware – Entwicklerdokumentation

Einstiegspunkt für alles, was zum Verständnis und zur Weiterentwicklung dieses
Projekts gebraucht wird.

- [repo-map.md](repo-map.md) – Aus welchen Repos das Gesamtsystem besteht und wie sie
  zusammenhängen
- [build-process.md](build-process.md) – Wie ein Build/Flash Schritt für Schritt
  abläuft
- [board-provisioning.md](board-provisioning.md) – Wie ein neues Board angelegt und
  in Betrieb genommen wird
- [architecture.md](architecture.md) – Node-/HAL-/App-Architektur der Firmware
  (lebendes Dokument)
- [plan_v2/00-overview.md](plan_v2/00-overview.md) – Der mehrstufige Überarbeitungsplan
  (Stand 2026), falls du wissen willst, warum etwas gerade so ist, wie es ist, oder
  was als Nächstes geplant ist

## Kurzfassung für Eilige

`sensact_firmware` ist die ESP-IDF-Firmware für die "sensact"-Hausautomatisierungs-
Boards (Rollladen, Relais, Dimmer, Fingerprint-Türsteuerung u.a.). Sie ist Teil des
größeren `sensact`-Repos, bindet aber mehrere eigenständige Repos ein (Wifimanager,
Web-UI-Bausteine, Build-Tools) – siehe [repo-map.md](repo-map.md) für Details, bevor
du an mehreren Stellen gleichzeitig etwas änderst.

Ein Build läuft heute über `builder/gulpfile.ts` (Node/TypeScript, wird gerade
schrittweise auf ein C#-Tool umgestellt, siehe
[plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md)) –
Details in [build-process.md](build-process.md).

# sensact_firmware – Entwicklerdokumentation

Einstiegspunkt für alles, was zum Verständnis und zur Weiterentwicklung dieses
Projekts gebraucht wird.

- [docs/repo-map.md](docs/repo-map.md) – Aus welchen Repos das Gesamtsystem besteht und
  wie sie zusammenhängen
- [docs/build-process.md](docs/build-process.md) – Wie ein Build/Flash-Durchlauf abläuft
- [docs/board-provisioning.md](docs/board-provisioning.md) – Wie ein neues Board angelegt
  und in Betrieb genommen wird
- [docs/architecture.md](docs/architecture.md) – Node-/HAL-/App-Architektur der Firmware
- [Sensact-Apps: Kommandos & Status](../../espidf-components/sensact-applicationmodel/README.md)
  – verbindliche Referenz aller Apps (auch für die Web-UI)
- [Host-Unit-Tests der Apps](../../espidf-components/sensact-applicationmodel/host_test/README.md)
  – GoogleTest auf dem PC, kein Board nötig
- [model/README.md](model/README.md) – XML-Hausmodell: bearbeiten, Regeln, auf die Station bringen
- [editor/README.md](editor/README.md) – Editor für das Hausmodell
- [docs/concept-modular-firmware.md](docs/concept-modular-firmware.md) – Konzept und Stand der
  datengetriebenen Basis-Firmware (Stationspaket, Konfiguration, Editor)
- [docs/known-issues.md](docs/known-issues.md) – offene Punkte, Entscheidungsfragen und
  Erledigtes
- [docs/plan_v2/00-overview.md](docs/plan_v2/00-overview.md) – Archiviertes
  Planungsdokument einer früheren Überarbeitungsphase, falls historischer Kontext
  gebraucht wird

## Kurzfassung für Eilige

`sensact_firmware` ist die ESP-IDF-Firmware für die "sensact"-Hausautomatisierungs-
Boards (Rollladen, Relais, Dimmer, Fingerprint-Türsteuerung u.a.). Sie ist Teil des
größeren `sensact`-Repos, bindet aber mehrere eigenständige Repos ein (Wifimanager,
Web-UI-Bausteine, Build-Tools) – siehe [docs/repo-map.md](docs/repo-map.md) für Details,
bevor du an mehreren Stellen gleichzeitig etwas änderst.

Die Firmware ist stationsneutral: welche Apps eine Station hat, kommt aus dem
Stationspaket, das der Builder aus dem XML-Hausmodell (`model/`) erzeugt. Erstes Flashen per
USB: `dotnet run --project builder -- Pipeline --model <Name>`; danach nach
Modelländerungen nur `BuildPackage --model <Name>` und das Paket über die Weboberfläche
hochladen. Details in [docs/build-process.md](docs/build-process.md),
insbesondere die einmaligen manuellen Vorbereitungen dort, bevor der erste Build auf
einer neuen Maschine läuft.

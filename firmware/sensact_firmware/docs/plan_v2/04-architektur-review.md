# Stufe 4 – Schrittweise Architektur-Review (laufender Backlog)

Diese Stufe ist bewusst **kein einmaliger Block**, sondern ein fortlaufender Prozess:
jede Session (egal ob an Stufe 1, 2, 3 oder 5 gearbeitet wird) trägt neue Funde hier
ein, statt sie "mal eben nebenbei" zu fixen. Größere Fixes werden als eigener
Punkt hier abgehakt, sobald sie erledigt sind.

**Leitprinzip bei jedem Refactoring (Entscheidung von Klaus):** Unit-Tests
(Stufe 5) kommen erst ganz zum Schluss, aber jede hier durchgeführte
Umstrukturierung muss so gestaltet sein, dass der Code hinterher gut testbar ist
(z.B. Abhängigkeiten weiter über Interfaces wie `iSensactContext` statt über
konkrete ESP-IDF-Typen führen). Testbarkeit ist also schon jetzt ein
Bewertungskriterium für Architektur-Entscheidungen, auch ohne dass schon Tests
geschrieben werden.

## Empfohlene Reihenfolge für die eigentliche Review-Arbeit

Nicht alles auf einmal – pro Sitzung ein Modul, mit kurzem Ergebnis-Vermerk hier:

1. Node-Architektur: `nodemaster.hh`, `busmaster.hh`, `interfaces.hh`,
   `node_applicationhost.cc/.hh`, `node_gatewayhost.hh`, `node_iohost.hh`
2. HAL-Board-Varianten: `main/hal/hal_sensact*.hh` (6 Dateien) – prüfen, wie viel
   davon tatsächlich Board-spezifisch sein *muss* vs. Copy-Paste ist
3. Sensact-Apps: `sensact/espidf-components/sensact-applicationmodel/apps/*`
   (überschneidet sich mit Stufe 5 – dort werden ohnehin alle Apps einzeln
   angefasst, Architektur-Findings fallen dabei nebenbei ab)
4. Wifimanager (`espidf-component-webmanager`) – größtenteils bereits in Stufe 3
   behandelt, hier nur das, was über den Wifi-Teil hinausgeht (HTTP/OTA/Login/
   File-Handling)
5. Build-/Codegen-Tooling – nach Abschluss von Stufe 2, da sich davor ohnehin alles
   ändert
6. Cross-Cutting: `messagecodes.hh`, `logger.hh`, `journal_plugin.cc/.hh`,
   `sensact_projectconfig.hh`

## Bereits bekannte Funde (Startbestand des Backlogs)

Status-Legende: 🔴 Offen · 🟡 In Arbeit · 🟢 Erledigt

### 🟡 `configware` war ein unentdeckter, manueller Pipeline-Schritt
- `C:\repos\sensact\configware\` (C#-Hausmodell-Generator, erzeugt die meisten
  `.inc`-Dateien für `sensact-applicationmodel`) lief bisher komplett manuell und
  unabhängig vom Gulp-Build – nirgends dokumentiert, keine Automatisierung,
  Fehlerquelle bei vergessener Neugenerierung nach Modelländerungen.
- Entschieden (2026-07-31): wird vollständig in den neuen C#-Build-Orchestrator
  verschmolzen, siehe
  [02-builder-migration-csharp.md](02-builder-migration-csharp.md). Status 🟡, da
  die Entscheidung getroffen, die Umsetzung aber Teil von Stufe 2 ist.

### 🔴 Tote Dateien in `main/__archive/`
- Dateien: `busmaster.ccold`, `nodemaster.ccold`, `connect.hhold`, `updater.hh`,
  `lcd_manager.hh`.
- Keine davon ist in `main/CMakeLists.txt` referenziert (nur `journal_plugin.cc`,
  `node_applicationhost.cc`, `model_node.cc`, `main.cc` werden kompiliert), die
  Endungen `.ccold`/`.hhold` sind ohnehin keine gültigen C++-Endungen. `updater.hh`
  und `connect.hhold` nutzen zudem veraltete APIs (`esp_spi_flash.h`).
- Vorschlag: löschen (Git-Historie bleibt ja erhalten, falls doch mal gebraucht).
- Aufwand: trivial. Risiko: keins (nicht im Build).

### 🟢 Widersprüchliche Zertifikatspfade (erledigt 2026-07-31)
- War: `gulpfile.ts:42` nutzt `OneDrive - HSOS\certificates` als Quelle der Root-CA,
  `main/CMakeLists.txt` bettete aber `%USERPROFILE%\netcase\certificates\rootCA.pem.crt`
  ein (`netcase` = Altlast einer früheren Migration).
- Klärung durch Klaus: ausschließlich `OneDrive - HSOS\certificates` ist korrekt.
- Fix: `main/CMakeLists.txt:7` (`EMBED_TXTFILES`) korrigiert auf
  `${USERPROFILE}/OneDrive - HSOS/certificates/rootCA.pem.crt`.

### ⚪ `board_info.json` vs. `board_info copy.json` im Repo-Root – kein Problem
- Klärung durch Klaus: `board_info copy.json` ist absichtlich vorhanden, wird für
  interne kleine Tests gebraucht. Kein Aufräum-Kandidat, ignorieren.

### 🔴 Absoluter Pfad-Wildwuchs (Portabilität)
- Hart codierte Pfade in `gulpfile.ts` (`C:\\repos\\sensact\\firmware\\sensact_firmware`,
  `C:\\repos\\espidf-component-webmanager`, `C:\\repos\\generated`), in
  `CMakeLists.txt` (`C:/repos/generated/`, absolute `EXTRA_COMPONENT_DIRS`), und
  `%USERPROFILE%\OneDrive - HSOS\...` für Board-Secrets.
- Nicht zwingend "falsch" für ein Ein-Entwickler-Setup, aber macht das Projekt
  nicht auf einer anderen Maschine/für einen zweiten Mitwirkenden lauffähig, ohne
  Quellcode zu ändern.
- Vorschlag (niedrige Priorität, kein Blocker): ein einziges, nicht eingechecktes
  `local.settings.json`/`.env` als Quelle dieser Pfade, das von Gulp (heute) bzw.
  dem C#-Builder (nach Stufe 2) gelesen wird. Guter Kandidat, um direkt im Zuge von
  Stufe 2 mitzuerledigen (der neue Builder liest ohnehin `appsettings.json`).
- **In Diskussion (2026-07-31):** `C:\repos\generated\` projekt-lokal machen (z.B.
  `sensact_firmware\generated\`) statt eines maschinenweiten Pfads außerhalb des
  Repos. Reduziert genau diesen Fund für den größten der hart codierten Pfade.
  Klaus prüft das noch; Entscheidung + Umsetzungsdetails siehe
  [02-builder-migration-csharp.md](02-builder-migration-csharp.md#offene-fragen).
  `%USERPROFILE%\OneDrive - HSOS\...` bleibt davon unberührt (Board-Secrets sollen
  bewusst außerhalb des Repos liegen).

### 🔴 `webmanager::M` – "God Class"
- Eine einzige Singleton-Klasse (`cpp/webmanager.hh`, ~1430 Zeilen) vereint:
  WLAN-Verbindungs-Statemachine, HTTP-Fileserver (Upload/Download/Delete), OTA-Update,
  Login/Session-Verwaltung, Wifimanager-Websocket-Protokoll, allgemeines
  Plugin-Dispatching.
- Macht die Klasse schwer zu testen (kein Host-Build möglich, da tief mit ESP-IDF-
  Headern verwoben) und schwer zu verstehen/ändern (Stufe 3 arbeitet notgedrungen
  direkt in dieser Datei).
- Vorschlag (mittelfristig, nicht Teil von Stufe 3's MVP): Aufteilen in z.B.
  `WifiConnectionManager` (reine State-Machine, host-testbar), `HttpFileServer`,
  `OtaUpdater`, `SessionAuth`, `WifimanagerProtocolHandler` – orchestriert von einer
  dünnen `M`-Fassade.
- Konkret gefundene Bugs innerhalb dieser Klasse siehe
  [03-wifimanager-review.md](03-wifimanager-review.md).

### 🔴 HAL-Board-Varianten – Duplizierungsgrad ungeklärt
- 6 Board-Varianten (`hal_sensactHs_modular.hh`, `hal_sensactHsNano3.hh`,
  `hal_sensactOutdoor.hh`, `hal_sensactUp3.hh`, `hal_sensactUpControl.hh`,
  `hal_sensactUpHs.hh`, je 125–288 Zeilen) erben von einer gemeinsamen Basis
  (`hal_esp32.hh`, `cESP32`). Noch nicht im Detail geprüft, wie viel Pin-/Config-Code
  in den einzelnen Board-Dateien tatsächlich dupliziert ist vs. wie viel echte,
  unvermeidbare Board-Spezifika sind. **To-Do**: bei der Node-Architektur-Review
  (Punkt 2 der Reihenfolge oben) explizit gegenüberstellen und ggf. gemeinsame
  Tabellen-/Config-Strukturen extrahieren.

### 🔴 `RECONNECTS_ON_STARTUP` vs. `RECONNECTS_ON_OPERATION` – siehe Stufe 3
- Wird im Zuge von Stufe 3 behoben (Deadline-Modell statt zwei inkonsistenten
  Attempt-Zählern). Hier nur verlinkt, damit der Fund nicht doppelt in zwei
  Dokumenten gepflegt werden muss.

### 🔴 Keine Tests, kein CI, irgendwo im Projekt
- Weder Firmware noch Builder noch Wifimanager haben aktuell automatisierte Tests.
  Stufe 5 adressiert das für die Sensact-Apps; Stufe 2 sieht Tests für den neuen
  C#-Builder vor. Wifimanager-State-Machine bleibt vorerst ungetestet (siehe
  "optional/stretch" in Stufe 3) – als eigenständiger Backlog-Punkt festhalten, falls
  später Kapazität dafür da ist.

## Vorgehen für neue Funde

- [ ] Neuer Fund → hier als eigener Abschnitt mit Status 🔴, kurzer Beschreibung,
      betroffene Datei(en)/Repo, Vorschlag, Aufwand/Risiko-Einschätzung eintragen.
- [ ] Beim Beheben: Status auf 🟢 setzen, kurz vermerken, in welchem Commit/welcher
      PR das passiert ist.
- [ ] Größere/grundsätzliche Findings (z.B. "God Class" aufteilen) nicht in dieser
      Datei im Detail ausplanen, sondern bei Bedarf eine eigene Datei
      `04a-<thema>.md` daneben anlegen und hier nur verlinken – damit dieses
      Dokument als schnell überfliegbare Übersicht nutzbar bleibt.

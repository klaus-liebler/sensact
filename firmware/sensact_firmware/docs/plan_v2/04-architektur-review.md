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

### 🟢 `configware` war ein unentdeckter, manueller Pipeline-Schritt (erledigt 2026-08-17)
- `C:\repos\sensact\configware\` (C#-Hausmodell-Generator, erzeugt die meisten
  `.inc`-Dateien für `sensact-applicationmodel`) lief ursprünglich komplett
  manuell und unabhängig vom Gulp-Build – nirgends dokumentiert, keine
  Automatisierung, Fehlerquelle bei vergessener Neugenerierung nach
  Modelländerungen.
- Entschieden (2026-07-31), umgesetzt (2026-08-01): vollständig in den
  C#-Build-Orchestrator verschmolzen (Phase `GenerateModelFiles`), siehe
  [02-builder-migration-csharp.md](02-builder-migration-csharp.md).
  `configware/`-Verzeichnis selbst am 2026-08-17 gelöscht, nachdem eine
  zwischenzeitliche Divergenz der zwei parallel gepflegten Generator-Kopien
  bestätigte, dass der alte Pfad nur noch Risiko ohne Nutzen war (s.
  [build-process.md](../build-process.md)).

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

### 🔴 Login-Browser merkt sich Zugangsdaten nicht (analysiert 2026-08-17)
- **Mechanische Ursache gefunden**: `main/main.cc:1-2` hat `HTTP`/`HTTPS` fest
  einkompiliert (`//#define HTTP` / `#define HTTPS`) statt über den generischen
  `board_settings.firmware`-Mechanismus konfigurierbar zu sein – der Server läuft
  deshalb IMMER über `httpd_ssl_start` mit einem selbstsignierten Zertifikat
  (`builder/Certificates.cs`, signiert von einer privaten Root-CA unter
  `OneDrive - HSOS\certificates\rootCA.pem.crt`). Nichts deutet darauf hin, dass
  diese Root-CA je in einem Betriebssystem-/Browser-Truststore installiert wurde.
- Browser (v.a. Chrome/Edge) zeigen bei einer HTTPS-Seite mit nicht vertrauens-
  würdigem Zertifikat eine Warn-Zwischenseite, die bei jedem Besuch neu bestätigt
  werden muss – UND (das ist der eigentliche Grund für das gemeldete Symptom):
  **Chrome bietet auf einer per Zertifikatswarnung erreichten Seite grundsätzlich
  kein "Passwort speichern?"-Prompt an** (bewusste Sicherheitsentscheidung, keine
  Zugangsdaten gegen eine nicht verifizierbare Identität zu speichern).
- Kleinere Mitursachen: `<input>`-Felder im Login-Formular
  (`espidf-component-webmanager/cpp/webmanager.hh`,
  `handle_login_form`) haben kein `autocomplete="username"`/
  `"current-password"`; das Session-Cookie hat trotz HTTPS kein `Secure`-Attribut;
  das externe `<link href='https://fonts.googleapis.com/...'>` kann im
  AP-Fallback-Modus (kein Internet) ohnehin nie geladen werden.
- **Vorgeschlagene Alternativen** (Trade-off, keine davon eindeutig "richtig" –
  Entscheidung steht noch aus):
  1. **Private Root-CA einmalig im Client-Truststore installieren** (dokumentierter
     Provisioning-Schritt, z.B. Download-Link auf der AP-Fallback-Startseite).
     Sauberste Lösung (echtes TLS, normales Browser-Passwort-Verhalten danach),
     aber Einrichtungsaufwand pro Client-Gerät/Browser.
  2. **Auf Klartext-HTTP umschalten** für das rein lokale Admin-Interface (der
     `HTTP`-Zweig existiert bereits, ist nur auskommentiert) – Chrome bietet auf
     HTTP-Formularen im privaten/lokalen Netz ganz normal das Passwort-Speichern
     an. Trade-off: Zugangsdaten/Session-Cookie gehen unverschlüsselt übers WLAN
     (bei einem rein lokal genutzten Smart-Home-Gerät oft akzeptabel, s.
     vergleichbare Consumer-IoT-Geräte).
  3. **HTTP Basic Auth statt Formular-Login.** Browser merken sich Basic-Auth-
     Zugangsdaten sehr zuverlässig (auch über HTTP), kein `<form>`/Cookie-Handling
     nötig. Nachteil: nicht stylebarer nativer Browser-Dialog, kein sauberer
     Logout-Mechanismus.
  4. **SPA-seitiges Session-Persistieren** (`localStorage`/langlebiges Token) als
     Ergänzung zu jeder der obigen Optionen – löst das eigentliche
     "Browser-Autofill"-Problem nicht, reduziert aber, wie oft überhaupt neu
     eingeloggt werden muss.
  - Empfehlung fürs Erste (niedrigster Aufwand, behebt das gemeldete Symptom
    direkt): Option 2, `HTTP`/`HTTPS` dabei über `board_settings.firmware` statt
    hart im Quellcode umschaltbar machen (konsistent mit allen anderen
    board-spezifischen Einstellungen).

### 🟡 Builder soll über sensact hinaus wiederverwendbar werden
- Nutzerwunsch (2026-08-17): der C#-Builder (`builder/`) soll künftig auch in
  anderen, ähnlich aufgebauten ESP32-Firmware-Projekten (s. z.B.
  `C:\repos\labathome\labathome_firmware`) einsetzbar sein. Zielbild: pro
  Projekt bleiben nur `builder/appsettings.json` + eine dünne `builder/Program.cs`
  übrig, der gesamte übrige Code (`Paths.cs`, `Phases/*.cs`, `ModelGeneration/*`,
  `NpmProject.cs`, `Certificates.cs` etc.) wandert in ein eigenständiges,
  gemeinsames Repo/Paket (z.B. `C:\repos\dotnet_libs\firmware_builder`, analog zu
  `best_binary_buffers`) und wird von dort per `ProjectReference`/NuGet
  eingebunden statt pro Projekt kopiert zu werden.
- Voraussetzung/Vorarbeit bereits erledigt: `Paths.FindRootDir()` verlässt sich
  nicht mehr auf eine unbegrenzte Aufwärtssuche, sondern ausschließlich auf das
  unmittelbar übergeordnete Verzeichnis von `builder/` (s. Commit vom
  2026-08-17) – Voraussetzung dafür, dass derselbe Code in unterschiedlich
  verschachtelten Projektstrukturen zuverlässig funktioniert. Maschinen-/
  Projekt-abhängige absolute Pfade liegen ebenfalls bereits vollständig in
  `appsettings.json` (`NpmPackagesDir`, `WebmanagerWsProtocolDir`,
  `BoardsDir`, `CertsDir`), nicht mehr im Code.
- Noch offen/zu klären, bevor die Extraktion angegangen wird: welche Teile sind
  wirklich sensact-agnostisch (`ModelGeneration/*`, `GenerateSensactFiles.cs`
  sind z.B. eng an das sensact-Anwendungsmodell gekoppelt – klare Trennung
  "generischer Kern" vs. "projektspezifische Phase" nötig), wie Versionierung
  zwischen mehreren Konsumenten-Projekten funktioniert (ProjectReference wie bei
  `best_binary_buffers` vs. echtes NuGet-Paket), und ob `labathome_firmware`
  überhaupt zuerst auf das neue best_binary_buffers_schema/BestBinaryBuffers migriert sein muss
  (s. dortige `docs/MIGRATION_TO_BESTBINARYBUFFERS.md`), bevor eine Builder-
  Wiederverwendung dort sinnvoll ist.

## Vorgehen für neue Funde

- [ ] Neuer Fund → hier als eigener Abschnitt mit Status 🔴, kurzer Beschreibung,
      betroffene Datei(en)/Repo, Vorschlag, Aufwand/Risiko-Einschätzung eintragen.
- [ ] Beim Beheben: Status auf 🟢 setzen, kurz vermerken, in welchem Commit/welcher
      PR das passiert ist.
- [ ] Größere/grundsätzliche Findings (z.B. "God Class" aufteilen) nicht in dieser
      Datei im Detail ausplanen, sondern bei Bedarf eine eigene Datei
      `04a-<thema>.md` daneben anlegen und hier nur verlinken – damit dieses
      Dokument als schnell überfliegbare Übersicht nutzbar bleibt.

# Plan v2 – Überarbeitung sensact_firmware & Umfeld

Stand: 2026-07-31. Dies ist der Einstiegspunkt für die mehrstufige Überarbeitung.
Jede Stufe hat eine eigene Datei in diesem Ordner. Die Dateien sind so geschrieben,
dass sowohl Klaus als auch Claude (in einer neuen Session) ohne weiteren Kontext
direkt einsteigen können.

## Die 5 Stufen (wie besprochen)

1. [01-entwicklerdokumentation-und-build.md](01-entwicklerdokumentation-und-build.md) – Entwickler-Doku, insbesondere Build-Prozess
2. [02-builder-migration-csharp.md](02-builder-migration-csharp.md) – Builder von gulp/TypeScript auf C# umstellen
3. [03-wifimanager-review.md](03-wifimanager-review.md) – Wifimanager prüfen + konfigurierbares AP-Fallback
4. [04-architektur-review.md](04-architektur-review.md) – Schrittweise Architektur-/Konsistenz-Review (laufender Backlog)
5. [05-unit-tests-sensact-apps.md](05-unit-tests-sensact-apps.md) – Unit-Tests für die sensact-Apps

Die Reihenfolge entspricht deiner Aufzählung. Tatsächliche Abhängigkeiten sind aber
lockerer, als es die Nummerierung suggeriert – siehe "Reihenfolge & Parallelisierung"
unten.

## Wichtigster Befund vorab: das Projekt ist kein einzelnes Repo

`sensact_firmware` (dieses Verzeichnis) ist **kein eigenes Git-Repository**, sondern
nur ein Unterordner von `C:\repos\sensact` (`git rev-parse --show-toplevel` liefert
`C:/repos/sensact`). Der eigentliche Anwendungscode ist über mehrere, teils
unabhängige Repos verteilt:

| Repo (Remote) | Lokaler Pfad | Rolle |
|---|---|---|
| `klaus-liebler/sensact` | `C:\repos\sensact` | Enthält u.a. `firmware/sensact_firmware` (diese Firmware), `espidf-components/sensact-applicationmodel` (**die eigentlichen "sensact apps"**: blind, onoff, pump, rgbw, sound, …), `sensact-hal`, `sensact-middleware` |
| `klaus-liebler/espidf-component-webmanager` | `C:\repos\espidf-component-webmanager` | Wifimanager + generischer Webserver/OTA/Login, als ESP-IDF-Komponente eingebunden über `EXTRA_COMPONENT_DIRS` |
| `klaus-liebler/npm-packages` | `C:\repos\npm-packages` | Alle `@klaus-liebler/*`-Pakete: Build-Tools (`espidf-vite-secure-build-tools`), Web-UI-Bausteine (`web-components`, `web-components-sensact`), Test-Server-Helfer (`websocket_file_testserver`), u.a. |
| `klaus-liebler/factory_in_a_box` | `C:\repos\factory_in_a_box\firmware_factory_control_unit` | Enthält den **Referenz-Builder in C#** (`builder/`), der als Vorlage für Stufe 2 dient |

Konsequenz für die Arbeit:
- Änderungen an den "sensact apps" (Stufe 5) und an der Firmware selbst (Stufen 1–4)
  landen im selben Repo (`sensact`) – ein `git commit` in `sensact_firmware` committet
  ins Eltern-Repo.
- Änderungen am Wifimanager (Stufe 3) und an den Build-Tools/Web-Components
  (Stufen 1–2) betreffen **andere Repos mit eigenen Remotes**. Diese Session ist
  aktuell nur für `sensact_firmware` (+ ein paar `factory_in_a_box`-Pfade) als
  Arbeitsverzeichnis freigegeben. Für produktive Änderungen in
  `espidf-component-webmanager` oder `npm-packages` braucht es entweder eine neue
  Claude-Code-Session mit passendem Arbeitsverzeichnis, oder wir erarbeiten die
  Änderung hier als Vorschlag/Diff und du wendest sie dort an.
- Jede Stufe, die mehrere Repos berührt, braucht am Ende **mehrere Commits in
  mehreren Repos** – das wird in den jeweiligen Stufen-Dateien explizit vermerkt.

## Weitere Grundtatsachen (Referenz für alle Stufen)

- Aktuelle Build-Kette: `builder/gulpfile.ts` (gulp 5, TypeScript via `tsx`) orchestriert
  Codegenerierung, Zertifikate, Flatbuffers, Vite-Web-Build und ESP-IDF-Build/Flash.
  Alle Low-Level-Funktionen stecken im npm-Paket
  `@klaus-liebler/espidf-vite-secure-build-tools`
  (`C:\repos\npm-packages\@klaus-liebler\espidf-vite-secure-build-tools`).
- Generierte Artefakte landen zentral unter `C:\repos\generated\` (hart codiert in
  `CMakeLists.txt:4` und `gulpfile.ts:37`) sowie board-spezifisch unter
  `%USERPROFILE%\OneDrive - HSOS\esp32_boards\<mac-dir>\`. **In Diskussion:** ob
  `C:\repos\generated\` stattdessen projekt-lokal (`sensact_firmware\generated\`)
  liegen soll – siehe [02-builder-migration-csharp.md](02-builder-migration-csharp.md#offene-fragen).
- Es gibt aktuell **keine automatisierten Tests** irgendwo im Projekt (weder für die
  Firmware noch für den Builder noch für den Wifimanager) und **keinen `docs/`-Ordner**
  vor diesem Plan.
- Es gibt unklare bzw. potenziell falsche Altlasten, die während der Recherche
  aufgefallen sind (Details in den jeweiligen Stufen), u.a.:
  - ~~Widersprüchliche Zertifikatspfade~~ – **erledigt 2026-07-31**: `netcase\certificates`
    war eine Altlast, `main/CMakeLists.txt` nutzt jetzt ausschließlich
    `OneDrive - HSOS\certificates` (wie schon in `gulpfile.ts`).
  - `main/__archive/` enthält tote Dateien mit veralteten APIs (nicht im Build
    referenziert) – Aufräum-Kandidat für Stufe 4.
  - (`board_info copy.json` im Projekt-Root ist **kein** Problem – wird bewusst für
    interne Tests gebraucht.)

## Reihenfolge & Parallelisierung

- **Stufe 1 (Doku)** zuerst als schneller, risikoarmer Start – aber als *lebendes
  Dokument* verstehen: nach Stufe 2 (Builder-Migration) muss der Build-Abschnitt
  aktualisiert werden.
- **Stufe 2 (Builder-Migration)** ist die aufwändigste Einzelstufe und sollte in
  kleinen, parallel lauffähigen Schritten passieren (alter gulp-Weg bleibt nutzbar,
  bis der neue C#-Weg für alle Aufgaben Parität hat).
- **Stufe 3 (Wifimanager)** ist inhaltlich unabhängig von 1/2 und kann jederzeit
  parallel laufen – betrifft aber ein anderes Repo (`espidf-component-webmanager`).
- **Stufe 4 (Architektur-Review)** ist bewusst kein einmaliger Block, sondern ein
  fortlaufender Backlog, der ab sofort parallel zu allen anderen Stufen gepflegt
  wird (jede Stufe trägt Funde dort ein statt sie "nebenbei" zu reparieren).
- **Stufe 5 (Unit-Tests) kommt bewusst als Letztes** (Entscheidung von Klaus,
  siehe [05-unit-tests-sensact-apps.md](05-unit-tests-sensact-apps.md)) – nicht
  parallel zu den anderen Stufen. Trotzdem gilt schon *während* aller anderen
  Stufen: bei jedem Refactoring mitdenken, dass der Code hinterher gut testbar
  bleiben muss (z.B. sauber gegen `iSensactContext` statt gegen ESP-IDF-Header
  programmieren) – Testbarkeit ist ein Kriterium für Umbau-Entscheidungen in Stufe 4,
  auch wenn die Tests selbst erst am Ende geschrieben werden.

## Wie wir mit diesem Plan arbeiten

- Jede Stufen-Datei enthält eine Checkliste (`- [ ]`), die während der Umsetzung
  abgehakt wird – bitte die Dateien direkt editieren, nicht nur im Chat abhaken.
- Offene Entscheidungen sind explizit als "Offene Frage" markiert. Bei diesen bitte
  vor Beginn der Umsetzung kurz Rücksprache halten (per Chat), auch nachdem der Plan
  steht.
- Neue Erkenntnisse während der Umsetzung (z.B. weitere Inkonsistenzen) werden in
  `04-architektur-review.md` nachgetragen, nicht in dieser Übersicht.

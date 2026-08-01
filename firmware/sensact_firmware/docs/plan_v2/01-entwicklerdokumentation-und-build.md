# Stufe 1 – Entwicklerdokumentation (insbesondere Build-Prozess)

Ziel: Ein neuer Entwickler (oder Klaus in 6 Monaten) kann anhand von `docs/` allein
verstehen, woraus das System besteht, wie ein Build/Flash abläuft und wo welche
Artefakte herkommen – ohne den Gulp-Code lesen zu müssen.

## Warum das jetzt zuerst sinnvoll ist

Die Recherche für diesen Plan hat gezeigt: das Wissen über den Build-Prozess steckt
aktuell ausschließlich im Code (`gulpfile.ts`, mehrere TS-Module in
`espidf-vite-secure-build-tools`, `CMakeLists.txt`). Es gibt keine einzige
zusammenfassende Doku-Seite. Das macht sowohl Stufe 2 (Migration) als auch das
Onboarding neuer Mitwirkender unnötig schwer.

## Struktur des neuen `docs/`-Ordners (Vorschlag)

```
docs/
  README.md                  – Einstiegspunkt, verlinkt alles andere
  repo-map.md                – Die Repo-Landschaft (siehe 00-overview.md), Abhängigkeiten
  build-process.md           – Der Build-Prozess Schritt für Schritt (siehe unten)
  board-provisioning.md      – Wie ein neues Board angelegt/verbunden wird
  architecture.md            – Lebendes Architektur-Dokument, wird in Stufe 4 gepflegt
  plan_v2/                   – dieser Plan
```

## Inhalt von `build-process.md` (Kernstück dieser Stufe)

Muss mindestens folgende Punkte erklären, jeweils mit Verweis auf die Quelldatei:

1. **Voraussetzungen**
   - ESP-IDF Version (`dependencies.lock`: `idf 6.0.0`, Target `esp32s3`), `IDF_PATH`
     muss gesetzt sein.
   - Node.js/npm im `builder/`-Ordner (`npm install`), `flatc` (Flatbuffers-Compiler)
     im PATH.
   - Absoluter Pfad-Bedarf: `C:\repos\generated\` (hart codiert, siehe
     `CMakeLists.txt:4` und `gulpfile.ts:37`), `%USERPROFILE%\OneDrive - HSOS\...`
     für Board-Secrets. **Das ist ein Portabilitätsproblem** – in `docs/` erstmal nur
     dokumentieren ("so ist es aktuell"), die eigentliche Bereinigung ist Sache von
     Stufe 4 (siehe dort).
   - `.claude`/`.vscode`-Setup, falls relevant für neue Mitwirkende.

2. **Die Repo-Abhängigkeiten für einen Build** (Diagramm/Tabelle):
   - `EXTRA_COMPONENT_DIRS` aus `CMakeLists.txt:7`: `espidf-components`,
     `sensact/espidf-components`, `espidf-components-lcd`,
     `espidf-component-webmanager`.
   - `builder/package.json`-Dependencies auf `file:../../../../npm-packages/...` und
     `web/package.json`-Dependencies auf `file:../../../../generated/...` und
     `file:../../../../npm-packages/...`.

3. **Die npm-Scripts in `builder/package.json`** und was sie tun (Mapping auf
   Gulp-Tasks in `gulpfile.ts`):
   - `npm run i` → `info` – zeigt aktuellen Board-/Verbindungsstatus.
   - `npm run u` → `addOrUpdateConnectedBoard` – liest verbundenes Board aus (MAC
     etc.), legt/aktualisiert `board_info.json` board-spezifisch ab.
   - `npm run f` → `createFiles` (`gulpfile.ts:100`) – der zentrale Codegen-Schritt:
     1. `sensact.Sensact.PrepareSensactFiles()` (`sensact_code_generator.ts`):
        generiert `applicationIds.fbs`/`commandTypes.fbs`, TS-Pakete
        `sensact_sendCommandImplementation` und `sensact_appsbuilder` aus
        Board-/Node-spezifischen Templates.
     2. Flatbuffers-Codegen für C++ und TS (`flatbuffers_generate_c/_ts`, ruft
        `flatc` auf) aus `flatbuffers/*.fbs` (dieses Repo) und
        `espidf-component-webmanager/flatbuffers/*.fbs`.
     3. Zertifikate (lazy, nur wenn nicht vorhanden): selbstsigniertes ESP32-Zertifikat
        über `certificates.ts` (node-forge), signiert mit der lokalen Root-CA aus
        `OneDrive - HSOS\certificates`.
     4. Flash-Encryption-Key (lazy).
     5. Config-Dateien: `createObjectWithDefines()` sammelt alle Build-Defines
        (Board-Settings, Sensact-Node-Descriptor, Git-Hash, Zeitstempel, ASCII-Banner
        …) und schreibt sie dreifach raus: `cmake/config.json` (für CMake),
        C++-Header `runtimeconfig.hh`/`runtimeconfig_defines.hh`, TS-Paket
        `runtimeconfig_ts`.
   - `npm run wc` → `buildWebForCurrent` = `createFiles` + `buildAndCompressWebProject`
     (Vite-Build + Brotli-Kompression, `vite_helper.ts`).
   - `npm run c` → `buildForCurrent` = obiges + `idf.py build` (`espidf.ts`).
   - Default-Task (`gulp` ohne Argument) → volle Kette inkl. Flash:
     `addOrUpdateConnectedBoard` → `buildForCurrent` →
     `encryptFirmwareIfNecessary` → `flashFirmware`.

4. **Was wird wohin generiert** (Tabelle: Artefakt → Zielpfad → erzeugt von):
   Flatbuffers C++/TS, Runtimeconfig C++/TS, Web-Bundle, Zertifikate,
   Flash-Encryption-Key, `cmake/config.json`.

5. **Board-Provisioning-Flow** (eigene Datei `board-provisioning.md`, aber hier
   verlinkt): Wie sieht der Ablauf für ein *neues* Board aus? MAC auslesen → Eintrag
   unter `OneDrive - HSOS\esp32_boards\<mac-dir>\board_info.json` → `NODE_ID`/Board-Rolle
   festlegen → passendes `main/hal/<NODE_ID>/{hal.inc, station_config_hardware.inc,
   station_config_hosts.inc}` muss existieren (wer generiert diese – aktuell scheinbar
   manuell gepflegt, klären und dokumentieren!).

6. **Web-Dev-Server / Proxy** (aktuell in Bearbeitung, siehe `web/vite.config.mts`,
   uncommitted Änderung im Arbeitsverzeichnis zum Zeitpunkt der Planerstellung):
   - `npm run dev` im `web/`-Ordner startet Vite-Dev-Server.
   - `wsProxyPlugin` proxied WebSocket-Verbindungen zum echten Board
     (`WEBSOCKET_HOST`), weil Browser und Board unterschiedliche Origins haben und
     das Session-Cookie sonst nicht mitgeschickt wird. Der Proxy loggt sich selbst
     einmalig ein und cached das Session-Cookie.
   - Dieser Teil ist eng mit Stufe 3 (Wifimanager/Login-Review) verzahnt – auf
     Konsistenz zwischen beiden Stufen achten, insbesondere den dort dokumentierten
     Set-Cookie-Bug.

7. **Troubleshooting**: bekannte Stolpersteine (z.B. Zertifikatspfad-Diskrepanz
   zwischen `gulpfile.ts` und `main/CMakeLists.txt` – dokumentieren als bekanntes
   Problem, Fix in Stufe 4).

## Vorgehen

- [x] `docs/README.md`, `docs/repo-map.md`, `docs/build-process.md`,
      `docs/board-provisioning.md` angelegt (2026-07-31).
- [x] Mit Klaus geklärt: `main/hal/<NODE_ID>/*.inc`-Dateien werden reine Handarbeit
      gepflegt (kein Generator). In `board-provisioning.md` festgehalten.
- [x] `docs/architecture.md` als erste Bestandsaufnahme angelegt (2026-07-31,
      Node/HAL/Apps-Architektur + Cross-Cutting + bekannte tote Teile) – **noch nicht
      vollständig verifiziert**, wird im Zuge von Stufe 4 weiter geprüft/erweitert.
- [ ] Nach Abschluss von Stufe 2 (Builder-Migration): `build-process.md` aktualisieren,
      alte Gulp-Befehle durch die neuen C#-Befehle ersetzen bzw. beides
      gegenüberstellen, solange beide Wege parallel existieren.
- [ ] Laufend, im Zuge von Stufe 4: `architecture.md` verifizieren/erweitern.

## Entschieden

- Doku wird auf Deutsch geschrieben (bestätigt).

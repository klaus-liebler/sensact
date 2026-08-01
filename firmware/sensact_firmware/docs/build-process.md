# Build-Prozess

Diese Seite beschreibt, wie ein Build/Flash-Durchlauf für `sensact_firmware` heute
abläuft. Der Build wird gerade schrittweise von Gulp/TypeScript auf ein C#-Tool
migriert (siehe [plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md));
diese Seite beschreibt den **aktuellen** (Gulp-basierten) Stand und wird
aktualisiert, sobald Teile des C#-Tools produktiv sind.

## Voraussetzungen

- **ESP-IDF**: Version gemäß `dependencies.lock` (`idf 6.0.0`, Target `esp32s3`).
  Umgebungsvariable `IDF_PATH` muss gesetzt sein; der Build ruft intern
  `%IDF_PATH%\export.bat` auf, bevor er `idf.py`/`esptool`/`espsecure`/`espefuse`
  aufruft.
- **Node.js/npm** im `builder/`-Ordner (`npm install`), sowie im `web/`-Ordner.
- **`flatc`** (Flatbuffers-Compiler) im `PATH`.
- **Absolute Pfade, die auf diesem Rechner existieren müssen** (aktuell hart codiert,
  siehe [repo-map.md](repo-map.md) und
  [plan_v2/04-architektur-review.md](plan_v2/04-architektur-review.md) zur
  Diskussion, ob das projekt-lokal werden soll):
  - `C:\repos\generated\` – Ziel aller generierten Artefakte (`CMakeLists.txt:4`,
    `builder/gulpfile.ts:37`).
  - `%USERPROFILE%\OneDrive - HSOS\esp32_boards\<mac-dir>\` – board-spezifische
    Zertifikate, Flash-Encryption-Keys, `board_info.json`, `sensact.json`.
  - `%USERPROFILE%\OneDrive - HSOS\certificates\` – Root-CA für die selbstsignierten
    Board-Zertifikate. **Wichtig**: dies ist die einzig gültige Quelle; ein älterer,
    abweichender Pfad (`%USERPROFILE%\netcase\certificates`) war eine Altlast und
    wurde in `main/CMakeLists.txt` entfernt.

## Vorgelagerter Schritt: Hausmodell-Codegenerierung (`configware`)

**Bevor** `builder/gulpfile.ts` überhaupt läuft, muss das Hausautomations-Modell
generiert sein: `C:\repos\sensact\configware\` (C#, selbes Repo) definiert – als
C#-Klassen mit IntelliSense-Unterstützung – welche Apps auf welchem Node existieren
und wie sie verdrahtet sind (`configware_sattlerstrasse`/`configware_testmodel`,
jeweils per `dotnet run` manuell in Visual Studio gestartet). Der Generator schreibt
u.a. die `.inc`-Dateien, die `sensact-applicationmodel` direkt einbindet
(`glo2LocCmd.inc`, `applicationInitializers.inc`, `nodeDescription.inc` u.a.) sowie
`node_descriptor.json` nach `C:\repos\generated\sensact_model\` – **derselbe
Ordner**, den `gulpfile.ts` als `SENSACT_COMPONENT_GENERATED_PATH` verwendet.

**Das ist heute ein manueller, undokumentierter Schritt** – wird `configware` nach
einer Modelländerung nicht neu ausgeführt, baut die Firmware mit veraltetem Modell.
Wird laut [plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md)
vollständig in den neuen C#-Build-Orchestrator verschmolzen (eigene Phase
`GenerateModelFiles`, läuft automatisch vor der restlichen Codegenerierung) – diese
Seite wird aktualisiert, sobald das umgesetzt ist.

## Die Repo-Abhängigkeiten für einen Build

Siehe [repo-map.md](repo-map.md) für die vollständige Repo-Landschaft. Kurzfassung:
`EXTRA_COMPONENT_DIRS` (`CMakeLists.txt:7`) zieht ESP-IDF-Komponenten aus drei
weiteren Repos/Ordnern hinzu; `builder/package.json` und `web/package.json` ziehen
npm-Pakete aus `C:\repos\npm-packages\@klaus-liebler\...` sowie generierte Pakete aus
`C:\repos\generated\...`.

## Die npm-Scripts in `builder/package.json`

| Script | Gulp-Task | Was passiert |
|---|---|---|
| `npm run i` | `info` | Zeigt aktuellen Board-/Verbindungsstatus (MAC, Board-Name/-Version/-Rolle, Verschlüsselungsstatus). |
| `npm run u` | `addOrUpdateConnectedBoard` | Liest das aktuell per USB verbundene Board aus (MAC über eine eigene serielle SLIP/ROM-Loader-Implementierung, `esp32.ts`), legt/aktualisiert dessen `board_info.json` unter `OneDrive - HSOS\esp32_boards\<mac-dir>\`. |
| `npm run f` | `createFiles` (`gulpfile.ts:100`) | Der zentrale Codegen-Schritt, siehe unten. |
| `npm run wc` | `buildWebForCurrent` = `createFiles` + `buildAndCompressWebProject` | Zusätzlich: Vite-Build des `web/`-Projekts + Brotli-Kompression (`vite_helper.ts`). |
| `npm run c` | `buildForCurrent` = obiges + `buildFirmware` | Zusätzlich: `idf.py build`. |
| (Default, ohne Argument) | Vollkette | `addOrUpdateConnectedBoard` → `buildForCurrent` → `encryptFirmwareIfNecessary` → `flashFirmware`. |

### Was `createFiles` im Detail tut

1. **Sensact-Codegenerierung** (`sensact_code_generator.ts`, Klasse `Sensact`):
   generiert board-/node-spezifisch `applicationIds.fbs`/`commandTypes.fbs` sowie die
   TS-Pakete `sensact_sendCommandImplementation` und `sensact_appsbuilder` (Web-seitige
   "digitale Zwillinge" der Apps) aus Templates + node-spezifischen `.inc`-Snippets.
2. **Flatbuffers-Codegen** für C++ und TypeScript (`flatbuffers_generate_c/_ts`, ruft
   `flatc` auf) – Quellen sind `flatbuffers/*.fbs` in diesem Repo **und**
   `espidf-component-webmanager/flatbuffers/*.fbs`.
3. **Zertifikate** (lazy, nur wenn noch nicht vorhanden): selbstsigniertes
   ESP32-Zertifikat über `certificates.ts` (node-forge), signiert mit der Root-CA aus
   `OneDrive - HSOS\certificates`.
4. **Flash-Encryption-Key** (lazy, nur wenn noch nicht vorhanden).
5. **Config-Dateien**: `createObjectWithDefines()` sammelt alle Build-Defines
   (Board-Settings aus `board_info.json`, Sensact-Node-Descriptor, Git-Kurz-Hash,
   Zeitstempel, ASCII-Art-Banner …) und schreibt sie dreifach heraus:
   - `cmake/config.json` – gelesen von `CMakeLists.txt` via `file(READ)` +
     `string(JSON GET)`.
   - C++-Header `runtimeconfig.hh` (Namespace `cfg::`) und
     `runtimeconfig_defines.hh` (`#define`-Variante).
   - TS-Paket `runtimeconfig_ts` (für den Web-Client).

## Was wird wohin generiert

| Artefakt | Zielpfad | Erzeugt von |
|---|---|---|
| Flatbuffers C++ | `C:\repos\generated\flatbuffers_cpp\` | `flatbuffers_generate_c` |
| Flatbuffers TS | `C:\repos\generated\flatbuffers_ts\` (eigenes npm-Paket) | `flatbuffers_generate_ts` |
| Runtimeconfig C++ | `C:\repos\generated\runtimeconfig_cpp\` | `key_value_file_helper.createCppConfigurationHeader` |
| Runtimeconfig TS | `C:\repos\generated\runtimeconfig_ts\` (eigenes npm-Paket) | `key_value_file_helper.createTypeScriptRuntimeConfigProject` |
| CMake-Config | `C:\repos\generated\cmake\config.json` | `key_value_file_helper.createCMakeJsonConfigFile` |
| Web-Bundle (komprimiert) | `C:\repos\generated\web\index.compressed.br` | `vite_helper.buildAndCompressWebProject`, eingebettet via `main/CMakeLists.txt` (`EMBED_FILES`) |
| Board-Zertifikat/-Key | `OneDrive - HSOS\esp32_boards\<mac-dir>\certificates\` | `certificates.ts`, eingebettet via `main/CMakeLists.txt` (`EMBED_TXTFILES`) |
| Root-CA | `OneDrive - HSOS\certificates\rootCA.pem.crt` | (manuell/einmalig angelegt), eingebettet via `main/CMakeLists.txt` |
| Flash-Encryption-Key | `OneDrive - HSOS\esp32_boards\<mac-dir>\` | `idf.createRandomFlashEncryptionKeyLazily` |

## Neues Board anlegen / in Betrieb nehmen

Siehe [board-provisioning.md](board-provisioning.md) für den vollständigen Ablauf.

## Web-Dev-Server / Proxy

`npm run dev` im `web/`-Ordner startet den Vite-Dev-Server. Ein `wsProxyPlugin` in
`web/vite.config.mts` proxied WebSocket-Verbindungen zum echten Board, weil Browser
(Dev-Server-Origin) und Board unterschiedliche Origins haben und das Session-Cookie
sonst nicht mitgeschickt würde – der Proxy loggt sich selbst einmalig gegen
`/login` ein und cached das resultierende Session-Cookie für nachfolgende
Verbindungen. Dieser Teil hängt eng mit dem Login/Session-Code im Wifimanager
zusammen, siehe [plan_v2/03-wifimanager-review.md](plan_v2/03-wifimanager-review.md)
für einen dort bekannten Bug (fehlerhafter `Set-Cookie`-Header), der clientseitig im
Proxy umgangen wird.

## Troubleshooting

- **`idf.py build` schlägt mit "config.json not found" fehl**: `npm run f` (oder
  `wc`/`c`) wurde noch nicht ausgeführt – `createFiles` muss vor jedem
  CMake-Konfigurationslauf gelaufen sein.
- **Zertifikatsfehler beim Flashen**: prüfen, ob
  `%USERPROFILE%\OneDrive - HSOS\certificates\rootCA.pem.crt` existiert (wird nicht
  automatisch von `createFiles` angelegt, nur die board-spezifischen Zertifikate
  sind lazy-generiert).

# Build-Prozess

Diese Seite beschreibt, wie ein Build/Flash-Durchlauf für `sensact_firmware`
abläuft.

## Voraussetzungen

- **ESP-IDF**: Version gemäß `dependencies.lock` (`idf 6.0.0`, Target `esp32s3`).
  Umgebungsvariable `IDF_PATH` muss gesetzt sein (`BuildFirmware`/`FlashFirmware`
  prüfen das explizit und brechen mit einer klaren Fehlermeldung ab, wenn nicht);
  der Build ruft intern `%IDF_PATH%\export.bat` auf, bevor er `idf.py` aufruft.
  `export.bat` bricht unter MSYS-artigen Shells (git-bash: `MSYSTEM` gesetzt) sofort
  mit "This .bat file is for Windows CMD.EXE shell only." ab – Builder-Aufrufe, die
  bis `BuildFirmware`/`FlashFirmware` kommen sollen, brauchen daher eine echte
  `cmd.exe`/PowerShell-Umgebung.
- **.NET SDK** (aktuell `net10.0`) für `builder/`.
- **Node.js/npm** im `web/`-Ordner (für den Vite-Build) sowie in den generierten
  npm-Projekten unter `generated/` (werden vom Builder bei Bedarf selbst per
  `npm install` initialisiert).
- **`builder/appsettings.json`** (gitignored, personal – getrackt ist nur
  `appsettings.json.template`; wird beim ersten Build automatisch aus dem
  Template kopiert, s. `builder.csproj`s `EnsureAppSettings`-Target). Enthält
  ausschließlich maschinenabhängige, nicht versionierbare absolute Pfade:

  | Key | Bedeutung |
  |---|---|
  | `BoardStorage.BoardsDir` | Board-Archiv (pro Board ein Unterordner `<mac6hex>_<macDecimal>_<mac12hex>`: Zertifikate, Flash-Encryption-Key, `board_info.json`, `sensact.json`). |
  | `Certificates.CertsDir` | Private CA + Server-Root-Zertifikat. |
  | `Certificates.SubjectPrefix` | Zertifikats-Subject-Präfix (`/C=.../ST=.../L=.../O=...`). |
  | `NpmPackagesDir` | Checkout-Pfad des `npm-packages`-Repos (klaus-liebler/npm-packages) – von generierten npm-Projekten per relativem `file:`-Pfad referenziert. |
  | `WebmanagerBestBinaryBuffersSchemaDir` | Checkout-Pfad von `espidf-component-webmanager/best_binary_buffers_schema` – zweite BestBinaryBuffers-Schema-Quelle neben dem eigenen `best_binary_buffers_schema/`. |

  Alles andere (generierte Dateien, Repo-interne Pfade) ist projektlokal und
  braucht keine Konfiguration, s. unten.

## Einmalige manuelle Vorbereitung (neue Maschine)

Diese Schritte sind pro Maschine (nicht pro Checkout) genau einmal nötig, bevor der
erste `Pipeline`/`PipelineOffline`-Lauf durchläuft. Der Builder prüft/automatisiert
das nicht selbst.

- **ESP-IDF installieren**: Ziel-Version (s. `dependencies.lock`) irgendwohin
  auschecken (z.B. `C:\esp\v6.0\esp-idf`), darin einmalig `install.ps1`
  (bzw. `install.bat`) ausführen – legt das Python-Venv samt Toolchain unter
  `%USERPROFILE%\.espressif\python_env\` an (u.a. `esptool.exe`/`espefuse.exe`,
  von `PrepareContextWithRealHardware`/`FlashFirmware` gebraucht). Danach `IDF_PATH`
  dauerhaft (User-Umgebungsvariable) auf dieses Verzeichnis setzen.
- **Vor jedem Builder-Aufruf in einer neuen Shell**: `& "$env:IDF_PATH\export.ps1"`
  (bzw. `%IDF_PATH%\export.bat` in `cmd.exe`) einmal ausführen. Das bringt
  `esptool`/`espefuse` (und `idf.py`) auf `PATH`. Wichtig: `PrepareContextWithRealHardware`
  läuft in `Pipeline` **vor** `BuildFirmware` und damit vor dessen eigenem internen
  `export.bat`-Aufruf – ohne vorher von Hand ausgeführtes `export.ps1`/`export.bat`
  schlägt schon dieser erste Schritt mit "Datei nicht gefunden: esptool" fehl.
- **`npm-packages`-Repo** (`klaus-liebler/npm-packages`, Pfad s. `NpmPackagesDir`
  oben): `web/` referenziert mehrere `@klaus-liebler/*`-Unterpakete dort per
  `file:`-Pfad. npm legt `file:`-Abhängigkeiten als Symlink an; Imports innerhalb
  eines so verlinkten Pakets löst Node über dessen echten (Symlink-aufgelösten)
  Pfad auf, nicht über `web/node_modules` – ein `npm install` in `web/` allein
  installiert diese Unterpakete also nicht. Jedes Unterpaket, das selbst
  `dependencies` in seiner `package.json` hat, braucht deshalb einmalig ein
  **eigenes** `npm install` direkt in seinem Ordner. Aktuell betroffen:
  `sensact-base`, `usersettings_runtime`, `web-components`,
  `web-components-sensact` (nicht `commons`/`svgs` – die haben keine eigenen
  Dependencies). Fehlt das, bricht `BuildWebApp` mit einem Vite/Rollup-Fehler wie
  `Rollup failed to resolve import "@generated/wsprotocol_ts/..."` ab.
- **`npm install` in `web/`** selbst (für Vite & Co.).

## Hausmodell-Codegenerierung (`GenerateModelFiles`)

Das Hausautomations-Modell (welche Apps auf welchem Node existieren und wie sie
verdrahtet sind) steht als XML in `model/<Name>.xml` (s. [model/README.md](../model/README.md))
und wird über die Phase `GenerateModelFiles --model Sattlerstrasse16|Testmodel` in Code
übersetzt. Import und Codegenerierung (Bibliothek `modellib/`: `ModelGeneration/Xml/HouseXmlImporter.cs`,
`ModelGeneration/Generator.cs`, `SourceCodeGenerator`, ...) nutzen Builder und Editor gemeinsam.

Schreibt u.a. die `.inc`-Dateien nach `generated/sensact_model/` (Teil des projektlokalen
`GeneratedRoot`, s. unten). Seit der datengetriebenen Basis (Schritt 2) bindet die
ESP32-Firmware davon nur noch die **modellunabhängigen** Dateien unter `common/` ein
(Kommandos, Events, Dispatch); welche Apps eine Station hat, kommt zur Laufzeit aus der
Stationskonfiguration im Stationspaket (`BuildStationPackage`, s. Tabelle unten). Die
stationsspezifischen Dateien (`<NODE_ID>/applicationInitializers.inc` usw.) erzeugt der
Generator weiter – für die Host-Tests und die STM32-Altfirmware.

Kopiert danach die Enum-Quelle der Kommandos (`generated/sensact_model/common/commandTypes.enums.cs`)
nach `best_binary_buffers_schema/sensact_commandTypes.enums.cs` (git-getrackt). Die App-IDs
gehören seit Schritt 2 nicht mehr zum ws-Protokoll (dort `uint16_t`); deshalb muss
`GenerateModelFiles` vor `GenerateBestBinaryBufferFiles` laufen (`Pipeline` macht das in der
richtigen Reihenfolge).

## Generierte Dateien: projektlokal

Alle von `builder` erzeugten Dateien liegen unter `sensact_firmware/generated/`
(`SensactBuildContext.GeneratedRoot`, gitignored).

| Artefakt | Zielpfad | Erzeugt von (Phase) |
|---|---|---|
| ws-protocol C++ | `generated/wsprotocol_cpp/ws_protocol.hh` | `GenerateBestBinaryBufferFiles` |
| ws-protocol TS | `generated/wsprotocol_ts/` (eigenes npm-Paket) | `GenerateBestBinaryBufferFiles` |
| Runtimeconfig C++ | `generated/runtimeconfig_cpp/` | `GenerateRuntimeConfig` |
| Runtimeconfig TS | `generated/runtimeconfig_ts/` (eigenes npm-Paket) | `GenerateRuntimeConfig` |
| CMake-Config | `generated/cmake/config.json` | `GenerateRuntimeConfig` |
| Sensact-Model | `generated/sensact_model/` | `GenerateModelFiles` (s.o.) |
| `sendCommandImplementation` | `generated/sensact_sendCommandImplementation/` (eigenes npm-Paket) | `GenerateSensactFiles` |
| `sensactapps` (digitaler Zwilling für Web-UI) | `generated/sensact_appsbuilder/` (eigenes npm-Paket) | `GenerateSensactFiles` |
| App-Nummern/-Namen für die Web-UI | `generated/sensact_applicationids/` (eigenes npm-Paket) | `GenerateSensactFiles` |
| Web-Bundle (komprimiert) | `generated/web/index.compressed.br` | `BuildWebApp`; geht als Abschnitt 1 ins Stationspaket (`BuildStationPackage`), **nicht** mehr in die Firmware |
| Wartungsseite (komprimiert) | `generated/maintenance/index.compressed.br` | `BuildFirmware` aus `main/maintenance/index.html`, eingebettet via `main/CMakeLists.txt` (`EMBED_FILES`) |
| Stationspaket | `deploy/<NODE_ID>/<NODE_ID>.pkg`, Kopie in `littlefs_filesystem/station.pkg` | `BuildStationPackage` (Web-UI + Stationskonfiguration) |
| Board-Zertifikat/-Key | `<BoardsDir>\<mac-dir>\certificates\` | `GenerateCertificates`, eingebettet via `main/CMakeLists.txt` (`EMBED_TXTFILES`) |

Cross-Repo-Konsumenten (die npm-Pakete in `npm-packages/@klaus-liebler/*` sowie
`web/` selbst) referenzieren die generierten TS-Pakete per relativem
`file:`-Pfad; für `npm-packages/*` ist das ein Sibling-Pfad
(`../../../sensact/firmware/sensact_firmware/generated/...`), für `web/`
(gleiches Repo) ein kurzer (`../generated/...`).

Alle Namespaces mit echter Server-Implementierung nutzen
[BestBinaryBuffers](https://github.com/klaus-liebler/best_binary_buffers) als
Wire-Format (Schema als annotierter C#-Code unter `best_binary_buffers_schema/`).

## Die Repo-Abhängigkeiten für einen Build

Siehe [repo-map.md](repo-map.md) für die vollständige Repo-Landschaft.
Kurzfassung: `EXTRA_COMPONENT_DIRS` (`CMakeLists.txt`) zieht ESP-IDF-Komponenten
aus drei weiteren Repos/Ordnern hinzu; `web/package.json` zieht npm-Pakete aus
`npm-packages/@klaus-liebler/...` (Pfad kommt aus `appsettings.json`s
`NpmPackagesDir`, s.o.) sowie aus dem eigenen `generated/`.

## Die Phasen von `builder`

Aufruf: `dotnet run --project builder -- <Phase> [Argumente]` (aus dem
`builder/`-Verzeichnis auch ohne `--project builder`). Vollständige Liste s.
`builder/Program.cs`.

| Phase | Was passiert |
|---|---|
| `Info` | Zeigt aktuellen Board-/Verbindungsstatus (`board_info.json`, aufgelöste NodeId). |
| `GitStatus` | Git-Kurz-Hash etc. für den Build-Banner. |
| `PrepareContextWithRealHardware` | Liest Chip-Typ/MAC vom per USB verbundenen Board (über `esptool`), legt/aktualisiert dessen Board-Archiv und kopiert es als aktives Board in den Repo-Root (`board_info.json`). |
| `PrepareContextWithCommandLineArguments --nodeId <NODE_ID>` | Wählt ohne angeschlossenes Board ein (deterministisch aus der `NODE_ID` abgeleitetes, synthetisches) Board-Archiv – für Offline-Builds, s. [board-provisioning.md](board-provisioning.md). |
| `GenerateBestBinaryBufferFiles` | Liest `*.cs`-Schema-Dateien aus `best_binary_buffers_schema/` (eigenes Repo) **und** `WebmanagerBestBinaryBuffersSchemaDir` (s.o.), generiert `ws_protocol.hh`/`ws-protocol.ts` über `BestBinaryBuffers`. |
| `GenerateRuntimeConfig` | Sammelt Build-Defines (Board-Settings, Node-Descriptor, Git-Hash, Zeitstempel, Banner …), schreibt `cmake/config.json`, `runtimeconfig.hh`/`runtimeconfig_defines.hh`, `runtimeconfig_ts`. |
| `GenerateCertificates` | Lazy: Board-Zertifikat (.NET-Kryptografie aus `FirmwareBuilder.Common`, signiert mit der Root-CA aus `CertsDir`, Parameter s. `Certificates.*` in `appsettings.json`) + Flash-Encryption-Key, nur falls noch nicht vorhanden. |
| `GenerateModelFiles --model <Name>` | Erzeugt aus `model/<Name>.xml` die Modell-Dateien (`.inc`-Dateien, ws-protocol-Enum-Quellen), s. Abschnitt oben. |
| `GenerateModelFilesForHostTests --out <Verzeichnis>` | Wie `GenerateModelFiles`, aber immer aus dem Testmodel, in ein beliebiges Verzeichnis und **ohne** die Enum-Quellen nach `best_binary_buffers_schema/` zu kopieren. Wird automatisch von den Host-Unit-Tests der Apps aufgerufen (s. "Tests" unten). |
| `BuildPackage --model <Name>` | **Nach einer Änderung am Hausmodell:** nur das Stationspaket für das aktive Board (Generierung → Web-App → Paket), keine Firmware. Ergebnis `deploy/<NODE_ID>/<NODE_ID>.pkg`, Upload über die Weboberfläche (System → Station Package), das Gerät startet danach neu. |
| `BuildForOta --model <Name>` | **Nach einer Änderung an der Basis-Firmware:** Firmware und Paket für das **aktive** (echte) Board, ohne zu flashen (Host-Tests → Generierung → Web-App → Paket → Firmware → Deploy). Bricht ab, wenn das aktive Board keine Station des Modells ist. Anders als `PipelineOffline` mit den echten Zertifikaten des Boards. |
| `BuildStationPackage --model <Name>` | Stationspaket aus der zuletzt gebauten Web-UI (Abschnitt 1) und der Stationskonfiguration aus `model/<Name>.xml` (Abschnitt 2, `StationConfigWriter`): `deploy/<NODE_ID>/<NODE_ID>.pkg`, zusätzlich als `station.pkg`/`webui.br` ins LittleFS-Quellverzeichnis (`littlefs_filesystem/`) für den ersten USB-Flash. Läuft vor `BuildFirmware`. |
| `CollectDeployFiles` | Kopiert `build/sensact_firmware.bin` nach `deploy/<NODE_ID>/` und schreibt eine `README.txt` mit der Upload-Reihenfolge. |
| `RunHostTests` | Baut und startet die Host-Unit-Tests der Apps (s. "Tests"); Teil von `Pipeline`/`PipelineOffline`. |
| `GenerateSensactFiles` | Baut `sensact_sendCommandImplementation`/`sensact_appsbuilder` aus Templates (`web/templates/*.template.ts`) + Hausmodell-`.inc`-Snippets. |
| `BuildWebApp` | Vite-Build des `web/`-Projekts + Brotli-Kompression. |
| `BuildFirmware` | `export.bat && idf.py build` (ein einziger verketteter Prozess). |
| `FlashFirmware [--resetNVSPartition]` | Flasht (nur unverschlüsselter Pfad implementiert; ist der Flash-Encryption-Key im Board-Archiv als aktiviert markiert, bricht die Phase bewusst ab). `--resetNVSPartition` löscht zusätzlich die NVS-Partition (WLAN-/Usersettings vergessen). |
| `Pipeline --model <Name> [--resetNVSPartition]` | Kompletter Zyklus für ein angeschlossenes Board: `PrepareContextWithRealHardware` → `RunHostTests` → `GenerateModelFiles` → `GenerateBestBinaryBufferFiles` → `GenerateRuntimeConfig` → `GenerateCertificates` → `GenerateSensactFiles` → `BuildWebApp` → `BuildStationPackage` → `BuildFirmware` → `CollectDeployFiles` → `FlashFirmware`. |
| `PipelineOffline --nodeId <NODE_ID> --model <Name>` | Wie `Pipeline`, aber mit `PrepareContextWithCommandLineArguments` statt echter Hardware und ohne `FlashFirmware` – für Builds ohne angeschlossenes Board. |

## Tests

Die Sensact-Apps haben Host-Unit-Tests (GoogleTest, laufen auf dem PC, kein Board nötig):
`espidf-components/sensact-applicationmodel/host_test`, Bedienung s. dortige
[README.md](../../../espidf-components/sensact-applicationmodel/host_test/README.md).
Sie laufen als Schritt `RunHostTests` automatisch in `Pipeline` und `PipelineOffline`
(vor der Code-Generierung); ein roter Test bricht den Build ab. Einzeln:
`dotnet run --project builder -- RunHostTests` – aus PowerShell/cmd, nicht aus Git-Bash.

## Neues Board anlegen / in Betrieb nehmen

Siehe [board-provisioning.md](board-provisioning.md) für den vollständigen Ablauf.

## Web-Dev-Server / Proxy

`npm run dev` im `web/`-Ordner startet den Vite-Dev-Server. Ein `wsProxyPlugin` in
`web/vite.config.mts` proxied WebSocket-Verbindungen zum echten Board, weil Browser
(Dev-Server-Origin) und Board unterschiedliche Origins haben und das Session-Cookie
sonst nicht mitgeschickt würde – der Proxy loggt sich selbst einmalig gegen
`/login` ein und cached das resultierende Session-Cookie für nachfolgende
Verbindungen.

## Troubleshooting

- **`PrepareContextWithRealHardware`/`FlashFirmware` schlägt mit "Datei nicht
  gefunden: esptool" (o.ä. für `espefuse`) fehl**: `export.ps1`/`export.bat` wurde
  in der aktuellen Shell noch nicht ausgeführt, s. "Einmalige manuelle
  Vorbereitung" oben.
- **`BuildWebApp` schlägt mit `[vite]: Rollup failed to resolve import
  "@generated/..."` (oder einem `@klaus-liebler/...`-Import) fehl**: In einem der
  `npm-packages/@klaus-liebler/*`-Unterpakete fehlt `node_modules` – s. "Einmalige
  manuelle Vorbereitung" oben, Abschnitt `npm-packages`-Repo.
- **`idf.py build` schlägt mit "config.json not found" fehl**:
  `GenerateRuntimeConfig` wurde noch nicht ausgeführt – muss vor jedem
  CMake-Konfigurationslauf gelaufen sein (`Pipeline` macht das automatisch).
- **`idf.py` "nicht gefunden" / `export.bat`-Meldung "for Windows CMD.EXE shell only"**:
  die Shell, in der der Builder läuft, hat `MSYSTEM` gesetzt (git-bash/MSYS2) –
  `export.bat` bricht darin sofort ab, ohne `idf.py` in den `PATH` zu bringen.
  Builder-Aufrufe bis `BuildFirmware`/`FlashFirmware` in PowerShell oder `cmd.exe`
  ausführen.
- **`Konnte Repo-Wurzel nicht finden`**: `FindRootDir()` (gemeinsame Bibliothek
  `dotnet_libs/firmware_builder_common`, `BuildContextPaths.cs`)
  erwartet die `CMakeLists.txt` ausschließlich im unmittelbar übergeordneten
  Verzeichnis von `builder/` – kein generisches Aufwärtssuchen darüber hinaus.
  Tritt z.B. auf, wenn `builder/` versehentlich in ein anderes Verzeichnis
  kopiert wurde, ohne die zugehörige `CMakeLists.txt` mitzunehmen.
- **Zertifikatsfehler beim Flashen**: prüfen, ob die Root-CA unter `CertsDir`
  existiert (wird nicht automatisch von `GenerateCertificates` angelegt, nur
  die board-spezifischen Zertifikate sind lazy-generiert).

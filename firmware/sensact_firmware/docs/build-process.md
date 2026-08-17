# Build-Prozess

Diese Seite beschreibt, wie ein Build/Flash-Durchlauf für `sensact_firmware`
heute abläuft. Der frühere Gulp/TypeScript-Orchestrator (`builder/gulpfile.ts`)
ist seit August 2026 vollständig durch ein C#-Tool ersetzt (`builder/`, s.
[plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md));
diese Seite beschreibt den **aktuellen** Stand.

## Voraussetzungen

- **ESP-IDF**: Version gemäß `dependencies.lock` (`idf 6.0.0`, Target `esp32s3`).
  Umgebungsvariable `IDF_PATH` muss gesetzt sein (`BuildFirmware`/`FlashFirmware`
  prüfen das explizit und brechen mit einer klaren Fehlermeldung ab, wenn nicht);
  der Build ruft intern `%IDF_PATH%\export.bat` auf, bevor er `idf.py` aufruft.
- **.NET SDK** (aktuell `net10.0`) für `builder/` selbst.
- **Node.js/npm** im `web/`-Ordner (für den Vite-Build) sowie in den generierten
  npm-Projekten unter `generated/` (werden vom Builder bei Bedarf selbst per
  `npm install` initialisiert).
- **`builder/appsettings.json`** (gitignored, personal – getrackt ist nur
  `appsettings.json.template`; wird beim ersten Build automatisch aus dem
  Template kopiert, s. `builder.csproj`s `EnsureAppSettings`-Target). Enthält
  ausschließlich maschinenabhängige, nicht versionierbare absolute Pfade:

  | Key | Bedeutung |
  |---|---|
  | `BoardsDir` | Board-Archiv (pro Board ein Unterordner `<mac6hex>_<macDecimal>_<mac12hex>`: Zertifikate, Flash-Encryption-Key, `board_info.json`). |
  | `CertsDir` | Private CA + Server-Root-Zertifikat. |
  | `SubjectPrefix` | Zertifikats-Subject-Präfix (`/C=.../ST=.../L=.../O=...`). |
  | `NpmPackagesDir` | Checkout-Pfad des `npm-packages`-Repos (klaus-liebler/npm-packages) – von generierten npm-Projekten per relativem `file:`-Pfad referenziert. |
  | `WebmanagerWsProtocolDir` | Checkout-Pfad von `espidf-component-webmanager/ws-protocol` – zweite ws-protocol-Schema-Quelle neben dem eigenen `ws-protocol/`. |

  Alles andere (generierte Dateien, Repo-interne Pfade) ist projektlokal und
  braucht keine Konfiguration, s. unten.

## Vorgelagerter Schritt: Hausmodell-Codegenerierung (`configware`)

**Bevor** `builder` überhaupt läuft, muss das Hausautomations-Modell generiert
sein: `C:\repos\sensact\configware\` (C#, selbes Repo) definiert – als
C#-Klassen mit IntelliSense-Unterstützung – welche Apps auf welchem Node
existieren und wie sie verdrahtet sind (`configware_sattlerstrasse`/
`configware_testmodel`, jeweils per `dotnet run` manuell gestartet). Der
Generator schreibt u.a. die `.inc`-Dateien, die `sensact-applicationmodel`
direkt einbindet, sowie `node_descriptor.json` nach
`sensact_firmware/generated/sensact_model/` (Ziel kommt aus der jeweiligen
`appsettings.json` des configware-Konsolenprojekts, Key
`SourceCodeGenerator.BasePath`) – **derselbe Ordner**, den
`sensact-applicationmodel/CMakeLists.txt` über `${GENERATED_DIR}/sensact_model`
einbindet.

**Das ist weiterhin ein manueller, undokumentierter Schritt** – wird
`configware` nach einer Modelländerung nicht neu ausgeführt, baut die Firmware
mit veraltetem Modell. Soll laut
[plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md)
vollständig in `builder` verschmolzen werden (eigene Phase, liefe automatisch
vor der restlichen Codegenerierung) – bisher nicht umgesetzt.

## Generierte Dateien: projektlokal

Alle von `builder` selbst erzeugten Dateien liegen unter
`sensact_firmware/generated/` (`builder/Paths.cs`: `GeneratedRoot = RootDir +
"/generated"`, gitignored). Bis August 2026 lag das in einem repo-losen,
nirgends versionierten Verzeichnis `C:\repos\generated\` – das ist Geschichte,
`C:\repos\generated\` existiert nicht mehr.

| Artefakt | Zielpfad | Erzeugt von (Phase) |
|---|---|---|
| ws-protocol C++ | `generated/wsprotocol_cpp/ws_protocol.hh` | `GenerateWsProtocolFiles` |
| ws-protocol TS | `generated/wsprotocol_ts/` (eigenes npm-Paket) | `GenerateWsProtocolFiles` |
| Runtimeconfig C++ | `generated/runtimeconfig_cpp/` | `GenerateRuntimeConfig` |
| Runtimeconfig TS | `generated/runtimeconfig_ts/` (eigenes npm-Paket) | `GenerateRuntimeConfig` |
| CMake-Config | `generated/cmake/config.json` | `GenerateRuntimeConfig` |
| Sensact-Model | `generated/sensact_model/` | `configware_*` (s.o., **nicht** von `builder`) |
| `sendCommandImplementation` | `generated/sensact_sendCommandImplementation/` (eigenes npm-Paket) | `GenerateSensactFiles` |
| `sensactapps` (digitaler Zwilling für Web-UI) | `generated/sensact_appsbuilder/` (eigenes npm-Paket) | `GenerateSensactFiles` |
| Web-Bundle (komprimiert) | `generated/web/index.compressed.br` | `BuildWebApp`, eingebettet via `main/CMakeLists.txt` (`EMBED_FILES`) |
| Board-Zertifikat/-Key | `<BoardsDir>\<mac-dir>\certificates\` | `GenerateCertificates`, eingebettet via `main/CMakeLists.txt` (`EMBED_TXTFILES`) |

Cross-Repo-Konsumenten (die npm-Pakete in `npm-packages/@klaus-liebler/*` sowie
`web/` selbst) referenzieren die generierten TS-Pakete per relativem
`file:`-Pfad; für `npm-packages/*` ist das ein Sibling-Pfad
(`../../../sensact/firmware/sensact_firmware/generated/...`), für `web/`
(gleiches Repo) ein kurzer (`../generated/...`).

Flatbuffers gibt es hier nicht mehr – alle Namespaces mit echter
Server-Implementierung sind auf [BestBinaryBuffers](https://github.com/klaus-liebler/best_binary_buffers)
umgestellt (s. [plan_v2/03-wifimanager-review.md](plan_v2/03-wifimanager-review.md),
Abschnitt "ws-protocol-Migration").

## Die Repo-Abhängigkeiten für einen Build

Siehe [repo-map.md](repo-map.md) für die vollständige Repo-Landschaft.
Kurzfassung: `EXTRA_COMPONENT_DIRS` (`CMakeLists.txt`) zieht ESP-IDF-Komponenten
aus drei weiteren Repos/Ordnern hinzu; `web/package.json` zieht npm-Pakete aus
`npm-packages/@klaus-liebler/...` (Pfad kommt aus `appsettings.json`s
`NpmPackagesDir`, s.o.) sowie aus dem eigenen `generated/`.

## Die Phasen von `builder`

Aufruf: `dotnet run --project builder -- <Phase> [Argumente]` (aus dem
`builder/`-Verzeichnis auch ohne `--project builder`). Vollständige, aktuelle
Liste s. Kopfkommentar in `builder/Program.cs`.

| Phase | Was passiert |
|---|---|
| `Info` | Zeigt aktuellen Board-/Verbindungsstatus. |
| `GitStatus` | Git-Kurz-Hash etc. für den Build-Banner. |
| `GenerateWsProtocolFiles` | Liest `*.cs`-Schema-Dateien aus `ws-protocol/` (eigenes Repo) **und** `WebmanagerWsProtocolDir` (s.o.), generiert `ws_protocol.hh`/`ws-protocol.ts` über `BestBinaryBuffers`. |
| `GenerateRuntimeConfig` | Sammelt Build-Defines (Board-Settings, Node-Descriptor, Git-Hash, Zeitstempel, Banner …), schreibt `cmake/config.json`, `runtimeconfig.hh`/`runtimeconfig_defines.hh`, `runtimeconfig_ts`. |
| `GenerateCertificates` | Lazy: Board-Zertifikat (node-forge, signiert mit der Root-CA aus `CertsDir`) + Flash-Encryption-Key, nur falls noch nicht vorhanden. |
| `GenerateModelFiles --model <Name>` | Noch nicht portiert (s.o.) – `configware_*` bleibt ein separater, manueller Schritt. |
| `GenerateSensactFiles` | Baut `sensact_sendCommandImplementation`/`sensact_appsbuilder` aus Templates (`web/templates/*.template.ts`) + `configware`-`.inc`-Snippets. |
| `BuildWebApp` | Vite-Build des `web/`-Projekts + Brotli-Kompression. |
| `BuildFirmware` | `export.bat && idf.py build` (ein einziger verketteter Prozess, s. Kommentar in `BuildFirmware.cs` zur `PATH`-Vererbung). |
| `ReadHardwareIds` | Liest MAC etc. vom per USB verbundenen Board. |
| `FlashFirmware [--resetNVSPartition]` | Flasht (nur unverschlüsselter Pfad implementiert, s. Kommentar in `FlashFirmware.cs`); `--resetNVSPartition` löscht zusätzlich die NVS-Partition (WLAN-/Usersettings vergessen). |
| `Pipeline --model <Name> [--resetNVSPartition]` | Kompletter Zyklus: alle obigen Phasen (außer `Info`/`GitStatus`) einmal nacheinander, endet mit `FlashFirmware`. |

## Neues Board anlegen / in Betrieb nehmen

Siehe [board-provisioning.md](board-provisioning.md) für den vollständigen Ablauf.

## Web-Dev-Server / Proxy

`npm run dev` im `web/`-Ordner startet den Vite-Dev-Server. Ein `wsProxyPlugin` in
`web/vite.config.mts` proxied WebSocket-Verbindungen zum echten Board, weil Browser
(Dev-Server-Origin) und Board unterschiedliche Origins haben und das Session-Cookie
sonst nicht mitgeschickt würde – der Proxy loggt sich selbst einmalig gegen
`/login` ein und cached das resultierende Session-Cookie für nachfolgende
Verbindungen. Dieser Teil hängt eng mit dem Login/Session-Code im Wifimanager
zusammen, siehe [plan_v2/03-wifimanager-review.md](plan_v2/03-wifimanager-review.md).

## Troubleshooting

- **`idf.py build` schlägt mit "config.json not found" fehl**:
  `GenerateRuntimeConfig` wurde noch nicht ausgeführt – muss vor jedem
  CMake-Konfigurationslauf gelaufen sein (`Pipeline` macht das automatisch).
- **`Konnte Repo-Wurzel nicht finden`**: `builder/Paths.cs`s `FindRootDir()`
  erwartet die `CMakeLists.txt` ausschließlich im unmittelbar übergeordneten
  Verzeichnis von `builder/` – kein generisches Aufwärtssuchen darüber hinaus.
  Tritt z.B. auf, wenn `builder/` versehentlich in ein anderes Verzeichnis
  kopiert wurde, ohne die zugehörige `CMakeLists.txt` mitzunehmen.
- **Zertifikatsfehler beim Flashen**: prüfen, ob die Root-CA unter `CertsDir`
  existiert (wird nicht automatisch von `GenerateCertificates` angelegt, nur
  die board-spezifischen Zertifikate sind lazy-generiert).

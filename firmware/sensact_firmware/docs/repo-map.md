# Repo-Landschaft

`sensact_firmware` (dieses Verzeichnis, `C:\repos\sensact\firmware\sensact_firmware`)
ist **kein eigenes Git-Repository**, sondern ein Unterordner des Repos
`C:\repos\sensact` (Remote `klaus-liebler/sensact`, siehe `git rev-parse --show-toplevel`).
Der Build zieht darüber hinaus mehrere unabhängige Repos hinzu. Wer an mehr als
Kleinigkeiten arbeitet, sollte diese Landkarte kennen, bevor er/sie loslegt – ein
`git commit` in `sensact_firmware` landet im `sensact`-Repo, ein `git commit` in
`espidf-component-webmanager` landet dagegen in einem komplett anderen Repo mit
eigenem Remote.

## Repos im Überblick

| Repo (Remote) | Lokaler Pfad | Enthält |
|---|---|---|
| `klaus-liebler/sensact` | `C:\repos\sensact` | Dieses Firmware-Projekt (`firmware/sensact_firmware`, inkl. Hausmodell-Codegenerator als Phase `GenerateModelFiles` im `builder/`), die eigentlichen "sensact apps" (`espidf-components/sensact-applicationmodel`), HAL-Basis (`espidf-components/sensact-hal`), CAN-Middleware (`espidf-components/sensact-middleware`), Hardware-Doku/PCB-Layouts (`doc/`, `pcb/`) |
| `klaus-liebler/espidf-component-webmanager` | `C:\repos\espidf-component-webmanager` | Wifimanager + genereller Web-/OTA-/Login-Server (`cpp/webmanager.hh`), Flatbuffers-Schemas für Wifimanager/Systeminfo/Scheduler/Journal/Fingerprint u.a. |
| `klaus-liebler/npm-packages` | `C:\repos\npm-packages` | Alle `@klaus-liebler/*`-npm-Pakete: Build-Tools (`espidf-vite-secure-build-tools`), Web-UI-Bausteine (`web-components`, `web-components-sensact` – **hier liegt auch das Wifimanager-Frontend**), Test-Server-Helfer (`websocket_file_testserver`), gemeinsame Utilities (`commons`, `sensact-base`, `usersettings_codegeneration`) |
| `klaus-liebler/factory_in_a_box` | `C:\repos\factory_in_a_box\firmware_factory_control_unit` | Referenz-C#-Builder (`builder/`), Vorlage für die Migration in [plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md) |

## Wie die Repos beim Build zusammenspielen

- **`CMakeLists.txt:7`** (`EXTRA_COMPONENT_DIRS`) bindet ESP-IDF-Komponenten aus
  anderen Repos ein: `C:/repos/espidf-components`,
  `C:/repos/sensact/espidf-components`, `C:/repos/espidf-components-lcd`,
  `C:/repos/espidf-component-webmanager`.
- **`web/package.json`** hat `file:`-Abhängigkeiten sowohl auf
  `../../../../npm-packages/@klaus-liebler/...` (Web-UI-Bausteine, inkl.
  Wifimanager-Frontend) als auch auf `../generated/...` (generierte
  best_binary_buffers_schema/Runtimeconfig/Sensact-Apps-TS-Pakete, siehe
  [build-process.md](build-process.md)).
- Generierte Artefakte landen projektlokal unter
  `firmware/sensact_firmware/generated/` (s. `builder/Paths.cs`,
  [build-process.md](build-process.md)), board-spezifische Secrets/Zertifikate
  unter `%USERPROFILE%\OneDrive - HSOS\esp32_boards\<mac-dir>\` und
  `%USERPROFILE%\OneDrive - HSOS\certificates\`.
- Der Hausmodell-Codegenerator (welche Apps auf welchem Node, verdrahtet mit
  welchen Pins, aus C#-Klassen mit IntelliSense-Unterstützung) ist Teil von
  `builder/` selbst (Phase `GenerateModelFiles`), kein separates Tool mehr.

## Praktische Konsequenz für die Arbeit an diesem Plan

- Änderungen an den sensact-Apps (`espidf-components/sensact-applicationmodel`) und
  an der Firmware selbst landen im **selben** Repo (`sensact`) – unproblematisch.
- Änderungen am Wifimanager (Server: `espidf-component-webmanager`, Client:
  `npm-packages/@klaus-liebler/web-components`) und an den Build-Tools
  (`npm-packages/@klaus-liebler/espidf-vite-secure-build-tools`) betreffen **andere
  Repos** – eigene Commits, eigene Historie, ggf. eigene Claude-Code-Session mit
  passendem Arbeitsverzeichnis nötig.
- Der neue C#-Builder (Stufe 2) soll laut Entscheidung **im `sensact`-Repo** leben
  (nicht als eigenständiges Repo), siehe
  [plan_v2/02-builder-migration-csharp.md](plan_v2/02-builder-migration-csharp.md).

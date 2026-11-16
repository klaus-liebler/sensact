# Host-Unit-Tests der Sensact-Apps

Die Apps in `../apps` hängen nur am Interface `iSensactContext`. Dieses eigenständige
CMake-Projekt baut sie deshalb mit einem normalen PC-Compiler und testet sie mit GoogleTest gegen
einen Fake-Kontext – ohne ESP32, ohne ESP-IDF, in ~2 Sekunden.

## Ausführen

Voraussetzungen: CMake ≥ 3.24, Ninja, ein C++20-Compiler (getestet: MSYS2-UCRT64-GCC 16 unter
Windows), .NET-SDK (für die Codegenerierung, s. u.), beim ersten Konfigurieren Internetzugang
(GoogleTest wird per `FetchContent` geladen).

```powershell
cd C:\repos\sensact\espidf-components\sensact-applicationmodel\host_test
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build            # oder direkt: build\sensact_apps_tests.exe
```

Unter Windows aus **PowerShell/cmd** bauen, nicht aus Git-Bash: dort stehen Git-eigene MinGW-DLLs
vor denen von MSYS2 im `PATH`, und der Compiler bricht kommentarlos ab.

Nützlich beim Debuggen: `build\sensact_apps_tests.exe --gtest_filter=BlindTest.*` und
Umgebungsvariable `SENSACT_TEST_LOG=1` (zeigt die `LOGI`/`LOGD`-Ausgaben der Apps).

## Woher die generierten Dateien kommen

Die Apps binden vom Builder erzeugte `.inc`-Dateien ein (Kommando-Handler, `eApplicationID`, …).
Beim Konfigurieren ruft CMake deshalb den Builder auf:

```
dotnet run --project firmware/sensact_firmware/builder -- GenerateModelFilesForHostTests --out build/sensact_model
```

Der Schritt erzeugt die Dateien immer aus dem **Testmodel** (`builder/Models/Testmodel`) und
nur in das Build-Verzeichnis – `generated/` der Firmware und die getrackten Schema-Dateien
bleiben unberührt. Ändern sich Generator oder Testmodel, konfiguriert CMake beim nächsten
Build automatisch neu. Ohne .NET-SDK kann man mit `-DSENSACT_MODEL_DIR=<Pfad>` ein vorab
erzeugtes Verzeichnis angeben.

Die Tests verwenden App-IDs des Testmodells (`ID::BLIND_X1_XX1_42`, …) nur als Namen.

## Aufbau

| Datei | Zweck |
|---|---|
| `support/FakeSensactContext.*` | Fake für `iSensactContext`: Zeit (`now`), Ein-/Ausgänge, Drehgeber, aufgezeichnete Kommandos/Events/Sounds/Parameter. `Send<X>Command` stammt aus dem generierten `sendCommandImplementation.inc`, `Dispatch()` dekodiert mit dem generierten `parseCommand.inc` – Tests prüfen also auch den Wire-Codec |
| `support/TestHelpers.hh` | `RunFor()` (Taktung wie `cApplicationHost`: 20-ms-Tick, Bucket-Rotation, `NeedsFastLoop`), `Tick()`, `Status()`, Payload-Helfer |
| `support/BlindMotorModel.hh` | unabhängiges elektrisches Modell eines Rollladenmotors an zwei Relais; prüft bei jeder Schaltflanke die Sicherheitsinvarianten (kein Kurzschluss, kein Umschalten unter Last, Totzeit beim Reversieren) |
| `stubs/` | Ersatz für die wenigen ESP-IDF-Header, die über Hilfsheader hereinkommen (`esp_log.h`, `common-esp32.hh`) |
| `tests/test_<app>.cc` | eine Datei pro App; `test_blind_interlock.cc` = randomisierter Dauertest über alle Relais-Modi; `test_integration.cc` = Taster/Drehgeber → Codec → Rollladen/Dimmer/Netzteil |

## Neue Tests / neue App

1. App-`.cc` in `CMakeLists.txt` (`add_library(sensact_apps …)`) ergänzen.
2. `tests/test_<app>.cc` anlegen – wird automatisch gefunden.
3. Verhalten so formulieren, wie es sein **soll**; die Kommando-/Status-Tabellen in
   [../README.md](../README.md) sind die Referenz.

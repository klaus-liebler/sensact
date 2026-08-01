# Stufe 5 – Unit-Tests für die sensact-Apps

**Repo-Hinweis:** Zielcode liegt in
`C:\repos\sensact\espidf-components\sensact-applicationmodel\` – das ist **dasselbe
Git-Repo** wie `sensact_firmware` (beide sind Unterordner von `C:\repos\sensact`),
also unproblematisch bzgl. Commits. Liegt aber außerhalb des aktuellen
Session-Arbeitsverzeichnisses (`...\firmware\sensact_firmware`) – für Schreibzugriff
ggf. Pfad-Erlaubnis nötig bzw. neue Session mit weiterem Arbeitsverzeichnis.

## Was sind "sensact apps" konkret

Gefunden in `sensact-applicationmodel/apps/`: `blind`/`blindstimer` (Rollladen),
`onoff` (Relais mit ON/OFF/TOGGLE + Auto-Off-Timer), `pump`, `singlepwm`, `rgbw`,
`sound`, `milightcontroller`, `pushbutton`, `rotaryencoder`, `node`, `fingerprint`.
Jede App erbt von der abstrakten Basis `sensact::apps::cApplication`
(`cApplication.hh`: `Setup(iSensactContext*)`, `Loop(iSensactContext*)`,
`FillStatus(iSensactContext&, array<uint16_t,4>&)`, generierte
Command-Handler-Virtuals). Die Registry `cApplications::Glo2locCmd[]`/`Glo2locEvt[]`
(`cApplications.hh`, `model_applications.cc`) bildet `eApplicationID` auf konkrete
Instanzen ab. `cApplicationHost` (in `sensact_firmware/main/`) treibt
Setup/Loop/Status-Publishing aller Apps an.

## Warum das (relativ) gut testbar ist

Alle App-Methoden nehmen `iSensactContext*`/`iSensactContext&` entgegen – **eine
Abstraktion, keine ESP-IDF-Typen direkt**. Das ist die entscheidende
Testbarkeits-Voraussetzung: Wenn die App-Klassen selbst keine `esp_*`-Header
einbinden, lassen sie sich auf dem Host (Windows/Linux, ganz normaler
Desktop-Compiler) bauen und testen, ganz ohne ESP32-Hardware oder QEMU – wir müssen
nur ein Mock/Fake von `iSensactContext` bereitstellen.

## Vorgehen

- [ ] **Erster Schritt (Voraussetzung prüfen, bevor irgendwas gebaut wird):** alle
      Dateien in `sensact-applicationmodel/apps/*.cc/.hh` sowie `cApplication.hh`,
      `iSensactContext.hh` nach `#include <esp_...>`/`#include <freertos/...>`
      o.ä. durchsuchen. Falls einzelne Apps direkt ESP-IDF-Header einbinden
      (statt nur über `iSensactContext`), diese Stellen als Blocker vermerken – sie
      müssten zuerst entkoppelt werden (Architektur-Fund, ggf. nach
      `04-architektur-review.md` verschieben), bevor der Host-Test für genau diese
      App möglich ist.
- [ ] **Test-Framework-Entscheidung:** GoogleTest + GoogleMock, host-seitig (eigenes
      CMake-Projekt ohne ESP-IDF-Toolchain), nicht ESP-IDFs On-Target-Unity-Tests.
      Begründung: die Apps hängen nur an einem Interface (`iSensactContext`) – ein
      reiner Host-Build mit Mock ist erheblich schneller und einfacher aufzusetzen
      als ESP-IDFs `idf.py --preset linux`/QEMU-Testkomponenten, und es gibt mit
      `C:\repos\cpp-gtest-gmock-example` bereits eine funktionierende Referenz für
      CMake+GoogleTest-Setup, die als Vorlage dienen kann.
      **Bitte bestätigen**, falls eine andere Präferenz besteht (z.B. falls ESP-IDFs
      eigenes Unity-Test-Framework aus anderen Gründen gewünscht ist, etwa um auch
      HAL-nahen Code später mit demselben Framework zu testen).
- [ ] **Test-Projekt-Struktur anlegen** (Vorschlag, zur Bestätigung):
      `sensact-applicationmodel/test/` mit eigenem `CMakeLists.txt`
      (Host-Compiler, FetchContent für GoogleTest), das die App-`.cc`-Dateien direkt
      mitkompiliert (nicht die ESP-IDF-Komponente als Ganzes).
- [ ] **Einmalig:** `MockSensactContext : public iSensactContext` mit GoogleMock
      (`MOCK_METHOD`) für alle Interface-Methoden (`Now`, `SetU16Output`,
      `GetU16Input`, `SetRGBLed`, `GetRotaryEncoderValue`, `PlayMP3`, …) schreiben –
      wird von allen folgenden App-Tests wiederverwendet.
- [ ] **Pilot-App:** `onoff.cc` zuerst (kleinste, klarste App: ON/OFF/TOGGLE-Kommandos
      + Auto-Off-Timer). Damit das CMake/GoogleTest-Setup End-to-End einmal
      durchgespielt und als Muster für die restlichen Apps etabliert ist, bevor der
      Rollout beginnt.
- [ ] **Rollout auf die übrigen Apps**, je eigener Schritt/PR:
      `blind`/`blindstimer`, `pump`, `singlepwm`, `rgbw`, `sound`,
      `milightcontroller`, `pushbutton`, `rotaryencoder`, `node`, `fingerprint`.
      Reihenfolge nach Komplexität aufsteigend (nach `onoff` z.B. `pump`/`singlepwm`
      vor `blind`/`blindstimer`, die zustandsbehafteter sind – Timer-Logik).
- [ ] Je Test-Suite mindestens abdecken: Kommando-Handler (alle definierten
      Commands), `FillStatus`-Ausgabe für die wichtigsten Zustände, Timer-/
      Zeit-abhängiges Verhalten (über die gemockte `Now()` steuerbar), Edge Cases
      (z.B. ungültige Werte/Range-Grenzen, falls vorhanden).
- [ ] Optional, später: in `docs/build-process.md` (Stufe 1) dokumentieren, wie die
      Tests lokal ausgeführt werden (`ctest`/direkter Testbinary-Aufruf). CI ist
      aktuell nirgends im Projekt vorhanden – ob/wie das nachgerüstet wird, ist explizit
      **nicht Teil dieser Stufe**, nur als spätere Möglichkeit vermerkt.

## Entschieden

- Framework: **GoogleTest/GoogleMock**, host-seitig.
- Test-Projekt: **eigenständiges CMake-Projekt**, unabhängig von ESP-IDF buildbar.
- **Timing (wichtige Korrektur ggü. der ursprünglichen Annahme in
  [00-overview.md](00-overview.md)): Unit-Tests werden ganz zum Schluss umgesetzt,
  nicht parallel zu den anderen Stufen.** Diese Stufe ist also die letzte in der
  Praxis, nicht (wie ursprünglich vorgeschlagen) früh parallelisierbar. Wichtig
  während *aller* anderen Stufen: bei jedem Refactoring immer mitdenken, dass der
  entstehende Code hinterher gut testbar sein muss (z.B. weiterhin sauber gegen
  `iSensactContext` statt gegen ESP-IDF-Header programmieren) – Testbarkeit ist ein
  Kriterium für *jede* Umbau-Entscheidung, auch wenn die Tests selbst erst am Ende
  geschrieben werden. Siehe auch der entsprechende Hinweis in
  [04-architektur-review.md](04-architektur-review.md).

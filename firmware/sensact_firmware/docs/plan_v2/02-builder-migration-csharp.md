# Stufe 2 – Builder von gulp/TypeScript auf C# umstellen

Referenzprojekt: `C:\repos\factory_in_a_box\firmware_factory_control_unit\builder`
(fertiges C#-Konsolenprogramm für einen strukturell sehr ähnlichen Zweck – ESP-IDF-artiger
Build + Web-Build + Flash, dort für STM32). Die Architektur dieses Referenzprojekts wird
so weit wie sinnvoll übernommen.

## Architektur des Referenzprojekts (Vorlage)

- **Kein Framework, kein DI-Container.** `Program.cs` ist ein Top-Level-Statement-
  Programm: `argv[0]` = Phasenname, danach `--preset`/`--board`-Optionen. Ein simples
  `switch` ruft `Phases.<Phase>.Run(...)` auf. Zusätzlich 3 "Pipeline"-Verbund-Befehle,
  die einfach mehrere Phasen nacheinander in-process aufrufen (keine eigene
  Orchestrierungs-Abstraktion).
- **Phasen** sind `public static class` mit einer `Run(...)`-Methode, ohne gemeinsames
  Interface. Reihenfolge/Abhängigkeiten sind nur in Kommentaren dokumentiert und werden
  dadurch erzwungen, dass jede Phase wirft, wenn eine Voraussetzung (Datei/Verzeichnis)
  fehlt.
- **State fließt über das Dateisystem**, nicht über Rückgabewerte: ein
  "Board-Archiv"-Verzeichnis (`BoardArchive.cs`) pro Board-ID sammelt alle
  generierten Artefakte (Zertifikate, Keys, generierte Header/`.inc`/`.ts`-Dateien).
  `BoardContext.cs` merkt sich nur die zuletzt benutzte Board-ID
  (`build/.last-board-id`).
- Ein optionales SQLite (`BuilderDb.cs`) merkt sich Board-Typ pro Chip-UID, um
  Verwechslungen beim Flashen zu vermeiden. **Geklärt (siehe "Entschieden"): wird
  NICHT übernommen** – die ESP32-Seite hatte historisch genau so ein SQLite-basiertes
  Board-Tracking (`builder.db`, siehe unten) und ist bewusst davon auf das heutige
  dateibasierte Konzept (`board_info.json` pro Board-Verzeichnis) umgestiegen.
- **`ProcessRunner.cs`**: einzige Stelle, die Subprozesse startet, mit drei Varianten
  (`Run` → stdout als String, `RunBinary` → stdout als Bytes, `RunInherit` → Kind erbt
  Konsole für Live-Ausgabe). Wirft `ProcessException` mit stdout/stderr/exitcode bei
  Fehlern.
- **`Paths.cs`**: findet das Projekt-Root robust, indem es von `AppContext.BaseDirectory`
  nach oben läuft, bis es `CMakeLists.txt` + `builder/`-Sibling findet.
- **Konfiguration**: `appsettings.json` (gitignored, maschinenspezifisch) +
  `appsettings.json.template` (getrackt). `BuilderOptions.cs` bindet das per
  `Microsoft.Extensions.Configuration`, als `Lazy<T>`-Singleton. Ein MSBuild-Target
  kopiert das Template automatisch, falls `appsettings.json` fehlt.
- **TypeScript-Dateien im C#-Projekt sind kein Versehen**: `BuildWebApp.cs` ruft
  Node/Vite direkt auf (`node node_modules/vite/bin/vite.js build ...`), und
  `vite-plugin-single-file-firmware-asset.ts` ist ein echtes Vite-Plugin, das vom
  `web/vite.config.ts` importiert wird. Vite hat keine C#-API – dieser Teil bleibt
  zwangsläufig TypeScript/Node, das C#-Programm ruft es nur als Subprozess auf.
- **Keine Tests vorhanden** im Referenzprojekt (kein Testprojekt, keine `.sln`).
  Für unseren Port sollten wir das nicht unreflektiert kopieren, siehe unten.

## Mapping: aktuelle Gulp-Tasks → geplante C#-Phasen

| Gulp-Task (`gulpfile.ts`) | Neue C#-Phase | Bemerkung |
|---|---|---|
| `info` | `Info` | Liest/zeigt Board-Status |
| `addOrUpdateConnectedBoard` | `ReadHardwareIds` (Name in Anlehnung an Referenz) | **Wichtigster Unterschied zur Vorlage**: aktuell liest `esp32.ts` MAC/Efuse-Infos über ein selbstgebautes SLIP/ROM-Loader-Protokoll direkt über die serielle Schnittstelle (kein `esptool` o.ä.). Für C# empfehlenswert: **nicht neu implementieren**, sondern `esptool.py chip_id`/`read_mac`/`--after no_reset` via `ProcessRunner` aufrufen (analog zum Referenzprojekt, das auch alles über CLI-Tools abwickelt) – spart eine komplette Low-Level-Protokoll-Portierung. Muss geprüft werden, ob alle heute genutzten Felder (Chip-Typ, Efuse-Key-Purpose-Register für Flash-Encryption-Status) darüber erreichbar sind. |
| `createFiles` | `GenerateFiles` (mehrteilig, s.u.) | Größter Brocken |
| `buildAndCompressWebProject` | `BuildWebApp` | Direktes Vorbild in der Referenz vorhanden – Muster übernehmen |
| `buildFirmware` | `BuildFirmware` | `idf.py build` statt `cmake --preset`/`cmake --build`, sonst analog |
| `encryptFirmwareIfNecessary` + `flashFirmware` | `FlashFirmware` | `espsecure.py`/`espefuse.py`/`esptool.py` via `ProcessRunner`, analog zu `STM32_Programmer_CLI`-Aufrufen in der Vorlage |
| (implizit: Vollkette) | `pipeline:*`-Verbundbefehle | Wie in der Vorlage: einfache Sequenzen der obigen Phasen |

`GenerateFiles` zerfällt inhaltlich in mehrere Unterschritte, die je einzeln portiert
werden können (jeder Schritt = 1 Datei/Modul in `espidf-vite-secure-build-tools`, siehe
Referenz-Quellcode dort):

1. **Sensact-Codegenerierung** (`sensact_code_generator.ts` → `SensactCodeGenerator.cs`):
   Template-Expansion (aktuell simpler String-Replace `"//TEMPLATE_HERE"`) für
   `applicationIds.fbs`, `commandTypes.fbs`, generierte TS-Pakete
   `sensact_sendCommandImplementation`/`sensact_appsbuilder`. Die TS-Paket-Erzeugung
   bleibt zwangsläufig "Node-artig" (schreibt `package.json` + `npm install`), das
   C#-Programm kann das per `ProcessRunner` (npm) erledigen, muss aber die
   Template-Dateien selbst rendern.
2. **~~Flatbuffers~~ → WsProtocol-Codegenerierung** (`flatbuffers.ts`/`flatc` entfällt
   komplett, siehe eigener Abschnitt unten "Flatbuffers wird durch ws-protocol
   ersetzt"): kein externer Tool-Aufruf mehr, sondern eine in-process C#-Phase
   (`GenerateWsProtocolFiles`), portiert aus dem Referenzprojekt
   (`builder/Phases/ReadWebSocketProtocol.cs`).
3. **Zertifikate** (`certificates.ts`, nutzt `node-forge`): in C# gibt es mit
   `System.Security.Cryptography.X509Certificates` eine vollwertige native
   Alternative – **kein Shell-out zu openssl nötig** (die Referenzvorlage nutzt zwar
   `openssl` per Prozessaufruf, aber .NET kann X.509-Zertifikate + Signierung nativ,
   was robuster/portabler ist als ein weiterer externer Tool-Aufruf). Empfehlung:
   nativ in C# nachbauen statt openssl zu shellen.
4. **Config-Dateien** (`key_value_file_helper.ts` → `RuntimeConfigWriter.cs`): reines
   Text-Rendering (CMake-JSON, C++-Header, TS-Modul) aus einem
   `Dictionary<string, object>` – trivial zu portieren, kein Analogon in der Vorlage
   nötig (aber `ReadModbusRegisterMap.cs`/`ReadGitStatus.cs` in der Vorlage zeigen
   das gleiche Muster: "lies Quelle X, rendere Header/TS/JSON").
5. **Board-Kontext** (`context.ts`, `Context`/`ContextConfig`) →
   `BoardContext.cs`/`BoardArchive.cs`-Äquivalent: board-spezifisches Verzeichnis unter
   `OneDrive - HSOS\esp32_boards\<mac-dir>\` lesen/schreiben (`board_info.json`,
   `sensact.json`, Zertifikate, Flash-Encryption-Key). Sehr nah am
   `BoardArchive.cs`-Konzept der Vorlage, nur mit anderer Verzeichnisbenennung
   (`<mac-dir>` statt `<boardId>`).
6. **Git-Info** (`git.ts` → analog zu `GitInfo.cs`/`ReadGitStatus.cs` – **existiert in
   der Vorlage bereits 1:1 im passenden Zuschnitt**, kann fast unverändert übernommen
   werden).
7. **ASCII-Art-Banner** (`ascii_art.ts`): kleine Fleißarbeit, es gibt .NET-Pakete für
   Figlet-Fonts, oder man rendert den Banner-Text weiterhin einmalig per Node-Skript
   (nicht kritisch, niedrige Priorität).

## Testbarkeit des neuen Tools

Die Vorlage hat **keine Tests**. Empfehlung: diesen Mangel *nicht* übernehmen, sondern
für den C#-Builder von Anfang an die Teile testen, die keine Hardware/Netzwerk
brauchen:
- `RuntimeConfigWriter`/Template-Rendering (reine String-/JSON-Transformationen)
- `Paths`-Auflösung (Root-Suche)
- `BoardContext`/`BoardArchive`-Pfadlogik
- Zertifikatsgenerierung (Struktur/Gültigkeit prüfen, nicht den kryptografischen Inhalt)

Test-Framework: xUnit oder NUnit (Standard-.NET-Wahl, unabhängig von Stufe 5, die C++
betrifft).

## Bestätigt: SQLite war für ESP32-Boards eine Interims-Lösung, JSON-Dateien sind der Zielzustand

Klaus erinnerte sich, dass die ESP32-Board-Verwaltung früher auch mal SQLite-basiert
war, und bat um Verifikation. Bestätigt per Git-Historie in `C:\repos\npm-packages`:
- Commit `d832ff4` ("Initial commit") führte `@klaus-liebler/espidf-vite/database.ts`
  ein (SQLite-basierte Board-Verwaltung).
- Commit `1f9ad35` ("Generate everything in special 'GENERATED'-Directory",
  2025-01-26) **löschte `database.ts` (39 Zeilen) wieder** und baute die
  Board-Verwaltung stattdessen in `context.ts` auf Basis von JSON-Dateien
  (`board_info.json` pro Board-Verzeichnis) um – exakt das Konzept, das heute in
  `context.ts`/`gulpfile.ts` verwendet wird.
- Passender Beleg: unter `%USERPROFILE%\OneDrive - HSOS\esp32_boards\builder.db`
  liegt noch eine verwaiste SQLite-Datei (zuletzt geschrieben 2025-01-21, also
  unmittelbar vor der Umstellung) – ein stehengelassenes Artefakt der alten Lösung,
  seither nicht mehr angefasst. Kann bei Gelegenheit gelöscht werden (kein Code
  referenziert sie mehr), aber unkritisch, da außerhalb jedes Repos.
- **Konsequenz für den neuen ESP32-C#-Builder**: JSON-Dateien sind schon länger der
  bewusste Zielzustand, nicht SQLite (siehe oben, "Architektur des Referenzprojekts").

### Neue Aufgabe: STM32-Builder (`factory_in_a_box`) auf dasselbe Konzept umstellen

Klaus möchte, dass der STM32-Builder (`firmware_factory_control_unit/builder`, das
Referenzprojekt dieser Migration) konsistent zum – schon vor über einem Jahr
getroffenen – ESP32-Entschluss ebenfalls von SQLite (`BuilderDb.cs`, `boards.db`)
auf dateibasierte JSON-Verwaltung umgestellt wird. Das ist eine **separate Aufgabe
in einem dritten Repo** (`C:\repos\factory_in_a_box`, eigenes Remote), nicht Teil
des eigentlichen ESP32-Builder-Ports, aber inhaltlich direkt dadurch motiviert.

- [ ] `BuilderDb.cs` (`mcu_types`/`board_types`/`boards`/`flash_events`-Schema) durch
      ein dateibasiertes Äquivalent ersetzen, analog zum ESP32-Konzept: pro erkannter
      Chip-UID ein JSON-Datensatz (Board-Typ-Zuordnung), plus ein Append-Log für
      Flash-Events (z.B. eine Zeile pro Ereignis in einer JSONL-Datei statt einer
      SQL-Tabelle), abgelegt unter dem bestehenden `BoardsDir`
      (`BoardArchive.cs`/`appsettings.json`).
  - Betroffene Call-Sites: `TryGetBoardTypeNameForChipUid` (`BuilderDb.cs:67-78`,
    genutzt von `ReadHardwareIds`) und `RecordSuccessfulFlash`
    (`BuilderDb.cs:128-173`, genutzt von `FlashFirmware`) müssen auf die neue
    dateibasierte Quelle umgestellt werden.
  - Danach `BuilderDb.cs` und die SQLite-Abhängigkeit (falls als NuGet-Paket
    eingebunden) entfernen, `boards.db` aus `.gitignore`/Doku austragen.
- [ ] Eigener Commit in `factory_in_a_box`, unabhängig vom Zeitplan dieser Stufe hier
      – kann parallel oder danach passieren, ist keine Voraussetzung für den
      ESP32-C#-Port.
- **Nicht Teil der jetzigen Sitzung** (Fokus liegt auf Start von Stufe 1) – als
  eigenständiger, bereits entschiedener Task hier vorgemerkt, zur Umsetzung sobald
  an Stufe 2 bzw. an `factory_in_a_box` gearbeitet wird.

## `configware` (Hausmodell-Generator) wird voll in den Orchestrator verschmolzen

Wichtiger Nachtrag (2026-07-31): Es gibt eine bisher in diesem Plan nicht erfasste,
aber fundamentale Codegenerierungs-Stufe, die **vor** dem bisherigen
`createFiles`-Schritt liegt: `C:\repos\sensact\configware\` – dasselbe Repo wie
`sensact_firmware` (siehe [repo-map.md](../repo-map.md)), aber bisher völlig
unabhängig vom Gulp-Build.

**Ist-Zustand**: `common/` (geteilte Engine) + je ein eigenständiges
Konsolenprojekt pro Hausmodell (`configware_sattlerstrasse`, `configware_testmodel`),
jedes mit eigener Kopie einer `ApplicationId`-Enum. `common/Generator.cs`
(`SourceCodeGenerator<ApplicationIdType>`) nutzt Reflection über
`[SensactCommandMethod]`-attributierte Methoden der Basisklasse `SensactApplication`,
um rund 20 Artefakte zu erzeugen: C++/Flatbuffers-Enums (ApplicationIds,
CommandTypes, EventTypes), virtuelle/override-Deklarationen, TS-Sende-Funktionen,
pro-Node-`.inc`-Dateien (`glo2LocCmd.inc`, `applicationInitializers.inc`,
`nodeDescription.inc`, `nodeMasterApplicationId.inc`, `sensactapps_local.ts`),
JSON-Deskriptoren (`node_descriptor.json`) und die TS-UI-Builder-Datei
(`sensactapps.ts`). Output-Pfad laut `appsettings.json`:
`C:/repos/generated/sensact_model` – **identisch** mit
`SENSACT_COMPONENT_GENERATED_PATH` aus `gulpfile.ts`.

**Befund**: Die meisten `.inc`-Dateien, die `sensact-applicationmodel` (die
eigentlichen C++-Apps) per `#include` einbindet, stammen tatsächlich aus
`configware`, nicht aus der TS-Pipeline. `sensact_code_generator.ts` faltet nur
zwei der hier bereits erzeugten Snippets (`applicationIds.fbs`, `commandTypes.fbs`)
in Templates und liest `node_descriptor.json`. `configware` läuft bisher komplett
manuell (`dotnet run` in Visual Studio, `Program.cs` endet blockierend mit
`Console.ReadKey()`), ist nirgends in Gulp verdrahtet und war bisher auch nicht in
`docs/build-process.md` erfasst – eine reale Fehlerquelle ("Hausmodell geändert,
aber `configware` vergessen neu laufen zu lassen").

### Entschieden: volle Verschmelzung, kein separates Tool

`configware` wird kein eigenständiges Tool bleiben, sondern **vollständig Teil des
neuen C#-Build-Orchestrators** – als weitere, obligatorische Phase
(`GenerateModelFiles`) vor der bisherigen `GenerateFiles`-Phase.

**Geklärte Generics-Frage**: `ModelBuilder<ApplicationIdType>`/
`SourceCodeGenerator<ApplicationIdType>` sind bereits vollständig generisch – zwei
geschlossene generische Typen wie `ModelBuilder<Sattlerstrasse16.ApplicationId>` und
`ModelBuilder<Testmodel.ApplicationId>` können ohne Weiteres im selben
Assembly/Projekt koexistieren. Der einzige Grund für die bisherigen getrennten
Projekte war eine Namensraum-Kollision (beide definierten einen Typ
`Klli.Sensact.Model.ApplicationId`), kein Generics-/Type-System-Problem. Die
Laufzeitebene (`SensactApplication`, `Node`, `ModelContainer`, `Generator`) arbeitet
ohnehin durchgehend mit `ushort`, nicht mit dem Enum-Typ selbst – die
Enum-Typisierung existiert ausschließlich für die Autorenzeit (IntelliSense beim
Verdrahten in `Sattlerstrasse16Builder`/`TestmodelBuilder`).

**Zielstruktur** (innerhalb des neuen Orchestrator-Projekts):

```
Models/
  Sattlerstrasse16/
    ApplicationId.cs        – namespace ...Model.Sattlerstrasse16
    Sattlerstrasse16Builder.cs
  Testmodel/
    ApplicationId.cs        – namespace ...Model.Testmodel
    TestmodelBuilder.cs
ModelGeneration/             – ehemals configware/common: Generator, ModelBuilder<T>,
                                Node, SensactApplication, ...
```

- Auswahl, welches Modell generiert wird, per CLI-Parameter/Konfiguration (analog zu
  `--board`), z.B. `--model Sattlerstrasse16`. Welches Modell zu welchem physischen
  Board gehört, wird als neues Feld (`"model"`) in `board_info.json`/`sensact.json`
  hinterlegt – analog dazu, wie heute schon `NODE_ID` bestimmt, welches
  `main/hal/<NODE_ID>/` aktiv ist.
- `Program.cs` von `configware_*` wird aufgelöst: die eigentliche
  Generierungslogik (aktuell in `Main()` verwoben mit DI-Container-Aufbau und NLog)
  wird zu einer sauberen, aufrufbaren Methode, die die neue Phase
  `GenerateModelFiles` direkt in-process aufruft – kein Subprozess, keine zweite
  `.exe`.
- **Wird beim Merge mit erledigt**: das blockierende `Console.ReadKey()` am Ende von
  `Program.cs` entfernen (notwendige Voraussetzung für jede Automatisierung).
- `GenerateModelFiles` läuft **vor** `GenerateFiles`. Die bisherige "Sensact-
  Codegenerierung" (Unterschritt 1 von `GenerateFiles`, siehe Mapping-Tabelle oben)
  entfällt dadurch in ihrer heutigen Form (Template-Expansion aus vorgefertigten
  `.inc`-Schnipseln), weil die `.inc`-Dateien jetzt direkt und vollständig durch
  `GenerateModelFiles` entstehen, statt nur teilweise per Template zusammengefügt zu
  werden.

- [ ] `configware/common` (Engine) und die Modell-Definitionen
      (`configware_sattlerstrasse`, `configware_testmodel`) in die Zielstruktur
      oben überführen (Namensraum-Umbau `ApplicationId` pro Modell).
- [ ] `Console.ReadKey()`/interaktiven Konsolen-Flow aus der Generierungslogik
      entfernen, saubere aufrufbare Methode extrahieren.
- [ ] Modell-Auswahl-Mechanismus einbauen: **CLI-Flag** (`--model Sattlerstrasse16`),
      s. "Entschieden (Ergänzung)" unten.
- [ ] `GenerateModelFiles`-Phase vor `GenerateFiles` einhängen, Unterschritt 1 von
      `GenerateFiles` (Template-Expansion) entsprechend reduzieren/anpassen.
- [ ] `docs/build-process.md` und `docs/repo-map.md` (Stufe 1) um `configware`
      ergänzen (bisher dort nicht erfasst).
- [ ] `configware/` als eigener Unterordner in den neuen Orchestrator überführen
      (s. "Entschieden (Ergänzung)" unten) – kein Neuschrieb, sondern Verschieben +
      Namensraum-Umbau.

### Entschieden (Ergänzung, 2026-08-01)

- **Modell-Auswahl: CLI-Flag** (`--model Sattlerstrasse16`/`--model Testmodel`),
  keine Ableitung aus `board_info.json`/`board_settings`.
- **`configware` wird als eigener Unterordner** in den neuen Orchestrator überführt
  (kein reiner Neuschrieb) – d.h. `configware/common` → `ModelGeneration/`,
  `configware_sattlerstrasse`/`configware_testmodel` → `Models/Sattlerstrasse16/`
  bzw. `Models/Testmodel/` (s. Zielstruktur oben), Dateien werden verschoben und im
  Namensraum angepasst, nicht von Grund auf neu geschrieben.

## Flatbuffers wird durch das neue ws-protocol-Verfahren ersetzt

Nachtrag (2026-07-31): Klaus hatte eine parallele Änderung im Referenzprojekt
(`factory_in_a_box`) entdeckt: dort wurde Flatbuffers durch ein eigenes, viel
schlankeres binäres Nachrichtenprotokoll (`best_binary_buffers_schema/*.json` →
`builder/Phases/ReadWebSocketProtocol.cs` → generiertes `ws_protocol.hh`/
`ws-protocol.ts`) ersetzt. Auf Wunsch wurde zunächst geprüft, ob dieses Verfahren
sensacts tatsächliche Flatbuffers-Nutzung (12 Namespaces, sowohl in
`espidf-component-webmanager` als auch in `sensact_firmware` selbst) abdecken kann
– und dann der Generator direkt im Referenzprojekt entsprechend erweitert (siehe
unten). Das Ergebnis: **ja**, und der Ersatz wird jetzt Teil dieser Migration statt
weiterhin `flatc` als externe Abhängigkeit mitzuschleppen.

### Ursprüngliche Lücken und wie sie geschlossen wurden

Eine erste Prüfung von `ReadWebSocketProtocol.cs` gegen alle 12 sensact-Namespaces
(`ns01wifimanager` … `ns10journal`, `ns20sensact`, `ns21nodemaster`) ergab vier
Lücken gegenüber Flatbuffers. Alle vier wurden im Zuge dieser Planung im
Referenzprojekt-Generator nachgerüstet und mit Compile-/Typecheck-/Roundtrip-Tests
(ARM-g++, `tsc --strict`, Node-Ausführung) verifiziert:

| Lücke (Flatbuffers-Konstrukt) | Betroffene sensact-Schemas | Lösung im Generator |
|---|---|---|
| Verschachtelte Strings in Array-Elementen (`[AccessPoint]`, `[Finger]`, `[PartitionInfo]`, `[JournalItem]`, `[Range]`) | wifimanager, fingerprint, systeminfo, journal, nodemaster | Neuer Typ **Class** (`"classes"` in der Schema-Datei) – wie eine Message, darf Strings enthalten, wird per `{"type":"array","classes":["ns.Name"]}` referenziert |
| Feste Byte-/Wort-Arrays als `struct` (`Mac6`, `CanData`, `Payload`, `StatusPayload`, `Uint8x32`, `OneWeekIn15MinutesData`) | systeminfo, canmonitor, sensact, fingerprint, scheduler | Neuer Typ **Struct** (`"structs"`), referenziert per `{"type":"struct","structRef":"Name"}`; einzelne Bytes/Wörter per `"count":N` statt N einzelner Felder |
| Union als Feldtyp, nicht nur als Array-Element (`scheduler.Schedule.schedule:uSchedule`) | scheduler | Neuer Feldtyp `{"type":"class","classes":[...]}` – ein einzelnes polymorphes Feld (wie ein Array-Element, aber ohne Liste/Zähler), muss letztes Feld seines Objekts sein |
| Geteilte Enums über mehrere Nachrichten hinweg (`ApplicationId`/`CommandType`, von `configware` erzeugt) | sensact (`RequestCommand.id`, `RequestStatus.ids`, `NotifyStatus.id`, `ResponseStatusItem.id`) | Neue `"enums"`-Deklaration + `{"type":"enumRef","enumRef":"ns.Name"}`, plus: Namespaces sind jetzt **nicht mehr an Dateien gebunden**, sondern ergeben sich aus dem Punkt im `"name"` jeder Deklaration (`"sensact.ApplicationId"`) – dadurch kann `configware` sein Enum unabhängig von allen anderen Schema-Dateien beisteuern |

Details/Wire-Format aller vier Erweiterungen: `docs/websocket-protocol.md` (im
Referenzprojekt) Abschnitt 8. Diese Doku ist die eigentliche Spezifikation, hier nur
die sensact-spezifische Einordnung.

**Zwei weitere Lücken, erst beim tatsächlichen Portieren der restlichen 9
`espidf-component-webmanager`-Namespaces gefunden (nicht bei der ursprünglichen
Prüfung anhand der `.fbs`-Dateien allein sichtbar) – ebenfalls im
Referenzprojekt-Generator nachgerüstet und verifiziert:**

| Lücke | Gefunden bei | Lösung im Generator |
|---|---|---|
| Kein Fließkommatyp (`float`/`double`) | `ns04heaterexperiment` (8 float-Felder) | Neuer Feldtyp `float32` – bitweise Übertragung per `memcpy` (C++) bzw. `DataView.getFloat32`/`setFloat32` (TS), NICHT per Integer-Bitshift (das würde den Wert konvertieren statt das Bitmuster zu übertragen) |
| Kein 64-Bit-Ganzzahltyp | `ns02systeminfo` (`seconds_epoch`/`seconds_uptime:int64`), `ns10journal` (`last_message_timestamp:uint64`) | Neue Feldtypen `uint64`/`int64` – C++ wie gehabt (mit `uint64_t`-Akkumulator statt `uint32_t`, um Trunkierung/UB zu vermeiden), TypeScript bildet auf `number` ab (kein `DataView`-Zugriff für 64-Bit-`number` vorhanden) und zerlegt/komponiert manuell aus zwei 32-Bit-Hälften – **exakt nur für nicht-negative Werte bis 2^53**, für sensacts Zeitstempel-Anwendungsfälle ausreichend |
| Nur EIN abschließendes `classes`-Array/-Einzelfeld pro Message/Class erlaubt | `ns07fingerprint.ResponseFingers` (braucht ZWEI: `scheduleNames` und `fingers`, je ein eigenes `classes`-Array) | Validierung gelockert: statt "muss das letzte Feld sein" jetzt "muss Teil eines zusammenhängenden Blocks solcher Felder am Ende sein" – mehrere `classes`-Arrays/-Einzelfelder hintereinander sind erlaubt, da Encode/Decode ohnehin sequenziell durch alle Felder laufen und jedes Element selbstbeschreibend ist (kein technischer Grund für die Beschränkung auf genau eines) |

Details: `docs/websocket-protocol.md` Abschnitt 5 (float32/int64/uint64) und
Abschnitt 8.2/8.2b (trailing-Block-Regel).

### Wire-Format-Unterschied zu Flatbuffers: Dispatch statt Union-Wrapper

Sensacts Flatbuffers-Schemas nutzen durchgängig ein `RequestWrapper{request:Requests}`/
`ResponseWrapper{response:Responses}`-Muster (ein Flatbuffers-`union` aus allen
Request-/Response-Typen eines Namespace). Im neuen Protokoll entfällt dieser Wrapper
ersatzlos – der 4-Byte-Kopf jeder Nachricht (`namespaceId`+`messageTypeId`) übernimmt
die Dispatch-Funktion direkt: der Websocket-Handler auf Firmware-Seite
(`handle_webmanager_ws` in `espidf-component-webmanager/cpp/webmanager.hh`, s.
[03-wifimanager-review.md](03-wifimanager-review.md)) und der Client-Dispatcher
(`AppController` in `web-components`) müssen von "Flatbuffers-Root parsen, dann
`response_type()` switchen" auf "erste 4 Bytes lesen, dann auf
`(namespaceId, messageTypeId)` switchen" umgestellt werden – siehe
`web/src/ws-client.ts`-Beispiel in `docs/websocket-protocol.md` Abschnitt 7 im
Referenzprojekt als Vorlage für das Dispatch-Muster.

### Mapping: sensact-Namespace → benötigte ws-protocol-Konstrukte

| Namespace (`.fbs`-Datei, Repo) | Neue Konstrukte |
|---|---|
| `ns01wifimanager` (`espidf-component-webmanager`) | Class `AccessPoint` (Liste in `ResponseNetworkInformation`) – **erledigt** |
| `ns02systeminfo` (`espidf-component-webmanager`) | Struct `Mac6` (5×), Class `PartitionInfo` (Liste), erster Test von `int64`/`uint64` (`secondsEpoch`/`secondsUptime`) und `float32` (`chipTemperature`) – **erledigt**, fand die int64/uint64-Lücke |
| `ns03functionblock` (`espidf-component-webmanager`) | Bare-Skalar-Array-Workaround (`BoolValue`/`IntValue`/`FloatValue`/`ColorValue`, je ein `value`-Feld) für `bools`/`integers`/`floats`/`colors` in `ResponseDebugData` – **erledigt** |
| `ns04heaterexperiment` (`espidf-component-webmanager`) | Erster Test von `float32` (8 Felder in Request/ResponseHeater), geteiltes Enum `Mode` – **erledigt**, fand die float32-Lücke |
| `ns05chatbot` (`espidf-component-webmanager`) | nur Strings – keine neuen Konstrukte nötig – **erledigt** |
| `ns06canmonitor` (`espidf-component-webmanager`) | Struct `CanData` (8 Bytes, `count`) – **erledigt** |
| `ns07fingerprint` (`espidf-component-webmanager`) | Struct `Uint8x32` (32 Bytes, `count`), Class `Finger` (Liste), Class `StringValue` (Einzelfeld-Wrapper) für `schedule_names:[string]`; `ResponseFingers` braucht ZWEI aufeinanderfolgende `classes`-Arrays (`scheduleNames`+`fingers`) – **erledigt**, fand die "mehrere trailing classes-Felder"-Lücke (s. oben) |
| `ns08scheduler` (`espidf-component-webmanager`) | Struct `OneWeekIn15MinutesData` (84 Bytes, `count`); `Schedule` selbst wird zur **Class** mit `name:string` + einem **trailing einzelnen polymorphen Feld** (`type:"class"`, Predefined/OneWeekIn15Minutes/SunRandom); `ResponseSchedulerOpen`/`RequestSchedulerSave` betten `Schedule` dann ihrerseits als **einzelnes einelementiges polymorphes Feld** (`{"classes":["scheduler.Schedule"]}`) ein – s. ausgearbeitetes Beispiel im "Vorgehen" – **erledigt** |
| `ns09usersettings` (`espidf-component-webmanager`) | `SettingWrapper{setting_key,setting:Setting}` wird zu 4 eigenständigen Classes (`StringSettingWrapper`, `IntegerSettingWrapper`, `BooleanSettingWrapper`, `EnumSettingWrapper`, je mit `setting_key`+Wert) – heterogene Liste dieser 4 Classes ersetzt die verschachtelte Union sauberer als das Original – **erledigt** |
| `ns10journal` (`espidf-component-webmanager`) | Class `JournalItem` (Liste), erstmals `uint64` INNERHALB einer Class (`lastMessageTimestamp`, nicht nur direkt in einer Message) – **erledigt** |
| `ns20sensact` (`sensact_firmware`) | Struct `Payload`/`StatusPayload` (`count`), **geteiltes Enum** `sensact.ApplicationId`/`sensact.Command` (`enumRef`, aus `configware` gespeist statt aus Flatbuffers-`.fbs`-Text; Enum heißt `Command`, nicht `CommandType`, s. Status unten), Bare-Enum-Array-Workaround für `ids:[ApplicationId]` – **erledigt** |
| `ns21nodemaster` (`sensact_firmware`) | Class `Range` (Liste, wegen `name:string`) – **erledigt** |

### Betroffene Repos – dies ist eine Migration über DREI Repos hinweg

- **`sensact`** (dieses Repo, umfasst sowohl `firmware/sensact_firmware/` als auch
  `configware/` – s. [repo-map.md](../repo-map.md)):
  - `ns20sensact`/`ns21nodemaster` → neue `best_binary_buffers_schema/*.json`-Dateien; `main/`
    (Nutzung der generierten Typen in C++).
  - `configware` muss statt `.fbs`-Text für `ApplicationId`/`CommandType`
    (`GenerateApplicationIds`/`GenerateCommandTypes` in `Generator.cs`) eine
    ws-protocol-`"enums"`-Deklaration erzeugen – fällt zeitlich mit der
    `configware`-Verschmelzung (s. oben) zusammen, sollte im selben Zug erledigt
    werden.
- **`espidf-component-webmanager`** (eigenes Repo): `ns01`–`ns10` → neue
  `best_binary_buffers_schema/*.json`-Dateien; `webmanager.hh` (Dispatch-Umstellung, s.o.).
- **`npm-packages`** (eigenes Repo): `web-components`s `AppController`/
  `WifimanagerController` u.a. Screen-Controller müssen von der generierten
  `flatbuffers`-JS-API auf die neue generierte `ws-protocol.ts`-API umgestellt
  werden (Dispatch, Encode/Decode-Aufrufe) – das ist der aufwändigste Einzelteil,
  weil hier die meiste UI-Logik hängt (siehe die Screen-Controller, die in der
  Kapazitätsprüfung für Stufe 3 bereits identifiziert wurden).

### Vorgehen

- [ ] Reihenfolge: **einen** Namespace zuerst komplett durchziehen, End-to-End
      inkl. Client-Umstellung, bevor die übrigen 11 im großen Batch folgen.
      **Entschieden (auf Wunsch bewusst mit dem komplexesten Fall zuerst, um
      restliche Lücken so früh wie möglich zu finden): `ns08scheduler`.**
      Begründung: einziger Namespace, der gleich drei Dinge gleichzeitig
      braucht – den größten Struct (`OneWeekIn15MinutesData`, 84 Bytes),
      das neueste/am wenigsten battle-getestete Feature (einzelnes polymorphes
      Feld) UND eine Class, die selbst wieder ein einzelnes polymorphes Feld als
      letztes Feld trägt (`Schedule`), referenziert von zwei anderen Nachrichten
      als einelementige Klassenliste – die bislang am wenigsten in Kombination
      geprüfte Konstruktion. `ns07fingerprint` (meiste Nachrichten-Paare, auch
      eine Class mit `[string]`-Workaround) wäre die zweitkomplexeste Wahl,
      falls `ns08scheduler` unerwartet reibungslos durchläuft und ein zweiter
      harter Testfall gewünscht ist.

      **Status (2026-07-31): Schema geschrieben und verifiziert.** Liegt bereits
      real (nicht nur als Plan-Skizze) unter
      `C:\repos\espidf-component-webmanager\ws-protocol\scheduler.json`. Verifiziert
      mit demselben Verfahren wie die Generator-Erweiterungen selbst: mit dem
      Referenzgenerator (`--ws-protocol-path` auf diese Datei) generiert, das
      Ergebnis mit `arm-none-eabi-g++` kompiliert und mit `tsc --strict` +
      echtem Node-Laufzeit-Roundtrip getestet – alle Fälle (verschachtelte
      Polymorphie `ResponseSchedulerOpen.payload`→`Schedule`→`schedule`, das
      84-Byte-Struct zwei Ebenen tief eingebettet, das leere `Predefined`,
      die homogene Klassenliste `ResponseSchedulerList.items`) bestehen.
      **Dabei einen echten Generator-Bug gefunden und behoben**: `GenerateCppClass`
      erzeugte für eine Class mit einem eigenen abschließenden `ClassField`
      (wie `Schedule.schedule`) nie die noetigen `Append*/Decode*Elements`-
      Hilfsfunktionen – nur `GenerateCppMessage` tat das. Genau der Fall, den die
      "komplexesten Namespace zuerst"-Strategie finden sollte; behoben in
      `ReadWebSocketProtocol.cs` (TS-Seite war bereits korrekt, da dort alles
      inline statt über benannte Hilfsfunktionen läuft).

      **Noch offen für `ns08scheduler`** (das Schema selbst ist fertig, aber die
      Migration ist erst mit diesen Schritten abgeschlossen): `webmanager.hh`s
      Dispatch auf `namespaceId`/`messageTypeId` umstellen, `AppController`/
      Scheduler-UI in `web-components` auf die neue `ws-protocol.ts`-API
      umstellen, alte `ns08scheduler.fbs` erst danach löschen (s. Checkliste
      unten) – das alles braucht die tatsächliche `GenerateWsProtocolFiles`-Phase
      im neuen C#-Orchestrator, die noch nicht existiert (Stufe 2 ist insgesamt
      noch nicht umgesetzt).

      Das Schema selbst, zur Referenz:
      ```json
      {
        "structs": [
          { "name": "scheduler.OneWeekIn15MinutesData", "fields": [
            { "name": "v", "type": "uint8", "count": 84 }
          ]}
        ],
        "classes": [
          { "name": "scheduler.Predefined", "fields": [] },
          { "name": "scheduler.OneWeekIn15Minutes", "fields": [
            { "name": "data", "type": "struct", "structRef": "scheduler.OneWeekIn15MinutesData" }
          ]},
          { "name": "scheduler.SunRandom", "fields": [
            { "name": "offsetMinutes", "type": "uint16" },
            { "name": "randomMinutes", "type": "uint16" }
          ]},
          { "name": "scheduler.Schedule", "fields": [
            { "name": "name", "type": "string" },
            { "name": "schedule", "type": "class", "classes": [
              "scheduler.Predefined", "scheduler.OneWeekIn15Minutes", "scheduler.SunRandom"
            ]}
          ]},
          { "name": "scheduler.SchedulerListItem", "fields": [
            { "name": "name", "type": "string" },
            { "name": "type", "type": "EnumU8", "enumName": "ScheduleType", "enumValues": [
              { "name": "PREDEFINED", "value": 0 }, { "name": "ONE_WEEK_IN_15_MINUTES", "value": 1 }, { "name": "SUN_RANDOM", "value": 2 }
            ]}
          ]}
        ],
        "messages": [
          { "name": "scheduler.RequestSchedulerList", "kind": "request", "fields": [] },
          { "name": "scheduler.ResponseSchedulerList", "kind": "response", "fields": [
            { "name": "items", "type": "array", "classes": ["scheduler.SchedulerListItem"] }
          ]},
          { "name": "scheduler.RequestSchedulerOpen", "kind": "request", "fields": [
            { "name": "name", "type": "string" },
            { "name": "type", "type": "EnumU8", "enumName": "ScheduleType", "enumValues": [
              { "name": "PREDEFINED", "value": 0 }, { "name": "ONE_WEEK_IN_15_MINUTES", "value": 1 }, { "name": "SUN_RANDOM", "value": 2 }
            ]}
          ]},
          { "name": "scheduler.ResponseSchedulerOpen", "kind": "response", "fields": [
            { "name": "payload", "type": "class", "classes": ["scheduler.Schedule"] }
          ]},
          { "name": "scheduler.RequestSchedulerSave", "kind": "request", "fields": [
            { "name": "payload", "type": "class", "classes": ["scheduler.Schedule"] }
          ]},
          { "name": "scheduler.ResponseSchedulerSave", "kind": "response", "fields": [
            { "name": "name", "type": "string" }
          ]},
          { "name": "scheduler.RequestSchedulerRename", "kind": "request", "fields": [
            { "name": "oldName", "type": "string" }, { "name": "newName", "type": "string" }
          ]},
          { "name": "scheduler.RequestSchedulerDelete", "kind": "request", "fields": [
            { "name": "name", "type": "string" }
          ]}
        ]
      }
      ```
      `payload` ist in `ResponseSchedulerOpen`/`RequestSchedulerSave` jeweils das
      EINZIGE Feld, erfüllt also trivial die "muss letztes Feld sein"-Regel für
      einzelne polymorphe Felder.
  - [x] `scheduler.json` schreiben und gegen den Referenzgenerator verifizieren
        (s. Status oben) – **erledigt**.
  - [x] `wifimanager`/`systeminfo`/`functionblock`/`heaterexperiment`/`chatbot`/
        `canmonitor`/`fingerprint`/`usersettings`/`journal` (die übrigen 9
        Namespaces) als `best_binary_buffers_schema/*.json` neu geschrieben (in
        `espidf-component-webmanager/best_binary_buffers_schema/`) – **erledigt (2026-08-01)**.
        Alle 10 Namespaces (inkl. `scheduler`) zusammen gegen den
        Referenzgenerator verifiziert (`--ws-protocol-path` auf das gesamte
        Verzeichnis): `arm-none-eabi-g++ -fsyntax-only` auf das generierte
        `ws_protocol.hh`, `tsc --strict --noEmit` auf das generierte
        `ws-protocol.ts`, sowie zwei Node-Laufzeit-Testsuiten mit echtem
        Encode/Decode-Roundtrip je Namespace (u.a. `secondsUptime` nahe 2^40,
        float32 in `heaterexperiment`, die zwei aufeinanderfolgenden
        `classes`-Arrays in `fingerprint.ResponseFingers`, die 4-gliedrige
        heterogene Liste in `usersettings.ResponseGetUserSettings`, `uint64`
        innerhalb einer Class in `journal.JournalItem`) – alle bestanden. Dabei
        die zwei oben dokumentierten Generator-Lücken (float32/int64, mehrere
        trailing `classes`-Felder) gefunden und behoben. `usersettings` nutzt
        das oben bestätigte 4-Classes-Design; `fingerprint.ResponseFingers`
        nutzt zwei separate `classes`-Arrays statt einer verschachtelten
        Struktur (s. Lücken-Tabelle oben); `fingerprint.NotifyEnrollNewFinger`/
        `NotifyFingerDetected` sind trotz "Notify"-Namen `kind:"response"`
        (nicht `"event"`), weil sie im Original Teil der `Responses`-Union
        waren (anders als `canmonitor.NotifyCanMessage`, das außerhalb beider
        Unions stand und deshalb `kind:"event"` ist).

        Nach jedem Testlauf wurde `best_binary_buffers_schema/ids.txt` (im Referenzprojekt)
        auf seine 2-Zeilen-Baseline zurückgesetzt und der (aus einem
        physischen Board gecachte) `build/.last-board-id` vor jedem Testlauf
        beiseite verschoben und danach wiederhergestellt – sonst hätte
        `ReadWebSocketProtocolAndGenerateFiles` ohne `--board`-Angabe die
        Test-Schemas ins reale Board-Archiv (`BoardsDir`, außerhalb des Repos)
        statt nach `Core/generated`/`web/generated` geschrieben. Das ist beim
        ersten Testlauf dieser Session tatsächlich passiert und wurde
        korrigiert (Board-Archiv mit dem echten `system.json`-Output
        neu generiert) – **Vorsicht bei künftigen Testläufen mit
        `--ws-protocol-path` in diesem Referenzprojekt**, falls ein Board
        gerade angeschlossen war/ist.
  - [x] `ns20sensact`/`ns21nodemaster` als `best_binary_buffers_schema/*.json` neu geschrieben
        (in `sensact_firmware/best_binary_buffers_schema/`) – **erledigt (2026-08-01)**.
  - [x] `configware`s `GenerateApplicationIds`/`GenerateCommandTypes` geben jetzt
        ZUSÄTZLICH (nicht ersetzend – s. unten) eine ws-protocol-`"enums"`-JSON
        aus – **erledigt (2026-08-01), aber bewusst nur ein Zwischenschritt**:
        `GenerateApplicationIds` schreibt neben `applicationIds`/`applicationIds.fbs`
        jetzt auch `applicationIds.enums.json` (`sensact.ApplicationId`, u16),
        `GenerateCommandTypes` analog `commandTypes.enums.json`
        (**`sensact.Command`**, u8 – bewusst NICHT `sensact.CommandType`: das
        Original-Flatbuffers-Template (`commandTypes.template.fbs`) nennt die
        Wire-Enum schon immer `Command`, der C#-interne Typname `CommandType`
        wurde nie auf die Wire-Ebene durchgereicht). Der bisherige `.fbs`-Output
        bleibt unverändert bestehen, damit der aktuell laufende
        Flatbuffers-Build nicht bricht – die eigentliche Umstellung (`.fbs`-Pfad
        entfernen) ist weiterhin für die `configware`-Verschmelzung vorgesehen,
        s. "Migrationsstrategie" unten.

        **Verifiziert** mit demselben Verfahren wie alle anderen Namespaces:
        `configware_sattlerstrasse` tatsächlich laufen lassen (liefert die
        ECHTEN ~360 `ApplicationId`-Werte des realen Sattlerstrasse16-Hausmodells,
        nicht nur Testdaten), die beiden erzeugten `.enums.json`-Dateien nach
        `sensact_firmware/best_binary_buffers_schema/` kopiert (`sensact_applicationIds.enums.json`/
        `sensact_commandTypes.enums.json`), `sensact.json`/`nodemaster.json`
        dagegen geschrieben (referenzieren die Enums per `enumRef`), dann mit dem
        Referenzgenerator generiert, `arm-none-eabi-g++ -fsyntax-only` +
        `tsc --strict` + echtem Node-Roundtrip getestet (u.a. `NO_APPLICATION`
        als u16-Maximalwert 65535, das ~360-Werte-Enum selbst, der
        `ids:[ApplicationId]`-Bare-Enum-Array-Workaround, `nodemaster.Range` als
        Class wegen des `name:string`-Felds) – alle bestanden, keine weiteren
        Generator-Lücken gefunden.

        Nebenbefund beim Ausführen von `configware_sattlerstrasse`, **behoben
        (2026-08-01)**: `configware_testmodel.csproj` zielte auf `net9.0`,
        `sensactmodelcommon` (das gemeinsame Projekt) auf `net10.0` –
        inkompatibel, `dotnet build` schlug fehl (`NU1201`). Auf `net10.0`
        gezogen (jetzt identisch mit `configware_sattlerstrasse.csproj`); dabei
        einen ZWEITEN, dadurch erst sichtbar gewordenen Fehler gefunden:
        `Testmodel.cs` referenzierte `RotaryEncoder.ROTARYENCODER_1`, das
        Enum (`common/Nodes/Node.cs`) kennt aber nur `ROTENC0`/`ROTENC1` –
        vermutlich Bit-Rot aus der Zeit, bevor die TFM-Inkompatibilität
        `configware_testmodel` faktisch unbuildbar machte. Auf `ROTENC0`
        korrigiert (passend zum einzigen Rotary-Encoder des Testmodells,
        analog zu den meisten `RotaryEncoder.ROTENC0`-Verwendungen in
        `Sattlerstrasse16.cs`). `configware_testmodel` läuft jetzt
        durch und erzeugt korrekt sein (kleineres, 27-Werte-)
        `applicationIds.enums.json` – **beide Hausmodelle sind damit
        buildbar**. Nach der Verifikation `configware_sattlerstrasse` erneut
        laufen lassen, um `C:/repos/generated/sensact_model` (gemeinsamer,
        nicht-repo-gebundener Ausgabeordner) wieder auf den echten
        Sattlerstrasse16-Stand zurückzusetzen (Testmodel-Lauf hätte ihn sonst
        mit den kleineren Testmodell-Werten überschrieben stehen lassen).
  - [ ] `webmanager.hh`s Dispatch (`handle_webmanager_ws`) auf
        `namespaceId`+`messageTypeId`-Switch umstellen (in
        `espidf-component-webmanager`).
  - [ ] `AppController`/Screen-Controller in `web-components` auf generierte
        `ws-protocol.ts`-API umstellen (in `npm-packages`).
- [ ] Alte `.fbs`-Dateien erst löschen, wenn der jeweilige Namespace vollständig
      umgestellt UND getestet ist (Parallelbetrieb pro Namespace möglich, da
      `namespaceId` ohnehin schon die fachliche Trennung ist).
- [ ] `flatbuffers`-Abhängigkeiten (npm-Paket, `flatc`-Toolchain-Vorbedingung in
      `docs/build-process.md`) erst entfernen, wenn ALLE 12 Namespaces umgestellt
      sind.

### Entschieden (Ergänzung)

- Reihenfolge: `ns08scheduler` zuerst (bewusst der komplexeste Fall, s. "Vorgehen"
  oben, um verbleibende Lücken so früh wie möglich zu finden), danach die übrigen
  11 Namespaces im Batch. Das oben ausgearbeitete Schema-Design ist noch
  ungetestet – erster Schritt bei Aufnahme der Arbeit ist, es tatsächlich zu
  generieren/kompilieren/durchzutesten (analog zum Vorgehen, mit dem die
  Generator-Erweiterungen selbst verifiziert wurden).
- `ns09usersettings`-Umbau auf 4 Classes: bestätigt, s. "Entschieden" oben.

## Migrationsstrategie

- [x] Neues C#-Projekt `builder/` neben dem bestehenden `builder/` angelegt
      (`sensact_firmware/builder/`) – **erledigt (2026-08-01)**. Grundgerüst
      analog zum Referenzprojekt: `builder.csproj` (net10.0, `Exe`,
      `Microsoft.Extensions.Configuration`-basiert), `Paths.cs`
      (Root-Suche über `CMakeLists.txt` + `builder/`-Sibling, analog zur
      Vorlage), `BuilderOptions.cs` (`appsettings.json`/`.template`-Muster
      1:1 übernommen, `BoardsDir`/`CertsDir` aus `gulpfile.ts` befüllt: `OneDrive
      - HSOS\esp32_boards`/`...\certificates`), `IBoardInfo.cs` (C#-Äquivalent
      von `IBoardInfo` aus `espidf-vite-secure-build-tools/utils.ts`, inkl.
      `BoardPaths.BoardSpecificPath`/`Mac6Char`/`Mac12Char` als Portierung von
      `paths.ts`s `mac_6char`/`mac_12char`/`Paths.boardSpecificPath` –
      Verzeichniskonvention `<mac6hex>_<macDezimal>_<mac12hex>` gegen die
      echte, real existierende Board-Archiv-Struktur verifiziert). `.gitignore`
      um `builder/appsettings.json`/`bin/`/`obj/` ergänzt (Muster von
      `factory_in_a_box/.gitignore` übernommen).
- [ ] Reihenfolge der Portierung (kleinstes Risiko zuerst):
  1. `Info`-Phase (nur Lesen, kein Risiko) – **erledigt (2026-08-01), mit
     bewusster Einschränkung**: beim genaueren Lesen von `context.ts` stellte
     sich heraus, dass `Context.printInfo()` ZUERST per `esp32.ts` (eigenes
     SLIP/ROM-Loader-Protokoll über die serielle Schnittstelle) die MAC eines
     TATSÄCHLICH ANGESCHLOSSENEN Boards abfragt und mit dem Cache vergleicht –
     also entgegen der Einstufung oben NICHT frei von der Hardware-Frage, die
     bewusst auf Schritt 11 (`ReadHardwareIds`) verschoben wird. Entschieden:
     `Info` liest fürs Erste NUR den zwischengespeicherten Stand
     (`<Repo-Root>/board_info.json`, dort abgelegt vom letzten echten
     Board-Connect) plus das zugehörige Board-Archiv unter `BoardsDir` – KEINE
     Live-Board-Abfrage, mit `TODO(ReadHardwareIds)`-Kommentar im Code markiert.
     Verifiziert gegen die echten Daten dieses Repos (`board_info.json` mit
     `mac:79336146357860` → korrekt aufgelöst zu Board-Archiv-Verzeichnis
     `d24e64_79336146357860_4827e2d24e64`, alle Felder – Name/Version/Rollen/
     Settings/Zeitstempel/Encryption-Status – stimmen mit der echten
     board-spezifischen `board_info.json` überein). "Is current board"-Zeile
     entsprechend durch einen erklärenden Platzhalter ersetzt, bis
     `ReadHardwareIds` existiert.
  2. Git-Info + ASCII-Banner (trivial, kein externer State) – **Git-Info
     erledigt (2026-08-01), ASCII-Banner bewusst zurückgestellt** (s. unten).
     `GitInfo.cs`/`GitInfoReader` fast 1:1 aus dem Referenzprojekt übernommen
     (reiner `git`-CLI-Aufruf, keine STM32-Spezifika). **Bewusst OHNE die
     4-Datei-Ausgabe der Vorlage** (`gitconstants.hh`/`firmware_constants.hh`/
     `build-info.ts`/`gitstatus.json`, board-archiv-gebunden) – sensact_firmware
     nutzt Git-Infos aktuell nur als EINEN Eintrag (`GIT_SHORT_HASH`) im
     gemeinsamen `defines`-Dictionary des Config-/Runtimeconfig-Writers
     (`gulpfile.ts`: `createObjectWithDefines()` → `cfg.create*`, s. Mapping-
     Tabelle Unterschritt 4) – die eigentliche Dateigenerierung entsteht daher
     erst dort, nicht als eigene Phase. Bis dahin per eigenständiger
     `GitStatus`-Phase (`dotnet run --project builder -- GitStatus`)
     verifizierbar – gegen rohe `git`-Befehle geprüft (Commit-Hash/Branch/Tag/
     Autor/Message/Dirty-Status stimmen exakt überein, `IsDirty` erkennt den
     echten, zum Testzeitpunkt tatsächlich unsauberen Arbeitsbaum korrekt).
     ASCII-Banner zurückgestellt: laut Plan ohnehin "nicht kritisch, niedrige
     Priorität" (s. Unterschritt 7 oben) – wird nachgeholt, wenn der
     Config-/Runtimeconfig-Writer (Schritt 4) tatsächlich angegangen wird, da
     der Banner-Text (`ascii_art.createAsciiArt`) ohnehin dort als Teil der
     `defines` gebraucht wird, nicht isoliert.
  3. `GenerateWsProtocolFiles` (portiert aus `ReadWebSocketProtocol.cs`, s. eigener
     Abschnitt "Flatbuffers wird durch ws-protocol ersetzt") – ersetzt den
     bisherigen `flatc`-Aufruf; kann isoliert entwickelt/getestet werden (reine
     Text-zu-Text-Generierung, kein Board-Kontext nötig), sollte aber inhaltlich erst
     NACH mindestens einem vollständig migrierten Namespace (s. dortiges "Vorgehen")
     production-scharf geschaltet werden – **portiert (2026-08-01)**.

     Die komplette Parsing-/Codegenerierungs-Logik (alles bis auf `Run()`) ist
     UNVERÄNDERT aus `ReadWebSocketProtocol.cs` übernommen (bereits vollständig
     generisch, keine STM32-Spezifika). Angepasst nur `Run()` selbst: kein
     Board-Archiv-Konzept (das ws-protocol gilt für die gesamte sensact-Flotte,
     nicht board-spezifisch), Ausgabe direkt nach
     `Paths.GeneratedWsProtocolCppDir`/`GeneratedWsProtocolTsDir`
     (`c:\repos\generated\wsprotocol_cpp`/`_ts` – sensacts eigene
     "gemeinsames externes generated-Verzeichnis"-Konvention aus `paths.ts`,
     nicht die repo-interne `Core/generated`-Konvention der Vorlage; bewusst
     NEUE Ordnernamen, solange Flatbuffers noch parallel produktiv läuft). Die
     Phase wird mit BEIDEN ws-protocol-Quellen aufgerufen
     (`Paths.WsProtocolDir` UND `Paths.WebmanagerWsProtocolDir`, Letzteres
     unverändert aus `gulpfile.ts`s `IDF_COMPONENT_WEBMANAGER_ROOT`
     übernommen), da erst beide zusammen den vollständigen sensact-
     Nachrichtensatz ergeben.

     **Echt end-to-end verifiziert** (nicht nur kompiliert): `dotnet run
     --project builder -- GenerateWsProtocolFiles` liest tatsächlich 14
     Dateien aus beiden Repos (die 4 unter `sensact_firmware/best_binary_buffers_schema/` +
     die 10 unter `espidf-component-webmanager/best_binary_buffers_schema/`) und erzeugt ein
     gemeinsames `ws_protocol.hh`/`ws-protocol.ts` mit allen 12 echten
     Namespaces (`canmonitor`, `chatbot`, `fingerprint`, `functionblock`,
     `heaterexperiment`, `journal`, `nodemaster`, `scheduler`, `sensact`,
     `systeminfo`, `usersettings`, `wifimanager` – **kein** `system`, das
     gehört nur zu `factory_in_a_box`). `arm-none-eabi-g++ -fsyntax-only` und
     `tsc --strict --noEmit` beide fehlerfrei auf dem echten Output. Zweiter
     Lauf bestätigt Idempotenz (keine neuen Zeilen in `ids.txt`). Dabei
     entstand `sensact_firmware/best_binary_buffers_schema/ids.txt` als ECHTE, eigene
     ID-Tabelle dieses Projekts (unabhängig von der Tabelle im
     Referenzprojekt) – **muss mitversioniert werden** (nicht gitignored, s.
     docs/websocket-protocol.md Abschnitt 8.5), analog zu
     `sensact.json`/`nodemaster.json` selbst.
  4. Config-/Runtimeconfig-Writer – **portiert (2026-08-01)**.

     `RuntimeConfigWriter.cs` (Portierung von `key_value_file_helper.ts`, rein
     generisches Text-Rendering eines flachen `defines`-Dictionary in 3
     Formate: `config.json` fürs CMake-Konfigurationssystem, C++-Header
     `runtimeconfig_defines.hh`/`runtimeconfig.hh` mit `#define`s bzw.
     `namespace cfg{ constexpr auto ...}`, TS-Modul `index.ts`) +
     `Phases/GenerateRuntimeConfig.cs` (Portierung von
     `gulpfile.ts:createObjectWithDefines()`), baut das Dictionary aus:
     `node_descriptor.json` des aktuellen Nodes (von `configware` erzeugt),
     `board_settings.web`/`.firmware` aus der board-spezifischen
     `board_info.json` (aktuell für alle bekannten Boards leer, Logik trotzdem
     1:1 inkl. der eigentümlichen Doppel-`JSON.stringify`-Behandlung aus dem
     TS-Original übernommen, NICHT stillschweigend "korrigiert"), sowie
     `NODE_ID`/`HOSTNAME`/`BOARD_*`/`APP_*`/`CREATION_DT*`/`GIT_SHORT_HASH`
     (Letzteres über `GitInfoReader`, s. Schritt 2). **Bewusst OHNE `BANNER`
     als echtes ASCII-Art-Rendering** – `ascii_art.ts` nutzt das npm-Paket
     `figlet`, keine triviale .NET-Entsprechung vorhanden, laut Plan ohnehin
     niedrige Priorität (s. Schritt 2 oben) – `BANNER` bekommt stattdessen
     vorläufig nur den reinen Text (`"<APP_NAME> <NODE_ID>"`) ohne ASCII-Art.

     **Wichtige Abhängigkeit, die diesen Schritt eigentlich hinter Schritt 7
     einordnen würde**: `node_descriptor.json` kommt von `configware`
     (`GenerateModelFiles`, Schritt 7, noch nicht portiert) – diese Phase setzt
     also voraus, dass `configware_sattlerstrasse`/`_testmodel` (das
     BESTEHENDE, noch nicht gemergte Konsolenprogramm) vorher schon einmal
     gelaufen ist und aktuelle Ausgaben unter `Paths.SensactModelGeneratedDir`
     hinterlassen hat (klare Fehlermeldung, falls nicht). Trotzdem wie geplant
     an dieser Stelle portiert (nicht auf Schritt 7 verschoben), weil die
     eigentliche Rendering-Logik unabhängig vom Ursprung der Eingabedaten ist
     und schon jetzt korrekt end-to-end getestet werden kann, solange
     `configware` weiterhin manuell vorher läuft.

     **Echt end-to-end verifiziert**: gegen die realen Daten des tatsächlich
     verbundenen Boards (`SNSCT_L3_WORK_HS`/Sattlerstraße 16, `node_id` aus der
     board-spezifischen `sensact.json`) 17 Defines erzeugt, alle drei
     Ausgabeformate inhaltlich mit dem erwarteten Ergebnis abgeglichen
     (inkl. Sonderzeichen `ß` in `SENSACT_MODEL_NAME` – dabei eine echte
     Diskrepanz zum TS-Original gefunden und behoben: `System.Text.Json`
     escaped Nicht-ASCII-Zeichen standardmäßig (`ß`), `JSON.stringify`
     tut das nicht – `JavaScriptEncoder.UnsafeRelaxedJsonEscaping` behebt das).
     Beide generierten C++-Header mit `arm-none-eabi-g++ -fsyntax-only`
     fehlerfrei kompiliert, `index.ts` mit `tsc --strict --noEmit` fehlerfrei
     typgeprüft.
  5. Zertifikate – **portiert (2026-08-01)**.

     `Certificates.cs`, bewusst NICHT 1:1 aus `certificates.ts` übersetzt
     (das nutzt `node-forge`), sondern nativ mit
     `System.Security.Cryptography.X509Certificates`/`CertificateRequest`
     neu gebaut, wie in "Entschieden" vorgesehen (kein `openssl`-Shell-out
     nötig, anders als im Referenzprojekt). Erzeugt dieselben drei
     Zertifikatsarten wie das Original: selbstsignierte Root-CA
     (`CreateRootCA`, nur für einmaliges, manuelles CA-Setup – NICHT Teil der
     eigentlichen Phase), signierte Board-/Server-Zertifikate mit IP+mehreren
     DNS-SANs (`CreateAndSignCert`) und Client-Zertifikate (`CreateAndSignClientCert`,
     feste IP `192.168.4.1`, ein DNS-SAN = Username) – RSA-2048/SHA-256, 3000
     Tage Gültigkeit, alle Werte/Erweiterungen 1:1 aus `certificates.ts`
     übernommen (inkl. der eigentümlichen Custom-OID-Extension mit fest
     codiertem Wert `"false"`, bewusst nicht "korrigiert"). Dazu
     `Phases/GenerateCertificates.cs` (Portierung des "Certificates
     (lazy)"-Abschnitts aus `gulpfile.ts:createFiles()`): erzeugt nur ein
     neues Board-Zertifikat, wenn im Board-Archiv noch keins liegt, signiert
     mit der echten, produktiv genutzten Root-CA (nur lesend).

     **Korrektur nach Rückmeldung**: Country/State/Locality/Organization für
     das Zertifikats-Subject standen zunächst als hartcodierte Konstanten in
     `Certificates.cs` – im Referenzprojekt steckt genau das dagegen in
     `appsettings.json` (`BuilderOptions.SubjectPrefix`, OpenSSL-`-subj`-Notation
     `/C=.../ST=.../L=.../O=...`, genutzt für `openssl req -subj` in
     `Phases/ReadHardwareIds.cs`). Behoben: `BuilderOptions.cs` bekam
     dasselbe `SubjectPrefix`-Feld (gleiches Format, damit `appsettings.json`
     zwischen beiden Projekten strukturell vergleichbar bleibt), `Certificates.cs`
     parst den Slash-String zur Laufzeit in ein `X500DistinguishedName`
     (RFC-2253-Notation, umgekehrte RDN-Reihenfolge) statt `openssl` direkt
     aufzurufen. Erneut end-to-end verifiziert (Test-Zertifikat gegen die
     echte Root-CA erzeugt, `openssl x509 -noout -subject` zeigt das korrekt
     zusammengesetzte Subject, `openssl verify` weiterhin `OK`).

     **Echten Fehler beim Portieren gefunden und behoben**: `certificates.ts`s
     `authorityKeyIdentifier`-Extension setzt bei genauerem Hinsehen NUR
     `authorityCertIssuer`+`serialNumber`, KEIN `keyIdentifier` – passend dazu
     hat die echte, produktiv genutzte Root-CA selbst keine
     Subject-Key-Identifier-Extension (nur `basicConstraints`+`keyUsage`, s.
     `createRootCaExtensions()`). Der naheliegende .NET-Aufruf
     `X509AuthorityKeyIdentifierExtension.CreateFromCertificate(caCert,
     includeKeyIdentifier: true, ...)` scheitert deshalb an der echten Root-CA
     mit `CryptographicException: certificate does not have a Subject Key
     Identifier extension` – behoben durch `includeKeyIdentifier: false`
     (was ohnehin dem Original entspricht, kein Kompromiss).

     **Echt end-to-end verifiziert, nicht nur kompiliert**: `GenerateCertificates`
     gegen das echte, verbundene Board ausgeführt – bestätigt korrektes
     Lazy-Verhalten (Zertifikat existiert bereits, kein Schreibzugriff,
     s. "Ausführung mit Vorsicht" unten). Für die eigentliche Kryptografie-
     Verifikation zusätzlich alle drei Zertifikatsarten testweise erzeugt
     (Leaf-Zertifikat + Client-Zertifikat signiert mit der ECHTEN,
     produktiven Root-CA – nur lesend, Ausgabe in ein Scratch-Verzeichnis,
     NICHT ins Board-Archiv; sowie eine neue selbstsignierte Test-Root-CA) und
     unabhängig mit `openssl` geprüft: `openssl x509 -text` zeigt korrekte
     Struktur (Version 3, positive Seriennummer, richtige Gültigkeitsdauer,
     Subject/Issuer, alle Erweiterungen inkl. SAN mit IP+3 DNS-Namen),
     `openssl verify -CAfile rootCA.pem.crt` bestätigt `OK` für Leaf- UND
     Client-Zertifikat (echte Kettenvalidierung gegen die reale Produktions-CA),
     Public-Key-Modulus von Zertifikat und privatem Schlüssel stimmen überein
     (MD5-Vergleich), die selbstsignierte Test-Root-CA verifiziert sich selbst.
     Alle drei Testfälle liefen über einen temporären Debug-Pfad in
     `Program.cs`, der nach erfolgreicher Verifikation wieder entfernt wurde.

     **Vorsicht bei künftigen Testläufen von `GenerateCertificates`**: die
     Phase schreibt (außerhalb des Lazy-Falls) ins ECHTE Board-Archiv
     (`BoardsDir`) – für reine Verifikation der Zertifikats-Logik selbst
     lieber `Certificates.CreateAndSignCert(...)` direkt mit einem
     Scratch-Ausgabepfad aufrufen (analog zum jetzt wieder entfernten
     Debug-Pfad), nicht die Phase selbst gegen ein Board mit bereits
     vorhandenem Zertifikat "erzwingen".
  6. Board-Kontext (Lesen/Schreiben `board_info.json` etc.) – **portiert
     (2026-08-01)**.

     `BoardContext.cs` (Portierung von `Context.get()` aus `context.ts`,
     NUR der `updateWithCurrentlyConnectedBoard: false`-Zweig – der `true`-Zweig
     braucht dieselbe Live-ESP32-Abfrage, die auch bei `Info` bewusst
     zurückgestellt wurde, s. Schritt 1 oben und Schritt 11): liest den
     zwischengespeicherten Stand (`Paths.BoardInfoJsonPath`), löst daraus das
     Board-Archiv-Verzeichnis auf, legt bei fehlender board-spezifischer
     `board_info.json` einen neuen Eintrag mit Default-Werten an
     (`DEFAULT_BOARD_NAME`/`DEFAULT_BOARD_VERSION` aus `gulpfile.ts`
     übernommen), und synchronisiert den Repo-Root-Cache zurück (Äquivalent zu
     `fs.cpSync(...)` am Ende von `Context.get()`).

     **Ersetzt duplizierte Logik**: `Phases/Info.cs`, `Phases/GenerateRuntimeConfig.cs`
     und `Phases/GenerateCertificates.cs` hatten bislang je eigene, inline
     geschriebene "lies `board_info.json`, löse Board-Verzeichnis auf"-Logik
     (aus der Zeit, bevor es eine gemeinsame Stelle dafür gab). **Bewusste
     Ausnahme**: `Info.cs` wurde NICHT auf `BoardContext.LoadCached()`
     umgestellt, weil das (wie `Context.get()`) bei fehlendem Eintrag einen
     NEUEN anlegt – für die rein informative `Info`-Phase (Äquivalent zu
     `Context.printInfo()`, die nie etwas anlegt) wäre das ein unerwarteter
     Seiteneffekt. `GenerateRuntimeConfig`/`GenerateCertificates` (die
     inhaltlich `Context.get()` entsprechen, nicht `Context.printInfo()`)
     wurden dagegen umgestellt – dabei gewinnen sie nebenbei auch das bisher
     fehlende "lege mit Default-Werten an, falls unbekannt"-Verhalten korrekt
     dazu, statt nur zu werfen.

     **Verifiziert**: beide umgestellten Phasen nach dem Refactoring erneut
     gegen das echte Board ausgeführt – identisches, weiterhin korrektes
     Ergebnis (17 Defines, Zertifikat-Lazy-Skip), `board_info.json` im
     Repo-Root nach dem Copy-Back-Schritt inhaltlich unverändert (war bereits
     konsistent mit dem board-spezifischen Stand).
  7. `GenerateModelFiles` (`configware`-Merge, siehe eigener Abschnitt oben) – muss
     vor Schritt 8 stehen, da dessen Output (`.inc`-Dateien) dort gebraucht wird;
     `configware`s Enum-Ausgabe wird dabei gleich auf `"enums"` (ws-protocol) statt
     `.fbs`-Text umgestellt – **portiert (2026-08-01)**.

     Wie in "Entschieden (Ergänzung)" oben festgelegt: `configware/common` →
     `builder/ModelGeneration/` verschoben (Generator, `ModelBuilder<T>`,
     `SourceCodeGenerator<T>`, `Node`, `SensactApplication`, `Applications/`,
     `Nodes/`, ... – **unverändert**, keine Namensraum-Anpassung nötig, da
     `common/` nachweislich (per Grep verifiziert) nie direkt von der
     modellspezifischen `ApplicationId` abhing, sondern ausschließlich über den
     generischen Typparameter). `configware_sattlerstrasse`/`configware_testmodel`
     → `builder/Models/Sattlerstrasse16/`
     bzw. `.../Models/Testmodel/` (Dateinamen `ApplicationId.cs`/
     `Sattlerstrasse16Builder.cs`/`TestmodelBuilder.cs`, s. Zielstruktur oben),
     `ApplicationId`-Namespace disambiguiert zu `Klli.Sensact.Model.Sattlerstrasse16`
     bzw. `.Testmodel` (die "Geklärte Generics-Frage" von oben bestätigt: beide
     geschlossenen generischen Typen `SourceCodeGenerator<Sattlerstrasse16.ApplicationId>`/
     `SourceCodeGenerator<Testmodel.ApplicationId>` koexistieren problemlos im
     selben Assembly). Neue Phase `Phases/GenerateModelFiles.cs` dispatcht per
     `--model Sattlerstrasse16|Testmodel`-CLI-Flag (s. "Entschieden") manuell auf
     die passende geschlossene generische Instanziierung (kein Reflection nötig,
     nur 2 Modelle). `configware_*`s DI-Container/NLog/blockierendes
     `Console.ReadKey()` ersatzlos entfernt – einfache aufrufbare Methode mit
     Standard-Konsolen-Logging (`Microsoft.Extensions.Logging.Console`).

     **Drei Compiler-Fehler beim Verschieben gefunden und behoben** (alles
     Altlasten der ursprünglichen Zwei-Projekte-Struktur, die durch das
     Zusammenführen erstmals sichtbar wurden):
     1. `System.ApplicationId` (ein echter BCL-Typ, durch `ImplicitUsings`
        global eingebunden) kollidierte mit `Klli.Sensact.Model.<Modell>.ApplicationId`
        – behoben per `using ApplicationId = Klli.Sensact.Model.<Modell>.ApplicationId;`
        (Alias statt normalem `using`) in beiden `*Builder.cs`-Dateien.
     2. Beide `*Builder.cs`-Dateien nutzten daneben an vielen Stellen
        `Model.ApplicationId.X` (kein `using`-Alias, sondern C#s relative
        Namespace-Auflösung: `Model` löste, weil der Code in `Klli.Sensact.Config`
        liegt, auf `Klli.Sensact.Model` auf) – das brach, seit `ApplicationId`
        eine Ebene tiefer liegt (`Klli.Sensact.Model.<Modell>`). Behoben durch
        einheitliches Ersetzen von `Model.ApplicationId` → `ApplicationId` (matcht
        jetzt den Alias aus Punkt 1) in beiden Dateien.
     3. `ILogger<T>` fehlte (`ModelGeneration/Generator.cs` nutzt es, `builder.csproj`
        hatte aber keine `Microsoft.Extensions.Logging`-Pakete) – ergänzt.

     **Kritischer Fund beim End-to-End-Vergleich, betrifft reale
     Hausautomatisierungs-Firmware**: `builder.csproj` hatte
     `<InvariantGlobalization>true</InvariantGlobalization>` (unreflektiert aus
     dem STM32-Referenzprojekt übernommen). `ModelGeneration/Generator.cs`
     verlässt sich an zwei Stellen auf `DateTime.Now.ToString()` OHNE
     Formatstring (kulturabhängig) für `SENSACT_MODEL_CREATION_DT_STRING` und
     die eingebettete `NodeDescription`-C++-Konstante – mit
     `InvariantGlobalization=true` wechselt das Format von
     `dd.MM.yyyy HH:mm:ss` (deutsches System, bisheriges Verhalten) auf
     `MM/dd/yyyy HH:mm:ss` (invariante Kultur). Erst durch einen echten
     Byte-für-Byte-Vergleich (s. Verifikation unten) entdeckt – wäre sonst
     unbemerkt in generierte Firmware-Strings eingeflossen. Behoben durch
     Entfernen von `InvariantGlobalization` (das ursprüngliche
     `configware_sattlerstrasse.csproj` hatte es nie gesetzt).

     **Danach auf Wunsch durchgängig auf ISO-Format umgestellt**
     (`yyyy-MM-dd HH:mm:ss` bzw. `yyyy-MM-dd`, statt der bisherigen
     kultur-/gebietsschema-abhängigen Formatierung) – betrifft
     `ModelGeneration/Generator.cs` (`SENSACT_MODEL_CREATION_DT_STRING`,
     `NodeDescription`), `Phases/GenerateRuntimeConfig.cs` (`CREATION_DT_STR`,
     vorher probeweise `dd.MM.yyyy` als de-DE-Nachbildung), `Phases/Info.cs`
     (`IBoardInfo.FromUnixMillis`, First/Last connected) und
     `Phases/GitStatus.cs` (Commit Date). Damit weicht dieses Datumsformat
     bewusst vom bisherigen TS-Verhalten (de-DE-lokalisiert) ab – das ist eine
     bewusste Entscheidung, kein Versehen.

     **Echt end-to-end verifiziert, mit dem höchsten Risiko aller bisherigen
     Schritte** (generiert echten Code für die real betriebene Hausautomatisierung):
     das bestehende `configware_sattlerstrasse`-Programm frisch laufen lassen und
     seinen kompletten Output (88 Dateien unter `C:/repos/generated/sensact_model`)
     als Referenz-Snapshot gesichert, danach `builder -- GenerateModelFiles
     --model Sattlerstrasse16` laufen lassen und rekursiv verglichen
     (`diff -rq`): **alle 88 Dateien identisch bis auf die beiden erwartbar
     zeitstempelbehafteten Dateien pro Node** (`node_descriptor.json`,
     `nodeDescription.inc` – enthalten den Erzeugungszeitpunkt, unterscheiden
     sich zwangsläufig zwischen zwei zeitlich getrennten Läufen). Nach dem
     ISO-Format-Umbau erneut verifiziert: identische Struktur, nur das
     (gewünschte) neue Datumsformat weicht ab. Zusätzlich `--model Testmodel`
     probeweise durchlaufen lassen (bestätigt, dass beide geschlossenen
     generischen Typen im selben Lauf/Assembly sauber funktionieren) und
     danach den gemeinsamen, externen `C:/repos/generated/sensact_model`-Ordner
     wieder auf den echten Sattlerstrasse16-Stand zurückgesetzt.

     **Erledigt (2026-08-17)**: das alte `configware/`-Verzeichnis (drei
     `.csproj`) wurde gelöscht. Anlass: die beiden parallel gepflegten Kopien
     der Generierungs-Engine (`configware/common/Generator.cs` vs.
     `builder/ModelGeneration/Generator.cs`) waren real auseinandergelaufen
     (JSON->`.cs`-Enum-Format-Umstellung hatte nur eine Kopie erreicht) – bei
     der Fehlersuche dafür bestätigt, dass `builder -- GenerateModelFiles`
     inzwischen production-tauglich ist (end-to-end gegen das echte
     Sattlerstrasse16-Modell verifiziert), also war die "Fallback/
     Vergleichsreferenz"-Begründung von oben nicht mehr nötig. Bei Bedarf über
     die Git-Historie wiederherstellbar.
  8. Sensact-Codegenerierung (jetzt nur noch der verbleibende Template-Teil, s.o.)
     – **portiert (2026-08-01)**.

     Beim genaueren Lesen von `sensact_code_generator.ts` (`Sensact.PrepareSensactFiles()`)
     stellte sich heraus, dass die frühere Einschätzung oben ("entfällt dadurch in
     ihrer heutigen Form") zu pauschal war: **zwei** der vier dortigen
     Template-Expansionen (`applicationIds.fbs`/`commandTypes.fbs`) sind
     tatsächlich obsolet – die erzeugt jetzt `configware`/`GenerateModelFiles`
     direkt als vollständige ws-protocol-`"enums"`-JSON (s. oben). Die
     **anderen zwei** (`sendCommandImplementation.ts`, `sensactapps.ts`)
     bleiben dagegen unverändert nötig: sie kombinieren einen TS-Template-Rahmen
     (`web/templates/*.template.ts`, feste Imports/Funktionssignatur) mit einem
     von `configware` erzeugten `.inc`-Schnipsel (`sendCommandImplementation.ts.inc`
     bzw. `sensactapps_local.ts.inc`) und legen dafür je ein eigenständiges,
     generiertes npm-Paket an (`@generated/sensact_sendCommandImplementation`/
     `@generated/sensact_appsbuilder`) – das ist reines Text-Rendering + `npm
     install`, unabhängig vom Flatbuffers/ws-protocol-Wechsel.

     Neue Dateien: `NpmProject.cs` (Portierung von `npm.ts`s
     `CreateAndInstallNpmProjectLazily` – schreibt `package.json` und ruft `npm
     install` nur auf, wenn sich der Inhalt geändert hat oder `node_modules/`
     fehlt) und `Phases/GenerateSensactFiles.cs` (Portierung der verbleibenden
     zwei Template-Expansionen aus `PrepareSensactFiles()`). `BoardContext.GetNodeId()`
     ergänzt (dritte Stelle, die das brauchte – zusätzlich `GenerateRuntimeConfig`/
     `GenerateCertificates` darauf umgestellt, ersetzt jeweils eigene
     `sensact.json`-Lese-Logik). **Bewusst unverändert gelassen**: die beiden
     `*.template.ts`-Dateien importieren weiterhin aus
     `@generated/flatbuffers_ts/sensact` – das auf `ws-protocol` umzustellen ist
     Teil der separaten, größeren `webmanager.hh`/`AppController`-Migration (s.
     "Vorgehen" oben), nicht dieses Schritts.

     **Echt end-to-end verifiziert**: bestehenden Output beider generierter
     npm-Projekte gesichert, `GenerateSensactFiles` gegen das echte, verbundene
     Board laufen lassen. `sendCommandImplementation.ts` und beide
     `package.json` **byte-identisch** zum vorherigen Stand (kein unnötiger
     `npm install`-Aufruf ausgelöst, da Inhalt unverändert). `sensactapps.ts`
     wich in einer Zeile ab (Beschreibungstext von `POWIT_L1_LVNG_C1`,
     "Wandlampen" → "Esszimmerlampe") – das ist KEINE Regression, sondern
     korrektes Verhalten: der alte Snapshot war schlicht älter als der
     aktuelle Stand von `Sattlerstrasse16Builder.cs` (per Grep bestätigt: die
     Quelle sagt bereits "Esszimmerlampe"), die neue Phase spiegelt also
     korrekt den aktuellen Quellstand wider.
  9. `BuildWebApp` (Vite-Aufruf) – **portiert (2026-08-01)**.

     Vite hat keine C#-API (wie schon in der Referenz festgestellt) – Aufruf
     deshalb über die Vite-CLI (`node web/node_modules/vite/bin/vite.js build
     web --outDir ... --sourcemap true --emptyOutDir`) statt der JS-API
     (`vite.build({...})`), die `vite_helper.ts` nutzt. Die dortigen
     Inline-Overrides `cssCodeSplit:false`/`esbuild.legalComments:'none'` sind
     nicht als CLI-Flags verfügbar, mussten aber auch nicht nachgebildet
     werden: `web/vite.config.mts` setzt beide bereits selbst identisch
     (redundante Doppelsetzung im TS-Original) – das Endergebnis der
     Konfigurations-Zusammenführung ist unabhängig vom CLI- vs.
     JS-API-Weg dasselbe. `ProcessRunner.cs` dafür unverändert aus dem
     Referenzprojekt übernommen (vollständig generisch, wird auch für
     `BuildFirmware`/`FlashFirmware` gebraucht).

     Brotli-Kompression (`index.html` → `index.compressed.br`) in C#
     nachgebildet (`System.IO.Compression.BrotliStream`,
     `CompressionLevel.SmallestSize` für Qualitätsstufe 11, analog zu Node's
     `zlib.brotliCompressSync()`-Standardeinstellung) statt Node dafür ein
     zweites Mal aufzurufen.

     **Echt end-to-end verifiziert**: `dotnet run --project builder --
     BuildWebApp` gegen das echte Web-Projekt laufen lassen – Vite-Build lief
     durch (152 Module transformiert, `viteSingleFile`-Plugin inlined JS/CSS
     wie gewohnt), komprimierte Größe (29.710 Byte) praktisch identisch zum
     vorherigen, Node-erzeugten Stand (29.702 Byte – die winzige Differenz
     kommt vom leicht unterschiedlichen JS-Bundle-Hash zwischen zwei
     Build-Läufen, nicht von der Brotli-Implementierung). Zusätzlich mit Node
     unabhängig gegengeprüft: `zlib.brotliDecompressSync()` auf die
     .NET-erzeugte `index.compressed.br` liefert byte-identisch dieselbe
     `index.html` zurück.

     **Nachtrag (2026-08-01): Lit-Template-Minifizierer + Inline-Brotli-Kompression
     aus der Referenz übernommen, auf Wunsch.** `vite-plugin-single-file-firmware-asset.ts`
     und `singlefile-minify.ts` (+`html-minifier-terser.d.ts`) unverändert aus dem
     Referenzprojekt nach `web/` kopiert, NUR die Ausgabe angepasst: statt eines
     fest verdrahteten `../build/assets/index.html.br` (STM32-Konvention) schreibt
     das Plugin jetzt nach `options.dir` (Vites tatsächlich aufgelöster
     `outDir`, unabhängig von Config-Datei/CLI-Flag) mit konfigurierbarem
     Dateinamen – aufgerufen als `singleFileFirmwareAssetPlugin("index.compressed.br")`
     in `web/vite.config.mts`, um den von `main/CMakeLists.txt` erwarteten
     Dateinamen (`EMBED_FILES "${GENERATED_DIR}/web/index.compressed.br"`)
     beizubehalten. Ersetzt `@klaus-liebler/vite-single-file` (aus
     `web/package.json` entfernt), neue Dev-Dependencies `terser`+`html-minifier-terser`
     (`^5.x`/`^7.2.0`, wie in der Referenz). `BuildWebApp.cs` dadurch
     vereinfacht: die zuvor selbst nachgebaute `BrotliStream`-Kompression
     entfällt ersatzlos, da das Plugin jetzt selbst brotli-komprimiert
     (genau wie in der Referenz, ein einziger Rutsch statt zweier Prozesse).

     **Nebenbefund beim `npm install`**: ein echter npm/Arborist-Bug (nicht
     durch diese Änderung verursacht) ließ `npm install` mit
     `TypeError: Cannot read properties of null (reading 'package')`
     abstürzen, sobald die per `file:`-Symlink eingebundenen
     `@klaus-liebler/*`/`@generated/*`-Pakete beim Scannen des bestehenden
     `node_modules`-Baums mit eingelesen wurden. Workaround: diese
     Symlink-Verzeichnisse kurz aus `node_modules` verschoben, `npm install`
     lief dann durch, npm hat sie beim Install automatisch wiederhergestellt
     (identische Ziele, keine Daten verloren).

     **Echt end-to-end verifiziert**: `BuildWebApp` erneut gegen das echte
     Web-Projekt laufen lassen. Komprimierte Größe **29.447 Byte** (vorher,
     ohne Minifizierer: 29.710 Byte) – kleiner trotz GRÖSSEREM unkomprimiertem
     HTML (147.413 vs. 144.754 Byte, vermutlich durch abweichende
     JS-Minifizierung/Terser-Ausgabe ggü. esbuilds Standardminifizierung) –
     Brotli gleicht das mehr als aus, weil es Whitespace ohnehin schon sehr
     effizient komprimiert; der eigentliche Gewinn ist bescheiden, aber
     real. Ausgabe unabhängig geprüft: mit Node dekomprimiert, Struktur
     validiert (genau 1 `<script>`, 1 `<style>`, `<!doctype>` vorhanden,
     `<head>`/`<body>` korrekt), den extrahierten `<script>`-Inhalt mit
     `node --check` auf syntaktische Gültigkeit geprüft – bestanden.
  10. `BuildFirmware` (`idf.py build`) – **portiert, aber NICHT verifiziert
      (2026-08-01)**.

      Portierung von `gulpfile.ts:buildFirmware()` (`espidf.ts:buildFirmware()`/
      `exec_in_idf_terminal()`): `export.bat` (setzt PATH/PYTHONPATH für die
      ESP-IDF-Python-Umgebung) und `idf.py build` müssen als EIN per `&&`
      verketteter Befehl im selben Prozess laufen (die von `export.bat`
      gesetzten Umgebungsvariablen gelten nur innerhalb dieses einen
      Prozesses) – deshalb über `cmd.exe /c "\"<IDF_PATH>\export.bat\" &&
      idf.py build"` per `ProcessRunner.RunInherit`, nicht als zwei getrennte
      Aufrufe.

      **Nicht end-to-end testbar in dieser Session**: beim Ausprobieren des
      `export.bat`/`idf.py`-Verkettungsmusters (zunächst nur mit `idf.py
      --version` als risikoarmer Testlauf) schlug bereits die
      Python-venv-Aktivierung selbst fehl – `activate_venv.py` suchte
      `C:\Users\mail\scoop\apps\python313\current\python.exe`, das nicht
      (mehr) existiert (Exit-Code 103). Das ist ein vorbestehendes
      Umgebungsproblem auf diesem Rechner (vermutlich eine veraltete
      ESP-IDF-Tool-Konfiguration nach einem scoop-Python-Update/-Umzug),
      unabhängig vom hier portierten Code – **auf Wunsch trotzdem portiert,
      Verifikation nachgeholt, sobald die Python-venv repariert ist**. Bis
      dahin bleibt offen, ob die `cmd.exe`-Verkettung/Quotierung in der Praxis
      tatsächlich funktioniert.
  11. `ReadHardwareIds` (MAC-Auslesen über `esptool`) – bewusst spät, weil hier die
      größte Verhaltensänderung ggü. dem aktuellen `esp32.ts`-SLIP-Ansatz steckt
      – **portiert und ECHT gegen ein angeschlossenes Board verifiziert (2026-08-01)**.

      Wie im Mapping-Kapitel empfohlen: KEIN Neubau des eigenen SLIP/ROM-Loader-
      Protokolls aus `esp32.ts`, sondern Aufruf von `esptool`/`espefuse`
      (`EspTool.cs`). Geprüft und bestätigt: **beide heute genutzten Felder sind
      darüber erreichbar** – `esptool read-mac` liefert Chip-Typ + MAC direkt im
      Klartext-Banner (kein `--format json` für diesen Befehl verfügbar, daher
      per Regex geparst), `espefuse summary --format json` liefert
      `KEY_PURPOSE_0`.."5"/`SPI_BOOT_CRYPT_CNT` bereits **chip-übergreifend
      decodiert** – im TS-Original musste `ESP32S3.updateChipInfo()` dafür noch
      chip-spezifische Efuse-Registeradressen von Hand kennen (separate
      `ESP32Classic`/`ESP32S3`-Unterklassen); `espefuse` nimmt diese Arbeit
      komplett ab. Die Purpose-Kombinatorik selbst (welche
      `KEY_PURPOSE`-Kombination + `SPI_BOOT_CRYPT_CNT`-Bit-Parität bedeutet
      "Verschlüsselung aktiv") 1:1 aus `esp32.ts` übernommen.

      **Zwei echte Stolpersteine beim Testen gegen das echte, angeschlossene
      Board gefunden**:
      1. `esptool` erkennt den angeschlossenen Port selbstständig (kein
         VID/PID-Scan wie `esp32.ts`s `FindProbablePorts()` nötig), `espefuse`
         dagegen NICHT – braucht zwingend `--port`. Behoben, indem der von
         `esptool read-mac`s Ausgabe erkannte Port (`Serial port COM4:`)
         mitgelesen und an den `espefuse`-Aufruf weitergereicht wird, statt
         ihn ein zweites Mal zu suchen oder fest zu verdrahten.
      2. **Echter Bug, reale Datei betroffen**: `IBoardInfo.cs`s berechnete
         Anzeige-Properties (`BoardRolesDisplay`/`BoardSettingsDisplay`,
         ursprünglich nur für `Phases/Info.cs`s Konsolenausgabe gedacht)
         hatten kein `[JsonIgnore]` – beim ersten echten Schreibzugriff von
         `BoardContext.LoadFromLiveMac()` (aktualisiert `last_connected_dt`
         eines bestehenden Eintrags, dafür `JsonSerializer.Serialize(board)`)
         wurden dadurch zwei zusätzliche, nie vorher in `board_info.json`
         vorhandene Felder in die ECHTE Datei dieses Boards geschrieben.
         Sofort bemerkt (Vorher/Nachher-Vergleich, Teil der etablierten
         Vorsichtsroutine), `[JsonIgnore]` ergänzt, betroffene Dateien
         (Repo-Root-Cache UND Board-Archiv) durch erneuten Lauf bereinigt.

      **`BoardContext.cs` erweitert**: `Load()` jetzt mit einem
      `updateLastConnected`-Parameter, geteilt zwischen `LoadCached()`
      (unverändert, `false`) und neu `LoadFromLiveMac(mac)`
      (`true` – Äquivalent zu `Context.get(config, updateWithCurrentlyConnectedBoard: true)`
      bzw. dem `addOrUpdateConnectedBoard`-Gulp-Task). `Phases/Info.cs`
      bewusst NICHT auf `LoadFromLiveMac` umgestellt (bliebe read-only) –
      stattdessen ruft es jetzt `EspTool.ReadHardwareIds()` best-effort in
      einem `try/catch` auf und vergleicht die live gelesene MAC gegen den
      Cache, ohne jemals zu schreiben – löst damit das seit Schritt 1
      offene `TODO(ReadHardwareIds)` ("Is current board") aus dem
      TS-Original nach.

      **Nebenbefund, unabhängig von diesem Code**: die ESP-IDF-Python-venv auf
      diesem Rechner ist defekt (s. Schritt 10, `BuildFirmware`) – `esptool`/
      `espefuse` brauchen sie aber gar nicht zwingend (eigenständige,
      pip-installierbare Pakete), deshalb per `pip install esptool` in eine
      andere, funktionierende Python-Installation auf diesem Rechner
      installiert. `EspTool.cs` ruft sie deshalb bewusst OHNE
      `export.bat`-Aktivierung auf (anders als `BuildFirmware.cs`/`idf.py`).

      **Echt end-to-end verifiziert, gegen das tatsächlich angeschlossene
      Board** (MAC `48:27:e2:d2:4e:64`, exakt das Board, mit dem in dieser
      Session durchgehend gearbeitet wurde): `esptool read-mac` erkennt Chip
      `ESP32-S3` und die korrekte MAC; `Phases/Info.cs` zeigt jetzt live
      "Is current board: yes"; `Phases/ReadHardwareIds.cs` aktualisiert
      `last_connected_dt` korrekt in Repo-Root-Cache UND Board-Archiv, alle
      anderen Felder bleiben unverändert. Flash-Encryption-Key-Erkennung
      meldet korrekt "no" (passend zu `SPI_BOOT_CRYPT_CNT = Disable`, per
      `espefuse summary` bestätigt) – der "Key vorhanden"-Zweig (ESP32-S3 mit
      `XTS_AES_256`/`_128`) konnte mangels eines Testboards MIT aktivierter
      Verschlüsselung nicht real durchgespielt werden, nur die Logik dafür
      1:1 aus `esp32.ts` übernommen. ESP32-"Classic"-Chips (nicht-S3, andere
      Efuse-Feldnamen) ebenfalls nicht real testbar – `EspTool.cs` fällt in
      diesem Fall konservativ auf "keine Verschlüsselung" zurück statt zu
      werfen.
  12. `FlashFirmware` (inkl. Encryption) – **unverschlüsselter Pfad portiert
      und ECHT gegen das angeschlossene Board geflasht (2026-08-01), letzter
      Schritt der Migrationsstrategie.**

      Portierung von `gulpfile.ts:flashFirmware()` (`espidf.ts:flashFirmware()`).
      Dispatcht wie im TS-Original anhand von `board_info.json`s
      `flash_encryption_key_burned_and_activated` – für jedes bisher bekannte
      sensact-Board `false`, also der unverschlüsselte Pfad. Neue Dateien:
      `FlasherArgs.cs` (liest das von `idf.py build` erzeugte
      `build/flasher_args.json`), `PartitionsCsv.cs` (Portierung von
      `parsePartitionsCSVFromFile()`, aktuell nur fürs spätere NVS-Handling
      vorbereitet, nicht im aktiven Pfad genutzt), `EspTool.WriteFlash()`
      (baut `esptool write-flash --flash-size keep <offset> <file> ...`).

      **Echter, im TS-Original nie aufgefallener Fehler beim Portieren
      gefunden**: `IFlasherConfiguration` (espidf.ts) erwartet ein Feld
      `partitionTable` (camelCase), das reale, von diesem ESP-IDF erzeugte
      `flasher_args.json` nennt es aber `"partition-table"` (mit Bindestrich)
      – ein Zugriff über die TS-Schnittstelle liefert dafür `undefined`,
      vermutlich nie bemerkt, weil dieser Wert praktisch nie gebraucht wurde.
      `FlasherArgs.cs` liest bewusst den ECHTEN Feldnamen, nicht den aus der
      TS-Schnittstelle.

      **Bewusst NICHT implementiert**: der verschlüsselte Pfad
      (`createRandomFlashEncryptionKeyLazily`/
      `encryptPartitions_Bootloader_App_PartitionTable_OtaData`/
      `burnFlashEncryptionKeyAndActivateEncryptedFlash`) – Letzteres brennt
      EFuses PERMANENT UND UNUMKEHRBAR. Kein aktuell bekanntes Board nutzt
      diesen Pfad; eine so folgenschwere, echte Hardware dauerhaft
      verändernde Operation gehört nicht "nebenbei" in eine
      Migrationsarbeit, sondern in einen eigenen, separat abgestimmten
      Schritt, falls tatsächlich einmal ein Board verschlüsselt werden soll.
      Ebenso ausgelassen: NVS-Überschreiben (in `gulpfile.ts` ohnehin per
      Konstante deaktiviert, bräuchte zusätzlich die noch nicht portierte
      Usersettings-NVS-Codegenerierung) – bewusst kein toter Code dafür
      mitgeschleppt.

      **Echt end-to-end verifiziert, mit expliziter Rückfrage vor der
      Ausführung** (echter Schreibzugriff auf das aktive, am Haus laufende
      Board – höheres Risiko als alles vorherige in dieser Session, da
      generierte Dateien beliebig oft neu erzeugt werden können, ein
      Flash-Vorgang dagegen die Firmware auf dem echten Gerät ersetzt):
      mit der vorhandenen (vom 2026-06-29 stammenden, da `BuildFirmware`
      mangels funktionierender ESP-IDF-venv keine frische Binary erzeugen
      konnte) `build/`-Firmware alle 5 Sektionen
      (Bootloader/App/Partition-Table/OTA-Data/Storage) erfolgreich
      geschrieben UND von `esptool` selbst verifiziert ("Hash of data
      verified" je Sektion), danach Hard-Reset über RTS-Pin – das Board lief
      nach dem Flash normal weiter. Damit sind 11 der 12
      Migrationsstrategie-Schritte real gegen das tatsächliche Board
      verifiziert (nur `BuildFirmware` bleibt wegen der kaputten venv offen,
      s. Schritt 10 – `FlashFirmware` selbst brauchte dafür keinen frischen
      Build, da `esptool`/`espefuse` unabhängig von `idf.py`/`export.bat`
      funktionieren).
- [ ] Für jeden migrierten Schritt: beide Wege (alt/neu) parallel gegen dasselbe Board
      laufen lassen und Ergebnis vergleichen (Diff der generierten Dateien), bevor der
      alte Gulp-Task entfernt wird.
- [ ] `builder/package.json`-Scripts (`i`, `f`, `u`, `c`, `wc`) erst löschen, wenn das
      C#-Äquivalent nachweislich das Gleiche tut.
- [ ] `docs/build-process.md` (Stufe 1) parallel nachziehen.

## Entschieden

- Das neue C#-Tool lebt im selben Repo (`sensact` → `firmware/sensact_firmware/`),
  nicht in einem eigenständigen Repo.
- `esptool.py`/`espsecure.py`/`espefuse.py` bleiben als Python-CLI-Aufrufe (über
  `ProcessRunner`, analog zum bisherigen `exec_in_idf_terminal`-Ansatz) – keine
  Nativisierung.
- SQLite-basierte Board-Typ-Historie wird **nicht** übernommen, dateibasiertes
  Konzept (`board_info.json`) bleibt Zielarchitektur – siehe eigener Abschnitt oben
  ("Bestätigt: SQLite war für ESP32-Boards eine Interims-Lösung...").
- `builder/` neben dem bestehenden `builder/` als Übergangszustand während der
  Migration ist ok (bestätigt).
- `ns09usersettings`-Umbau auf 4 eigenständige Classes statt einer Union-in-Class
  (s. Mapping-Tabelle oben) ist so gewünscht (bestätigt).

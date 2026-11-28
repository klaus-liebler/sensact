# Offene Punkte und Entscheidungsfragen

Lebende Liste. Neue Funde hier eintragen; Erledigtes mit Datum nach unten unter "Erledigt"
verschieben. (Der frühere Backlog `plan_v2/04-architektur-review.md` steht nur noch in der Git-Historie
bis 5114fa3; dort noch offene Punkte – `main/__archive/`, absolute Pfade, HAL-Duplikate, Login/HTTPS –
gelten weiter.)

## Entscheidung nötig

Zurzeit keine.

## Offen (technisch)

| # | Thema | Betroffen |
|---|---|---|
| T1 | **Firmware mit den App-Korrekturen vom 2026-11-16/17 ist gebaut (`SNSCT_L0_TECH_HS_1`, `SNSCT_NODE_TERRASSE`, `SNSCT_NODE_SIDEDOOR`), aber noch nicht geflasht und am Objekt erprobt.** `sdkconfig` ist nicht eingecheckt (`.gitignore`), maßgeblich ist `sdkconfig.defaults`; die lokale `sdkconfig` ist älter als die Defaults – bei Gelegenheit löschen und neu erzeugen lassen. Für den OTA-Rollback (T10) muss der **Bootloader einmal per USB** geflasht werden. Die früher ungeklärten Image-Größen (1,31 vs. 0,95 MB) waren ein Build-Fehler: CMake las `config.json` nur beim ersten Konfigurieren, ein Build für ein anderes Board übernahm Board-Init und NODE_ID des zuvor gebauten (behoben, s. Erledigt). Seit Schritt 2 enthält jede Firmware alle App-Typen (die Konfiguration entscheidet zur Laufzeit), daher 1,3–1,6 MB (≥ 18 % frei). Am Gerät prüfen, dass Ethernet und Webinterface laufen. | Firmware |
| T12 | **Beim nächsten Flashen von `SNSCT_NODE_SIDEDOOR` und `SNSCT_NODE_UP` gehen die gespeicherten Scheduler-Zeitpläne verloren** (BestBinaryBuffers-Formatänderung, Verlust akzeptiert). Nichts manuell löschen: die Firmware erkennt nicht lesbare Einträge beim Start, loggt sie und löscht sie aus dem NVS. Zeitpläne danach im Web-Interface neu anlegen. | Firmware SIDEDOOR/UP |
| T14 | **Stationspaket und datengetriebene Basis am Gerät erproben** (`SNSCT_L0_TECH_HS_1`): per USB flashen (`Pipeline --model Sattlerstrasse16`, schreibt Paket und Web-UI gleich ins LittleFS). Erwartet: Log "Station configuration loaded: SNSCT_L0_TECH_HS_1, … apps", alle Taster/Lampen/Rollläden wie bisher, Web-UI mit allen Apps. Dann eine Modelländerung (`BuildPackage`) über System → Station Package hochladen: Gerät startet neu und arbeitet mit der neuen Konfiguration. Paket einer anderen Station/Basis wird abgelehnt; ohne Paket Sicherheitsmodus mit Wartungsseite. OTA und Datei-Endpunkte verlangen den Login. | Firmware/Webmanager |
| T15 | **Neue Weboberfläche und Firmware-Log am Gerät ansehen** (mit T14): Arbeitsbereiche, Hell/Dunkel, Vollbild, Abmelden; Terminal zeigt die Startmeldungen (Puffer 48 Zeilen) und neue Zeilen live, `loglevel * debug` wirkt, die Glocke zählt Warnungen/Fehler. Ohne Gerät schon erprobt (Testserver, `npm run it`). | Web-UI/Webmanager |
| T16 | **Neue F103-Firmware am Gerät erproben** (`firmware/sensact_firmware_f103`, rtoos; zuerst `SNSCT_L1_LVNG_UP`): zuerst Bootloader und Firmware per SWD (Reset-Modus, weil die Altfirmware SWD abgeschaltet hatte), danach über CAN (T18). Prüfen: Taster IO1–IO8, Drehgeber (Richtung, Rastung), Lampen über PCA9685 0x40/0x41, Kommandos zu und von den ESP32-Stationen über CAN, Verhalten bei abgezogenem/gestörtem I2C-Kabel (Konsole: Zähler `recover`, `reinits`). Siliziumrevision auf dem Chip ablesen (ES096 2.2.9: Z/B nicht unterstützt). Im Emulator (Renode) schon erprobt, s. README dort. | F103-Stationen |
| T17 | **WS2812/IDENTIFY ausrollen:** Neuer App-Typ `rgbIndicator`, neues Kommando `IDENTIFY` und das erweiterte Konfigurationsschema ändern die Kompatibilitätskennung der Stationspakete: die ESP32-Stationen brauchen die **neue Basis-Firmware und neue Pakete** (sonst lehnen sie die Pakete ab). Erst dann sendet der Klingeltaster `PUSHB_LX_FRON_B1` (auf `SNSCT_L0_TECH_HS_1`) das Türklingelmuster auch an Wohnzimmer und Küche. ESP32-Build mit den neuen Quellen noch nicht ausgeführt (Host-Tests grün). Am F103: Sichtbarkeit von D4 und den Ecken durch die Abdeckung, Helligkeit der Rückmeldung am Knopf beurteilen. | ESP32-Stationen, F103 |
| T18 | **CAN-Bootloader am Gerät erproben** (`firmware/sensact_firmware_f103/bootloader`, 2026-11-28): einmalig `sensact_f103_boot.bin` an 0x08000000 und `sensact_f103.bin` an 0x08004000 per SWD. Prüfen: Konsole zeigt "sensact bootloader 1", nach 1 s "starting firmware" und die Firmware startet; `reboot --station 5 --bootloader` → 10 s Fenster; `flash` mit USB-CAN-Adapter (~15 s für 54 KB); Abbruch mitten im Update (Stecker ziehen) → Station bleibt im Bootloader (LED 10 Hz), erneutes `flash --station 5` geht. Erst gebaut mit GCC 13 (Syntax, Größe 5,5 KB), noch nicht mit GCC 16 und nicht im Emulator. Die ESP32-Basis braucht den neuen `ParseCanMessageType` (Bootloader-Frames sind kein App-Kommando); bis dahin landen sie dort als Kommando an App 0/256/512/768, die es nicht gibt. | F103-Stationen, ESP32-Basis |
| T7 | **Events werden nicht ausgewertet** (`cApplicationHost::OfferMessage` → `NOT_YET_IMPLEMENTED`) – bewusst, "for future use". | `node_applicationhost.cc` |

## Erledigt

### 2026-11-24 (2) – WS2812 der sensactUp-Stationen, IDENTIFY, rtoos zusammengeführt

- **A – Rückmeldung am Knopf (D4)** in der F103-Firmware: nach Taster/Drehgeber die Helligkeit eines
  lokalen Dimmers (warmweiß, 2,5 s), grün für ein anderes lokales Ziel, blau für ein Kommando über CAN;
  zweimal kurz rot alle 5 s, solange ein PCA9685 nicht antwortet oder CAN gestört ist.
- **B – Signal-App `rgbIndicator`** (gemeinsame Apps, Modell, Builder, Schema, Host-Tests): Dauerlicht
  (Farbe, Helligkeit) und Muster (Türklingel, Alarm, Hinweis, OK, Fehler). Wohnzimmer und Küche haben
  je eine für die vier Ecken (`SIGNL_L1_LVNG_CORNERS` 363, `SIGNL_L1_KTCH_CORNERS` 364,
  `toggleSignal="1"`); der Klingeltaster `PUSHB_LX_FRON_B1` schaltet sie mit (s. T17).
- **C – `IDENTIFY(ms)`** an die Knoten-App (Kommando 32, hinten angefügt): alle WS2812 blinken blau.
- WS2812 über TIM1_CH1 + DMA1 Kanal 5 (rtoos: neuer F1-DMA-Treiber); Versorgung U1 nur, solange etwas
  leuchtet. Emulator (Renode, eigenes F1-DMA-Modell in rtoos, CAN-Hub mit `CanInjector.cs`): IDENTIFY,
  Türklingel, Orientierungslicht, Rückmeldung und CAN-Verkehr erprobt.
- rtoos: die lokale EtherCAT/Security-Linie aus dem Submodul von factory_in_a_box mit dem F103-Port
  zusammengeführt (`27bb6d0`), factory_in_a_box zeigt auf den neuen Stand.

### 2026-11-24 – STM32F103-Stationen auf rtoos mit den Apps der ESP32-Basis

- Platine von `SNSCT_L1_LVNG_UP` (und `SNSCT_L1_KTCH_UP`) bestimmt: sensactUp **Rev 0.2** aus dem
  Initial-Commit (`pcb/sensactUp`, 2015; der heutige Ordner `pcb/sensactUp` ist eine andere Platine
  mit STM32F072), STM32F103CBT6, 12-MHz-Oszillator. Pinbelegung aus der Leiterplatte, gegen die
  Altfirmware geprüft. Fehler der Altfirmware: SWD abgeschaltet, HSE im Quarz- statt Bypass-Modus,
  5-V-Freigabe Open-Drain ohne Pull-up (schwebend), DCF77 auf PC4 (gibt es auf dem LQFP48 nicht).
- Entscheidung: **rtoos** statt der Altfirmware (Variante L3 aus `concept-modular-firmware.md`, mit
  einkompilierter Stationskonfiguration statt Code-Export). In rtoos neu: Cortex-M3-Port und
  Chip-Port `stm32f103` mit Errata ES096, Selbsttest in QEMU und Renode (rtoos `471a85e`).
- Neue Firmware `firmware/sensact_firmware_f103`: die Apps der ESP32-Basis, Stationskonfiguration
  aus dem Builder (`GenerateStm32F103Files --model … --nodeId …`), PCA9685 in einem eigenen Thread
  mit Bus-Recovery (lange Kabel). Katalog: `SensactUp02` hat jetzt die Firmware-Familie `stm32f103`,
  Bus `INTERNAL` mit benannten Pins (Nummern unverändert), Bus `BB` als I2C; die Stationen deklarieren
  ihre PCA9685 (`PCA0` 0x40, LVNG zusätzlich `PCA1` 0x41, wie in der Altfirmware).
- `station_config.cc`: App-Typen per `SENSACT_WITHOUT_SOUND/FINGERPRINT/MILIGHT/BLINDSTIMER`
  abwählbar (ESP32 und Host-Tests unverändert: alle dabei; 161 Host-Tests grün).

### 2026-11-17 (6) – Weboberfläche: Anwendungs-Shell aus factory_in_a_box, Firmware-Log

- Shell (Titelleiste, Arbeitsbereich-Umschalter mit Ribbon, Statusleiste, Hell/Dunkel, Vollbild) als
  gemeinsames Paket `@klaus-liebler/ui-shell` nach `npm-packages` verschoben; factory_in_a_box nutzt es
  von dort. `AppController` (web-components) baut sensact und labathome darauf auf: jeder ScreenController
  ist ein Arbeitsbereich, Startseite mit Kacheln, Benutzer/Abmelden in der Titelleiste, Build und Modell in
  der Statusleiste. Altes Grid-Layout und Google-Font entfernt.
- Firmware-Log im Browser (opt-in): `FirmwareLogPlugin` im Webmanager, Namespace `firmwarelog`, in sensact
  eingeschaltet; Arbeitsbereich Terminal mit Filtern, Export und Konsole. labathome hat den Namespace im
  Protokoll, das Plugin aber nicht angemeldet.
- Nebenbei: Dev-Proxy (`web/vite.config.mts`) verwarf Nachrichten, die der Browser vor dem Aufbau der
  Verbindung nach oben schickte (Anfragen direkt nach dem Verbinden gingen verloren) – jetzt gepuffert.
  Testserver: `ApplicationId` aus `@generated/sensact_applicationids` (seit Schritt 2 nicht mehr im
  Protokoll), simuliert das Firmware-Log.

### 2026-11-17 (5) – Schritt 2: datengetriebene Basis

- Basis-Firmware stationsneutral: App-Fabrik (`sensact-applicationmodel/station_config.cc`) legt
  die Apps zur Laufzeit aus der Stationskonfiguration an (Abschnitt 2 des Stationspakets, Schema
  `best_binary_buffers_schema/sensactcfg.cs`), Register `cApplications` statt generierter Tabellen,
  `eApplicationID` ohne Hausnamen, `eNodeID`/Knotenname aus der Konfiguration, Board-Init je
  Board-Typ mit Erweiterungs-Chips aus der Konfiguration, Wartungsseite statt eingebetteter Web-UI,
  Sicherheitsmodus ohne Konfiguration. Details: `docs/concept-modular-firmware.md` (Stand Schritt 2),
  `docs/architecture.md` ("Stationskonfiguration").
- BestBinaryBuffers: `Runtime.BinaryEncoder` (C#-Encoder auf dem geparsten Schema) und
  `SchemaCompiler.Parse`; Rundlauf-Test C# == TS == C++ (byte-gleich).
- Builder: `StationConfigWriter`, `BuildPackage` (nur Paket nach Modelländerung), `BuildForOta`
  (Firmware + Paket), `CollectDeployFiles`; Basis-Kennung unabhängig vom Hausmodell.
- ws-Protokoll: App-IDs als `uint16_t`; Web-UI-Namen aus `@generated/sensact_applicationids`
  (npm-packages: sensact-base, web-components-sensact angepasst).
- Host-Tests (161): Fabrik legt für alle ESP32-Stationen von Testmodel und Sattlerstrasse16 genau
  die Apps des bisherigen Code-Generators an; beschädigte/fremde Konfiguration wird abgelehnt.
- Behoben, beim Bauen aller Board-Typen gefunden: (1) CMake konfigurierte nach einem Board-Wechsel
  nicht neu (`config.json` jetzt `CMAKE_CONFIGURE_DEPENDS`) – Builds für ein anderes Board enthielten
  die Board-Init des vorher gebauten; erklärt die Größen aus T1. (2) `hal_sensactUp3.hh` kompilierte
  nie (fehlende `PlayNotes`, `I2C_INTERNAL`). (3) Host-Test-CMake erzeugte die Testdaten bei
  geänderten Builder-Quellen nicht neu. (4) `cApplication` ohne virtuellen Destruktor.
- Offen: Runtimeconfig/Zertifikat aus NVS (Firmware weiterhin je Gerät gebaut, aber nicht mehr je
  Hausmodell); `MONITORING` hat keine Stationskonfiguration und meldet sich mit `eNodeID::UNKNOWN`.

### 2026-11-17 (4) – XML-Hausmodell ist die Quelle der Wahrheit

- Schema (`model/schema/*.xsd`), Katalog (`model/catalog.xml`) und Hausmodelle
  (`model/Sattlerstrasse16.xml`, `model/Testmodel.xml`); `GenerateModelFiles` und die Host-Tests
  lesen das XML. C#-Hausmodelle, `ModelBuilder`, Exporter und die nicht mehr benötigten
  C#-Node-Klassen gelöscht. Generierte Dateien vor/nach der Umstellung identisch (bis auf
  den Erzeugungszeitpunkt). Details: `model/README.md`.
- App-Nummern stehen jetzt ausdrücklich im XML; vorher ergaben sie sich aus der Reihenfolge
  im C#-Enum (eine eingefügte Zeile hätte alle folgenden IDs verschoben).
- Die C#-Node-Klassen passten nicht zur realen Hardware (`SNSCT_L0_TECH_HS_1` als
  `SensactHs08`, `SNSCT_L3_WORK_HS` als `SensactUp02`); das XML nennt die realen Boards.
- Board-Typ einer NODE_ID kommt aus der `<station>` im XML (`NodeIdBoardTypeCatalog.cs` war eine
  zweite, von Hand gepflegte Tabelle); nur `MONITORING` (keine Station eines Hauses) steht noch dort.
- Prüfungen beim Laden gegen den Katalog (`HouseModelChecker`): Erweiterungen, Adressen, doppelt
  belegte Ein-/Ausgänge mit lesbaren Namen, Hardware-Voraussetzungen der Apps, alle Fehler mit
  Zeilennummer. Weboberfläche (`sensactapps*.ts.inc`) nachweislich unverändert.
- Kommando-Prüfung war wirkungslos: `CheckAndPrepare` las gesendete Kommandos nur aus Feldern
  `Command…`, die keine heutige App hat. Jetzt meldet jede App-Klasse `SentCommands()` (aus den
  `ctx->Send…Command()`-Aufrufen der C++-Apps), der Generator prüft gegen
  `ICanReactOnTheseCommands()` des Ziels. Beide Hausmodelle bestehen. Dabei gefunden:
  `RgbwApplication.ICanSendTheseEvents` war fälschlich als Kommando-Methode markiert.
- `builder/ModelGeneration` → Bibliothek `modellib/` (gemeinsam mit dem Editor); Builder, Bibliothek
  und Editor auf .NET 11.
- Editor, erste Version (`editor/`, Selbsttest `editor_tests/`), s. `editor/README.md`.
- Hausmodell `schemaVersion` 2: Zeiten als ISO-8601-Dauer (`autoOff="PT30M"` statt
  `autoOffMs="1800000"`, `upTime="PT40S"` statt `upTimeSec="40"` …), Helligkeiten in Prozent
  (`minimalBrightness="0.781"` statt `minimalOnLevel="512"`). Umrechnung beim Import
  (`Units`), generierter Code unverändert. Lautstärke (`sound.initialVolume`) bleibt Rohwert,
  weil ihre Bedeutung vom Verstärker abhängt (TAS5805M: invertiert).
- Stationspaket, Schritt 1 (s. Konzept, "Web-UI als Teil des Stationspakets"): Paketformat,
  `POST /package`, Auslieferung der Web-UI aus dem Paket mit Rückfall auf die eingebaute,
  Neuladen nach dem Upload, `BuildForOta`/`BuildStationPackage`. Gerät und Builder lesen das
  Format nachweislich gleich (Host-Test gegen ein echtes Paket); am Gerät noch nicht erprobt (T14).
- Sicherheitslücke geschlossen: `POST /ota` (Firmware-Update) und `/files/*` (Dateien lesen,
  schreiben, löschen) prüften den Login nicht – jetzt 401 ohne gültige Session (bei abgeschaltetem
  Login unverändert erlaubt). Betrifft auch labathome (gemeinsamer Webmanager); Skripte, die dort
  ohne Login hochladen, brauchen jetzt ein Session-Cookie.

### 2026-11-17 (3) – E13, E14, T13, Türklingel-Ton

- **E13**: Quellcode der STM32-Altfirmware als normale Dateien eingecheckt:
  `firmware/sensact_firmware_stm32/` (PlatformIO, aus `firmware/firmware_stm32.7z`) und
  `firmware/firmwareCubeIDE/` (STM32CubeIDE-Projekte, aus `firmwareCubeIDE.7z`); Archive entfernt
  (in der Git-Historie weiter vorhanden). Nicht übernommen: Eclipse-`.metadata`, Build-Ordner,
  CMSIS-Binärbibliotheken/DSP-Beispiele/Doku und Gerätheader nicht verwendeter Controller
  (s. `firmware/firmwareCubeIDE/.gitignore`, alles aus STM32Cube wiederherstellbar). Das
  WLAN-Passwort in `esp8266ntp/esp8266ntp.ino` ist durch Platzhalter ersetzt.
  `firmware/sensactOutdoor.7z` ist unverändert geblieben.
- **E14** (CI per GitHub Actions): entschieden – nein. Host-Tests laufen über den Builder
  (`RunHostTests`).
- **T13** (Scheduler-Standort Greven fest eincodiert): bewusst so belassen.
- **Türklingel spielte zu schnell und nur auf einem Lautsprecher** (`SNSCT_L0_TECH_HS_1`,
  TAS5805M): zwei Softwarefehler, kein Hardwaredefekt.
  - `aI2sCodecManager::WriteAudioData` (espidf-components/audio) schrieb bei Stereo-Material nur
    die halbe Datenmenge (Samples pro Kanal statt L+R-Werte) – von jedem dekodierten Block fehlte
    die zweite Hälfte, die Wiedergabe klang gehetzt/zu schnell. Alle vier eingebetteten MP3s sind
    Stereo (dingdong/siren 44,1 kHz, positive/negative 24 kHz).
  - Die aus ESP-ADF (Korvo DU1906, ein Lautsprecher) übernommene TAS5805M-DSP-Konfiguration
    mischte (L+R)/2 nur auf den linken Ausgang, der rechte bekam 0. Jetzt bekommen beide
    Ausgänge (L+R)/2.
  - Nebenbei: `TAS580x::SetControlState` verknüpfte mit `&&` statt `&` (Hi-Z beim Init wurde zu PLAY).
  - Am Gerät noch zu prüfen (s. T1).

### 2026-11-17 (2) – Antworten zu E5–E12, T2–T10 umgesetzt

- E5: Klingel ab Werk laut von 07:00 bis 22:00.
- E6: `cOnOff` mit `initialState=ACTIVE` und Auto-Off startet *aus* – bewusste Annahme, bleibt.
- E7: Taster lesen ihren Startzustand in `Setup()`: ein beim Booten gedrückter oder klemmender
  Taster löst nichts aus, bis er losgelassen wird (vorher TOGGLE bzw. Dauerfahrt beim
  Rollladentaster); kein Doppelklick direkt nach dem Booten.
- E8: `SNSCT_L3_TECH_HS_1`, `SNSCT_L1_KTCH_UP`, `SNSCT_L1_LVNG_UP` sind STM32-Hardware.
- E10: Hausmodell des Editors als **eine** XML-Datei. E11: STM32-Altknoten über Code-Export (L1).
  E12: Web-Login genügt als Absicherung der Uploads.
- T2: `iSensactContext::GetUnixTime`/`GetLocalTime` (Wanduhr, `false` solange kein SNTP);
  Tag/Nacht-Lautstärke der Klingel damit testbar, unbekannte Uhrzeit = laut.
- T3: `cBlindsTimer` fertig: öffnet/schließt zur Morgen-/Abenddämmerung + Versatz + Zufall je
  Rollladen und Tag, per `SET_VERTICAL_TARGET` aus dem Überfahrbereich; Ein/Aus bleibt über
  Neustart erhalten; nichts ohne Uhr, keine nachgeholten Zeitpunkte. Im Builder:
  `BlindsTimerApplication`, `ModelBuilder.AddBlindsTimer(...)` (Standort als Parameter).
- T4: `cMilightController` ohne FreeRTOS/ESP-IDF (lock-freier Ringpuffer statt Queue,
  Adapter zum Funkempfänger im Board-Init der Terrasse); Ziele (Licht, Rollläden je Tastengruppe,
  Einschaltdauer) aus dem Modell; host-getestet.
- T5: kein `using namespace std;` mehr in Headern; `TAG`-Makros in `pushbutton.hh`,
  `rotaryencoder.hh`, `fingerprint.hh`, `led/crgb.hh`, `rgbled/crgb.hh` entfernt (das `#undef TAG`
  am Ende löschte das `TAG` einbindender Dateien).
- T6: `sunsetsunrise.hh` überarbeitet: `DaysSinceJan1_2000UTC`, `std::numbers::pi`, Konstanten
  statt Makros, keine Debug-`printf`s mehr, neue Funktionen `SunriseAndSunsetOfDay` und
  `NextSunriseAndSunset` (nächste *zukünftige* Ereignisse, `false` bei Polartag/-nacht), gegen
  Referenzwerte für Greven getestet. Dabei gefunden: der Webmanager-Scheduler übergab die
  Tageszahl statt der Unix-Zeit und Grad statt Bogenmaß – sonnenstandsabhängige Zeitpläne
  rechneten mit einem Tag im Januar 1970; korrigiert.
- T8: Builder-Schritt `RunHostTests`, Teil von `Pipeline` und `PipelineOffline`: ein roter Test
  bricht den Build ab. (Automatisch bei jedem Push: s. E14.)
- T9: `typescript` + `npm run typecheck` (auch `npm test`) in `web-components-sensact`; läuft
  fehlerfrei.
- T10: OTA-Upload-Handler: Fehler → HTTP-Fehlerantwort statt Neustart, Timeout-Wiederholung,
  `esp_ota_abort` bei Abbruch, Images anderer Projekte werden abgelehnt; App-Rollback in
  `sdkconfig` aktiviert (wirkt nach einmaligem USB-Flash des Bootloaders, s. T1).

### 2026-11-17 – Entscheidungen Modularisierung/Editor

- Editor: ASP.NET Core mit Blazor; alle Konfigurationsdaten (Hausmodell und Katalog der
  Boards/Hardware-Features) als versionierbare XML-Dateien; BestBinaryBuffers nur als
  Geräteformat. STM32-Altknoten (F103/F407) bleiben und werden berücksichtigt (s. E11).
- E9: Listen innerhalb einer App → BestBinaryBuffers erweitert (Listen auch in Klassen,
  BestBinaryBuffers `8aef43f`).

### 2026-11-16 (2) – Entscheidungen E1–E4 umgesetzt

- E1: Minimalhelligkeit der Wandlampen `PWM___L1_KTCH_W1`/`PWM___L1_LVNG_W1` (und Testmodel) =
  `MIN_DIM_LEVEL_5_PERCENT_PWM` (`ModelBuilder.cs`, Level 59*256 → `level2pwm[59]` = 4,95 % PWM).
- E2: `cBlind` führt die Position nach dem Booten als unbekannt: keine Komfortfunktion, Fahrten
  in eine Endlage laufen über den vollen Weg; bekannt nach der ersten ununterbrochenen Fahrt
  von mindestens voller Fahrzeit.
- E3: Dimmgeschwindigkeit 5 s für den vollen Bereich bestätigt.
- Firmware ließ sich seit `ba5554c` nicht linken (`vTaskGetRunTimeStats` ohne die nötigen
  FreeRTOS-Optionen im getrackten `sdkconfig`): `nodemaster.hh` loggt die CPU-Last jetzt nur,
  wenn die Optionen aktiv sind; `sdkconfig.defaults` um die zwei fehlenden Optionen ergänzt.
- E4: `SET_VERTICAL_TARGET` beim Rollladen nach Klaus' ursprünglicher Intention: der u16 ist die
  interne Positionsskala ohne die unteren 15 Bit (vorher fälschlich `<< 16`, ein Bit zu weit):
  16384..49151 = Teilöffnung 0..100 %, darunter/darüber sicher ganz zu/auf (Überfahren bis zum
  mech. Endschalter), 0/65535 = Dauerbetrieb bis zum nächsten Kommando. `UP`/`DOWN(1)`
  (forced) ebenfalls Dauerbetrieb bis `STOP`.
- T11: Sicherheitsgrenze für Rollladen-Dauerbetrieb: spätestens nach 10 Fahrzeiten am Stück
  Stopp (falls das beendende Kommando verloren geht).

### 2026-11-16 – Qualitätssicherung der Apps (Host-Unit-Tests eingeführt)

Host-Testprojekt `espidf-components/sensact-applicationmodel/host_test` (119 Tests), dabei
gefunden und behoben:

**Rollladen (`cBlind`)**
- Bei den vier Verschaltungen mit Leistungs- und Richtungsrelais wurde das Richtungsrelais
  beim Vorbereiten der Gegenrichtung nicht zurückgesetzt: ein schneller Doppeldruck (Stopp,
  dann Gegenrichtung innerhalb von 500 ms) ließ den Motor **in die falsche Richtung** fahren,
  während die App die richtige meldete. (`BlindAllModes.QuickDoublePressReversesDirectionCorrectly`,
  `BlindRandomized.*`)
- `SET_VERTICAL_TARGET`: `target << 16` machte Werte ≥ 32768 negativ (Fahrt endlos nach unten),
  0 bedeutete "Stopp". Neu definiert, s. E4.
- Ein neues Ziel in Gegenrichtung während der Fahrt ging verloren (nur Stopp).
- Zwischenziele wurden eine Sekunde Fahrweg zu früh erreicht (Hysterese auch beim Anhalten).
- Start/Stopp wurden nicht als `OK_CHANGED` gemeldet → Web-UI zeigte den Endzustand verzögert.
- Positionsrechnung in 64 Bit mit Begrenzung (Überlauf bei stark verspätetem `Loop()`).

**Licht (`cSinglePWM`, `cOnOff`, `cRgbw`)**
- Dimmen per Gedrückthalten war seit der Umstellung 0..255 → 0..65535 praktisch wirkungslos
  (±1 pro Loop, voller Bereich ≈ 20 min). Jetzt zeitbasiert, s. E3.
- Ging das `STOP` nach dem Dimmen verloren, blieb das Dimmen für immer aktiv: kein Auto-Off
  mehr und Statusmeldung in *jedem* Loop.
- Ein ausstehendes Auto-On (`OFF(ms)`) wurde durch spätere Bedienung nicht verworfen – die Lampe
  schaltete sich später scheinbar grundlos ein.
- Mit `minimalOnLevel = 0` (z. B. Terrassen-Spots) ließ sich die Lampe nach "Slider auf 0" nie
  wieder einschalten (gespeicherte Helligkeit 0). `SET_VERTICAL_TARGET(0)` = aus.
- Erstes `START` in den ersten 5 s nach dem Booten dimmte abwärts.
- In den ersten 5,5 s nach dem Booten kein Heartbeat → Standby-Netzteil blieb aus.
- `cOnOff`: ein `HEARTBEAT` verkürzte ein dauerhaft eingeschaltetes Relais auf 30 s; Schalten
  wurde nicht als `OK_CHANGED` gemeldet.
- `cRgbw`: Auto-Off wurde nie ausgewertet; Änderungen nicht als `OK_CHANGED` gemeldet;
  `TOGGLE` stellte eine per `SET_RGBW` gewählte Farbe nicht wieder her; großer negativer
  `STEP_VERTICAL` (Drehgeber) sprang auf eine falsche Farbe.

**Sonstige Apps**
- `cRotaryEncoder2PWM`: Zählerüberlauf (65535 → 0) wurde als 65535 Rastungen rückwärts
  gewertet; schnelles Drehen (≥ 8 Rastungen pro Takt) lief über int16 ins Gegenteil.
  Header war nicht eigenständig kompilierbar (`N()`/`NID()` ohne `cApplications.hh`), ebenso
  `pushbutton.hh` (fehlendes `<vector>`).
- `cPump`: Zustand wurde nie umgeschaltet (Relais dauerhaft an) und mit 1 statt `ACTIVE`
  angesteuert.
- `cSound`: ungültige Sound-Nummer rief `PlayMP3(nullptr, 0)` auf.
- `cBlindsTimer`: sendete nach Aktivierung in jedem Loop `UP` und `DOWN` – an sich selbst.
- `cFingerprint`: falscher Log-Tag; negativer Index möglich.
- `sunsetsunrise.hh` (Repo `espidf-components`): Nicht-`inline`-Funktionen im Header (ODR).

**Web-UI (`npm-packages/.../web-components-sensact/sensactapps_base.ts`)**
- OnOff las den Zustand aus Slot 0 statt 1 (zeigte immer "aus"), BlindsTimer ebenso; Sound
  zeigte die Sound-Nummer als Lautstärke und "spielt" nie an.

**Dokumentation**
- `architecture.md`: 20-ms-Tick mit Bucket-Rotation statt "100-ms-Zyklus", Webmanager-Abschnitt
  aktualisiert, Links auf App-Referenz und Tests.
- `repo-map.md`: fehlendes Repo `espidf-components` ergänzt, Flatbuffers → BestBinaryBuffers,
  nicht mehr existierendes `builder/Paths.cs` ersetzt.
- `build-process.md`: "node-forge" → .NET, `Paths.cs`, neue Phase
  `GenerateModelFilesForHostTests`, Abschnitt "Tests".
- Neu: App-Referenz `espidf-components/sensact-applicationmodel/README.md` (Kommandos und
  Statusbelegung aller Apps) und `host_test/README.md`.

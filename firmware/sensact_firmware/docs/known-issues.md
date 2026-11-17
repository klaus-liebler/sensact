# Offene Punkte und Entscheidungsfragen

Lebende Liste. Neue Funde hier eintragen; Erledigtes mit Datum nach unten unter "Erledigt"
verschieben. (Der frühere Backlog `plan_v2/04-architektur-review.md` ist archiviert; dort noch
offene Punkte – `main/__archive/`, absolute Pfade, HAL-Duplikate, Login/HTTPS – gelten weiter.)

## Entscheidung nötig

| # | Thema | Stand / Vorschlag |
|---|---|---|
| E5 | **Klingel-Lautstärke ab Werk.** Default-Zeitfenster Beginn = Ende = 00:00 heißt "nie laut" – die Klingel läuft mit `lowVolume` (25 %), bis jemand das Fenster einstellt und speichert. | Default z. B. 07:00–22:00? |
| E6 | **`cOnOff` mit `initialState=ACTIVE` und Auto-Off** startet *aus* (bewusste Annahme im Code). | So lassen oder "an nach Boot, aus nach Auto-Off-Zeit"? |
| E7 | **Taster beim Booten gedrückt/klemmt** → ein `TOGGLE` (Flanke gegen den Startwert "losgelassen"). | Startzustand in `Setup()` einlesen? |
| E8 | **Knoten ohne ESP32-Board-Init**: `SNSCT_L3_TECH_HS_1`, `SNSCT_L1_KTCH_UP`, `SNSCT_L1_LVNG_UP` stehen im Hausmodell, haben aber kein `main/hal/<NODE_ID>/` und keinen Eintrag in `NodeIdBoardTypeCatalog.cs`. | Laufen diese noch mit der STM32-Firmware? Dann hier so vermerken. |
| E9 | **Modularisierung (zurückgestellt): Listen innerhalb einer App** – BestBinaryBuffers erlaubt in Klassen keine variablen Listen (`cSinglePWM`-Ausgänge, Toggle-Ziele, RGBW-Farben). | Feste Kapazität, BestBinaryBuffers erweitern (Empfehlung) oder eigene Listenelemente? S. [concept-modular-firmware.md](concept-modular-firmware.md) |
| E10 | **Modularisierung: Format der Hausmodell-Datei** des Editors. | Binär (BestBinaryBuffers) mit Text-Export für Git, oder Hausmodell als Text und nur die Stationsdateien binär? |
| E11 | **Modularisierung: STM32-Stationen** (hängt mit E8 zusammen). | Alle Knoten auf ESP32, oder muss der Editor auch für STM32 exportieren? |
| E12 | **Modularisierung: Absicherung des Uploads** von Basis-Firmware und Konfigurationen. | Web-Login ausreichend, oder Signatur (Secure Boot ist nicht aktiv)? |

## Offen (technisch)

| # | Thema | Betroffen |
|---|---|---|
| T1 | **Firmware mit den App-Korrekturen vom 2026-11-16 ist gebaut (`SNSCT_L0_TECH_HS_1`, `idf.py build` erfolgreich), aber noch nicht geflasht und am Objekt erprobt.** Getrackte `sdkconfig` weicht von `sdkconfig.defaults` ab (z. B. FreeRTOS-Laufzeitstatistik aus) – bei Gelegenheit `sdkconfig` neu erzeugen und Unterschiede sichten. | Firmware |
| T12 | **Beim nächsten Flashen von `SNSCT_NODE_SIDEDOOR` und `SNSCT_NODE_UP` gehen die gespeicherten Scheduler-Zeitpläne verloren** (Fingerprint-Türzeiten): BestBinaryBuffers hat seit 2026-11-17 ein neues Format für polymorphe Felder (Byte-Längenpräfix), alte NVS-Einträge sind nicht mehr lesbar. Entscheidung: Verlust akzeptiert, keine Migration. Zeitpläne danach im Web-Interface neu anlegen. | Firmware SIDEDOOR/UP |
| T2 | **Keine Wanduhrzeit in `iSensactContext`.** `cSound` ruft `::time()` direkt auf (Tag/Nacht nicht testbar), `cBlindsTimer` kann ohne Uhrzeit nicht implementiert werden. Vorschlag: `virtual bool GetLocalTime(tm&)` o. ä. im Interface, implementiert in `cApplicationHost`. | `iSensactContext.hh`, `node_applicationhost.cc`, `sound.cc`, `blindstimer.cc` |
| T3 | **`cBlindsTimer` unfertig** (Zeitberechnung fehlt; löst derzeit nie aus). Im Builder gibt es keine zugehörige Application-Klasse – die App ist im Modell nicht verwendbar. | `blindstimer.cc` |
| T4 | **`cMilightController` hängt an FreeRTOS/`esp_timer` und an fest eincodierten App-IDs von Sattlerstrasse16** (`PWM___LX_BACK_C1`, `BLIND_LX_BACK_J2/J3`) – kompiliert mit keinem anderen Modell, nicht host-testbar. Ziele gehören ins Modell (Konstruktorparameter). | `milightcontroller.cc` |
| T5 | **Header-Hygiene**: `using namespace std;` auf Dateiebene in `singlepwm.hh`, `rgbw.hh`; `TAG`-Makro in Headern definiert (`crgb.hh` aus `espidf-components/led` definiert `TAG "CRGB"` für jeden Includer, außerdem `pushbutton.hh`, `rotaryencoder.hh`, `fingerprint.hh`) → Makro-Neudefinitionen je nach Include-Reihenfolge. | apps, `espidf-components/led` |
| T6 | **`sunsetsunrise.hh`**: Funktionsname `DaysSinceJan1_2020UTC` rechnet ab 1.1.2000; nutzt nicht standardisiertes `M_PI`. (Nicht-`inline`-Funktionen im Header wurden am 2026-11-16 behoben.) | `espidf-components/common` |
| T7 | **Events werden nicht ausgewertet**: `cApplicationHost::OfferMessage` beantwortet Application-Events mit `NOT_YET_IMPLEMENTED`. | `node_applicationhost.cc` |
| T8 | **Host-Tests nicht in der Pipeline, kein CI.** | Builder |
| T10 | **OTA-Upload-Handler** (`espidf-component-webmanager/cpp/webmanager_base.hh`, `handle_ota_post`): `ESP_ERROR_CHECK(esp_ota_begin(...))` startet das Gerät bei einem Fehler neu; ein Empfangs-Timeout wird als fataler Fehler behandelt (der `HTTPD_SOCK_ERR_TIMEOUT`-Zweig ist unerreichbar, weil `recv_len <= 0` vorher greift); bei Abbruch fehlt `esp_ota_abort`; App-Rollback ist nicht aktiviert. Voraussetzung für das Modularisierungskonzept, s. [concept-modular-firmware.md](concept-modular-firmware.md). | Webmanager |
| T9 | **Web-UI ohne Typprüfung lokal**: im `web-components-sensact`-Paket ist kein `typescript` installiert, `tsc` lief für die Änderungen vom 2026-11-16 nicht. | `npm-packages` |

## Erledigt

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

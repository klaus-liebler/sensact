# sensact-applicationmodel – die Sensact-Apps

Eine *App* ist die kleinste adressierbare Einheit des Systems (Taster, Rollladen, Dimmer, …),
eindeutig über ihre `eApplicationID`. Welche App auf welchem Knoten läuft und wie sie verdrahtet
ist, beschreibt das Hausmodell
(`firmware/sensact_firmware/model/Sattlerstrasse16.xml`, s. dortige README). Die Firmware legt die
Apps einer Station **zur Laufzeit** an: `station_config.cc` liest die Stationskonfiguration
(Abschnitt 2 des Stationspakets, Schema `best_binary_buffers_schema/sensactcfg.cs`) und trägt die
Apps in `cApplications` ein (`Get(id)`, `AppName(id)`, `FindFirst(type)`). Aus dem generierten
Verzeichnis kommen nur noch modellunabhängige Dateien (Kommandos, Events, Dispatch).

Diese Seite ist die **verbindliche Beschreibung von Kommandos und Statusmeldungen** der Apps.
Wer eine App ändert oder eine Bedienoberfläche dafür baut (`npm-packages/@klaus-liebler/web-components-sensact/sensactapps_base.ts`),
hält sich an diese Tabellen – und passt sie hier an, wenn sich etwas ändert.

Tests: [host_test/README.md](host_test/README.md) (Host-Unit-Tests mit GoogleTest, ohne ESP32).

## Grundlagen

- Jede App erbt von `cApplication` (`cApplication.hh`) und spricht die Außenwelt ausschließlich
  über `iSensactContext` an (Zeit, Ein-/Ausgänge, Kommandos senden, Sound, Parameter). Keine
  App bindet ESP-IDF-Header ein – nur deshalb sind
  die Apps auf dem PC testbar. Bitte so lassen.
- Kommandos kommen über CAN, von einer lokalen App oder aus der Web-UI. Der Payload wird vom
  generierten `parseCommand.inc` dekodiert und landet in `On<KOMMANDO>Command(...)`. Eine App
  überschreibt nur die Handler, die sie versteht; alle anderen sind wirkungslos.
- **Taktung** (`cApplicationHost::Loop`, `firmware/sensact_firmware/main`): der Nodemaster
  tickt alle 20 ms; jede App ist per Bucket-Rotation spätestens alle 100 ms dran, eine App, die
  `NeedsFastLoop()==true` meldet (z. B. PWM während eines Fades), in jedem Tick. Apps rechnen
  deshalb zeitbasiert (`ctx->Now()`), nie in "Loop-Aufrufen".
- **Statusmeldung**: `FillStatus` füllt vier `uint16_t`. Konvention (`cApplication.hh`):
  `[0]` Modus/Zustand, `[1]` aktueller Wert, `[2]` Bewegung, `[3]` Ziel – die Belegung je App
  steht unten. Der Host fragt den Status ab, wenn `Loop()` `OK_CHANGED` liefert (sofort an
  Web-UI und CAN) und sonst reihum als `REGULAR_STATUS` (bei vielen Apps auf einem Knoten nur
  alle paar zehn Sekunden). **Jede für Benutzer sichtbare Änderung muss daher `OK_CHANGED`
  liefern**, sonst sieht die Web-UI sie stark verzögert.
- Konstanten: `magic::ACTIVE` = 0xFFFF / `magic::INACTIVE` = 0 für digitale Ausgänge,
  `magic::TIME_MAX` = "nie".

## Apps

### `cOnOff` – Relais (Licht, Steckdose, Standby-Netzteil) – `onoff.cc`

Modell: `OnOffApplication(relay, initialState, autoOffMsecs)`.

| Kommando | Wirkung |
|---|---|
| `ON(ms)` | an; aus nach `ms`, sonst nach `autoOffMsecs`, sonst nie |
| `OFF(ms)` | aus (`ms` wird ignoriert) |
| `TOGGLE`, `TOGGLE_SPECIAL` | umschalten (an: wie `ON(0)`) |
| `HEARTBEAT(sender)` | "ich brauche dich noch": an für mindestens `autoOffMsecs` (bzw. 30 s, wenn 0); verkürzt nie eine bereits längere Einschaltdauer |

Status: `[1]` 1 = an, 0 = aus; Rest 0.
Besonderheit: `initialState=ACTIVE` mit `autoOffMsecs>0` startet **aus** (Annahme: das
Einschalten liegt lange zurück); nur mit `autoOffMsecs==0` startet das Relais an.

### `cSinglePWM` – dimmbare Lampe – `singlepwm.cc`

Modell: `SinglePWMApplication(outputs, minimalOnLevel, initialStoredTargetLevel, autoOffMsecs, standbyController)`.
Helligkeit 0..65535 (0 = aus); am Ausgang über eine wahrnehmungsangepasste Kennlinie
(`level2pwm[level >> 8]`). **Alle Helligkeitswerte im Modell sind in dieser Skala** – Werte aus
der früheren 0..255-Skala (z. B. `minimalOnLevel=150`) ergeben PWM 0.

| Kommando | Wirkung |
|---|---|
| `ON(ms)` | an mit zuletzt eingestellter Helligkeit; optional aus nach `ms` |
| `OFF(ms)` | aus (Helligkeit wird gemerkt); optional wieder an nach `ms` |
| `TOGGLE` | an/aus |
| `SET_VERTICAL_TARGET(level)` | 0 = aus, sonst `max(level, minimalOnLevel)` |
| `STEP_VERTICAL(step)` | relativ um `2*step`; bei ausgeschalteter Lampe: `step>0` → minimal, `step<0` → maximal |
| `UP` / `DOWN` | dimmen hoch/runter, bis `STOP` oder Anschlag (voller Bereich in 5 s); `DOWN` bei ausgeschalteter Lampe schaltet mit Minimalhelligkeit ein |
| `START` | dimmen, abwechselnd hoch/runter (Ein-Taster-Bedienung); nach 5 s Pause wieder "hoch" |
| `STOP` | Dimmen beenden |

Jede Bedienung startet `autoOffMsecs` neu und verwirft ein ausstehendes Auto-On. Änderungen der
Helligkeit werden in ~0,8 s für den vollen Bereich gefadet.
Standby-Netzteil: solange die Lampe leuchtet, geht alle 5,5 s ein `HEARTBEAT` an
`standbyController` (typisch ein `cOnOff` mit Nachlaufzeit).
Status: `[0]` 1 = an, `[1]` gespeicherte Helligkeit, `[2]` Dimmrichtung (−1/0/+1 als uint16),
`[3]` Zielhelligkeit.

### `cBlind` – Rollladen/Markise an zwei Relais – `blind.cc`

Modell: `BlindApplication(relay1, relay2, RelayInterlockMode, upTimeSeconds, downTimeSeconds)`.
Position wird aus der Fahrzeit geschätzt. Nach dem Booten ist sie **unbekannt**: keine
Komfortfunktion, und Fahrten per `UP`/`DOWN(0)` in eine Endlage laufen
vorsichtshalber über den vollen Weg (×1,3, gilt für `UP`/`DOWN(0)`). Bekannt ist sie nach der ersten ununterbrochenen
Fahrt von mindestens voller Fahrzeit.

`RelayInterlockMode`: `R1_DOWN__R2_UP` / `R1_UP__R2_DOWN` = je ein Relais pro Richtung;
`R1_POWER__R2_UP`, `R1_POWER__R2_DOWN`, `R1_UP__R2_POWER`, `R1_DOWN__R2_POWER` = ein
Leistungs- und ein Richtungsrelais (Name = welches Relais was schaltet, `_UP`/`_DOWN` = Richtung,
wenn das Richtungsrelais angezogen ist).
Ablauf jeder Fahrt: Richtungsrelais stellen → 200 ms → Leistung an. Richtungswechsel immer
über Stopp mit ≥ 500 ms Stillstand; nach 500 ms Stillstand fallen beide Relais ab
(`ENERGY_SAVE`). Das Richtungsrelais schaltet nie unter Last, bei direkter Ansteuerung sind nie
beide Relais an (durch Tests über alle sechs Modi abgesichert).

| Kommando | Wirkung |
|---|---|
| `UP(0)` / `DOWN(0)` | Taster-Semantik: fährt sicher in die Endlage; läuft der Motor schon, stoppt er. Komfortfunktion (nur bei bekannter Position): steht der Rollladen schon in dieser Endlage, fährt er in die Gegenrichtung |
| `UP(1)` / `DOWN(1)` | "forced": **Dauerbetrieb** in diese Richtung ohne Rücksicht auf die geschätzte Position, bis ein anderes Kommando kommt (Gedrückthalten, Ende per `STOP`); Sicherheitsgrenze: spätestens nach 10 Fahrzeiten am Stück Stopp (gilt für jeden Dauerbetrieb) |
| `STOP` | anhalten |
| `SET_VERTICAL_TARGET(t)` | `t << 15` = interne Position: **16384..49151 = Teilöffnung 0..100 %**; 1..16383 bzw. 49152..65534 = sicher ganz zu/auf (über die Endlage hinaus bis zum mech. Endschalter), dann Stopp; **0 / 65535 = Dauerbetrieb** ab/auf, bis ein anderes Kommando kommt. Ziel in Gegenrichtung reversiert nach der Totzeit |

Status: `[0]` Zustand (0 `ENERGY_SAVE`, 1 `STOP`, 2 `PREPARE_UP`, 3 `PREPARE_DOWN`, 4 `UP`,
5 `DOWN`), `[1]` Position in % (0 = zu, 100 = offen), `[2]` 4/5 während der Fahrt, sonst 0,
`[3]` Ziel in % (im Stillstand = Position). `OK_CHANGED` bei jedem Zustandswechsel und
einmal pro Sekunde während der Fahrt.

### Taster – `pushbutton.hh`

Ein beim Booten bereits gedrückter oder klemmender Taster löst nichts aus, bis er losgelassen
wird. Gemeinsame Flankenerkennung (`cPushbutton`): kurz < 400 ms ≤ lang; "gehalten" feuert einmal
nach 400 ms; Doppelklick = zwei Loslass-Flanken innerhalb 600 ms.

| App | kurz (beim Loslassen) | gehalten (nach 400 ms) | Loslassen nach lang | Doppelklick |
|---|---|---|---|---|
| `cPushbuttonSingle2Toggle` | – (sendet `TOGGLE` an alle Ziele schon beim **Drücken**) | – | – | – |
| `cPushbuttonSingle2PwmSingle` | `TOGGLE` | `START` | `STOP` | zusätzlich `ON(0)` |
| `cPushbuttonDual2Blind` (je Taste) | `UP(0)`/`DOWN(0)` | `UP(1)`/`DOWN(1)` | `STOP` | – |
| `cPushbuttonDual2PWM` (je Taste) | `TOGGLE` | `UP(0)`/`DOWN(0)` | `STOP` | – |

Status: `[1]` gedrückt (Dual-Varianten: `[1]` Taste "runter", `[2]` Taste "hoch").

### `cRotaryEncoder2PWM` – Drehgeber mit Taster – `rotaryencoder.hh`

Drücken → `TOGGLE`; Drehen → `STEP_VERTICAL(Rastungen * 4096)` (auf int16 begrenzt). Die
erste Rastung nach > 800 ms Pause weckt nur auf; Drehen < 200 ms nach Drücken/Loslassen wird
ignoriert. Status: `[1]` Zählerstand.

### `cRgbw` – RGB(W)-Streifen – `rgbw.cc`

`SET_RGBW(r,g,b,w)` (je 0..255), `SET_SIGNAL(i)` = Farbe `i` aus der Liste bekannter Farben,
`STEP_VERTICAL(±)` = nächste/vorige bekannte Farbe, `TOGGLE` = aus / zuletzt gezeigte Farbe,
`OFF`. Auto-Off nach `autoOffMsecs`, Heartbeat an das Standby-Netzteil wie bei `cSinglePWM`.
Ein nicht angeschlossener W-Kanal ist `magic::OUTPUT_NULL`.
Status: `[0..3]` = R, G, B, W (0..255).

### `cRgbIndicator` – Signal-/Orientierungslicht aus RGB-LEDs – `rgbindicator.cc`

Modell: `<rgbIndicator leds="1 2 3 4" color="#FFB060" toggleSignal="0" autoOff="..."/>` (Board-Feature
`rgbLed`; sensactUp: 0 = LED am Knopf, 1..4 = Ecken). Schreibt über `SetRGBLed` in die LEDs `leds`.

| Kommando | Wirkung |
|---|---|
| `ON(ms)` | Dauerlicht in `color` und der eingestellten Helligkeit; aus nach `ms`, sonst nach `autoOff`, sonst nie |
| `OFF` | aus, ein laufendes Muster endet |
| `TOGGLE` | `toggleSignal` = 0: Dauerlicht an/aus; sonst dieses Muster (Türklingel am Taster: 1) |
| `SET_RGBW(r,g,b,w)` | Farbe (Weiß mischt sich in alle Kanäle) und an; alles 0 = aus |
| `SET_VERTICAL_TARGET(level)` | Helligkeit 0..65535 und an; 0 = aus (Orientierungslicht) |
| `SET_SIGNAL(n)` | Muster über dem Dauerlicht: 1 Türklingel (weiß, 3 × 200 ms), 2 Alarm (rot 2 Hz, bis `OFF`, höchstens 60 s), 3 Hinweis (blau, 3 × an- und abschwellend), 4 OK (grün 1 s), 5 Fehler (rot 1 s); 0 beendet; danach wieder das Dauerlicht |

Status: `[0]` 1 = Dauerlicht an, `[1]` Helligkeit, `[2]` laufendes Muster, `[3]` Farbe als RGB565.

### Knoten-App `cSensactNode` – `node.cc`

`PING(x)` → Event `PONG(x)`. `IDENTIFY(ms)` (0 = 10 s, höchstens 60 s): die Station zeigt sich über
`iSensactContext::Identify` (F103: alle WS2812 blinken blau; Default: nichts).

### `cSound` – Klingel/Signalgeber – `sound.cc`

| Kommando | Wirkung |
|---|---|
| `TOGGLE` | spielt den konfigurierten Standardsound mit Tag-/Nacht-Lautstärke (Klingeltaster) |
| `SET_SIGNAL(i)` | spielt Sound `i` (1 Dingdong, 2 Sirene, 3 Positiv, 4 Negativ) mit der manuellen Lautstärke; ungültig → stoppt |
| `SET_VERTICAL_TARGET(v)` | manuelle Lautstärke 0..65535 |
| `ON` / `OFF` | Stummschaltung aus/an (stumm = mit Lautstärke 0 abspielen) |
| `STOP` | Wiedergabe abbrechen |
| `SET_PARAM(i, v)` | 1 Tag-Lautstärke, 2 Nacht-Lautstärke, 3 Beginn, 4 Ende des "lauten" Zeitfensters (Viertelstunden 0..95; Beginn > Ende = über Mitternacht; Beginn == Ende = nie laut; ab Werk 07:00–22:00) |
| `SAVE_PARAM` | die vier Parameter dauerhaft speichern (NVS) |

Status: `[0]` spielt (0/1), `[1]` zuletzt gespielter Sound, `[2]` manuelle Lautstärke, `[3]` stumm.
Die Reihenfolge der Sounds ist Teil des Protokolls und in der Web-UI dupliziert.

### Weitere

| App | Kurzbeschreibung | Status |
|---|---|---|
| `cLightbarrier` (`lightbarrier.cc`) | steigende Flanke (Pegel konfigurierbar) → `ON(0)` an das Ziel. Helligkeitsabhängigkeit (`brightnessSensor`) noch nicht implementiert | `[1]` aktiv |
| `cSensactNode` (`node.cc`) | `PING(x)` → Event `PONG` mit `x` | keiner (`NO_STATUS`) |
| `cFingerprint` (`fingerprint.hh`) | erkannter Finger mit Aktion `a` → `ON(1000)` an Ziel `a % 3` | keiner |
| `cPump` (`pump.hh`) | Relais zyklisch `onInterval` an / `offInterval` aus, beginnt mit Aus-Phase | `[1]` an |
| `cMilightController` (`milightcontroller.cc`) | Milight-Funkfernbedienung (FUT089): `ON`/`OFF`/Dimmer-Rad → Lampe `light`, Tastengruppe n "0"/"1" → `UP`/`DOWN` an `blinds[n-1]` (Ziele aus dem Modell). Wiederholungen innerhalb 500 ms werden verworfen; Übergabe aus dem Funk-Task über einen lock-freien Ringpuffer | keiner |

### `cBlindsTimer` – Rollläden nach Dämmerung – `blindstimer.cc`

Modell: `<blindsTimer id=… number=… blinds="…" [latitudeDeg/longitudeDeg, sonst `<location>` des Hauses] dawn="CIVIL" openOffsetMin closeOffsetMin maxRandomMin />`.

- Öffnet jeden Rollladen zur Morgendämmerung (`dawn`: Sonnenaufgang, bürgerliche/nautische/
  astronomische Dämmerung) + `openOffsetMinutes` + Zufall 0..`maxRandomMinutes`, schließt zur
  Abenddämmerung + `closeOffsetMinutes` + Zufall. Der Zufall ist je Rollladen und Tag verschieden
  (Anwesenheitssimulation), aber reproduzierbar.
- Gesendet wird `SET_VERTICAL_TARGET` aus dem Überfahrbereich von `cBlind` (65000 = sicher ganz
  auf, 1000 = sicher ganz zu, jeweils mit Stopp).
- `ON`/`OFF` schalten den Timer ein/aus; der Zustand bleibt über einen Neustart erhalten.
- Ohne gestellte Uhr (SNTP) passiert nichts; verpasste Zeitpunkte (Knoten war aus, Uhr
  vorgestellt) werden nicht nachgeholt.

Status: `[1]` aktiv, `[2]`/`[3]` Minuten bis zum nächsten Öffnen/Schließen (0xFFFF = unbekannt).

### Wanduhr

`iSensactContext::GetUnixTime()`/`GetLocalTime()` liefern die per SNTP gestellte Uhrzeit (UTC
bzw. Zeitzone des Knotens) und `false`, solange die Uhr nicht gestellt ist. `Now()` ist dagegen
die Zeit seit dem Booten in ms. Apps rufen nie `::time()` selbst auf.

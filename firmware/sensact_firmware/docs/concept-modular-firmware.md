# Konzept: Basis-Firmware + Stations-Hardware-Konfiguration + Applikations-Konfiguration

Stand 2026-11-16. **Status: zurückgestellt (Umsetzung später).** Entschieden: Weg B
(datengetrieben), Kodierung mit BestBinaryBuffers (nicht JSON), dazu ein Editor (s. unten).
Offen sind nur noch die Fragen am Ende.

## Ziel

Heute ist eine Station ein einziger, monolithischer Build, der vor Ort per USB geflasht wird.
Künftig drei unabhängig austauschbare Teile:

1. **Basis-Firmware** – für alle Stationen gleich, Update per OTA über das Webinterface.
2. **Stations-Hardware-Konfiguration** – welches Board, welche I2C-Erweiterungen (PCA9555,
   PCA9685 …) an welchen Adressen, welche Peripherie (Audio, Drehgeber, Ethernet …).
3. **Applikations-Konfiguration** – welche Apps auf dieser Station laufen, mit welchen
   Parametern und Verdrahtungen (heute: das Hausmodell, Ausschnitt für diesen Knoten).

Teile 2 und 3 werden unabhängig erzeugt und über die Website auf das Gerät übertragen.

## Was die Basis-Firmware heute stationsspezifisch macht

Diese Kopplungen müssen in jedem Fall gelöst werden, egal ob mit ELF-Loader oder ohne:

| Kopplung | Heute | Nötig |
|---|---|---|
| Board-Init (HAL-Klasse, Busmaster, I2C-Bausteine) | `main/hal/<NODE_ID>/board_init.hh` per Include-Pfad einkompiliert | zur Laufzeit gewählt |
| App-Instanzen | generierte `applicationInitializers.inc`/`glo2LocCmd.inc` als statische C++-Objekte | zur Laufzeit angelegt |
| `eApplicationID` | Enum über *alle* Apps des Hauses, `CNT` dimensioniert Arrays (`statusBuffer`), Namenstabelle einkompiliert; dieselbe Enum steht im ws-Protokoll-Schema | App-ID als `uint16_t` + Namen aus der Konfiguration |
| Web-UI | `sensactapps.ts` pro Knoten generiert, Bundle in die Firmware eingebettet | UI baut die App-Liste zur Laufzeit aus der Konfiguration |
| Runtimeconfig | `NODE_ID`, Hostname, Passwort, `WIFI_ENABLED` … als `#define` (`runtimeconfig.hh`) | aus NVS bzw. der Konfiguration |
| Board-Zertifikat/-Schlüssel | per `EMBED_TXTFILES` einkompiliert | einmalig ins NVS/eine Partition provisioniert |

Gute Nachricht: OTA-Partitionen (`ota_0`/`ota_1`), PSRAM und ein OTA-Upload-Handler
(`webmanager_base.hh`, `handle_ota_post`) sind bereits vorhanden. Der Handler muss aber
gehärtet werden (s. `known-issues.md`, T10), und Rollback (`CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE`)
ist nicht aktiv.

## Zwei Wege für Teil 2 und 3

### A) ELF-Loader (`espressif/elf_loader`)

Die Konfigurationen sind eigenständig gebaute, ladbare Module (`project_elf`/`project_so`),
die die Basis-Firmware zur Laufzeit relokiert (unter ESP32-S3 auch nach PSRAM) und aufruft.
Unterstützt ESP32-S3, statische Konstruktoren, eigene Symboltabellen der Basis.

Was das hier konkret bedeuten würde:

- **App-Modul**: enthält entweder nur die Instanzen (dann müssten Konstruktoren, vtables und
  `std::vector`-Speicherverwaltung der App-Klassen aus der Basis exportiert werden – laut
  Espressif sind "Non-static member functions, object layouts, and cross-module ownership"
  ausdrücklich *nicht ABI-sicher*) oder zusätzlich den App-Code selbst (sauberer Schnitt an
  `iSensactContext`, dann wird aber auch App-Logik nur noch per Modul aktualisiert).
- **Hardware-Modul**: die HAL-Klassen benutzen Dutzende ESP-IDF-Treiber (I2C, TWAI, GPIO,
  LEDC, I2S, Ethernet …). Diese müssten alle über die Symboltabelle exportiert werden – oder
  der Treibercode bleibt in der Basis und das Modul wählt nur aus und parametriert. Dann ist
  es aber faktisch eine Datenbeschreibung in Maschinencode-Form.
- Jedes Modul ist an exakt die Basis-Version gebunden, gegen deren Header und Symboltabelle
  es gebaut wurde (C++-ABI, Klassenlayouts). Eine Versionsprüfung beim Laden ist Pflicht;
  sonst führt ein falsches Modul zum Absturz – bei einer Station ohne USB-Zugang ggf. zur
  Boot-Schleife.
- Ausnahmen/RTTI werden nicht unterstützt; Abstürze im Modul brauchen eigene Debug-ELFs zur
  Auswertung; Modul-Code läuft aus PSRAM (Cache-Konkurrenz mit Flash/PSRAM-Zugriffen).

### B) Datengetriebene Konfiguration (entschieden)

Die Basis-Firmware enthält *alle* HAL-Varianten und *alle* App-Klassen (die App-Klassen
enthält sie heute schon vollständig; die 6 HAL-Varianten passen bei 8 MB Flash problemlos).
Hardware- und App-Konfiguration sind **Beschreibungen**, keine Programme:

- Hardware: `{ boardType: "SensactHsNano3", i2c: [{port:0, pca9555:[0x20,0x21…], pca9685:[…]}], audio: …, ethernet: … }`
- Apps: `[{ id: 512, name: "BLIND_L0_CORR_J1", type: "BLIND", relay1: …, relay2: …, mode: "R1_DOWN__R2_POWER", up_s: 25, down_s: 22 }, …]`

Die Beschreibungen werden mit BestBinaryBuffers kodiert, mit Schema-Version und Prüfsumme
(s. Abschnitt "Kodierung"); die Beispiele oben zeigen nur den Inhalt, nicht das Format.
Erzeugt werden sie vom Editor (s. Abschnitt "Editor"), bis dahin vom C#-Builder aus dem
heutigen Hausmodell.
Die Basis legt beim Booten per Fabrik (`CreateApp(type, params)`, `CreateBoard(desc)`) die
Objekte an. Upload über die Website ins LittleFS, Prüfung, Neustart.

Vorteile gegenüber A:

- **Kein ABI-Risiko**: eine Konfiguration kann die Basis nicht zum Absturz bringen, nur
  abgelehnt werden (Schema-Version, Plausibilität). Fehlt sie oder ist sie ungültig, startet
  die Basis im **Sicherheitsmodus**: Webinterface, OTA und Upload funktionieren, Apps/Relais
  bleiben aus.
- "Unabhängig kompilieren" wird zu "unabhängig erzeugen": Sekunden statt eines ESP-IDF-Builds,
  keine Toolchain nötig.
- Die Konfiguration ist im Builder **vor** dem Upload prüfbar, und die Fabrik und der Parser
  sind mit den vorhandenen Host-Tests testbar.
- Dieselbe Beschreibung baut zur Laufzeit die Web-UI.

Grenze: Neue Treiber oder neue App-Typen erfordern weiterhin ein Basis-Update – das dann
aber bequem per OTA. Stationsspezifische *Sonderlogik* (Code, den es nur auf einer Station
gibt) geht so nicht.

### C) Hybrid

B als Grundlage. Später, falls wirklich stationsspezifischer Code gebraucht wird, ein
optionaler App-Typ `ELF_APP`, der ein `elf_loader`-Modul lädt, das genau eine `cApplication`
über `iSensactContext` implementiert (sauberer, schmaler C++-Schnitt).

## Vorgeschlagene Etappen

0. **Schemas** der Hardware- und App-Konfiguration in BestBinaryBuffers festlegen (inkl. der
   Entscheidung zu Listen innerhalb einer App, s. unten); ggf. BestBinaryBuffers erweitern.
1. **OTA härten**: Handler-Fehler beheben, Rollback aktivieren (neue Basis bestätigt sich
   nach erfolgreichem Start selbst, sonst zurück zur alten), Signatur- bzw. Projektprüfung
   (`esp_app_desc_t`) vor dem Umschalten.
2. **Basis stationsneutral machen**: Runtimeconfig und Board-Zertifikat aus NVS,
   `eApplicationID` → `uint16_t` + Namen zur Laufzeit, `statusBuffer` dynamisch, ws-Protokoll
   auf `uint16_t`-App-IDs.
3. **Hardware-Konfiguration als Daten**: Format + Fabrik für die 6 Board-Varianten, Builder
   erzeugt sie aus `NodeIdBoardTypeCatalog`/`board_init.hh`-Wissen; `main/hal/<NODE_ID>/` entfällt.
4. **App-Konfiguration als Daten**: Format + App-Fabrik (inkl. Host-Tests), `cMilightController`
   bekommt seine Ziele als Parameter statt fest eincodierter IDs.
5. **Web-UI**: Upload-Seite für beide Konfigurationen und die Basis, Anzeige der geladenen
   Versionen, Sicherheitsmodus; App-Liste zur Laufzeit statt generiertem `sensactapps.ts`.
6. **Editor** (s. unten) – kann parallel zu 2–5 entstehen, sobald die Schemas aus 0 stehen;
   bis dahin erzeugt der Builder die Konfigurationsdateien aus dem heutigen C#-Modell.
7. Optional: `ELF_APP` (C).

Bestehende Stationen werden einmalig per USB auf die neue Basis gebracht; danach nur noch
OTA + Konfigurations-Upload.

## Kodierung mit BestBinaryBuffers

Die Schemas der Konfigurationen sind – wie das ws-Protokoll – annotierter C#-Code
(`best_binary_buffers_schema/`). BestBinaryBuffers erzeugt daraus den C++-Decoder für die
Basis-Firmware und den TypeScript-Encoder/-Decoder für Editor und Web-UI; beide Seiten sprechen
garantiert dasselbe Format. `///`-Kommentare der Schema-Felder landen im generierten Code und
können im Editor als Feldbeschreibung erscheinen.

### Struktur: flache Liste von Varianten

Regel von BestBinaryBuffers: Ein Array mit Längenpräfix darf es nur als **letztes Feld einer
Message** geben, und nur eines; Klassen enthalten keine Listen. Deshalb ist jede Konfiguration
eine Message mit Kopf und *einer* abschließenden polymorphen Liste (`[BinaryUnion]`):

```csharp
[BinaryMessage(MessageKind.Event)]
public class StationAppConfig
{
    public uint SchemaVersion;      // muss zur Basis-Firmware passen, sonst Ablehnung
    public uint ConfigRevision;     // fortlaufend, vom Editor vergeben
    public ushort NodeId;
    public string NodeName;
    public IAppConfig[] Apps;       // polymorph: je App-Typ eine Klasse
}

[BinaryUnion] public interface IAppConfig { }

[BinaryType] public class BlindConfig : IAppConfig
{
    public ushort AppId; public string Name;
    public ushort Relay1; public ushort Relay2;
    public RelayInterlockMode Mode;
    public uint UpTimeMs; public uint DownTimeMs;
}
```

Die Hardware-Konfiguration genauso: eine flache Liste von Elementen (`Board`, `I2cBus`,
`Pca9555{Bus, Address}`, `Pca9685{Bus, Address}`, `Audio…`, `Ethernet…`), die sich über
Index/ID aufeinander beziehen, statt verschachtelt zu sein.

Auf der Firmware-Seite passt das gut: BestBinaryBuffers dekodiert polymorphe Listen lazy über
einen generierten Visitor – der Visitor ist direkt die Fabrik (`BlindConfig` → `new cBlind(...)`).
Fabrik und Decoder sind mit den bestehenden Host-Tests prüfbar.

### Entscheidung nötig: Listen innerhalb einer App

Einige Apps brauchen selbst eine Liste variabler Länge: `cSinglePWM` (mehrere Ausgänge),
`cPushbuttonSingle2Toggle` (mehrere Ziele), `cRgbw` (bekannte Farben), `cBlindsTimer`
(mehrere Rollläden). Drei Möglichkeiten:

1. **Feste Kapazität** mit `[BinaryCount(N)]` + Zählerfeld (z. B. bis 16 Ausgänge) – geht
   sofort, verschwendet ein paar Bytes, harte Obergrenze.
2. **BestBinaryBuffers erweitern**: Arrays mit Längenpräfix auch in Klassen erlauben.
   Sauberste Lösung, betrifft aber den Kern der Bibliothek (Präfix wie bei Message-Arrays
   `ushort`; die Regel "polymorphes Feld muss das letzte sein" und die lazy Dekodierung müssen
   dafür überdacht werden).
3. **Eigene Listenelemente** in der flachen Liste (`PwmOutput{AppId, Output}`,
   `ToggleTarget{AppId, Target}`) – kein Bibliotheksumbau, aber die Fabrik muss die Elemente
   einer App einsammeln.

Empfehlung: 2, wenn die Bibliothek ohnehin weiterentwickelt wird; sonst 1.

### Versionierung

BestBinaryBuffers-Layouts sind fest: ein neues Feld ändert das Format. Daher `SchemaVersion`
im Kopf; die Basis-Firmware akzeptiert nur ihre eigene Version (später ggf. Migration der
Vorgängerversion). Type-/Class-IDs bleiben über die ID-Map von BestBinaryBuffers stabil, auch
wenn Klassen umbenannt werden.

## Editor

Ein Werkzeug, mit dem das ganze Haus gepflegt wird. Es ersetzt die heutigen C#-Hausmodelle
(`Sattlerstrasse16Builder.cs`) als Quelle der Wahrheit.

**Funktionen**

- **Stationen** anlegen/bearbeiten: Name, Node-ID, Hostname, Board-Settings (z. B.
  `WIFI_ENABLED`).
- **Hardware konfigurieren**: Board-Variante wählen, I2C-Erweiterungen (PCA9555/PCA9685 mit
  Adresse) hinzufügen, Peripherie (Audio, Drehgeber, Ethernet …). Daraus ergibt sich die Liste
  der verfügbaren Ein-/Ausgänge mit lesbaren Namen ("PCA9555 #2, Pin 7") statt der heutigen
  Adressarithmetik (`BUS0 + I2C + 5`).
- **Apps hinzufügen und parametrieren**: App-Typ wählen; Parameterformular aus dem Schema
  (Typ, Beschreibung aus dem `///`-Kommentar, ggf. Einheit/Wertebereich); Ein-/Ausgänge aus der
  Hardware-Liste; Ziele (welcher Taster welchen Rollladen steuert) per Auswahl aus allen Apps
  des Hauses, auch stationsübergreifend.
- **Prüfen** wie heute `CheckAndPrepare` im Builder: doppelt belegte Ein-/Ausgänge, doppelte
  App-IDs, Ziele, die das gesendete Kommando nicht verstehen, Werte außerhalb des Bereichs
  (z. B. die alte 0..255-Helligkeitsskala).
- **App-IDs** werden einmal vergeben und nie wiederverwendet (sie stehen in CAN-Nachrichten
  anderer Stationen).
- **Export** je Station: Hardware- und App-Konfiguration als BestBinaryBuffers-Datei. Später
  (mit der Modularisierung) direkt hochladen, mit Versionsvergleich gegen das Gerät.

**Technik (Vorschlag)**: Web-Anwendung (TypeScript/lit + Vite wie `web/`), damit die
vorhandenen `web-components` und der generierte BestBinaryBuffers-TS-Code direkt nutzbar sind
und sie später auch Konfigurationen an Stationen schicken kann. Die Parameterformulare werden
aus den Schema-Metadaten erzeugt, nicht pro App von Hand gebaut. Dafür bräuchte
BestBinaryBuffers zusätzliche Feld-Attribute (Einheit, Wertebereich, "ist ein Ausgang",
"ist eine App-ID"), die in den generierten TS-Code übernommen werden.

**Hausmodell-Datei**: das ganze Haus als eine BestBinaryBuffers-Datei; die Stationsdateien
werden daraus abgeleitet. Weil Binärdateien in Git nicht vergleichbar sind, gehört eine
Text-Darstellung dazu (Export/Diff-Werkzeug), damit Änderungen am Haus nachvollziehbar bleiben.

**Übergang**: Der Builder exportiert das heutige C#-Modell einmalig in die Hausmodell-Datei;
ab dann wird im Editor gepflegt. Die Code-Generierung der Apps (`applicationInitializers.inc`
usw.) entfällt mit der Modularisierung.

## Offene Fragen

1. Listen innerhalb einer App: feste Kapazität, BestBinaryBuffers erweitern oder eigene
   Listenelemente (s. oben)?
2. Hausmodell-Datei binär (BestBinaryBuffers) mit Text-Export für Git – oder die
   Hausmodell-Datei selbst in einem Textformat und nur die Stationsdateien in
   BestBinaryBuffers?
3. Sollen alle Knoten auf ESP32 umgestellt werden, oder bleiben STM32-Stationen
   (s. `known-issues.md`, E8)? Davon hängt ab, ob der Editor auch für STM32-Stationen
   exportieren muss.
4. Absicherung des Uploads: genügt das Login des Webinterfaces, oder sollen Basis und
   Konfiguration signiert sein (Secure Boot ist derzeit nicht aktiv)?

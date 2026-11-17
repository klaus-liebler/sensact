# Konzept: Basis-Firmware + Stations-Hardware-Konfiguration + Applikations-Konfiguration

Stand 2026-11-17. **Status: zurückgestellt (Umsetzung später).** Entschieden:

- Weg B (datengetrieben); Gerätedateien in BestBinaryBuffers (nicht JSON).
- BestBinaryBuffers wird erweitert, damit Listen auch innerhalb einer App-Konfiguration
  möglich sind.
- Editor als ASP.NET-Core-Anwendung mit Blazor (Bedienung im Browser, Backend .NET).
- Alle Konfigurationsdaten – Hausmodell und Katalog der Boards/Hardware-Features – liegen in
  versionierbaren, direkt lesbaren XML-Dateien; BestBinaryBuffers ist nur das Format, das auf
  die Geräte geht.
- Ältere STM32-Knoten (F103 mit 20 KB RAM, F407) bleiben noch länger in Betrieb und werden
  mitberücksichtigt (s. "STM32-Altknoten").

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

0. **Schemas** der Hardware- und App-Konfiguration in BestBinaryBuffers festlegen;
   BestBinaryBuffers um einen C#-Generator erweitern (s. "Editor"; Listen in Klassen sind
   bereits umgesetzt).
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
6. **Editor** (s. unten) – kann parallel zu 2–5 entstehen, sobald die Schemas aus 0 stehen.
   Sinnvolle Reihenfolge innerhalb: Katalog-XML → Hausmodell-XML (einmaliger Import aus dem
   heutigen C#-Modell) → Code-Export für die bestehenden Firmwares (ESP32 und STM32, ersetzt
   `GenerateModelFiles`) → Bearbeitungsoberfläche → BestBinaryBuffers-Export für die neue Basis.
   So ist der Editor schon nützlich, bevor die modulare Basis-Firmware fertig ist.
7. Optional: `ELF_APP` (C).

Bestehende Stationen werden einmalig per USB auf die neue Basis gebracht; danach nur noch
OTA + Konfigurations-Upload.

## Kodierung mit BestBinaryBuffers

Die Schemas der Konfigurationen sind – wie das ws-Protokoll – annotierter C#-Code
(`best_binary_buffers_schema/`). BestBinaryBuffers erzeugt daraus den C++-Decoder für die
Basis-Firmware, TypeScript für die Web-UI der Station und – neu, s. "Editor" – C#-Typen samt
Encoder für das Editor-Backend; alle Seiten sprechen garantiert dasselbe Format.
`///`-Kommentare der Schema-Felder landen im generierten Code und erscheinen im Editor als
Feldbeschreibung.

### Struktur: flache Liste von Varianten

Regeln von BestBinaryBuffers (Stand 2026-11-17, Commits `9199fe1`, `8aef43f`):

- Listen mit Elementen fester Größe und Listen von Strings: beliebig viele, an beliebiger
  Stelle, in Messages **und Klassen** (mit `[BinaryMaxItemCount]`).
- Polymorphe Felder (`[BinaryUnion]`, einzeln oder als Liste): beliebig viele, an beliebiger
  Stelle, in Messages und Klassen. Eine Klasse darf sich darüber nicht selbst enthalten
  (Größe unbegrenzt, wird abgelehnt). Polymorphe Felder tragen ein
  Byte-Längenpräfix (u32). Damit kommt der Decoder auf dem ESP32 weiterhin ohne Heap und ohne
  doppeltes Parsen aus (`Decode()` merkt sich nur Anzahl, Start und Länge; die Elemente dekodiert
  erst der Visitor), weiß aber trotzdem, wo das nächste Feld beginnt. (Bis 2026-11-16 gab es
  das Präfix nicht; damals musste ein polymorphes Feld das einzige und letzte seines Besitzers
  sein.)

Für die Konfigurationen reicht trotzdem eine Message mit Kopf und *einer* polymorphen
Liste:

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

### Listen innerhalb einer App (entschieden, umgesetzt in BestBinaryBuffers `8aef43f`)

Einige Apps brauchen selbst eine Liste variabler Länge: `cSinglePWM` (mehrere Ausgänge),
`cPushbuttonSingle2Toggle` (mehrere Ziele), `cRgbw` (bekannte Farben), `cBlindsTimer`
(mehrere Rollläden). Dafür erlaubt BestBinaryBuffers seit 2026-11-17 Listen – fester
Elementgröße, Strings und polymorph – auch in Klassen, nicht nur in Messages. Für Listen fester Elementgröße und
String-Listen ist das unkritisch (ihre Länge steht beim Dekodieren sofort fest), polymorphe
Listen haben seit 2026-11-17 das Byte-Längenpräfix. Verworfene Alternativen: feste Kapazität
mit `[BinaryCount(N)]` (harte Obergrenze) und eigene Listenelemente in der flachen Liste
(Fabrik müsste zusammensuchen).

### Versionierung

BestBinaryBuffers-Layouts sind fest: ein neues Feld ändert das Format. Daher `SchemaVersion`
im Kopf; die Basis-Firmware akzeptiert nur ihre eigene Version (später ggf. Migration der
Vorgängerversion). Type-/Class-IDs bleiben über die ID-Map von BestBinaryBuffers stabil, auch
wenn Klassen umbenannt werden.

## Editor

Ein Werkzeug, mit dem das ganze Haus gepflegt wird. Es ersetzt die heutigen C#-Hausmodelle
(`Sattlerstrasse16Builder.cs`) als Quelle der Wahrheit.

### Funktionen

- **Stationen** anlegen/bearbeiten: Name, Node-ID, Hostname, Board-Settings (z. B.
  `WIFI_ENABLED`), Board-Typ aus dem Katalog.
- **Hardware konfigurieren**: Board-Variante wählen, I2C-Erweiterungen (PCA9555/PCA9685 mit
  Adresse) hinzufügen, Peripherie (Audio, Drehgeber, Ethernet …). Was ein Board kann, steht im
  Katalog; daraus ergibt sich die Liste der verfügbaren Ein-/Ausgänge mit lesbaren Namen
  ("PCA9555 #2, Pin 7") statt der heutigen Adressarithmetik (`BUS0 + I2C + 5`).
- **Apps hinzufügen und parametrieren**: App-Typ wählen; Parameterformular aus dem Schema
  (Typ, Beschreibung aus dem `///`-Kommentar, Einheit/Wertebereich); Ein-/Ausgänge aus der
  Hardware-Liste; Ziele (welcher Taster welchen Rollladen steuert) per Auswahl aus allen Apps
  des Hauses, auch stationsübergreifend. App-Typen, die die Firmware einer Station nicht kann
  (z. B. auf einem STM32-Altknoten), werden dort nicht angeboten.
- **Prüfen** wie heute `CheckAndPrepare` im Builder: doppelt belegte Ein-/Ausgänge, doppelte
  App-IDs, Ziele, die das gesendete Kommando nicht verstehen, Werte außerhalb des Bereichs
  (z. B. die alte 0..255-Helligkeitsskala), Ressourcengrenzen (RAM des F103, s. unten).
- **App-IDs** werden einmal vergeben und danach **nicht mehr verändert**. Ändern geht nur
  nach einer sehr deutlichen Sicherheitsabfrage, weil die ID in den Konfigurationen und
  CAN-Nachrichten anderer Stationen steht und all diese Stationen danach neu konfiguriert werden
  müssen (der Editor zeigt an, welche betroffen sind). Zusätzlich empfehlenswert: IDs gelöschter
  Apps nicht neu vergeben, damit eine noch nicht aktualisierte Station keine fremde App anspricht.
- **Export** je Station, abhängig von deren Firmware-Familie (s. "STM32-Altknoten"):
  BestBinaryBuffers-Dateien für die neue ESP32-Basis, generierter Code für die heutigen
  Firmwares. Später (mit der Modularisierung) direkt hochladen, mit Versionsvergleich gegen
  das Gerät.

### Technik: ASP.NET Core mit Blazor (entschieden)

Bedienung im Browser, Backend in .NET. Empfehlung: Blazor mit **interaktivem Server-Rendering**
statt WebAssembly – der Editor ist ein lokales Werkzeug, das Backend liest und schreibt die
XML-Dateien direkt im Git-Arbeitsverzeichnis, es braucht keine eigene API-Schicht, und die
Firmware-nahen .NET-Teile (BestBinaryBuffers, Code-Export) laufen ohnehin im Backend. Der Editor
kann den heutigen C#-Builder (`FirmwareBuilder.Common`) mitbenutzen.

Woher die Formulare ihr Wissen haben: Die App-Konfigurationsklassen (`BlindConfig`, …) sind
BestBinaryBuffers-Schema in C#. Dafür bekommt BestBinaryBuffers einen **C#-Generator**:
Er erzeugt dieselben Typen samt Encoder/Decoder für .NET. Damit hat das Backend

1. typisierte Klassen für jede App-Konfiguration (Formulare per Reflection über Felder und
   Attribute erzeugt, nicht pro App von Hand gebaut),
2. den Encoder für die Gerätedateien, garantiert formatgleich mit dem C++-Decoder der Firmware,
3. die Grundlage für die XML-Abbildung derselben Klassen (s. unten).

Für die Formulare braucht BestBinaryBuffers zusätzliche, rein beschreibende Feld-Attribute,
z. B. Einheit, Wertebereich, "ist ein Ausgang/Eingang", "ist eine App-ID". Auf das Wire-Format
haben sie keinen Einfluss.

### Datenhaltung: XML (entschieden)

Alle Konfigurationsdaten liegen in XML-Dateien, die direkt lesbar und in Git sinnvoll
vergleichbar sind. Zwei Arten:

**Katalog (Basisinformationen)** – welche Boards und Erweiterungen es gibt und was sie können;
ändert sich selten, wird mit der Firmware gepflegt:

```xml
<catalog>
  <board type="SensactHsNano3" mcu="ESP32-S3" firmware="esp32-base">
    <canBus />
    <i2cBus id="0" />
    <audio kind="TAS580x" />
    <ethernet />
  </board>
  <board type="SensactUp02" mcu="STM32F103" ramKiB="20" firmware="stm32-legacy">
    <io id="IO1" kind="input" /> <!-- ... -->
  </board>
  <extension type="PCA9555" kind="io" channels="16" bus="i2c" addressRange="0x20-0x27" />
  <extension type="PCA9685" kind="pwm" channels="16" bus="i2c" addressRange="0x40-0x7F" />
</catalog>
```

**Hausmodell** – Stationen, ihre Hardware und Apps, in **einer** Datei (entschieden):

```xml
<house name="Sattlerstraße 16" schemaVersion="1">
  <station node="SNSCT_L0_TECH_HS_1" board="SensactHsNano3" hostname="l0tech">
    <setting name="WIFI_ENABLED" value="false" />
    <extension type="PCA9555" bus="0" address="0x20" name="Relais 1" />
    <app id="BLIND_L0_CORR_J1" type="Blind" description="Flur">
      <relay1 io="Relais 1/3" /> <relay2 io="Relais 1/4" />
      <mode>R1_DOWN__R2_POWER</mode>
      <upTimeMs>25000</upTimeMs> <downTimeMs>22000</downTimeMs>
    </app>
  </station>
</house>
```

Regeln für gute Diffs: feste Reihenfolge (Stationen, Apps nach ID), ein Element pro App,
Ein-/Ausgänge und Ziele über lesbare Namen, keine generierten Zusatzdaten. Die XML-Abbildung der
App-Parameter folgt aus den (generierten) C#-Klassen, damit Schema, XML und Gerätedatei nicht
auseinanderlaufen können. Eine XSD erleichtert das Bearbeiten in anderen Editoren, die
inhaltliche Prüfung macht das Backend beim Laden und Speichern.

**Übergang**: Ein einmaliger Import erzeugt Katalog und Hausmodell aus den heutigen C#-Klassen
(`ModelGeneration/Nodes/*`, `Sattlerstrasse16Builder.cs`); danach wird nur noch im Editor bzw.
in den XML-Dateien gepflegt, und die C#-Hausmodelle entfallen.

## STM32-Altknoten

Ein Teil der Stationen läuft noch länger auf der alten STM32-Firmware: F407 (sensactHS) und
F103 (sensactUp, 20 KB RAM). Deren Quellcode ist eine eigene Codebasis
(PlatformIO/STM32Cube, eigene App-Implementierungen in `lib/core` bzw. `modules/core`), liegt
seit 2026-11-17 als normale Dateien im Repo (`firmware/sensact_firmware_stm32/` – PlatformIO,
`firmware/firmwareCubeIDE/` – ältere STM32CubeIDE-Projekte). Sie bindet
das Hausmodell bereits heute genauso ein wie die ESP32-Firmware: über generierte
`applicationInitializers.inc`/`glo2LocPointers.inc`.

Varianten:

| | Ansatz | Bewertung |
|---|---|---|
| L1 | **Code-Export für Altknoten**: Der Editor erzeugt für Stationen der Familie `stm32-legacy` weiterhin die `.inc`-Dateien wie heute; die alte Firmware wird damit gebaut und per USB/ST-Link geflasht. | Kein Umbau der Altfirmware, kein RAM-Bedarf für Parser/Fabrik, ein gemeinsames Hausmodell für alle Knoten. **Empfehlung.** |
| L2 | **Datengetrieben auch auf STM32**: Konfiguration im Flash; der BestBinaryBuffers-Decoder liest sie ohne Kopie direkt aus dem Flash, die Fabrik legt die Apps in einen statischen Speicherbereich. | Für den F407 machbar, für den F103 mit 20 KB RAM eng; Update ohne Neukompilieren, aber nur per USB (kein OTA auf diesen Boards). Lohnt erst, wenn Konfigurationen dort häufig wechseln. |
| L3 | **Gemeinsame App-Logik**: die ESP32-Apps (`sensact-applicationmodel`, nur von `iSensactContext` abhängig) auch auf STM32 übersetzen; die Altfirmware liefert nur den Kontext. | Langfristig sauber (eine App-Implementierung, eine Testsuite statt zwei auseinanderlaufender Kopien), aber erheblicher Umbau; `std::vector`/virtuelle Aufrufe auf dem F103 prüfen. |

Entschieden: **L1**; L3 bleibt langfristige Option, wenn die doppelte App-Logik zu sehr
auseinanderläuft. In allen Varianten gilt: Altknoten und neue Knoten sprechen über CAN
dieselben App-IDs und Kommandos; der Editor prüft, dass eine Station nur App-Typen und
Kommandos verwendet, die ihre Firmware-Familie kennt.

## Entscheidungen 2026-11-17 (zuvor offene Fragen)

1. Absicherung des Uploads: das Login des Webinterfaces genügt, keine Signatur.
2. Hausmodell als **eine** XML-Datei (plus Katalog).
3. STM32-Altknoten: **L1** (Code-Export).


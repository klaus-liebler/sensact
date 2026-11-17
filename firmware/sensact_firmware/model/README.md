# XML-Hausmodell

**Quelle der Wahrheit** für Stationen, Hardware und Apps (seit 2026-11-17; vorher C#-Klassen
unter `builder/Models/`, in der Git-Historie bis `3f54589`). Konzept:
[docs/concept-modular-firmware.md](../docs/concept-modular-firmware.md), Abschnitt "Datenhaltung: XML".

| Datei | Inhalt |
|---|---|
| `Sattlerstrasse16.xml` | das Hausmodell |
| `Testmodel.xml` | kleines Modell für die Host-Tests der Apps (`sensact-applicationmodel/host_test`) |
| `catalog.xml` | Katalog der Boards (Busse, benannte Pins, Features) und Erweiterungs-Chips |
| `schema/sensact-house.xsd` | Schema des Hausmodells – mit Erklärung der Ein-/Ausgangs-Schreibweise und der Bedeutung der Reihenfolge |
| `schema/sensact-catalog.xsd` | Schema des Katalogs |

## Bearbeiten

Von Hand, bis der Editor existiert. VS Code mit der Erweiterung "XML" (Red Hat) liest die
XSDs über `xsi:noNamespaceSchemaLocation`, zeigt Fehler sofort an und schlägt Elemente und
Attribute vor.

Regeln, die man kennen muss (Details im XSD):

- **`number` einer App/Station nie ändern** – sie ist die ID auf dem CAN-Bus und steht in den
  Konfigurationen anderer Stationen. Neue Apps bekommen eine noch nicht vergebene Nummer
  (auch nicht die eines `<reserved>`-Eintrags); gelöschte Apps als `<reserved>` stehen lassen.
- **Stationen nicht umsortieren**, neue hinten anfügen: ihre Reihenfolge ergibt `eNodeID`.
- **Erweiterungen gleichen Typs nicht umsortieren**: ihre Reihenfolge ergibt den Chip-Index.
- Die Reihenfolge der Apps einer Station ist die Anzeigereihenfolge im Web-UI.
- Ein-/Ausgänge: `IN0.13` (Erweiterung.Kanal), `K1` (benannter Pin aus dem Katalog),
  `GPIO.21` / `BB.13` (Bus.Index), `INPUT0`, `INPUT1`, `NONE`.

## Verwenden

```
dotnet run --project builder -- GenerateModelFiles --model Sattlerstrasse16
```

liest `model/Sattlerstrasse16.xml`, prüft es gegen das XSD (mit Zeilennummer bei Fehlern) und
erzeugt die Firmware-Dateien; ist Teil von `Pipeline`/`PipelineOffline`. Die Host-Tests
erzeugen ihre Dateien aus `Testmodel.xml`.

## Neuer App-Typ

1. App in `sensact-applicationmodel` (C++) und `builder/ModelGeneration/Applications/*Application.cs`
   (Codegenerierung) wie bisher.
2. Element und Typ in `schema/sensact-house.xsd` (Gruppe `Apps`).
3. Abbildung Element → Objekt in `builder/ModelGeneration/Xml/HouseXmlImporter.cs`.

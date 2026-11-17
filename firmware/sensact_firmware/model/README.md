# XML-Hausmodell

Künftige Quelle der Wahrheit für Stationen, Hardware und Apps (Konzept:
[docs/concept-modular-firmware.md](../docs/concept-modular-firmware.md), Abschnitt "Datenhaltung: XML").

| Datei | Inhalt |
|---|---|
| `schema/sensact-catalog.xsd` | Schema des Katalogs (Boards, Busse, benannte Pins, Features, Erweiterungs-Chips) |
| `catalog.xml` | Katalog der verwendeten Boards |
| `schema/sensact-house.xsd` | Schema des Hausmodells (Stationen, Erweiterungen, Apps) – mit Erklärung der Ein-/Ausgangs-Schreibweise und der Bedeutung der Reihenfolge |
| `Sattlerstrasse16.xml`, `Testmodel.xml` | Hausmodelle |

## Stand

Beide Richtungen existieren und sind geprüft:

| Builder-Schritt | Was er tut |
|---|---|
| `ExportModelXml` | C#-Hausmodelle (`builder/Models/*`) → XML; prüft gegen die XSDs und jede Ein-/Ausgangsadresse per Rückübersetzung |
| `GenerateModelFiles --model <Name> --source xml` | generiert die Firmware-Dateien aus dem XML statt aus C# (Vorgabe noch `--source cs`) |
| `VerifyModelXml` | generiert beide Modelle aus C# und aus XML und vergleicht alle Dateien zeilengenau (2026-11-17: 88 + 64 Dateien identisch) |

Solange die Vorgabe `cs` ist, sind die C#-Modelle maßgeblich und das XML wird aus ihnen
erzeugt – Änderungen also im C#-Modell machen und `ExportModelXml` laufen lassen. Nach der
Umstellung auf `xml` entfallen die C#-Modelle und es wird nur noch das XML gepflegt.

```
dotnet run --project builder -- ExportModelXml
dotnet run --project builder -- VerifyModelXml
```

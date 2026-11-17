# XML-Hausmodell

Künftige Quelle der Wahrheit für Stationen, Hardware und Apps (Konzept:
[docs/concept-modular-firmware.md](../docs/concept-modular-firmware.md), Abschnitt "Datenhaltung: XML").

| Datei | Inhalt |
|---|---|
| `schema/sensact-catalog.xsd` | Schema des Katalogs (Boards, Busse, benannte Pins, Features, Erweiterungs-Chips) |
| `catalog.xml` | Katalog der verwendeten Boards |
| `schema/sensact-house.xsd` | Schema des Hausmodells (Stationen, Erweiterungen, Apps) – mit Erklärung der Ein-/Ausgangs-Schreibweise |
| `Sattlerstrasse16.xml`, `Testmodel.xml` | aus den C#-Modellen exportiert |

**Stand:** Noch maßgeblich sind die C#-Modelle (`builder/Models/*`); die XML-Dateien werden
aus ihnen erzeugt und **nicht** von Hand bearbeitet, bis der Rückweg (XML → Codegenerierung)
existiert:

```
dotnet run --project builder -- ExportModelXml
```

Der Schritt prüft Katalog und Ergebnis gegen die XSDs und jede Ein-/Ausgangsadresse per
Rückübersetzung in die heutige InOutId.

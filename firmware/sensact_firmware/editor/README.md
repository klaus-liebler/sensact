# Sensact-Editor (erste Version)

Bearbeitet das XML-Hausmodell (`../model/*.xml`) im Browser. ASP.NET Core Blazor mit
interaktivem Server-Rendering, .NET 11. Lokales Werkzeug ohne Anmeldung, lauscht nur auf
`localhost`.

```
dotnet run --project editor          # öffnet http://localhost:5247
dotnet run --project editor_tests    # Selbsttest der Editor-Logik auf einer Kopie von model/
```

## Was er kann

- Hausmodell wählen (`model/*.xml`), Stationen mit Board und Erweiterungen ansehen, Apps je Station.
- App bearbeiten: Das Formular entsteht aus `model/schema/sensact-house.xsd` (Auswahllisten für
  Aufzählungen, Vorschläge für Ein-/Ausgänge der Station aus dem Katalog und für Ziele aus allen
  Apps, Standardwerte, Hilfetexte aus den XSD-Kommentaren). Zeitangaben mit benannten
  Vorgabewerten (5 Minuten, 2 Stunden …) oder als ISO-8601-Dauer, daneben lesbar angezeigt
  ("= 1 h 30 min"); Helligkeiten als Prozent 0…100.
- App hinzufügen (bekommt die nächste nie vergebene Nummer) und löschen (nur ohne Verweise; die
  Nummer bleibt als `<reserved>` vergeben).
- Prüfung nach jeder Änderung mit denselben Regeln wie der Builder: XSD, Katalog
  (`HouseModelChecker`), Kommandos (`SourceCodeGenerator.CheckAndPrepare`). Gespeichert wird nur
  ein fehlerfreies Modell.
- Speichern ändert die Datei nur dort, wo etwas geändert wurde (Formatierung, Kommentare,
  Zeilenenden bleiben) – Git-Diffs zeigen genau die Änderung.

## Was (noch) nicht

- Namen (`id`) und Nummern von Apps ändern – Nummern bewusst nie, Umbenennen mit Nachziehen
  aller Verweise ist möglich, aber noch nicht gebaut.
- Stationen und Erweiterungen anlegen/bearbeiten (bis dahin im XML).
- Paket bauen und auf die Geräte laden aus dem Editor heraus (bis dahin:
  `dotnet run --project builder -- BuildPackage --model <Haus>` und Upload über die
  Weboberfläche, s. `model/README.md`).

Aufbau: `Services/AppSchema.cs` (Formularbeschreibung aus dem XSD),
`Services/HouseModelService.cs` (Laden, Ändern, Prüfen, Speichern),
`Components/Pages/Home.razor` (Oberfläche). Import, Prüfungen und Codegenerierung kommen aus
`../modellib` – derselbe Code wie im Builder.

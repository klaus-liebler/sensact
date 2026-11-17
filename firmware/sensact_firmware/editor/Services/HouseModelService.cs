using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;
using Klli.Sensact.Config;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model.Xml;

namespace SensactEditor.Services;

/// <summary>Ein Befund der Pruefung; Line ist die Zeile im aktuellen (ggf. ungespeicherten) Stand.</summary>
public sealed record Issue(string Source, string Message);

/// <summary>
/// Haelt ein Hausmodell (model/&lt;Name&gt;.xml) im Speicher, aendert es und schreibt es zurueck.
/// Geprueft wird mit denselben Regeln wie im Builder: XSD, HouseModelChecker (Katalog) und
/// SourceCodeGenerator.CheckAndPrepare (Ziele verstehen die gesendeten Kommandos). Gespeichert wird
/// nur ein fehlerfreies Modell. Formatierung und Kommentare der Datei bleiben erhalten
/// (PreserveWhitespace), damit Git-Diffs nur die eigentliche Aenderung zeigen.
///
/// Regeln aus model/README.md, die der Editor durchsetzt: Nummern werden nie geaendert (neue Apps
/// bekommen eine noch nie vergebene Nummer), geloeschte Apps werden zu &lt;reserved&gt;, Stationen und
/// Erweiterungen werden nicht umsortiert.
/// </summary>
public sealed class HouseModelService
{
	private readonly object sync = new();

	public string ModelDir { get; }
	public string HouseName { get; private set; } = "";
	public string HousePath { get; private set; } = "";
	public string HouseXsdPath { get; }
	public Catalog Catalog { get; }
	public AppSchema Schema { get; }
	public XDocument Doc { get; private set; } = new();
	public bool Dirty { get; private set; }
	public IReadOnlyList<Issue> Issues { get; private set; } = [];
	public DateTime? LastSaved { get; private set; }

	/// <summary>Wird nach jeder Aenderung ausgeloest (alle offenen Browser-Tabs aktualisieren sich).</summary>
	public event Action? Changed;

	public HouseModelService(IConfiguration config, IWebHostEnvironment env)
	{
		ModelDir = Path.GetFullPath(Path.Combine(env.ContentRootPath, config["ModelDir"] ?? "../model"));
		HouseXsdPath = Path.Combine(ModelDir, "schema", "sensact-house.xsd");
		Catalog = Catalog.Load(Path.Combine(ModelDir, "catalog.xml"));
		Schema = new AppSchema(HouseXsdPath);
		Open(config["House"] ?? "Sattlerstrasse16");
	}

	public IEnumerable<string> AvailableHouses() =>
		Directory.EnumerateFiles(ModelDir, "*.xml").Select(Path.GetFileNameWithoutExtension).Where(n => n != "catalog").Order()!;

	public void Open(string houseName)
	{
		lock (sync)
		{
			HouseName = houseName;
			HousePath = Path.Combine(ModelDir, houseName + ".xml");
			// Zeilenende und abschliessenden Umbruch merken: der XML-Parser normalisiert alles auf "\n",
			// beim Speichern soll die Datei aber nur dort anders sein, wo wirklich geaendert wurde.
			string raw = File.ReadAllText(HousePath);
			newline = raw.Contains("\r\n") ? "\r\n" : "\n";
			endsWithNewline = raw.EndsWith('\n');
			Doc = XDocument.Parse(raw, LoadOptions.PreserveWhitespace);
			Dirty = false;
			Revalidate();
		}
		Changed?.Invoke();
	}

	public void Reload() => Open(HouseName);

	// ------------------------------------------------------------------ Lesen

	public XElement House => Doc.Root!;
	public IEnumerable<XElement> Stations => House.Elements("station");
	public static IEnumerable<XElement> AppsOf(XElement station) => station.Elements().Where(e => e.Name != "setting" && e.Name != "extension");
	public IEnumerable<XElement> AllApps => Stations.SelectMany(AppsOf);
	public static string Id(XElement e) => (string?)e.Attribute("id") ?? "";

	/// <summary>Alle Namen, auf die ein Ziel verweisen kann (Apps und Stationen)</summary>
	public IEnumerable<string> AllTargetNames => Stations.Select(Id).Concat(AllApps.Select(Id)).Order(StringComparer.Ordinal);

	/// <summary>Alle vergebenen Nummern inkl. &lt;reserved&gt;</summary>
	private IEnumerable<int> UsedNumbers => Stations.Concat(AllApps).Concat(House.Elements("reserved"))
		.Select(e => (int?)e.Attribute("number") ?? 0);

	public int NextFreeNumber => UsedNumbers.DefaultIfEmpty(0).Max() + 1;

	/// <summary>Moegliche Ein-/Ausgangsangaben einer Station (fuer die Vorschlagsliste)</summary>
	public IReadOnlyList<string> IoSuggestions(XElement station, IoDirectionHint dir)
	{
		var result = new List<string>();
		if (!Catalog.Boards.TryGetValue((string)station.Attribute("board")!, out var board)) return result;
		var want = dir == IoDirectionHint.Input ? IoDirection.Input : IoDirection.Output;
		result.AddRange(dir == IoDirectionHint.Input ? ["INPUT0", "INPUT1"] : ["NONE"]);
		result.AddRange(board.Buses.SelectMany(b => b.Pins).Where(p => p.Direction == want).Select(p => p.Name));
		foreach (var e in station.Elements("extension"))
		{
			if (Catalog.Extensions.TryGetValue((string)e.Attribute("type")!, out var t) && t.Direction == want)
			{
				string name = (string)e.Attribute("name")!;
				result.AddRange(Enumerable.Range(0, t.Channels).Select(i => $"{name}.{i}"));
			}
		}
		return result;
	}

	/// <summary>Wer verweist auf diese App/Station? (Attribut-Ebene, fuer Loeschschutz und Anzeige)</summary>
	public IReadOnlyList<string> ReferencesTo(string id)
	{
		var result = new List<string>();
		foreach (var app in AllApps)
		{
			if (!Schema.Types.TryGetValue(app.Name.LocalName, out var type)) continue;
			foreach (var f in type.Fields.Where(f => f.Kind is FieldKind.AppRef or FieldKind.AppRefList))
			{
				var v = (string?)app.Attribute(f.Name);
				if (v != null && v.Split(' ', StringSplitOptions.RemoveEmptyEntries).Contains(id)) result.Add($"{Id(app)}.{f.Name}");
			}
		}
		return result;
	}

	// ------------------------------------------------------------------ Aendern

	/// <summary>Setzt ein Attribut; leerer Wert entfernt es (dann gilt der Standardwert aus dem XSD).</summary>
	public void SetAttribute(XElement element, string name, string? value)
	{
		if (name is "id" or "number") throw new InvalidOperationException("Name und Nummer einer App werden nicht geaendert.");
		lock (sync)
		{
			element.SetAttributeValue(name, string.IsNullOrWhiteSpace(value) ? null : value.Trim());
			Touch();
		}
	}

	public XElement AddApp(XElement station, string element, string id, string? description)
	{
		if (!Schema.Types.ContainsKey(element)) throw new ArgumentException($"Unbekannter App-Typ {element}");
		if (!System.Text.RegularExpressions.Regex.IsMatch(id, "^[A-Z][A-Za-z0-9_]*$")) throw new ArgumentException("Name: Grossbuchstabe am Anfang, dann Buchstaben, Ziffern, _.");
		if (AllTargetNames.Contains(id) || House.Elements("reserved").Any(r => Id(r) == id)) throw new ArgumentException($"Name {id} ist schon vergeben.");
		lock (sync)
		{
			var app = new XElement(element, new XAttribute("id", id), new XAttribute("number", NextFreeNumber));
			if (!string.IsNullOrWhiteSpace(description)) app.Add(new XAttribute("description", description.Trim()));
			InsertChild(station, app, after: station.Elements().LastOrDefault());
			Touch();
			return app;
		}
	}

	/// <summary>Loescht eine App; ihre Nummer bleibt als &lt;reserved&gt; vergeben. Verweise anderer Apps verhindern das Loeschen.</summary>
	public void DeleteApp(XElement app)
	{
		var refs = ReferencesTo(Id(app));
		if (refs.Count > 0) throw new InvalidOperationException($"{Id(app)} wird noch verwendet von: {string.Join(", ", refs)}");
		lock (sync)
		{
			var reserved = new XElement("reserved",
				new XAttribute("id", Id(app)),
				new XAttribute("number", (string)app.Attribute("number")!),
				new XAttribute("note", $"{app.Name.LocalName} geloescht {DateTime.Now:yyyy-MM-dd}"));
			RemoveWithIndent(app);
			InsertChild(House, reserved, after: House.Elements().LastOrDefault());
			Touch();
		}
	}

	/// <summary>Fuegt ein Kind mit derselben Einrueckung wie seine Geschwister ein.</summary>
	private static void InsertChild(XElement parent, XElement child, XElement? after)
	{
		string indent = IndentOf(parent) + "  ";
		if (after != null)
		{
			after.AddAfterSelf(new XText("\n" + indent), child);
		}
		else
		{
			parent.RemoveNodes();
			parent.Add(new XText("\n" + indent), child, new XText("\n" + IndentOf(parent)));
		}
	}

	private static string IndentOf(XElement e) =>
		e.PreviousNode is XText t ? t.Value[(t.Value.LastIndexOf('\n') + 1)..] : "";

	private static void RemoveWithIndent(XElement e)
	{
		if (e.PreviousNode is XText t && string.IsNullOrWhiteSpace(t.Value)) t.Remove();
		e.Remove();
	}

	private void Touch()
	{
		Dirty = true;
		Revalidate();
		Changed?.Invoke();
	}

	// ------------------------------------------------------------------ Pruefen und Speichern

	private string newline = "\n";
	private bool endsWithNewline;

	private string Serialize()
	{
		var sb = new StringBuilder();
		if (Doc.Declaration != null) sb.Append(Doc.Declaration).Append('\n');
		// Nicht XElement.ToString(): das ersetzt "\n" durch Environment.NewLine.
		var settings = new XmlWriterSettings { OmitXmlDeclaration = true, NewLineHandling = NewLineHandling.None, ConformanceLevel = ConformanceLevel.Fragment };
		using (var sw = new StringWriter(sb))
		using (var w = XmlWriter.Create(sw, settings))
		{
			Doc.Root!.WriteTo(w);
		}
		if (endsWithNewline) sb.Append('\n');
		return newline == "\n" ? sb.ToString() : sb.ToString().Replace("\n", newline);
	}

	public void Revalidate()
	{
		var issues = new List<Issue>();
		var text = Serialize();
		var doc = XDocument.Parse(text, LoadOptions.SetLineInfo);

		var schemas = new XmlSchemaSet();
		schemas.Add(null, HouseXsdPath);
		doc.Validate(schemas, (_, e) => issues.Add(new Issue("Schema", $"Zeile {e.Exception?.LineNumber}: {e.Message}")));

		if (issues.Count == 0)
		{
			issues.AddRange(HouseModelChecker.Check(doc, Catalog).Select(m => new Issue("Katalog", m)));
		}
		if (issues.Count == 0)
		{
			try
			{
				var result = new HouseXmlImporter(Catalog).Import(doc);
				var logger = new CollectingLogger();
				var settings = new AppSettings { SourceCodeGenerator = new SourceCodeGeneratorOptions { BasePath = Path.Combine(Path.GetTempPath(), "sensact_editor_check") } };
				new SourceCodeGenerator(settings, result.Ids, logger).CheckAndPrepare(new ModelContainer(result.Model));
				issues.AddRange(logger.Errors.Select(m => new Issue("Modell", m)));
			}
			catch (Exception ex)
			{
				issues.Add(new Issue("Modell", ex.Message));
			}
		}
		Issues = issues;
	}

	public void Save()
	{
		lock (sync)
		{
			Revalidate();
			if (Issues.Count > 0) throw new InvalidOperationException("Das Modell hat Fehler und wird nicht gespeichert.");
			File.WriteAllText(HousePath, Serialize(), new UTF8Encoding(false));
			Dirty = false;
			LastSaved = DateTime.Now;
		}
		Changed?.Invoke();
	}

	private sealed class CollectingLogger : ILogger<SourceCodeGenerator>
	{
		public List<string> Errors { get; } = [];
		public IDisposable? BeginScope<TState>(TState state) where TState : notnull => null;
		public bool IsEnabled(LogLevel logLevel) => logLevel >= LogLevel.Error;
		public void Log<TState>(LogLevel logLevel, EventId eventId, TState state, Exception? exception, Func<TState, Exception?, string> formatter)
		{
			if (logLevel >= LogLevel.Error) Errors.Add(formatter(state, exception));
		}
	}
}

// Phase "GenerateWsProtocolFiles". Erzeugt aus allen *.cs-Schema-Dateien in den uebergebenen Quellen
// zwei Dateien: ws_protocol.hh (C++) und ws-protocol.ts (TypeScript).
//
// Die gesamte Parsing-/Codegenerierungs-Logik (vormals ~1800 Zeilen direkt in dieser Datei, JSON-Schema-
// basiert) lebt jetzt in der eigenstaendigen Bibliothek BestBinaryBuffers (eigenes Repo, s.
// C:\repos\dotnet_libs\best_binary_buffers\README.md) -- dort ist das Schema kein separates JSON-Format
// mehr, sondern echter, mit [BinaryType]/[BinaryMessage]/[BinaryUnion]/[BinaryCount] annotierter C#-Code
// (Structs/Classes/Enums/Interfaces), analog zu MemoryPack/ZeroFormatter ("C# als Schema-Definition-
// Sprache"). Diese Datei ist entsprechend auf einen duennen Wrapper eingedampft: Quellverzeichnisse zu
// *.cs-Dateien aufloesen, ids.txt laden/speichern (BestBinaryBuffers fasst das Dateisystem selbst nicht
// an -- IdMap ist eine vom Aufrufer verwaltete Datenstruktur, s. dortiges IdMap.cs), Bibliothek aufrufen.
//
// Wire-Format-Aenderungen ggue. dem bisherigen JSON-Schema-Generator (s. BestBinaryBuffers/README.md):
// Strings sind jetzt null-terminiert statt laengenpraefigiert, Array-Count-Praefixe sind jetzt 2 statt
// 4 Byte (ushort statt uint32). Unkritisch, weil best_binary_buffers_schema/ (damals noch
// "ws-protocol/") zum Zeitpunkt der Umstellung noch nie committet war (kein ausgeliefertes Geraet
// haengt an der alten Kodierung).
using BestBinaryBuffers;

namespace Builder.Phases;

public static class GenerateWsProtocolFiles
{
	// Loest eine einzelne --ws-protocol-path-Angabe auf: ein VERZEICHNIS liefert alle direkt darin
	// liegenden *.cs-Dateien (nicht rekursiv), eine einzelne .cs-DATEI wird direkt uebernommen.
	// Relative Pfade werden gegen Paths.RootDir aufgeloest, absolute Pfade unveraendert uebernommen.
	private static List<string> ResolveSourceFiles(string source)
	{
		var resolved = Path.IsPathRooted(source) ? source : Path.Combine(Paths.RootDir, source);
		if (Directory.Exists(resolved))
		{
			return Directory.GetFiles(resolved, "*.cs").ToList();
		}
		if (File.Exists(resolved))
		{
			return [resolved];
		}
		throw new InvalidOperationException(
			$"ws-protocol-Quelle nicht gefunden (weder Verzeichnis noch Datei): {resolved}" +
			(source != resolved ? $" (angegeben als \"{source}\")" : ""));
	}

	// 'sources' -- Verzeichnisse und/oder einzelne *.cs-Dateien, deren Inhalte zu EINEM gemeinsamen
	// ws_protocol.hh/ws-protocol.ts zusammengefasst werden. Ohne Angabe (null/leer) faellt das auf
	// Paths.BestBinaryBuffersSchemaDir zurueck ("best_binary_buffers_schema/" im Repo-Root von
	// sensact_firmware) -- fuer den kompletten sensact-Datenaustausch braucht es aber IMMER
	// zusaetzlich espidf-component-webmanager/best_binary_buffers_schema (die anderen Namespaces),
	// deshalb ruft Program.cs diese Phase mit beiden Quellen explizit auf, statt sich auf den
	// Default zu verlassen.
	public static void Run(IReadOnlyList<string>? sources = null)
	{
		var effectiveSources = (sources is { Count: > 0 }) ? sources : [Paths.BestBinaryBuffersSchemaDir];

		var files = effectiveSources.SelectMany(ResolveSourceFiles)
			.Select(Path.GetFullPath)
			.Distinct()
			.OrderBy(f => f, StringComparer.Ordinal)
			.ToList();
		if (files.Count == 0)
		{
			throw new InvalidOperationException(
				$"Keine ws-protocol-*.cs-Dateien gefunden (durchsucht: {string.Join(", ", effectiveSources)}).");
		}

		var idMapPath = Path.Combine(Paths.BestBinaryBuffersSchemaDir, "ids.txt");
		var idMap = IdMap.Load(idMapPath);

		Directory.CreateDirectory(Paths.GeneratedWsProtocolCppDir);
		Directory.CreateDirectory(Paths.GeneratedWsProtocolTsDir);
		SchemaCompiler.Compile(
			files,
			idMap,
			Path.Combine(Paths.GeneratedWsProtocolCppDir, "ws_protocol.hh"),
			Path.Combine(Paths.GeneratedWsProtocolTsDir, "ws-protocol.ts"));

		// Macht wsprotocol_ts als eigenstaendiges npm-Paket "@generated/wsprotocol_ts" konsumierbar
		// (analog zum bisherigen "@generated/flatbuffers_ts", s. dessen package.json) -- ws-protocol.ts
		// selbst ist abhaengigkeitsfrei (kein flatbuffers-Import mehr noetig), daher keine "dependencies".
		File.WriteAllText(Path.Combine(Paths.GeneratedWsProtocolTsDir, "package.json"),
			"""{"name":"@generated/wsprotocol_ts","version":"1.0.0","main":"ws-protocol.ts","author":"BestBinaryBuffers","license":"No License","description":"Generated during build from BestBinaryBuffers (s. C:\\repos\\dotnet_libs\\best_binary_buffers)."}""");

		idMap.SaveIfDirty(idMapPath);

		var sourcesLabel = string.Join(", ", effectiveSources);
		Console.WriteLine($"{files.Count} Datei(en) aus {effectiveSources.Count} Quelle(n) ({sourcesLabel}) " +
			$"-> {Paths.GeneratedWsProtocolCppDir} / {Paths.GeneratedWsProtocolTsDir}");
	}
}

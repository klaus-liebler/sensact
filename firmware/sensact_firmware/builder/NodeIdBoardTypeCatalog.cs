// NODE_ID -> (BoardTypeName, BoardTypeVersion) -- backt IBuildContext.BoardTypeName/BoardTypeVersion
// auf Sensact-Seite. BoardTypeName ist der Board-Typ aus dem Katalog (model/catalog.xml) und
// entspricht dem Namespace der main/hal/hal_sensact*.hh, die main/hal/<NODE_ID>/board_init.hh
// einbindet.
//
// Quelle ist seit 2026-11-17 das XML-Hausmodell: das Attribut board der <station> mit dieser
// NODE_ID in model/*.xml (vorher eine von Hand gepflegte Tabelle hier). Steht eine NODE_ID in
// mehreren Hausmodellen (z. B. Testmodel und Sattlerstrasse16), muessen die Board-Typen
// uebereinstimmen. Einzige Ausnahme ist MONITORING: ein Geraet ohne Apps, das keine Station eines
// Hauses ist (der Generator haengt es selbst an).
// BoardTypeVersion ist mangels echter Hardware-Revisions-Versionierung vorerst ueberall ein Platzhalter.
using System.Xml.Linq;

namespace Builder;

public static class NodeIdBoardTypeCatalog
{
	private const string PlaceholderVersion = "1.0.0";

	private static readonly IReadOnlyDictionary<string, string> NotInAnyHouse =
		new Dictionary<string, string>(StringComparer.Ordinal)
		{
			["MONITORING"] = "SensactHsNano3",
		};

	public static (string BoardTypeName, string BoardTypeVersion) Resolve(string nodeId)
	{
		var modelDir = Path.Combine(Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!, "model");
		var found = Directory.EnumerateFiles(modelDir, "*.xml")
			.Where(f => !Path.GetFileName(f).Equals("catalog.xml", StringComparison.OrdinalIgnoreCase))
			.SelectMany(f => XDocument.Load(f).Root!.Elements("station")
				.Where(s => (string?)s.Attribute("id") == nodeId)
				.Select(s => (File: Path.GetFileName(f), Board: (string)s.Attribute("board")!)))
			.ToList();

		var boards = found.Select(x => x.Board).Distinct(StringComparer.Ordinal).ToList();
		if (boards.Count > 1)
		{
			throw new InvalidOperationException(
				$"NODE_ID \"{nodeId}\" hat in den Hausmodellen verschiedene Board-Typen: " +
				string.Join(", ", found.Select(x => $"{x.File}: {x.Board}")));
		}
		if (boards.Count == 1)
		{
			return (boards[0], PlaceholderVersion);
		}
		if (NotInAnyHouse.TryGetValue(nodeId, out var board))
		{
			return (board, PlaceholderVersion);
		}
		throw new InvalidOperationException(
			$"Unbekannte NODE_ID \"{nodeId}\": keine <station id=\"{nodeId}\"> in {modelDir}\\*.xml.");
	}
}

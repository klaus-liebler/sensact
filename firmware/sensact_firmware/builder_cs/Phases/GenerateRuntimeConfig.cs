// Portierung von gulpfile.ts: createObjectWithDefines() + der drei cfg.create*-Aufrufe
// (key_value_file_helper.ts, s. RuntimeConfigWriter.cs). Baut das flache "defines"-Dictionary aus:
//   - node_descriptor.json des aktuellen Nodes (von configware erzeugt, s. Paths.SensactModelGeneratedDir
//     -- SOLANGE GenerateModelFiles/configware-Merge (Migrationsstrategie-Schritt 7) noch nicht
//     portiert ist, ist das weiterhin der Output-Ordner des bestehenden configware_*-Programms;
//     dieser Schritt setzt also voraus, dass configware_sattlerstrasse/_testmodel VOR dieser Phase
//     einmal gelaufen ist)
//   - board_settings.web/firmware aus der board-spezifischen board_info.json (aktuell fuer alle
//     bekannten Boards leer, s. unten)
//   - NODE_ID/HOSTNAME/BOARD_*/APP_*/CREATION_DT*/GIT_SHORT_HASH
// BEWUSST NOCH OHNE BANNER (ascii_art.ts nutzt das npm-Paket "figlet" -- keine triviale
// .NET-Entsprechung vorhanden, laut Plan ohnehin niedrige Prioritaet, s. Plan-Doc Schritt 2).
using System.Text.Json;

namespace Builder.Phases;

public static class GenerateRuntimeConfig
{
	private static string AppName = "sensact";
	private static string AppVersion = "1.0";

	private static object JsonElementToObject(JsonElement el) => el.ValueKind switch
	{
		JsonValueKind.String => el.GetString()!,
		JsonValueKind.Number => el.TryGetInt64(out var l) ? l : el.GetDouble(),
		JsonValueKind.True => true,
		JsonValueKind.False => false,
		JsonValueKind.Array => el.EnumerateArray().Select(e => e.GetString() ?? "").ToList(),
		_ => el.GetRawText(),
	};

	public static void Run()
	{
		var defines = new Dictionary<string, object>();

		var ctx = BoardContext.LoadCached();
		var b = ctx.Board;
		var boardDir = ctx.BoardDirectory;
		var nodeId = ctx.GetNodeId();

		// AddSensactNodeAndModelDescriptorToDefines() -- von configware erzeugt, s. Kommentar am Dateianfang.
		var nodeDescriptorPath = Path.Combine(Paths.SensactModelGeneratedDir, nodeId, "node_descriptor.json");
		if (!File.Exists(nodeDescriptorPath))
		{
			throw new InvalidOperationException(
				$"Keine node_descriptor.json unter {nodeDescriptorPath} -- configware_sattlerstrasse/_testmodel " +
				"muss vor dieser Phase einmal gelaufen sein (s. Migrationsstrategie-Schritt 7, noch nicht portiert).");
		}
		foreach (var prop in JsonDocument.Parse(File.ReadAllText(nodeDescriptorPath)).RootElement.EnumerateObject())
		{
			defines[prop.Name] = JsonElementToObject(prop.Value);
		}

		// board_settings.web/firmware -- fuer alle real bekannten Boards aktuell leer ({}), daher
		// hier (noch) folgenlos, aber der Vollstaendigkeit halber genauso uebernommen wie in
		// gulpfile.ts: JEDER Wert wird VOR dem Einfuegen bereits einmal JSON-stringifiziert (nicht
		// nur beim finalen Rendering) -- so verhaelt sich auch das TS-Original (moeglicherweise ein
		// Bestandteil, der nie mit echten Settings getestet wurde; hier bewusst 1:1 uebernommen statt
		// stillschweigend "korrigiert").
		if (b.BoardSettings.ValueKind == JsonValueKind.Object)
		{
			foreach (var group in new[] { "web", "firmware" })
			{
				if (!b.BoardSettings.TryGetProperty(group, out var groupEl) || groupEl.ValueKind != JsonValueKind.Object) continue;
				foreach (var prop in groupEl.EnumerateObject())
				{
					defines[prop.Name] = JsonSerializer.Serialize(JsonElementToObject(prop.Value));
				}
			}
		}

		var now = DateTimeOffset.UtcNow;
		var git = GitInfoReader.ReadGitInfo();

		defines["NODE_ID"] = nodeId;
		defines["HOSTNAME"] = nodeId;
		defines["BOARD_NAME"] = b.BoardName;
		defines["BOARD_VERSION"] = b.BoardVersion;
		defines["BOARD_ROLES"] = JsonElementToObject(b.BoardRoles);
		defines["BOARD_MAC"] = b.Mac;
		defines["BOARD_DIRECTORY"] = boardDir;
		defines["APP_NAME"] = AppName;
		defines["APP_VERSION"] = AppVersion;
		defines["CREATION_DT"] = now.ToUnixTimeSeconds();
		// ISO-Format (yyyy-MM-dd HH:mm:ss), auf Wunsch durchgaengig statt der ehemaligen
		// de-DE-Formatierung (now.toLocaleString("de-DE", MyFavouriteDateTimeFormat) im TS-Original).
		defines["CREATION_DT_STR"] = now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss");
		defines["GIT_SHORT_HASH"] = git.CommitHash;
		// TODO(BANNER): ascii_art.createAsciiArt() nutzt npm-Paket "figlet" -- s. Kommentar am
		// Dateianfang. Platzhalter statt ASCII-Art-Rendering.
		defines["BANNER"] = $"{AppName} {nodeId}";

		RuntimeConfigWriter.CreateCppConfigurationHeader(Paths.GeneratedRuntimeConfigCppDir, defines);
		RuntimeConfigWriter.CreateCMakeJsonConfigFile(Paths.GeneratedCMakeDir, defines);
		RuntimeConfigWriter.CreateTypeScriptRuntimeConfig(Paths.GeneratedRuntimeConfigTsDir, defines);

		Console.WriteLine($"{defines.Count} Defines -> {Paths.GeneratedRuntimeConfigCppDir}, {Paths.GeneratedCMakeDir}, {Paths.GeneratedRuntimeConfigTsDir}");
	}
}

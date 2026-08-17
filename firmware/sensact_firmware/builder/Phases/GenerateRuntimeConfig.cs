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
		// Teilmenge von 'defines', die zusaetzlich ins TS-Runtimeconfig-Paket geschrieben wird --
		// dieses Paket wird von web/ importiert und landet damit ungeschuetzt im an den Browser
		// ausgelieferten JS-Bundle (auch fuer nicht eingeloggte Besucher lesbar, s. Kommentar bei
		// board_settings.firmware unten). NUR unkritische Werte gehoeren hier rein.
		var clientDefines = new Dictionary<string, object>();

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
			var value = JsonElementToObject(prop.Value);
			defines[prop.Name] = value;
			clientDefines[prop.Name] = value;
		}

		// board_settings.web/firmware. Bug gefunden und behoben (2026-08-17): dieser Pfad
		// stringifizierte jeden Wert bereits hier ein zweites Mal (JsonSerializer.Serialize VOR dem
		// Einfuegen, obendrauf zum finalen Rendering in RuntimeConfigWriter.StringifyValue) -- 1:1 aus
		// gulpfile.ts uebernommen und nie mit echten Settings getestet (fuer alle bisher bekannten
		// Boards war board_settings immer leer {}). Erstmals mit echten Werten
		// (WEBMANAGER_AUTH_USERNAME/_PASSWORD) ausgeloest: erzeugte Werte mit doppelt eingebetteten
		// Anfuehrungszeichen und kaputt escapten Unicode-Zeichen. Jetzt wie jeder andere Define: roher
		// Wert rein, Stringifizierung einmalig beim Rendern.
		//
		// Sicherheitsfix (2026-08-18): "firmware"-Settings (u.a. WEBMANAGER_AUTH_USERNAME/_PASSWORD)
		// gehen NUR noch in 'defines' (-> C++-Header, CMake-Config -- beide bleiben serverseitig),
		// NICHT mehr in 'clientDefines' (-> TS-Paket, wird in web/ eingebunden und landet im an den
		// Browser ausgelieferten JS-Bundle). Vorher stand das Admin-Passwort im Klartext im
		// generierten runtimeconfig_ts/index.ts und damit im JS-Bundle -- fuer jeden Besucher der
		// Login-Seite per "Quelltext anzeigen" lesbar, unabhaengig vom Login-Schutz selbst. "web"-
		// Settings sind dagegen bewusst fuer den Client gedacht und gehen in beide Ausgaben.
		if (b.BoardSettings.ValueKind == JsonValueKind.Object)
		{
			if (b.BoardSettings.TryGetProperty("web", out var webEl) && webEl.ValueKind == JsonValueKind.Object)
			{
				foreach (var prop in webEl.EnumerateObject())
				{
					var value = JsonElementToObject(prop.Value);
					defines[prop.Name] = value;
					clientDefines[prop.Name] = value;
				}
			}
			if (b.BoardSettings.TryGetProperty("firmware", out var firmwareEl) && firmwareEl.ValueKind == JsonValueKind.Object)
			{
				foreach (var prop in firmwareEl.EnumerateObject())
				{
					defines[prop.Name] = JsonElementToObject(prop.Value);
				}
			}
		}

		var now = DateTimeOffset.UtcNow;
		var git = GitInfoReader.ReadGitInfo();

		void SetShared(string key, object value)
		{
			defines[key] = value;
			clientDefines[key] = value;
		}

		SetShared("NODE_ID", nodeId);
		SetShared("HOSTNAME", nodeId);
		SetShared("BOARD_NAME", b.BoardName);
		SetShared("BOARD_VERSION", b.BoardVersion);
		SetShared("BOARD_ROLES", JsonElementToObject(b.BoardRoles));
		SetShared("BOARD_MAC", b.Mac);
		// BOARD_DIRECTORY ist ein lokaler Dateisystempfad des Entwicklerrechners -- kein Geheimnis,
		// aber auch kein Client-relevanter Wert, deshalb nur serverseitig (C++/CMake).
		defines["BOARD_DIRECTORY"] = boardDir;
		SetShared("APP_NAME", AppName);
		SetShared("APP_VERSION", AppVersion);
		SetShared("CREATION_DT", now.ToUnixTimeSeconds());
		// ISO-Format (yyyy-MM-dd HH:mm:ss), auf Wunsch durchgaengig statt der ehemaligen
		// de-DE-Formatierung (now.toLocaleString("de-DE", MyFavouriteDateTimeFormat) im TS-Original).
		SetShared("CREATION_DT_STR", now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss"));
		SetShared("GIT_SHORT_HASH", git.CommitHash);
		// TODO(BANNER): ascii_art.createAsciiArt() nutzt npm-Paket "figlet" -- s. Kommentar am
		// Dateianfang. Platzhalter statt ASCII-Art-Rendering.
		SetShared("BANNER", $"{AppName} {nodeId}");

		RuntimeConfigWriter.CreateCppConfigurationHeader(Paths.GeneratedRuntimeConfigCppDir, defines);
		RuntimeConfigWriter.CreateCMakeJsonConfigFile(Paths.GeneratedCMakeDir, defines);
		RuntimeConfigWriter.CreateTypeScriptRuntimeConfig(Paths.GeneratedRuntimeConfigTsDir, clientDefines);

		Console.WriteLine($"{defines.Count} Defines -> {Paths.GeneratedRuntimeConfigCppDir}, {Paths.GeneratedCMakeDir}; {clientDefines.Count} Defines -> {Paths.GeneratedRuntimeConfigTsDir}");
	}
}

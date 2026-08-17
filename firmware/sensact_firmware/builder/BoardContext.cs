// Portierung von Context.get() (context.ts) -- Board-Kontext (Lesen/Schreiben von board_info.json).
// Deckt jetzt BEIDE Zweige ab: LoadCached() (updateWithCurrentlyConnectedBoard:false, liest den
// zwischengespeicherten Stand) und LoadFromLiveMac() (updateWithCurrentlyConnectedBoard:true,
// nutzt eine per EspTool.cs live vom angeschlossenen Board gelesene MAC, aktualisiert
// last_connected_dt) -- beide teilen sich dieselbe "lies/lege board_info.json an"-Logik in Load().
//
// Ersetzt die bisher in Phases/Info.cs, Phases/GenerateRuntimeConfig.cs und
// Phases/GenerateCertificates.cs jeweils separat inline geschriebene "lies board_info.json,
// loese Board-Verzeichnis auf"-Logik durch eine einzige, wiederverwendete Stelle.
using System.Text.Json;

namespace Builder;

public sealed class BoardContext
{
	// Aus gulpfile.ts uebernommen (DEFAULT_BOARD_NAME/DEFAULT_BOARD_VERSION) -- Projekt-Konstanten,
	// keine Maschinenpfade, deshalb im Code statt in appsettings.json.
	private const string DefaultBoardName = "SENSACT_UP_HS";
	private const int DefaultBoardVersion = 0;

	public required IBoardInfo Board { get; init; }

	public required string BoardDirectory { get; init; }

	public required long Mac { get; init; }

	// GetNodeId() aus sensact_code_generator.ts -- board-spezifische sensact.json (NICHT zu
	// verwechseln mit best_binary_buffers_schema/sensact.cs!), enthaelt nur {"node_id": "..."}, bestimmt analog
	// zu heute schon NODE_ID, welches main/hal/<NODE_ID>/ aktiv ist.
	public string GetNodeId()
	{
		var sensactConfigPath = Path.Combine(BoardDirectory, "sensact.json");
		if (!File.Exists(sensactConfigPath))
		{
			throw new InvalidOperationException($"Keine sensact.json im Board-Archiv gefunden ({sensactConfigPath}).");
		}
		var nodeId = JsonSerializer.Deserialize<Dictionary<string, string>>(File.ReadAllText(sensactConfigPath))?.GetValueOrDefault("node_id");
		if (string.IsNullOrEmpty(nodeId))
		{
			throw new InvalidOperationException($"{sensactConfigPath} enthaelt keinen node_id.");
		}
		return nodeId;
	}

	private static BoardContext Load(long mac, bool updateLastConnected)
	{
		var boardsDir = BuilderOptions.Current.BoardsDir;
		var boardDir = BoardPaths.BoardSpecificPath(boardsDir, mac);
		Directory.CreateDirectory(boardDir);

		var boardInfoJsonPath = BoardPaths.BoardSpecificPath(boardsDir, mac, "board_info.json");
		IBoardInfo board;
		if (!File.Exists(boardInfoJsonPath))
		{
			Console.WriteLine($"Keine board_info.json unter {boardInfoJsonPath} -- lege neuen Eintrag mit Default-Werten an.");
			var nowMs = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
			board = new IBoardInfo
			{
				Mac = mac,
				BoardName = DefaultBoardName,
				BoardVersion = DefaultBoardVersion,
				BoardRoles = JsonSerializer.SerializeToElement(""),
				BoardSettings = JsonSerializer.SerializeToElement(new Dictionary<string, object>()),
				FirstConnectedDt = nowMs,
				LastConnectedDt = nowMs,
				FlashEncryptionKeyBurnedAndActivated = false,
			};
			File.WriteAllText(boardInfoJsonPath, JsonSerializer.Serialize(board));
		}
		else
		{
			board = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(boardInfoJsonPath))
				?? throw new InvalidOperationException($"{boardInfoJsonPath} konnte nicht gelesen werden.");
			if (updateLastConnected)
			{
				board.LastConnectedDt = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
				File.WriteAllText(boardInfoJsonPath, JsonSerializer.Serialize(board));
			}
		}

		// Aequivalent zu fs.cpSync(boardInfoJsonPath, currentBoardInfoJsonPath) am Ende von
		// Context.get() -- haelt den Repo-Root-Cache aktuell, falls der board-spezifische Stand
		// (z.B. von Hand editierte board_settings, oder gerade aktualisiertes last_connected_dt)
		// neuer ist als der Cache.
		File.Copy(boardInfoJsonPath, Paths.BoardInfoJsonPath, overwrite: true);

		return new BoardContext { Board = board, BoardDirectory = boardDir, Mac = mac };
	}

	public static BoardContext LoadCached()
	{
		if (!File.Exists(Paths.BoardInfoJsonPath))
		{
			throw new InvalidOperationException(
				$"Keine board_info.json unter {Paths.BoardInfoJsonPath} -- noch kein Board jemals verbunden gewesen " +
				"(fuer eine Live-Board-Abfrage s. Phases/ReadHardwareIds.cs).");
		}
		var cached = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(Paths.BoardInfoJsonPath))
			?? throw new InvalidOperationException($"{Paths.BoardInfoJsonPath} konnte nicht gelesen werden.");
		return Load(cached.Mac, updateLastConnected: false);
	}

	// Aequivalent zu Context.get(config, updateWithCurrentlyConnectedBoard: true) -- s.
	// Phases/ReadHardwareIds.cs (addOrUpdateConnectedBoard-Task-Aequivalent).
	public static BoardContext LoadFromLiveMac(long mac) => Load(mac, updateLastConnected: true);
}

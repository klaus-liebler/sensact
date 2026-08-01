// Neuschrieb des bisherigen "info"-Gulp-Tasks (gulpfile.ts -> Context.printInfo(), s.
// espidf-vite-secure-build-tools/context.ts). Liest den zwischengespeicherten Stand
// (board_info.json im Projekt-Root) und das zugehoerige board-spezifische Archiv unter BoardsDir,
// PLUS (jetzt, seit EspTool.cs/ReadHardwareIds existiert) einen best-effort Live-Abgleich der
// tatsaechlich angeschlossenen Board-MAC gegen den Cache -- ABSICHTLICH read-only: anders als
// ReadHardwareIds.Run() (das Aequivalent zu Context.get(config, true)) legt diese Phase NIE einen
// neuen Board-Archiv-Eintrag an und aktualisiert NIE last_connected_dt, genau wie
// Context.printInfo() im TS-Original nie mutierend war. Wenn kein Board angeschlossen ist oder
// esptool fehlschlaegt, wird das nicht als Fehler behandelt (Info soll auch ohne angeschlossenes
// Board nutzbar bleiben) -- nur die "Is current board"-Zeile faellt dann auf einen erklaerenden
// Platzhalter zurueck.
using System.Text.Json;

namespace Builder.Phases;

public static class Info
{
	public static void Run()
	{
		if (!File.Exists(Paths.BoardInfoJsonPath))
		{
			Console.WriteLine($"Keine board_info.json unter {Paths.BoardInfoJsonPath} gefunden -- noch kein Board jemals verbunden gewesen (oder Cache geloescht).");
			return;
		}

		var cached = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(Paths.BoardInfoJsonPath))
			?? throw new InvalidOperationException($"{Paths.BoardInfoJsonPath} konnte nicht gelesen werden.");

		Console.WriteLine("              MAC: " + BoardPaths.Mac6Char(cached.Mac) + " (decimal: " + cached.Mac + ")");
		try
		{
			var hw = EspTool.ReadHardwareIds();
			Console.WriteLine("       Chip type: " + hw.ChipType + " (live)");
			Console.WriteLine(" Is current board: " + (hw.Mac == cached.Mac ? "yes" : "no"));
		}
		catch (Exception ex)
		{
			Console.WriteLine($" Is current board: (unbekannt -- Live-Board-Abfrage nicht moeglich: {ex.Message})");
		}

		var boardsDir = BuilderOptions.Current.BoardsDir;
		var boardDir = BoardPaths.BoardSpecificPath(boardsDir, cached.Mac);
		if (!Directory.Exists(boardDir))
		{
			Console.WriteLine($"Kein Board-Archiv unter {boardDir} gefunden -- dieses Board ist noch nicht bekannt.");
			return;
		}

		var boardInfoJsonPath = BoardPaths.BoardSpecificPath(boardsDir, cached.Mac, "board_info.json");
		if (!File.Exists(boardInfoJsonPath))
		{
			Console.WriteLine($"Board-Archiv {boardDir} existiert, enthaelt aber keine board_info.json.");
			return;
		}

		var b = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(boardInfoJsonPath))
			?? throw new InvalidOperationException($"{boardInfoJsonPath} konnte nicht gelesen werden.");

		Console.WriteLine("       Board Name: " + b.BoardName);
		Console.WriteLine("    Board Version: " + b.BoardVersion);
		Console.WriteLine("      Board Roles: " + b.BoardRolesDisplay);
		Console.WriteLine("   Board Settings: " + b.BoardSettingsDisplay);
		Console.WriteLine("  First connected: " + IBoardInfo.FromUnixMillis(b.FirstConnectedDt));
		Console.WriteLine("   Last connected: " + IBoardInfo.FromUnixMillis(b.LastConnectedDt));
		Console.WriteLine("Encryption active: " + (b.FlashEncryptionKeyBurnedAndActivated ? "yes" : "no"));
	}
}

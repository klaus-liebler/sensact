// Portierung von gulpfile.ts:addOrUpdateConnectedBoard() (Context.get(config, true)). Fragt per
// EspTool.cs (esptool/espefuse) das TATSAECHLICH angeschlossene Board ab, legt bei Bedarf einen
// neuen Board-Archiv-Eintrag an bzw. aktualisiert last_connected_dt eines bekannten Eintrags.
namespace Builder.Phases;

public static class ReadHardwareIds
{
	public static void Run()
	{
		var hw = EspTool.ReadHardwareIds();
		Console.WriteLine("      Chip type: " + hw.ChipType);
		Console.WriteLine("            MAC: " + BoardPaths.Mac6Char(hw.Mac) + " (decimal: " + hw.Mac + ")");
		Console.WriteLine("Flash-Encryption-Key vorhanden: " + (hw.HasFlashEncryptionKey ? "yes" : "no"));

		var boardDir = BoardPaths.BoardSpecificPath(BuilderOptions.Current.BoardsDir, hw.Mac);
		var wasKnown = Directory.Exists(boardDir);

		var ctx = BoardContext.LoadFromLiveMac(hw.Mac);

		Console.WriteLine(wasKnown
			? $"Board bekannt ({ctx.Board.BoardName}) -- last_connected_dt aktualisiert."
			: "NEUES Board -- Eintrag mit Default-Werten angelegt.");
		Console.WriteLine($"Board-Verzeichnis: {ctx.BoardDirectory}");
	}
}

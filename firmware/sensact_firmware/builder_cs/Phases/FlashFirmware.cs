// Portierung von gulpfile.ts:flashFirmware() (espidf.ts:flashFirmware()/flashEncryptedFirmware()).
// Dispatcht wie im TS-Original anhand von board_info.json's flash_encryption_key_burned_and_activated:
// unverschluesselt (dieser Zweig ist fuer JEDES bisher bekannte sensact-Board der aktive Pfad, s.
// board_info.json) oder verschluesselt.
//
// BEWUSST NUR DER UNVERSCHLUESSELTE PFAD VOLLSTAENDIG IMPLEMENTIERT: der verschluesselte Pfad
// braucht zusaetzlich createRandomFlashEncryptionKeyLazily()/encryptPartitions_.../
// burnFlashEncryptionKeyAndActivateEncryptedFlash() (espidf.ts) -- Letzteres BRENNT EFUSES
// PERMANENT UND UNUMKEHRBAR. Das ist eine so folgenschwere, in dieser Form nie rueckgaengig zu
// machende Operation, dass sie bewusst NICHT im Zuge dieser Migrationsarbeit "nebenbei" mitportiert
// wurde -- kein aktuell bekanntes Board nutzt diesen Pfad, s. board_info.json
// (flash_encryption_key_burned_and_activated:false ueberall). Eigener, separat abgestimmter Schritt,
// falls/wenn tatsaechlich ein Board verschluesselt werden soll.
//
// NVS-Ueberschreiben (OVERWRITE_NVS_TO_DELETE_WIFI_SETTINGS_AND_ALL_OTHER_SETTINGS in gulpfile.ts)
// ist dort bereits per Konstante auf false gesetzt -- der NVS-Zweig braucht zusaetzlich die (noch
// nicht portierte) Usersettings-NVS-Codegenerierung (nvs_partition_gen/GENERATED_USERSETTINGS) und
// wurde deshalb hier konsequent ausgelassen statt eine Kopie toten Codes mitzuschleppen.
namespace Builder.Phases;

public static class FlashFirmware
{
	// Aus gulpfile.ts uebernommen: ein normaler Flash-Vorgang ueberschreibt die
	// WLAN-/Usersettings-NVS-Partition NICHT, schreibt aber jedes Mal die Storage-Partition (LittleFS) neu.
	private const bool WriteStorage = true;

	public static void Run()
	{
		var ctx = BoardContext.LoadCached();
		if (ctx.Board.FlashEncryptionKeyBurnedAndActivated)
		{
			throw new NotImplementedException(
				"Verschluesselter Flash-Pfad ist bewusst nicht implementiert (brennt EFuses permanent) -- s. Kommentar am Dateianfang.");
		}

		var f = FlasherArgs.Load(Paths.BuildDir);
		List<(string Offset, string File)> sections =
		[
			(f.Bootloader.Offset, Path.Combine(Paths.BuildDir, f.Bootloader.File)),
			(f.App.Offset, Path.Combine(Paths.BuildDir, f.App.File)),
			(f.PartitionTable.Offset, Path.Combine(Paths.BuildDir, f.PartitionTable.File)),
			(f.Otadata.Offset, Path.Combine(Paths.BuildDir, f.Otadata.File)),
		];
		if (WriteStorage && f.Storage is not null)
		{
			sections.Add((f.Storage.Offset, Path.Combine(Paths.BuildDir, f.Storage.File)));
		}

		foreach (var (_, file) in sections)
		{
			if (!File.Exists(file))
			{
				throw new InvalidOperationException($"Zu flashende Datei nicht gefunden: {file} -- zuerst BuildFirmware ausfuehren.");
			}
		}

		Console.WriteLine($"Flashe {sections.Count} Sektionen (unverschluesselt) auf Board {ctx.Board.BoardName}...");
		EspTool.WriteFlash(sections);
		Console.WriteLine("Flash (nicht verschluesselt) abgeschlossen.");
	}
}

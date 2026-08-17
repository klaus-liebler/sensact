// Portierung der MAC-/Chip-/Flash-Verschluesselungs-Erkennung aus esp32.ts -- NICHT durch Neubau
// des dortigen eigenen SLIP/ROM-Loader-Protokolls, sondern durch Aufruf von esptool/espefuse, wie
// im Mapping der Migrationsplanung empfohlen ("nicht neu implementieren, sondern esptool.py ...
// via ProcessRunner aufrufen"). Real mit angeschlossenem Board geprueft: alle heute genutzten
// Felder (Chip-Typ, MAC, Efuse-Key-Purpose-Register fuer Flash-Encryption-Status) sind darueber
// erreichbar -- "espefuse summary --format json" liefert KEY_PURPOSE_0.."5"/SPI_BOOT_CRYPT_CNT
// bereits chip-uebergreifend decodiert (kein manuelles Register-/Adress-Mapping pro Chip-Variante
// mehr noetig, anders als im TS-Original mit separaten ESP32Classic/ESP32S3-Unterklassen).
//
// BEWUSST OHNE export.bat-Aktivierung (anders als Phases/BuildFirmware.cs/idf.py): esptool/espefuse
// sind eigenstaendige, pip-installierbare Pakete, keine Abhaengigkeit vom kompletten
// ESP-IDF-Environment noetig -- werden deshalb direkt ueber PATH aufgerufen. In dieser Session
// zusaetzlich real relevant: die ESP-IDF-Python-venv war defekt (s. BuildFirmware.cs), esptool/
// espefuse liessen sich aber unabhaengig davon per "pip install esptool" in eine andere,
// funktionierende Python-Umgebung installieren und funktionieren darueber einwandfrei. esptool
// erkennt den angeschlossenen Port zudem selbststaendig (kein VID/PID-Scan wie in esp32.ts's
// FindProbablePorts() noetig, solange genau ein Board angeschlossen ist).
using System.Text.Json;
using System.Text.RegularExpressions;

namespace Builder;

public sealed record HardwareIds(string ChipType, long Mac, bool HasFlashEncryptionKey);

public static class EspTool
{
	// KeyPurpose-Werte aus esp32.ts uebernommen (dort ein TS-Enum, hier direkt die numerischen
	// Werte, gelesen aus espefuse's "raw_value" statt per Bit-Shift aus einem roh gelesenen Register).
	private const int UserEmpty = 0;
	private const int XtsAes256Key1 = 2;
	private const int XtsAes256Key2 = 3;
	private const int XtsAes128Key = 4;

	public static HardwareIds ReadHardwareIds()
	{
		var (chipType, mac, port) = ReadMacAndChipType();
		var hasEncryptionKey = ReadFlashEncryptionKeyPresence(port);
		return new HardwareIds(chipType, mac, hasEncryptionKey);
	}

	// esptool erkennt den angeschlossenen Port selbststaendig (s. Kommentar oben) -- anders als
	// espefuse, das --port zwingend explizit braucht (kein Auto-Detect). Deshalb wird der von
	// esptool gefundene Port hier mitgelesen und an ReadFlashEncryptionKeyPresence() weitergereicht,
	// statt ihn ein zweites Mal zu suchen oder fest zu verdrahten.
	private static (string ChipType, long Mac, string Port) ReadMacAndChipType()
	{
		var output = ProcessRunner.Run("esptool", ["read-mac"]);
		var chipMatch = Regex.Match(output, @"Chip type:\s*(\S+)");
		var macMatch = Regex.Match(output, @"MAC:\s*([0-9a-fA-F:]{17})");
		var portMatch = Regex.Match(output, @"Serial port (\S+):");
		if (!chipMatch.Success || !macMatch.Success || !portMatch.Success)
		{
			throw new InvalidOperationException($"\"esptool read-mac\" lieferte kein auswertbares Ergebnis:\n{output}");
		}
		var macBytes = macMatch.Groups[1].Value.Split(':').Select(h => Convert.ToByte(h, 16)).ToArray();
		long mac = 0;
		foreach (var b in macBytes) mac = (mac << 8) | b;
		return (chipMatch.Groups[1].Value, mac, portMatch.Groups[1].Value);
	}

	private static int ReadRawValue(JsonElement root, string fieldName)
	{
		var raw = root.GetProperty(fieldName).GetProperty("raw_value").GetString()!;
		if (raw.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) raw = raw[2..];
		return Convert.ToInt32(raw, 16);
	}

	private static bool ReadFlashEncryptionKeyPresence(string port)
	{
		// Vor dem eigentlichen JSON-Objekt schreibt espefuse noch Banner-/Verbindungszeilen auf
		// stdout (kein reines "--format json" moeglich) -- deshalb ab dem ersten "{" geparst.
		var output = ProcessRunner.Run("espefuse", ["--port", port, "summary", "--format", "json"]);
		var jsonStart = output.IndexOf('{');
		if (jsonStart < 0)
		{
			throw new InvalidOperationException($"\"espefuse summary --format json\" lieferte kein auswertbares JSON:\n{output}");
		}
		using var doc = JsonDocument.Parse(output[jsonStart..]);
		var root = doc.RootElement;

		if (!root.TryGetProperty("SPI_BOOT_CRYPT_CNT", out _) || !root.TryGetProperty("KEY_PURPOSE_0", out _))
		{
			// Chip ohne diese Efuse-Felder (z.B. das urspruengliche ESP32 "Classic" -- s.
			// ESP32Classic in esp32.ts, das GAR KEINE Verschluesselungs-Erkennung implementiert).
			// Konservativ: keine Verschluesselung annehmen statt zu werfen.
			return false;
		}

		var purpose0 = ReadRawValue(root, "KEY_PURPOSE_0");
		var purpose1 = ReadRawValue(root, "KEY_PURPOSE_1");
		var spiBootCryptCnt = ReadRawValue(root, "SPI_BOOT_CRYPT_CNT");
		var hasOddBitCount = spiBootCryptCnt is 0b1 or 0b11 or 0b111;

		if (purpose0 == XtsAes256Key1 && purpose1 == XtsAes256Key2)
		{
			if (!hasOddBitCount)
			{
				throw new InvalidOperationException(
					$"Encryption Key ist XTS_AES_256, aber SPI_BOOT_CRYPT_CNT hat keine ungerade Anzahl gesetzter Bits, sondern 0b{Convert.ToString(spiBootCryptCnt, 2)}.");
			}
			return true;
		}
		if (purpose0 == XtsAes128Key && purpose1 == UserEmpty)
		{
			if (!hasOddBitCount)
			{
				throw new InvalidOperationException(
					$"Encryption Key ist XTS_AES_128, aber SPI_BOOT_CRYPT_CNT hat keine ungerade Anzahl gesetzter Bits, sondern 0b{Convert.ToString(spiBootCryptCnt, 2)}.");
			}
			return true;
		}
		if (purpose0 != UserEmpty || purpose1 != UserEmpty)
		{
			throw new InvalidOperationException("Unerwartete Key-Purposes in KEY_PURPOSE_0/KEY_PURPOSE_1.");
		}
		return false;
	}

	// Portierung von flashFirmware()/flashEncryptedFirmware()'s gemeinsamem esptool-Aufruf
	// (espidf.ts): "--port <port> write-flash --flash-size keep <offset1> <file1> <offset2> <file2> ...".
	// Kein --port noetig (esptool erkennt den Port selbst, s. Kommentar oben), es sei denn mehrere
	// Boards waeren gleichzeitig angeschlossen -- fuer den Einzelboard-Fall wie in dieser Session
	// ausreichend.
	public static void WriteFlash(IReadOnlyList<(string Offset, string File)> sections)
	{
		List<string> args = ["write-flash", "--flash-size", "keep"];
		foreach (var (offset, file) in sections)
		{
			args.Add(offset);
			args.Add(file);
		}
		ProcessRunner.RunInherit("esptool", args);
	}

	// Fuer --resetNVSPartition (s. FlashFirmware.Run): loescht NUR die angegebene Region (z.B. die
	// nvs-Partition), ohne die restlichen Partitionen anzufassen -- im Gegensatz zu "erase-flash",
	// das den GESAMTEN Chip loescht.
	public static void EraseRegion(string offset, long size)
	{
		ProcessRunner.RunInherit("esptool", ["erase-region", offset, $"0x{size:X}"]);
	}
}

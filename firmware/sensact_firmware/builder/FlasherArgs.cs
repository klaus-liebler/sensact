// Liest build/flasher_args.json (von ESP-IDF beim Build automatisch erzeugt) -- Portierung von
// GetFlashArgs()/IFlasherConfiguration (espidf.ts), aber an die ECHTE Datei angepasst: die
// TS-Schnittstelle erwartet ein Feld "partitionTable" (camelCase), die real von diesem ESP-IDF
// erzeugte Datei nennt es aber "partition-table" (mit Bindestrich) -- ein echter, vorbestehender
// Fehler im TS-Original (nie aufgefallen, weil dieser Wert dort vermutlich nie tatsaechlich
// gebraucht wurde). Hier bewusst mit dem tatsaechlichen Feldnamen gelesen statt den TS-Fehler
// nachzubilden.
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Builder;

public sealed class FlashSection
{
	[JsonPropertyName("offset")]
	public required string Offset { get; init; }

	[JsonPropertyName("file")]
	public required string File { get; init; }
}

public sealed class FlasherArgs
{
	[JsonPropertyName("bootloader")]
	public required FlashSection Bootloader { get; init; }

	[JsonPropertyName("app")]
	public required FlashSection App { get; init; }

	[JsonPropertyName("partition-table")]
	public required FlashSection PartitionTable { get; init; }

	[JsonPropertyName("otadata")]
	public required FlashSection Otadata { get; init; }

	[JsonPropertyName("storage")]
	public FlashSection? Storage { get; init; }

	public static FlasherArgs Load(string buildDir)
	{
		var path = Path.Combine(buildDir, "flasher_args.json");
		if (!File.Exists(path))
		{
			throw new InvalidOperationException($"{path} nicht gefunden -- zuerst BuildFirmware ausfuehren.");
		}
		return JsonSerializer.Deserialize<FlasherArgs>(File.ReadAllText(path))
			?? throw new InvalidOperationException($"{path} konnte nicht gelesen werden.");
	}
}

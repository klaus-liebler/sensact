// C#-Aequivalent von IBoardInfo (npm-packages/@klaus-liebler/espidf-vite-secure-build-tools/utils.ts)
// -- Feldnamen bewusst 1:1 (snake_case) uebernommen, da dieselbe board_info.json-Datei sowohl vom
// bestehenden TS-Builder als auch vom neuen C#-Builder gelesen/geschrieben wird (Uebergangszustand).
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Builder;

public sealed class IBoardInfo
{
	[JsonPropertyName("mac")]
	public long Mac { get; set; }

	[JsonPropertyName("board_name")]
	public string BoardName { get; set; } = "";

	[JsonPropertyName("board_version")]
	public long BoardVersion { get; set; }

	// TS-Typ ist "string|Array<string>" -- als rohes JsonElement uebernommen statt eines der beiden
	// Faelle zu erzwingen, da diese Phase board_roles nur anzeigt, nicht strukturell auswertet.
	[JsonPropertyName("board_roles")]
	public JsonElement BoardRoles { get; set; }

	[JsonPropertyName("first_connected_dt")]
	public long FirstConnectedDt { get; set; }

	[JsonPropertyName("last_connected_dt")]
	public long LastConnectedDt { get; set; }

	[JsonPropertyName("board_settings")]
	public JsonElement BoardSettings { get; set; }

	[JsonPropertyName("flash_encryption_key_burned_and_activated")]
	public bool FlashEncryptionKeyBurnedAndActivated { get; set; }

	// [JsonIgnore] noetig: sonst serialisiert JsonSerializer.Serialize(IBoardInfo) diese
	// berechneten Anzeige-Properties MIT in board_info.json hinein (reale Datei, real betroffen --
	// s. Fund beim ersten echten ReadHardwareIds-Testlauf, behoben inkl. Reparatur der bereits
	// verunreinigten Dateien, s. docs/plan_v2/02-builder-migration-csharp.md).
	[JsonIgnore]
	public string BoardRolesDisplay => BoardRoles.ValueKind switch
	{
		JsonValueKind.String => BoardRoles.GetString() is { Length: > 0 } s ? s : "(none)",
		JsonValueKind.Array => BoardRoles.GetArrayLength() == 0 ? "(none)" : string.Join(", ", BoardRoles.EnumerateArray().Select(e => e.GetString())),
		_ => "(none)",
	};

	[JsonIgnore]
	public string BoardSettingsDisplay => BoardSettings.ValueKind is JsonValueKind.Undefined or JsonValueKind.Null
		? "(none)"
		: BoardSettings.GetRawText();

	// Unix-Millisekunden (JS Date.now()-Konvention) -> lokale Zeit als ISO-String, analog zu
	// "new Date(b.first_connected_dt).toLocaleString()" in context.ts, aber durchgaengig
	// ISO-Format (yyyy-MM-dd HH:mm:ss) statt kulturabhaengiger Formatierung.
	public static string FromUnixMillis(long ms) => DateTimeOffset.FromUnixTimeMilliseconds(ms).ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss");
}

// Mac-Formatierung + Board-Verzeichnisname, analog zu mac_12char/mac_6char (utils.ts) und
// Paths.boardSpecificPath (paths.ts). Verzeichnisname-Konvention: "<mac6hex>_<macDezimal>_<mac12hex>".
public static class BoardPaths
{
	public static string Mac12Char(long mac) => mac.ToString("x12");

	public static string Mac6Char(long mac) => Mac12Char(mac)[6..];

	public static string BoardDirectoryName(long mac) => $"{Mac6Char(mac)}_{mac}_{Mac12Char(mac)}";

	public static string BoardSpecificPath(string boardsDir, long mac, params string[] subPathParts) =>
		Path.Combine([boardsDir, BoardDirectoryName(mac), .. subPathParts]);
}

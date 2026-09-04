// Sensact-spezifisch, NICHT Teil der geteilten Lib. Wird NICHT eager auf SensactBuildContext
// mitgefuehrt, sondern von genau den Schritten, die ihn brauchen, zur Laufzeit per statischem
// Loader aus sensact.json im BoardArchiveDir erzeugt -- funktioniert, obwohl [BuildStep]-Methoden
// nur IBuildContextEsp32 (nicht die konkrete SensactBuildContext-Klasse) entgegennehmen, weil der
// Loader nur ctx.BoardArchiveDir braucht, das bereits Teil des Basis-Interfaces ist.
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Builder;

public interface ISensactContext
{
	// Bereitgestellter Software-Funktionsumfang (welcher iHost/welche Apps laufen; bestimmt
	// indirekt, welche hal_sensact*.hh main/hal/<NodeId>/board_init.hh einbindet) -- NICHT
	// dasselbe wie IBuildContext.BoardTypeName (physische Hardware-Familie), s. NodeIdBoardTypeCatalog.
	string NodeId { get; }

	// vormals board_info.json's board_settings.web
	IReadOnlyDictionary<string, object> WebSettings { get; }

	// vormals board_info.json's board_settings.firmware
	IReadOnlyDictionary<string, object> FirmwareSettings { get; }
}

public static class SensactContextLoader
{
	private sealed record SensactJsonRecord(
		[property: JsonPropertyName("node_id")] string? NodeId,
		[property: JsonPropertyName("board_settings")] JsonElement BoardSettings);

	private sealed record SensactContext(
		string NodeId,
		IReadOnlyDictionary<string, object> WebSettings,
		IReadOnlyDictionary<string, object> FirmwareSettings) : ISensactContext;

	public static ISensactContext Load(string boardArchiveDir)
	{
		var path = Path.Combine(boardArchiveDir, "sensact.json");
		if (!File.Exists(path))
		{
			throw new InvalidOperationException($"Keine sensact.json im Board-Archiv gefunden ({path}).");
		}

		var record = JsonSerializer.Deserialize<SensactJsonRecord>(File.ReadAllText(path))
			?? throw new InvalidOperationException($"{path} konnte nicht gelesen werden.");
		if (string.IsNullOrEmpty(record.NodeId))
		{
			throw new InvalidOperationException($"{path} enthaelt keinen node_id.");
		}

		return new SensactContext(
			NodeId: record.NodeId,
			WebSettings: ExtractSection(record.BoardSettings, "web"),
			FirmwareSettings: ExtractSection(record.BoardSettings, "firmware"));
	}

	private static IReadOnlyDictionary<string, object> ExtractSection(JsonElement boardSettings, string sectionName)
	{
		var result = new Dictionary<string, object>();
		if (boardSettings.ValueKind == JsonValueKind.Object
			&& boardSettings.TryGetProperty(sectionName, out var section)
			&& section.ValueKind == JsonValueKind.Object)
		{
			foreach (var prop in section.EnumerateObject())
			{
				result[prop.Name] = JsonElementToObject(prop.Value);
			}
		}
		return result;
	}

	private static object JsonElementToObject(JsonElement el) => el.ValueKind switch
	{
		JsonValueKind.String => el.GetString()!,
		JsonValueKind.Number => el.TryGetInt64(out var l) ? l : el.GetDouble(),
		JsonValueKind.True => true,
		JsonValueKind.False => false,
		JsonValueKind.Array => el.EnumerateArray().Select(e => e.GetString() ?? "").ToList(),
		_ => el.GetRawText(),
	};
}

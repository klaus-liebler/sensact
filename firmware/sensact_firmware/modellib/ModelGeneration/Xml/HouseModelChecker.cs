using System.Xml;
using System.Xml.Linq;

namespace Klli.Sensact.Model.Xml;

/// <summary>
/// Prueft ein Hausmodell gegen den Katalog -- alles, was das XSD allein nicht kann (s. Kopf von
/// model/schema/sensact-house.xsd). Laeuft beim Laden nach der XSD-Pruefung und sammelt alle
/// Fehler mit Zeilennummer, statt beim ersten abzubrechen. Was die Codegenerierung danach
/// zusaetzlich prueft (Ziel versteht das gesendete Kommando), bleibt dort.
/// </summary>
public static class HouseModelChecker
{
	private static readonly string[] InputAttributes = ["input", "inputUp", "inputDown"];
	private static readonly string[] OutputAttributes = ["output", "relay1", "relay2", "outputR", "outputG", "outputB", "outputW"];
	private static readonly string[] OutputListAttributes = ["outputs"];

	/// <summary>App-Element -> Board-Feature, das es voraussetzt</summary>
	private static readonly IReadOnlyDictionary<string, string> RequiredFeature = new Dictionary<string, string>
	{
		["sound"] = "audio",
		["rotaryEncoder"] = "rotaryEncoder",
		["fingerprint"] = "fingerprint",
		["milightController"] = "milight",
		["rgbIndicator"] = "rgbLed",
	};

	/// <summary>Ein-/Ausgaenge, die mehrfach verwendet werden duerfen</summary>
	private static readonly HashSet<ushort> Shareable = [IoAddressing.INPUT0, IoAddressing.INPUT1, IoAddressing.OUTPUT_NULL];

	public static List<string> Check(XDocument doc, Catalog catalog)
	{
		var errors = new List<string>();
		void Err(XObject at, string message) => errors.Add($"Zeile {((IXmlLineInfo)at).LineNumber}: {message}");

		var house = doc.Root!;
		bool hasLocation = house.Element("location") != null;

		foreach (var s in house.Elements("station"))
		{
			string stationId = (string)s.Attribute("id")!;
			string boardType = (string)s.Attribute("board")!;
			if (!catalog.Boards.TryGetValue(boardType, out var board))
			{
				Err(s, $"Station {stationId}: Board-Typ \"{boardType}\" fehlt im Katalog (vorhanden: {string.Join(", ", catalog.Boards.Keys)}).");
				continue;
			}

			// ---- Erweiterungen
			var extensions = new List<ExtensionInstance>();
			var reservedNames = board.Buses.Select(b => b.Name).Concat(board.Buses.SelectMany(b => b.Pins).Select(p => p.Name)).ToHashSet(StringComparer.Ordinal);
			var names = new HashSet<string>(StringComparer.Ordinal);
			var addresses = new HashSet<(string, byte)>();
			foreach (var e in s.Elements("extension"))
			{
				string name = (string)e.Attribute("name")!, type = (string)e.Attribute("type")!, busName = (string)e.Attribute("bus")!;
				byte address = Catalog.ParseHexByte((string)e.Attribute("address")!);
				string what = $"Station {stationId}, Erweiterung {name}";
				if (!names.Add(name)) Err(e, $"{what}: Name doppelt.");
				if (reservedNames.Contains(name)) Err(e, $"{what}: Name ist schon ein Bus oder Pin von {boardType}.");
				if (!catalog.Extensions.TryGetValue(type, out var extType))
				{
					Err(e, $"{what}: Typ \"{type}\" fehlt im Katalog.");
					continue;
				}
				var bus = board.Buses.FirstOrDefault(b => b.Name == busName);
				if (bus == null)
				{
					Err(e, $"{what}: {boardType} hat keinen Bus \"{busName}\" (vorhanden: {string.Join(", ", board.Buses.Select(b => b.Name))}).");
					continue;
				}
				if (bus.Kind != extType.BusKind) Err(e, $"{what}: {type} braucht einen Bus der Art {extType.BusKind}, {busName} ist {bus.Kind}.");
				if (address < extType.AddressFrom || address > extType.AddressTo) Err(e, $"{what}: Adresse 0x{address:X2} ausserhalb 0x{extType.AddressFrom:X2}..0x{extType.AddressTo:X2}.");
				if (!addresses.Add((busName, address))) Err(e, $"{what}: Adresse 0x{address:X2} auf {busName} doppelt.");
				extensions.Add(new ExtensionInstance(name, type, busName, address));
			}

			// ---- Apps
			var io = new IoAddressing(catalog, boardType, extensions);
			var used = new Dictionary<(IoDirection, ushort), string>();
			var usedLeds = new HashSet<byte>();
			foreach (var a in s.Elements().Where(x => x.Name != "setting" && x.Name != "extension"))
			{
				string appId = (string)a.Attribute("id")!;
				string kind = a.Name.LocalName;

				void Use(XAttribute attr, string reference, IoDirection dir)
				{
					ushort id;
					try
					{
						id = io.FromRef(reference, dir);
					}
					catch (FormatException ex)
					{
						Err(attr, $"{appId}.{attr.Name}: {ex.Message}");
						return;
					}
					if (Shareable.Contains(id)) return;
					string here = $"{appId}.{attr.Name}";
					if (used.TryGetValue((dir, id), out var other))
					{
						Err(attr, $"{here}: {(dir == IoDirection.Input ? "Eingang" : "Ausgang")} \"{reference}\" ist schon von {other} belegt.");
					}
					else
					{
						used[(dir, id)] = here;
					}
				}

				foreach (var attr in a.Attributes())
				{
					string n = attr.Name.LocalName;
					if (InputAttributes.Contains(n)) Use(attr, attr.Value, IoDirection.Input);
					else if (OutputAttributes.Contains(n)) Use(attr, attr.Value, IoDirection.Output);
					else if (OutputListAttributes.Contains(n))
					{
						foreach (var r in attr.Value.Split(' ', StringSplitOptions.RemoveEmptyEntries)) Use(attr, r, IoDirection.Output);
					}
				}

				// Zeitangaben muessen in die internen Typen passen (u32 ms bzw. ganze Sekunden)
				foreach (var attr in a.Attributes())
				{
					try
					{
						switch (attr.Name.LocalName)
						{
							case "autoOff" or "onInterval" or "offInterval" or "lightOn": Units.DurationToMs(attr.Value); break;
							case "upTime" or "downTime": Units.DurationToWholeSeconds(attr.Value); break;
						}
					}
					catch (FormatException ex)
					{
						Err(attr, $"{appId}.{attr.Name}: {ex.Message}");
					}
				}

				if (RequiredFeature.TryGetValue(kind, out var feature) && !board.Features.ContainsKey(feature))
				{
					Err(a, $"{appId}: <{kind}> braucht das Board-Feature \"{feature}\", das {boardType} nicht hat.");
				}
				if (kind == "rotaryEncoder" && board.Features.TryGetValue("rotaryEncoder", out int count))
				{
					int index = ((string)a.Attribute("encoder")!) == "ROTENC1" ? 1 : 0;
					if (index >= count) Err(a, $"{appId}: {boardType} hat nur {count} Drehgeber.");
				}
				if (kind == "rgbIndicator" && board.Features.TryGetValue("rgbLed", out int leds))
				{
					foreach (var token in ((string?)a.Attribute("leds") ?? "").Split(' ', StringSplitOptions.RemoveEmptyEntries))
					{
						if (!byte.TryParse(token, out var led) || led >= leds)
						{
							Err(a, $"{appId}: LED {token} gibt es nicht ({boardType} hat {leds} RGB-LEDs, 0..{leds - 1}).");
						}
						else if (!usedLeds.Add(led))
						{
							Err(a, $"{appId}: LED {led} gehoert schon einer anderen Signal-App dieser Station.");
						}
					}
					if (a.Attribute("color") is XAttribute color)
					{
						try { Units.ColorToRgb(color.Value); } catch (FormatException ex) { Err(color, $"{appId}: {ex.Message}"); }
					}
				}
				if (kind == "blindsTimer" && !hasLocation && (a.Attribute("latitudeDeg") == null || a.Attribute("longitudeDeg") == null))
				{
					Err(a, $"{appId}: latitudeDeg/longitudeDeg fehlen und das Haus hat kein <location>.");
				}
			}
		}
		return errors;
	}
}

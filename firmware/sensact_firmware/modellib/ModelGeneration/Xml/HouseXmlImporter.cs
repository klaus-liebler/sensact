using System.Globalization;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Xml;

/// <summary>Station aus dem XML-Hausmodell; der Board-Typ steht im Katalog, nicht in einer C#-Klasse.</summary>
public sealed class XmlStation(ushort nodeId, string nodeName, string boardType) : Node(nodeId, nodeName)
{
	public string BoardType { get; } = boardType;
}

/// <summary>
/// Liest ein Hausmodell nach model/schema/sensact-house.xsd und
/// baut daraus die Objekte (Model, Node, *Application), die bis 2026-11-17 die C#-Hausmodelle
/// erzeugt haben, plus die Tabelle aller App-IDs. Die Codegenerierung (SourceCodeGenerator) bleibt
/// unveraendert. Standardwerte wie im XSD.
/// </summary>
public sealed class HouseXmlImporter
{
	private readonly Catalog catalog;

	public HouseXmlImporter(Catalog catalog)
	{
		this.catalog = catalog;
	}

	public sealed record Result(Common.Model Model, AppIdTable Ids);

	public Result Import(string housePath, string houseXsdPath)
	{
		var doc = XDocument.Load(housePath, LoadOptions.SetLineInfo);
		var schemas = new XmlSchemaSet();
		schemas.Add(null, houseXsdPath);
		var errors = new List<string>();
		doc.Validate(schemas, (_, e) => errors.Add($"Zeile {e.Exception?.LineNumber}: {e.Message}"));
		if (errors.Count > 0)
		{
			throw new InvalidOperationException($"{housePath} verletzt das Schema:\n" + string.Join('\n', errors));
		}
		errors = HouseModelChecker.Check(doc, catalog);
		if (errors.Count > 0)
		{
			throw new InvalidOperationException($"{housePath} passt nicht zum Katalog:\n" + string.Join('\n', errors));
		}
		return Import(doc);
	}

	/// <summary>Version des Hausmodell-Formats, die dieser Import versteht (Attribut schemaVersion).</summary>
	public const int SchemaVersion = 2;

	public Result Import(XDocument doc)
	{
		var house = doc.Root!;
		int version = (int?)house.Attribute("schemaVersion") ?? 0;
		if (version != SchemaVersion)
		{
			throw new FormatException($"Hausmodell hat schemaVersion {version}, erwartet wird {SchemaVersion}.");
		}
		var numbers = new Dictionary<string, ushort>(StringComparer.Ordinal);
		foreach (var e in house.Elements("station").SelectMany(s => s.Elements().Prepend(s)).Concat(house.Elements("reserved")))
		{
			if (e.Attribute("number") is XAttribute n)
			{
				numbers[(string)e.Attribute("id")!] = (ushort)(int)n;
			}
		}
		var ids = AppIdTable.FromNamesWithoutSpecials(numbers.Select(kv => new AppIdEntry(kv.Key, kv.Value)));

		var location = house.Element("location");
		var model = new Common.Model((string)house.Attribute("name")!);
		foreach (var s in house.Elements("station"))
		{
			string board = (string)s.Attribute("board")!;
			var station = new XmlStation((ushort)(int)s.Attribute("number")!, (string)s.Attribute("id")!, board);
			var extensions = s.Elements("extension").Select(e => new ExtensionInstance(
				(string)e.Attribute("name")!, (string)e.Attribute("type")!, (string)e.Attribute("bus")!,
				byte.Parse(((string)e.Attribute("address")!)[2..], NumberStyles.HexNumber, CultureInfo.InvariantCulture))).ToList();
			var io = new IoAddressing(catalog, board, extensions);
			foreach (var a in s.Elements().Where(e => e.Name != "setting" && e.Name != "extension"))
			{
				station.Applications.Add(ImportApp(a, io, numbers, location));
			}
			model.Nodes.Add(station);
		}
		return new Result(model, ids);
	}

	private static SensactApplication ImportApp(XElement a, IoAddressing io, IReadOnlyDictionary<string, ushort> numbers, XElement? location)
	{
		ushort id = (ushort)(int)a.Attribute("number")!;
		string name = (string)a.Attribute("id")!;
		string desc = (string?)a.Attribute("description") ?? string.Empty;

		string S(string attr) => (string?)a.Attribute(attr) ?? throw new FormatException($"{name}: Attribut {attr} fehlt.");
		ushort In(string attr) => io.FromRef(S(attr), IoDirection.Input);
		ushort Out(string attr) => io.FromRef(S(attr), IoDirection.Output);
		uint Ms(string attr, uint def) => a.Attribute(attr) is XAttribute x ? Units.DurationToMs(x.Value) : def;
		uint Sec(string attr) => Units.DurationToWholeSeconds(S(attr));
		ushort Pct(string attr, ushort def) => a.Attribute(attr) is XAttribute x ? Units.PercentToU16(x.Value) : def;
		ushort U16(string attr, ushort def) => a.Attribute(attr) is XAttribute x ? (ushort)(int)x : def;
		int I32(string attr, int def) => a.Attribute(attr) is XAttribute x ? (int)x : def;
		double Dbl(string attr, double? def) => a.Attribute(attr) is XAttribute x ? double.Parse(x.Value, CultureInfo.InvariantCulture)
			: def ?? throw new FormatException($"{name}: {attr} fehlt und das Haus hat kein <location>.");
		bool Bool(string attr, bool def) => a.Attribute(attr) is XAttribute x ? (bool)x : def;
		ushort Ref(string attr) => a.Attribute(attr) is XAttribute x ? numbers[x.Value] : Common.Model.NO_APPLICATION_ID;
		List<ushort> Refs(string attr) => a.Attribute(attr) is XAttribute x
			? x.Value.Split(' ', StringSplitOptions.RemoveEmptyEntries).Select(r => numbers[r]).ToList() : [];
		List<ushort> Outs(string attr) => S(attr).Split(' ', StringSplitOptions.RemoveEmptyEntries).Select(r => io.FromRef(r, IoDirection.Output)).ToList();
		T Enum<T>(string attr, T def) where T : struct, System.Enum => a.Attribute(attr) is XAttribute x ? System.Enum.Parse<T>(x.Value) : def;
		double? houseLat = location == null ? null : double.Parse((string)location.Attribute("latitudeDeg")!, CultureInfo.InvariantCulture);
		double? houseLon = location == null ? null : double.Parse((string)location.Attribute("longitudeDeg")!, CultureInfo.InvariantCulture);

		return a.Name.LocalName switch
		{
			"blind" => new BlindApplication(id, name, desc, Out("relay1"), Out("relay2"),
				System.Enum.Parse<RelayInterlockMode>(S("relayMode")), Sec("upTime"), Sec("downTime")),
			"onOff" => new OnOffApplication(id, name, desc, Out("output"),
				Bool("initiallyOn", false) ? PowerState.ACTIVE : PowerState.INACTIVE, Ms("autoOff", 0)),
			"singlePwm" => new SinglePWMApplication(id, name, desc, new HashSet<ushort>(Outs("outputs")),
				Pct("minimalBrightness", 0), Pct("initialBrightness", ushort.MaxValue), Ms("autoOff", 0), Ref("standbyController")),
			"rgbw" => new RgbwApplication(id, name, desc, Out("outputR"), Out("outputG"), Out("outputB"), Out("outputW"),
				Ms("autoOff", 0), Ref("standbyController")),
			"sound" => new SoundApplication(id, name, desc, U16("initialVolume", ushort.MaxValue), U16("defaultSignalOnToggle", 0)),
			"pump" => new PumpApplication(id, name, desc, Out("output"), Ms("onInterval", 0), Ms("offInterval", 0)),
			"toggleButton" => new PushButtonSingle2ToggleApplication(id, name, In("input"), new HashSet<ushort>(Refs("targets"))),
			"dimButton" => new PushButtonSingle2PwmSingleApplication(id, name, In("input"), Ref("target")),
			"blindButtons" => new PushButtonDual2BlindApplication(id, name, In("inputUp"), In("inputDown"), Ref("target")),
			"dualDimButtons" => new PushButtonDual2PWMApplication(id, name, In("inputUp"), In("inputDown"), Ref("target")),
			"rotaryEncoder" => new RotaryEncoder2PWMApplication(id, name, (ushort)Enum("encoder", RotaryEncoder.ROTENC0), Ref("target")),
			"lightbarrier" => new LightbarrierApplication(id, name, desc, In("input"), Ref("target"), Ref("brightnessSensor"), Bool("activeHigh", false)),
			"brightnessSensor" => new BrightnessSensorApplication(id, name, desc, S("sensorName"), Ref("toggleTarget"), I32("limitForPassingToggle", 0)),
			"fingerprint" => new FingerprintApplication(id, name, desc, Ref("target1"), Ref("target2"), Ref("target3")),
			"milightController" => new MilightControllerApplication(id, name, desc, Ref("light"), Refs("blinds"), Ms("lightOn", 12u * 60 * 60 * 1000)),
			"blindsTimer" => new BlindsTimerApplication(id, name, desc, new HashSet<ushort>(Refs("blinds")),
				Dbl("latitudeDeg", houseLat), Dbl("longitudeDeg", houseLon), Enum("dawn", Dawn.CIVIL),
				I32("openOffsetMin", 0), I32("closeOffsetMin", 0), U16("maxRandomMin", 0)),
			"forwarder" => new ForwarderApplication(id, name, desc, Ref("target")),
			"placeholder" => new PlaceholderApplication(id, name, desc),
			_ => throw new NotSupportedException($"{name}: App-Typ <{a.Name.LocalName}> unbekannt."),
		};
	}
}

/// <summary>
/// Umrechnung der lesbaren Einheiten im Hausmodell in die internen Werte der Firmware:
/// ISO-8601-Dauer -> Millisekunden (u32), Prozent -> u16-Skala 0..65535.
/// </summary>
public static class Units
{
	public static TimeSpan ParseDuration(string iso) => XmlConvert.ToTimeSpan(iso);

	public static uint DurationToMs(string iso)
	{
		double ms = ParseDuration(iso).TotalMilliseconds;
		if (ms < 0 || ms > uint.MaxValue) throw new FormatException($"Dauer {iso} ausserhalb 0..{TimeSpan.FromMilliseconds(uint.MaxValue)}.");
		return (uint)Math.Round(ms);
	}

	public static uint DurationToWholeSeconds(string iso)
	{
		var ts = ParseDuration(iso);
		if (ts.Ticks % TimeSpan.TicksPerSecond != 0) throw new FormatException($"Dauer {iso}: nur ganze Sekunden.");
		if (ts.TotalSeconds > ushort.MaxValue) throw new FormatException($"Dauer {iso}: hoechstens {ushort.MaxValue} s.");
		return (uint)ts.TotalSeconds;
	}

	public static ushort PercentToU16(string percent)
	{
		decimal p = decimal.Parse(percent, CultureInfo.InvariantCulture);
		if (p < 0 || p > 100) throw new FormatException($"Prozentwert {percent} ausserhalb 0..100.");
		return (ushort)Math.Round(p * 655.35m, MidpointRounding.AwayFromZero);
	}

	/// <summary>Kuerzeste Prozentangabe (hoechstens drei Nachkommastellen), die exakt wieder diesen u16-Wert ergibt.</summary>
	public static string U16ToPercent(ushort v)
	{
		for (int digits = 0; digits <= 3; digits++)
		{
			decimal p = Math.Round(v / 655.35m, digits, MidpointRounding.AwayFromZero);
			if (Math.Round(p * 655.35m, MidpointRounding.AwayFromZero) == v) return p.ToString(CultureInfo.InvariantCulture);
		}
		throw new InvalidOperationException($"u16-Wert {v} nicht als Prozent darstellbar.");
	}

	/// <summary>Lesbare Dauer fuer Anzeigen, z. B. "1 h 30 min", "0,5 s", "unbegrenzt".</summary>
	public static string Describe(TimeSpan ts)
	{
		if (ts == TimeSpan.Zero) return "0 (unbegrenzt/aus)";
		var parts = new List<string>();
		if (ts.Days > 0) parts.Add($"{ts.Days} d");
		if (ts.Hours > 0) parts.Add($"{ts.Hours} h");
		if (ts.Minutes > 0) parts.Add($"{ts.Minutes} min");
		double s = ts.Seconds + ts.Milliseconds / 1000.0;
		if (s > 0) parts.Add($"{s.ToString("0.###", CultureInfo.GetCultureInfo("de-DE"))} s");
		return string.Join(" ", parts);
	}
}

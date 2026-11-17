using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Xml;

/// <summary>
/// Einmaliger Uebergang (s. docs/concept-modular-firmware.md, "Datenhaltung: XML"): schreibt ein
/// C#-Hausmodell als XML nach model/schema/sensact-house.xsd. Prueft dabei
///   - jede Ein-/Ausgangsadresse per Rueckuebersetzung (lesbarer Name -> InOutId),
///   - das Ergebnis gegen das XSD.
/// Die App-Nummern kommen aus dem ApplicationId-Enum, dessen Werte heute nur implizit ueber die
/// Reihenfolge festliegen -- im XML stehen sie ausdruecklich.
/// </summary>
public sealed class HouseXmlExporter<TId> where TId : struct, Enum
{
	private readonly Catalog catalog;
	private readonly string firmwareRoot;
	private readonly Func<string, string?> boardTypeOfNodeId;

	/// <param name="boardTypeOfNodeId">Board-Typ einer ESP32-Station (NodeIdBoardTypeCatalog) oder null
	/// fuer Altknoten -- dann gilt der Name der C#-Node-Klasse.</param>
	public HouseXmlExporter(Catalog catalog, string firmwareRoot, Func<string, string?> boardTypeOfNodeId)
	{
		this.catalog = catalog;
		this.firmwareRoot = firmwareRoot;
		this.boardTypeOfNodeId = boardTypeOfNodeId;
	}

	private static string Name(ushort id) =>
		Enum.GetName(typeof(TId), Enum.ToObject(typeof(TId), id)) ?? throw new InvalidOperationException($"App-Nummer {id} fehlt im Enum {typeof(TId).Name}.");

	private static string? Ref(ushort id) => id == Common.Model.NO_APPLICATION_ID ? null : Name(id);

	private static string? Refs(IEnumerable<ushort> ids)
	{
		var names = ids.Where(i => i != Common.Model.NO_APPLICATION_ID).Select(Name).ToList();
		return names.Count == 0 ? null : string.Join(' ', names);
	}

	private static string Num(double d) => d.ToString("R", CultureInfo.InvariantCulture);

	public XDocument Export(Common.Model model)
	{
		var used = new HashSet<ushort>();
		XNamespace xsi = XmlSchema.InstanceNamespace;
		var house = new XElement("house",
			new XAttribute("name", model.Name),
			new XAttribute("schemaVersion", 1),
			new XAttribute(XNamespace.Xmlns + "xsi", xsi.NamespaceName),
			new XAttribute(xsi + "noNamespaceSchemaLocation", "schema/sensact-house.xsd"));

		// Standort: von der ersten Rollladen-Zeitschaltung (heute einzige Quelle); dort dann weggelassen
		var timer = model.Nodes.SelectMany(n => n.Applications).OfType<BlindsTimerApplication>().FirstOrDefault();
		if (timer != null)
		{
			house.Add(new XElement("location",
				new XAttribute("latitudeDeg", Num(timer.LatitudeDeg)),
				new XAttribute("longitudeDeg", Num(timer.LongitudeDeg))));
		}

		foreach (var node in model.Nodes.OrderBy(n => n.NodeId))
		{
			used.Add(node.NodeId);
			string board = boardTypeOfNodeId(node.NodeName) ?? node.GetType().Name;
			var extensions = ExtensionsFromBoardInit(node.NodeName);
			var io = new IoAddressing(catalog, board, extensions);

			var station = new XElement("station",
				new XAttribute("id", node.NodeName),
				new XAttribute("number", node.NodeId),
				new XAttribute("board", board));
			foreach (var e in extensions)
			{
				station.Add(new XElement("extension",
					new XAttribute("name", e.Name),
					new XAttribute("type", e.Type),
					new XAttribute("bus", e.Bus),
					new XAttribute("address", $"0x{e.Address:X2}")));
			}
			foreach (var app in node.Applications.OrderBy(a => a.ApplicationId))
			{
				used.Add(app.ApplicationId);
				station.Add(ExportApp(app, io, timer));
			}
			house.Add(station);
		}

		foreach (TId value in Enum.GetValues<TId>())
		{
			ushort n = Convert.ToUInt16(value);
			if (n == Common.Model.MASTER_APPLICATION_ID || n == Common.Model.NO_APPLICATION_ID || used.Contains(n)) continue;
			house.Add(new XElement("reserved", new XAttribute("id", value.ToString()), new XAttribute("number", n)));
		}

		return new XDocument(new XDeclaration("1.0", "utf-8", null), house);
	}

	private XElement ExportApp(SensactApplication app, IoAddressing io, BlindsTimerApplication? houseLocationSource)
	{
		string In(ushort id) => Checked(io, id, IoDirection.Input);
		string Out(ushort id) => Checked(io, id, IoDirection.Output);

		(string element, object?[] attrs) = app switch
		{
			BlindApplication a => ("blind", new object?[] {
				A("relay1", Out(a.OutputResource1)), A("relay2", Out(a.OutputResource2)),
				A("relayMode", a.RelayMode.ToString()),
				A("upTimeSec", a.UpTimeInSeconds), A("downTimeSec", a.DownTimeInSeconds) }),
			OnOffApplication a => ("onOff", new object?[] {
				A("output", Out(a.OutputRessource)),
				AIf("autoOffMs", a.AutoOffIntervalMsecs, a.AutoOffIntervalMsecs != 0),
				AIf("initiallyOn", "true", a.InitialPowerState == PowerState.ACTIVE) }),
			SinglePWMApplication a => ("singlePwm", new object?[] {
				A("outputs", string.Join(' ', a.OutputResources.Select(Out))),
				A("minimalOnLevel", a.MinimalOnLevel),
				AIf("initialLevel", a.InitialStoredTargetLevel, a.InitialStoredTargetLevel != ushort.MaxValue),
				AIf("autoOffMs", a.AutoOffIntervalMsecs, a.AutoOffIntervalMsecs != 0),
				A("standbyController", Ref(a.StandbyController)) }),
			RgbwApplication a => ("rgbw", new object?[] {
				A("outputR", Out(a.OutputRessourceR)), A("outputG", Out(a.OutputRessourceG)),
				A("outputB", Out(a.OutputRessourceB)), A("outputW", Out(a.OutputRessourceW)),
				AIf("autoOffMs", a.AutoOffIntervalMsecs, a.AutoOffIntervalMsecs != 0),
				A("standbyController", Ref(a.IdOfStandbyController)) }),
			SoundApplication a => ("sound", new object?[] {
				AIf("initialVolume", a.DefaultVolume, a.DefaultVolume != ushort.MaxValue),
				AIf("defaultSignalOnToggle", a.defaultSignalOnToggle, a.defaultSignalOnToggle != 0) }),
			PumpApplication a => ("pump", new object?[] {
				A("output", Out(a.OutputRessource)),
				A("onIntervalMs", a.OnIntervalMsecs), A("offIntervalMs", a.OffIntervalMsecs) }),
			PushButtonSingle2ToggleApplication a => ("toggleButton", new object?[] {
				A("input", In(a.InputResource)), A("targets", Refs(a.TargetApplicationIds)) }),
			PushButtonSingle2PwmSingleApplication a => ("dimButton", new object?[] {
				A("input", In(a.InputResource)), A("target", Ref(a.TargetApplicationId)) }),
			PushButtonDual2BlindApplication a => ("blindButtons", new object?[] {
				A("inputUp", In(a.InputResource1)), A("inputDown", In(a.InputResource2)), A("target", Ref(a.TargetApplicationId)) }),
			PushButtonDual2PWMApplication a => ("dualDimButtons", new object?[] {
				A("inputUp", In(a.InputResource1)), A("inputDown", In(a.InputResource2)), A("target", Ref(a.TargetApplicationId)) }),
			RotaryEncoder2PWMApplication a => ("rotaryEncoder", new object?[] {
				A("encoder", ((RotaryEncoder)a.InputRotaryRessource).ToString()), A("target", Ref(a.TargetApplication)) }),
			LightbarrierApplication a => ("lightbarrier", new object?[] {
				A("input", In(a.InputRessource)), A("activeHigh", a.ActiveSignalLevel ? "true" : "false"),
				A("target", Ref(a.FinalTarget)), A("brightnessSensor", Ref(a.BrightnessSensor)) }),
			BrightnessSensorApplication a => ("brightnessSensor", new object?[] {
				A("sensorName", a.SensorName), A("toggleTarget", Ref(a.ToggleTarget)),
				A("limitForPassingToggle", a.LimitForPassingToggle) }),
			FingerprintApplication a => ("fingerprint", new object?[] {
				A("target1", Ref(a.Target1)), A("target2", Ref(a.Target2)), A("target3", Ref(a.Target3)) }),
			MilightControllerApplication a => ("milightController", new object?[] {
				A("light", Ref(a.Light)), A("blinds", Refs(a.Blinds)),
				AIf("lightOnMs", a.LightOnMsecs, a.LightOnMsecs != 12u * 60 * 60 * 1000) }),
			BlindsTimerApplication a => ("blindsTimer", new object?[] {
				A("blinds", Refs(a.BlindTargets)),
				AIf("latitudeDeg", Num(a.LatitudeDeg), houseLocationSource == null || a.LatitudeDeg != houseLocationSource.LatitudeDeg || a.LongitudeDeg != houseLocationSource.LongitudeDeg),
				AIf("longitudeDeg", Num(a.LongitudeDeg), houseLocationSource == null || a.LatitudeDeg != houseLocationSource.LatitudeDeg || a.LongitudeDeg != houseLocationSource.LongitudeDeg),
				AIf("dawn", a.Dawn.ToString(), a.Dawn != Dawn.CIVIL),
				AIf("openOffsetMin", a.OpenOffsetMinutes, a.OpenOffsetMinutes != 0),
				AIf("closeOffsetMin", a.CloseOffsetMinutes, a.CloseOffsetMinutes != 0),
				AIf("maxRandomMin", a.MaxRandomMinutes, a.MaxRandomMinutes != 0) }),
			ForwarderApplication a => ("forwarder", new object?[] { A("target", Ref(a.FinalTarget)) }),
			PlaceholderApplication => ("placeholder", Array.Empty<object?>()),
			_ => throw new NotSupportedException($"App-Typ {app.GetType().Name} ({app.ApplicationName}) hat noch keine XML-Abbildung."),
		};

		if (app.ApplicationName != Name(app.ApplicationId))
		{
			throw new InvalidOperationException($"App {app.ApplicationName}: Name passt nicht zur Nummer {app.ApplicationId} ({Name(app.ApplicationId)}).");
		}
		var el = new XElement(element,
			new XAttribute("id", app.ApplicationName),
			new XAttribute("number", app.ApplicationId));
		if (!string.IsNullOrWhiteSpace(app.ApplicationDescription))
		{
			el.Add(new XAttribute("description", app.ApplicationDescription));
		}
		el.Add(attrs.Where(x => x != null));
		return el;
	}

	private static XAttribute? A(string name, object? value) => value == null ? null : new XAttribute(name, value);
	private static XAttribute? AIf(string name, object value, bool condition) => condition ? new XAttribute(name, value) : null;

	private static string Checked(IoAddressing io, ushort id, IoDirection dir)
	{
		string r = io.ToRef(id, dir);
		ushort back = io.FromRef(r, dir);
		if (back != id)
		{
			throw new InvalidOperationException($"Ein-/Ausgang 0x{id:X4} ({dir}) -> \"{r}\" -> 0x{back:X4}: Rueckuebersetzung stimmt nicht.");
		}
		return r;
	}

	/// <summary>
	/// Erweiterungs-Chips einer ESP32-Station, gelesen aus main/hal/&lt;NODE_ID&gt;/board_init.hh
	/// (Reihenfolge = Chip-Index). Alle Boards haben genau einen I2C-Busmaster ("SensactBus").
	/// </summary>
	private List<ExtensionInstance> ExtensionsFromBoardInit(string nodeId)
	{
		var result = new List<ExtensionInstance>();
		string path = Path.Combine(firmwareRoot, "main", "hal", nodeId, "board_init.hh");
		if (!File.Exists(path)) return result;
		string text = File.ReadAllText(path);
		int i = 0;
		foreach (Match m in Regex.Matches(text, @"new PCA9555::M\(PCA9555::Device::Dev(\d+)"))
		{
			result.Add(new ExtensionInstance($"IN{i++}", "PCA9555", "SensactBus", (byte)(0x20 + int.Parse(m.Groups[1].Value, CultureInfo.InvariantCulture))));
		}
		i = 0;
		foreach (Match m in Regex.Matches(text, @"new PCA9685::M\(PCA9685::Device::Dev(\d+)"))
		{
			result.Add(new ExtensionInstance($"OUT{i++}", "PCA9685", "SensactBus", (byte)(0x40 + int.Parse(m.Groups[1].Value, CultureInfo.InvariantCulture))));
		}
		return result;
	}

	public static void Validate(XDocument doc, string houseXsdPath)
	{
		var schemas = new XmlSchemaSet();
		schemas.Add(null, houseXsdPath);
		var errors = new List<string>();
		doc.Validate(schemas, (_, e) => errors.Add($"{e.Severity}: {e.Message}"), addSchemaInfo: false);
		if (errors.Count > 0)
		{
			throw new InvalidOperationException("Hausmodell-XML verletzt das Schema:\n" + string.Join('\n', errors));
		}
	}

	public static void Save(XDocument doc, string path)
	{
		var settings = new XmlWriterSettings
		{
			Indent = true,
			IndentChars = "  ",
			NewLineChars = "\n",
			Encoding = new UTF8Encoding(false),
		};
		using var w = XmlWriter.Create(path, settings);
		doc.Save(w);
	}
}

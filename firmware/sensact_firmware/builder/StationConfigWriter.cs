// Stationskonfiguration (Abschnitt 2 des Stationspakets, Schema best_binary_buffers_schema/
// sensactcfg.cs): was die datengetriebene Basis-Firmware beim Start anlegt -- Busse mit ihren
// Erweiterungs-Chips und die Apps der Station. Kodiert mit BestBinaryBuffers.Runtime.BinaryEncoder
// auf demselben geparsten Schema (und derselben ids.txt) wie der C++-Decoder der Firmware.
//
// Die App-Parameter kommen aus denselben Modellobjekten, aus denen bisher der C++-Code
// (applicationInitializers.inc) generiert wurde -- Werte in der internen Darstellung (InOutId,
// Millisekunden, u16-Pegel).
using System.Globalization;
using BestBinaryBuffers;
using BestBinaryBuffers.Model;
using BestBinaryBuffers.Runtime;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using Klli.Sensact.Model.Xml;

namespace Builder;

public static class StationConfigWriter
{
	public const ushort FormatVersion = 1;
	private const string Ns = "sensactcfg";

	/// <summary>Parst die Schemas (sensact + Webmanager) mit der ids.txt des Projekts -- dieselben Ids wie ws_protocol.hh.</summary>
	public static IReadOnlyList<NamespaceDef> ParseSchema(IReadOnlyList<string> schemaDirs, IdMap idMap) =>
		SchemaCompiler.Parse(schemaDirs.SelectMany(d => Directory.GetFiles(d, "*.cs")).ToList(), idMap);

	public static byte[] Encode(IReadOnlyList<NamespaceDef> schema, Klli.Sensact.Model.Common.Model model, AppIdTable ids, Catalog catalog, string nodeId)
	{
		int nodeIndex = model.Nodes.FindIndex(n => n.NodeName == nodeId);
		if (nodeIndex < 0) throw new InvalidOperationException($"Station {nodeId} gibt es im Hausmodell {model.Name} nicht.");
		var node = model.Nodes[nodeIndex];
		var station = node as XmlStation ?? throw new InvalidOperationException($"Station {nodeId}: kein XML-Hausmodell.");
		if (!catalog.Boards.TryGetValue(station.BoardType, out var board)) throw new InvalidOperationException($"Board {station.BoardType} fehlt im Katalog.");

		var buses = new List<object>();
		foreach (var bus in board.Buses)
		{
			switch (bus.Kind)
			{
				case "gpio":
					buses.Add(C("GpioBus", ("Index", bus.Index)));
					break;
				case "i2c":
					byte[] Chips(string type) => station.Extensions.Where(e => e.Bus == bus.Name && e.Type == type).Select(e => e.Address).ToArray();
					buses.Add(C("I2cBus", ("Index", bus.Index), ("Pca9555", Chips("PCA9555")), ("Pca9685", Chips("PCA9685"))));
					break;
				default:
					throw new InvalidOperationException($"Station {nodeId}: Bus {bus.Name} (Art {bus.Kind}) wird von der ESP32-Basis nicht unterstuetzt.");
			}
		}

		var apps = new List<object> { C("NodeApp", ("Id", node.NodeId)) };
		foreach (var app in node.Applications)
		{
			if (Map(app) is { } cfg) apps.Add(cfg);
		}

		ushort appIdCount = (ushort)(ids.Entries.Where(e => e.Number != Klli.Sensact.Model.Common.Model.NO_APPLICATION_ID).Max(e => e.Number) + 1);
		var names = ids.Entries.Where(e => e.Number != Klli.Sensact.Model.Common.Model.NO_APPLICATION_ID)
			.Select(e => (object)C("AppName", ("Id", e.Number), ("Name", e.Name))).ToList();

		var value = new Dictionary<string, object?>
		{
			["FormatVersion"] = FormatVersion,
			["NodeId"] = (ushort)nodeIndex,
			["NodeAppId"] = node.NodeId,
			["NodeName"] = node.NodeName,
			["HouseName"] = model.Name,
			["Created"] = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss", CultureInfo.InvariantCulture),
			["AppIdCount"] = appIdCount,
			["Buses"] = buses,
			["Apps"] = apps,
			["Names"] = names,
		};
		return new BinaryEncoder(schema).EncodeMessage(Ns, "StationConfig", value);
	}

	private static Dictionary<string, object?> C(string cls, params (string Key, object? Value)[] fields)
	{
		var d = new Dictionary<string, object?> { [BinaryEncoder.ClassKey] = cls };
		foreach (var (k, v) in fields) d[k] = v;
		return d;
	}

	private static int MicroDeg(double deg) => (int)Math.Round(deg * 1e6);

	/// <summary>Modellobjekt -> Konfigurationselement; null fuer Apps ohne Laufzeitobjekt (Platzhalter).</summary>
	private static Dictionary<string, object?>? Map(SensactApplication app) => app switch
	{
		PlaceholderApplication => null,
		BlindApplication a => C("BlindApp", ("Id", a.ApplicationId), ("Relay1", a.OutputResource1), ("Relay2", a.OutputResource2),
			("Mode", a.RelayMode.ToString()), ("UpTimeMs", (uint)(a.UpTimeInSeconds * 1000)), ("DownTimeMs", (uint)(a.DownTimeInSeconds * 1000))),
		OnOffApplication a => C("OnOffApp", ("Id", a.ApplicationId), ("Output", a.OutputRessource),
			("InitiallyOn", a.InitialPowerState == PowerState.ACTIVE), ("AutoOffMs", a.AutoOffIntervalMsecs)),
		SinglePWMApplication a => C("SinglePwmApp", ("Id", a.ApplicationId), ("Outputs", a.OutputResources.ToArray()),
			("MinimalLevel", a.MinimalOnLevel), ("InitialLevel", a.InitialStoredTargetLevel), ("AutoOffMs", a.AutoOffIntervalMsecs), ("StandbyController", a.StandbyController)),
		RgbwApplication a => C("RgbwApp", ("Id", a.ApplicationId), ("OutputR", a.OutputRessourceR), ("OutputG", a.OutputRessourceG),
			("OutputB", a.OutputRessourceB), ("OutputW", a.OutputRessourceW), ("AutoOffMs", a.AutoOffIntervalMsecs), ("StandbyController", a.IdOfStandbyController)),
		SoundApplication a => C("SoundApp", ("Id", a.ApplicationId), ("InitialVolume", a.DefaultVolume), ("DefaultSignalOnToggle", a.defaultSignalOnToggle)),
		PumpApplication a => C("PumpApp", ("Id", a.ApplicationId), ("Output", a.OutputRessource), ("OnIntervalMs", a.OnIntervalMsecs), ("OffIntervalMs", a.OffIntervalMsecs)),
		PushButtonSingle2ToggleApplication a => C("ToggleButtonApp", ("Id", a.ApplicationId), ("Input", a.InputResource), ("Targets", a.TargetApplicationIds.ToArray())),
		PushButtonSingle2PwmSingleApplication a => C("DimButtonApp", ("Id", a.ApplicationId), ("Input", a.InputResource), ("Target", a.TargetApplicationId)),
		PushButtonDual2PWMApplication a => C("DualDimButtonsApp", ("Id", a.ApplicationId), ("InputUp", a.InputResource1), ("InputDown", a.InputResource2), ("Target", a.TargetApplicationId)),
		PushButtonDual2BlindApplication a => C("BlindButtonsApp", ("Id", a.ApplicationId), ("InputUp", a.InputResource1), ("InputDown", a.InputResource2), ("Target", a.TargetApplicationId)),
		RotaryEncoder2PWMApplication a => C("RotaryEncoderApp", ("Id", a.ApplicationId), ("Encoder", a.InputRotaryRessource), ("Target", a.TargetApplication)),
		LightbarrierApplication a => C("LightbarrierApp", ("Id", a.ApplicationId), ("Input", a.InputRessource), ("ActiveHigh", a.ActiveSignalLevel),
			("BrightnessSensor", a.BrightnessSensor), ("Target", a.FinalTarget)),
		FingerprintApplication a => C("FingerprintApp", ("Id", a.ApplicationId), ("Target1", a.Target1), ("Target2", a.Target2), ("Target3", a.Target3)),
		MilightControllerApplication a => C("MilightControllerApp", ("Id", a.ApplicationId), ("Light", a.Light), ("Blinds", a.Blinds.ToArray()), ("LightOnMs", a.LightOnMsecs)),
		BlindsTimerApplication a => C("BlindsTimerApp", ("Id", a.ApplicationId), ("Blinds", a.BlindTargets.ToArray()),
			("LatitudeMicroDeg", MicroDeg(a.LatitudeDeg)), ("LongitudeMicroDeg", MicroDeg(a.LongitudeDeg)), ("Dawn", a.Dawn.ToString()),
			("OpenOffsetMin", (short)a.OpenOffsetMinutes), ("CloseOffsetMin", (short)a.CloseOffsetMinutes), ("MaxRandomMin", a.MaxRandomMinutes)),
		_ => throw new NotSupportedException($"{app.ApplicationName}: App-Typ {app.GetType().Name} hat (noch) keine Laufzeit-App in der Basis-Firmware."),
	};
}

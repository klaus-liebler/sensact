using System.Globalization;
using System.Xml.Linq;

namespace Klli.Sensact.Model.Xml;

public enum IoDirection { Input, Output }

/// <summary>
/// Katalog (model/catalog.xml) als Objekte -- nur das, was fuer die Adressierung der
/// Ein-/Ausgaenge gebraucht wird. Regeln s. model/schema/sensact-catalog.xsd.
/// </summary>
public sealed class Catalog
{
	public sealed record Pin(string Name, ushort Index, IoDirection Direction);
	public sealed record Bus(int Index, string Name, string Kind, IReadOnlyList<Pin> Pins);
	public sealed record Board(string Type, string Firmware, IReadOnlyList<Bus> Buses);
	public sealed record ExtensionType(string Type, IoDirection Direction, int Channels);

	public IReadOnlyDictionary<string, Board> Boards { get; }
	public IReadOnlyDictionary<string, ExtensionType> Extensions { get; }

	private Catalog(IReadOnlyDictionary<string, Board> boards, IReadOnlyDictionary<string, ExtensionType> extensions)
	{
		Boards = boards;
		Extensions = extensions;
	}

	public static Catalog Load(string path)
	{
		var doc = XDocument.Load(path);
		var boards = doc.Root!.Elements("board").Select(b => new Board(
			(string)b.Attribute("type")!,
			(string)b.Attribute("firmware")!,
			b.Elements("bus").Select(bus => new Bus(
				(int)bus.Attribute("index")!,
				(string)bus.Attribute("name")!,
				(string)bus.Attribute("kind")!,
				bus.Elements("pin").Select(p => new Pin(
					(string)p.Attribute("name")!,
					(ushort)(int)p.Attribute("index")!,
					ParseDirection((string)p.Attribute("direction")!))).ToList())).ToList()))
			.ToDictionary(b => b.Type, StringComparer.Ordinal);
		var extensions = doc.Root!.Elements("extension").Select(e => new ExtensionType(
			(string)e.Attribute("type")!,
			ParseDirection((string)e.Attribute("direction")!),
			(int)e.Attribute("channels")!))
			.ToDictionary(e => e.Type, StringComparer.Ordinal);
		return new Catalog(boards, extensions);
	}

	public static IoDirection ParseDirection(string s) => s switch
	{
		"input" => IoDirection.Input,
		"output" => IoDirection.Output,
		_ => throw new FormatException($"Unbekannte Richtung \"{s}\""),
	};
}

/// <summary>Verbauter Erweiterungs-Chip einer Station (Element &lt;extension&gt; im Hausmodell).</summary>
public sealed record ExtensionInstance(string Name, string Type, string Bus, byte Address);

/// <summary>
/// Uebersetzt zwischen der heutigen numerischen InOutId und der lesbaren Schreibweise im
/// Hausmodell (Typ IoRef, s. sensact-house.xsd) -- in beide Richtungen, damit der Export per
/// Rueckuebersetzung pruefen kann, dass nichts verloren geht.
/// </summary>
public sealed class IoAddressing
{
	public const ushort INPUT0 = 0x3FFE;
	public const ushort INPUT1 = 0x3FFF;
	public const ushort OUTPUT_NULL = 0x3FFC;

	private readonly Catalog catalog;
	private readonly Catalog.Board board;
	private readonly IReadOnlyList<ExtensionInstance> extensions;

	public IoAddressing(Catalog catalog, string boardType, IReadOnlyList<ExtensionInstance> extensions)
	{
		this.catalog = catalog;
		if (!catalog.Boards.TryGetValue(boardType, out var b))
		{
			throw new InvalidOperationException($"Board-Typ \"{boardType}\" fehlt im Katalog.");
		}
		board = b;
		this.extensions = extensions;
	}

	/// <summary>Chips gleicher Richtung auf einem Bus, in Deklarationsreihenfolge (= Chip-Index).</summary>
	private List<ExtensionInstance> ChipsOn(string busName, IoDirection dir) =>
		extensions.Where(e => e.Bus == busName && catalog.Extensions.TryGetValue(e.Type, out var t) && t.Direction == dir).ToList();

	public string ToRef(ushort id, IoDirection dir)
	{
		if (dir == IoDirection.Input && id == INPUT0) return "INPUT0";
		if (dir == IoDirection.Input && id == INPUT1) return "INPUT1";
		if (dir == IoDirection.Output && id == OUTPUT_NULL) return "NONE";

		int busIndex = id >> 14;
		int local = id & 0x3FFF;
		var bus = board.Buses.FirstOrDefault(b => b.Index == busIndex);
		if (bus == null)
		{
			return $"#0x{id:X4}";
		}
		switch (bus.Kind)
		{
			case "i2c":
				var chips = ChipsOn(bus.Name, dir);
				int chip = local >> 4;
				if (chip < chips.Count)
				{
					return $"{chips[chip].Name}.{local & 0x0F}";
				}
				break;
			case "gpio":
				var pin = bus.Pins.FirstOrDefault(p => p.Index == local && p.Direction == dir);
				if (pin != null)
				{
					return pin.Name;
				}
				break;
		}
		return $"{bus.Name}.{local}";
	}

	public ushort FromRef(string reference, IoDirection dir)
	{
		switch (reference)
		{
			case "INPUT0" when dir == IoDirection.Input: return INPUT0;
			case "INPUT1" when dir == IoDirection.Input: return INPUT1;
			case "NONE" when dir == IoDirection.Output: return OUTPUT_NULL;
		}
		if (reference.StartsWith("#0x", StringComparison.Ordinal))
		{
			return ushort.Parse(reference.AsSpan(3), NumberStyles.HexNumber, CultureInfo.InvariantCulture);
		}
		string name = reference;
		int? number = null;
		int dot = reference.LastIndexOf('.');
		if (dot > 0)
		{
			name = reference[..dot];
			number = int.Parse(reference[(dot + 1)..], CultureInfo.InvariantCulture);
		}

		var ext = extensions.FirstOrDefault(e => e.Name == name);
		if (ext != null)
		{
			if (number == null) throw new FormatException($"\"{reference}\": Kanal fehlt (Erweiterung.Kanal).");
			if (!catalog.Extensions.TryGetValue(ext.Type, out var type)) throw new FormatException($"\"{reference}\": Typ {ext.Type} fehlt im Katalog.");
			if (type.Direction != dir) throw new FormatException($"\"{reference}\": {ext.Type} ist kein {dir}.");
			if (number >= type.Channels) throw new FormatException($"\"{reference}\": {ext.Type} hat nur {type.Channels} Kanaele.");
			var bus = board.Buses.FirstOrDefault(b => b.Name == ext.Bus) ?? throw new FormatException($"\"{reference}\": Bus {ext.Bus} gibt es auf {board.Type} nicht.");
			int chip = ChipsOn(bus.Name, dir).IndexOf(ext);
			return Compose(bus.Index, chip * 16 + number.Value);
		}
		if (number == null)
		{
			foreach (var b in board.Buses)
			{
				var pin = b.Pins.FirstOrDefault(p => p.Name == name && p.Direction == dir);
				if (pin != null) return Compose(b.Index, pin.Index);
			}
			throw new FormatException($"\"{reference}\": weder Pin noch Erweiterung von {board.Type}.");
		}
		var rawBus = board.Buses.FirstOrDefault(b => b.Name == name) ?? throw new FormatException($"\"{reference}\": unbekannter Bus/Erweiterung \"{name}\".");
		return Compose(rawBus.Index, number.Value);
	}

	private static ushort Compose(int busIndex, int local)
	{
		if (local < 0 || local > 0x3FFF) throw new FormatException($"Lokaler Index {local} ausserhalb 0..16383.");
		return (ushort)((busIndex << 14) | local);
	}
}

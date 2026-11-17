// Stationspaket (s. docs/concept-modular-firmware.md, "Web-UI als Teil des Stationspakets", und das
// Gegenstueck auf dem Geraet: espidf-component-webmanager/cpp/station_package.hh).
//
// Inhalt: Abschnitt 1 die Web-UI der Station, Abschnitt 2 die Stationskonfiguration (Busse, Apps;
// StationConfigWriter). Die Basis-Firmware ist stationsneutral; das Paket passt zu jeder Basis mit
// derselben compatId -- also mit denselben Kommandos, demselben ws-Protokoll und demselben
// Konfigurationsschema. Das Hausmodell geht NICHT in die Kennung ein: eine Modellaenderung braucht
// nur ein neues Paket, keine neue Firmware.
using System.Security.Cryptography;
using System.Text;

namespace Builder;

public static class StationPackage
{
	private static readonly byte[] Magic = "SNSCTPKG"u8.ToArray();
	private const ushort FormatVersion = 1;
	public const ushort SectionWebUiHtmlBr = 1;
	public const ushort SectionStationConfig = 2;
	private const int HeaderSize = 128;
	private const int SectionSize = 48;

	// Dateien unter generated/sensact_model/common, die vom Hausmodell abhaengen (App-Nummern und
	// -Namen, Knoten, App-Liste der Web-UI) -- die Basis-Firmware benutzt sie nicht.
	private static readonly HashSet<string> ModelSpecificCommonFiles = new(StringComparer.OrdinalIgnoreCase)
	{
		"applicationIds.inc", "applicationIds.fbs.inc", "applicationIds.enums.cs", "applicationNames.inc",
		"nodeIds.inc", "sensactapps.ts.inc",
	};

	/// <summary>
	/// SHA-256 ueber alles, was Basis-Firmware und Paket gemeinsam haben muessen: die
	/// modellunabhaengigen generierten Dateien (Kommandos, Events, Dispatch) und den
	/// BestBinaryBuffers-Code (ws-Protokoll, Konfigurationsschema). Gleiche Basis -> gleiche Kennung,
	/// unabhaengig von Station und Hausmodell.
	/// </summary>
	public static byte[] ComputeCompatId()
	{
		var common = Path.Combine(SensactBuildContext.SensactModelGeneratedDir, "common");
		var roots = new[] { common, SensactBuildContext.GeneratedWsProtocolCppDir };
		using var sha = IncrementalHash.CreateHash(HashAlgorithmName.SHA256);
		foreach (var root in roots)
		{
			if (!Directory.Exists(root))
			{
				throw new InvalidOperationException($"{root} fehlt -- GenerateModelFiles und GenerateBestBinaryBufferFiles muessen vorher laufen.");
			}
			foreach (var file in Directory.EnumerateFiles(root, "*", SearchOption.AllDirectories)
				.Where(f => !(root == common && ModelSpecificCommonFiles.Contains(Path.GetFileName(f))))
				.OrderBy(f => Path.GetRelativePath(root, f).Replace('\\', '/'), StringComparer.Ordinal))
			{
				sha.AppendData(Encoding.UTF8.GetBytes(Path.GetFileName(root) + "/" + Path.GetRelativePath(root, file).Replace('\\', '/') + "\0"));
				// Zeilenenden normalisieren: dieselben Quellen sollen unabhaengig von Git-Einstellungen dieselbe Kennung geben
				sha.AppendData(NormalizeNewlines(File.ReadAllBytes(file)));
				sha.AppendData([0]);
			}
		}
		return sha.GetHashAndReset();
	}

	private static byte[] NormalizeNewlines(byte[] data)
	{
		var result = new List<byte>(data.Length);
		for (int i = 0; i < data.Length; i++)
		{
			if (data[i] == '\r' && i + 1 < data.Length && data[i + 1] == '\n') continue;
			result.Add(data[i]);
		}
		return result.ToArray();
	}

	public static string Hex(byte[] bytes) => Convert.ToHexString(bytes).ToLowerInvariant();

	/// <summary>Schreibt ein Paket mit den angegebenen Abschnitten (Art, Inhalt) fuer eine Station.</summary>
	public static void Write(string path, string nodeId, byte[] compatId, IReadOnlyList<(ushort Type, byte[] Data)> sections, string label)
	{
		var nodeBytes = Encoding.ASCII.GetBytes(nodeId);
		if (nodeBytes.Length > 31) throw new ArgumentException($"NODE_ID {nodeId} laenger als 31 Zeichen.");
		var labelBytes = Encoding.UTF8.GetBytes(label);
		if (labelBytes.Length > 39) labelBytes = labelBytes[..39];

		int headerSize = HeaderSize + SectionSize * sections.Count;
		using var ms = new MemoryStream();
		using var w = new BinaryWriter(ms); // little endian
		w.Write(Magic);
		w.Write(FormatVersion);
		w.Write((ushort)sections.Count);
		w.Write((uint)headerSize);
		w.Write(Pad(nodeBytes, 32));
		w.Write(compatId);
		w.Write((ulong)DateTimeOffset.UtcNow.ToUnixTimeSeconds());
		w.Write(Pad(labelBytes, 40));
		uint offset = (uint)headerSize;
		foreach (var (type, data) in sections)
		{
			w.Write(type);
			w.Write((ushort)0);
			w.Write(offset);
			w.Write((uint)data.Length);
			w.Write((uint)0);
			w.Write(SHA256.HashData(data));
			offset += (uint)data.Length;
		}
		if (ms.Length != headerSize) throw new InvalidOperationException("Paketkopf hat falsche Groesse.");
		foreach (var (_, data) in sections) w.Write(data);
		w.Flush();
		Directory.CreateDirectory(Path.GetDirectoryName(path)!);
		File.WriteAllBytes(path, ms.ToArray());
	}

	private static byte[] Pad(byte[] b, int len)
	{
		var r = new byte[len];
		Array.Copy(b, r, Math.Min(b.Length, len - 1));
		return r;
	}
}

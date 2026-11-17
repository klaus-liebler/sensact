// Stationspaket (s. docs/concept-modular-firmware.md, "Web-UI als Teil des Stationspakets", und das
// Gegenstueck auf dem Geraet: espidf-component-webmanager/cpp/station_package.hh).
//
// Schritt 1: Das Paket enthaelt die Web-UI der Station. Es passt zu genau den Firmwares, die aus
// demselben generierten Hausmodell und ws-Protokoll gebaut wurden -- das stellt die compatId sicher,
// die der Builder hier berechnet, in die Firmware einbaut (STATION_COMPAT_ID, s.
// GenerateRuntimeConfig) und in jedes Paket schreibt.
using System.Security.Cryptography;
using System.Text;

namespace Builder;

public static class StationPackage
{
	private static readonly byte[] Magic = "SNSCTPKG"u8.ToArray();
	private const ushort FormatVersion = 1;
	private const ushort SectionWebUiHtmlBr = 1;
	private const int HeaderSize = 128;
	private const int SectionSize = 48;

	/// <summary>
	/// SHA-256 ueber alles, was Firmware und Web-UI gemeinsam aus Hausmodell und ws-Protokoll erzeugt
	/// bekommen: generated/sensact_model/common, generated/sensact_model/&lt;NODE_ID&gt; (ohne die Dateien
	/// mit Erzeugungszeitpunkt) und generated/wsprotocol_cpp. Gleiche Eingaben -> gleiche Kennung.
	/// </summary>
	public static byte[] ComputeCompatId(string nodeId)
	{
		var model = SensactBuildContext.SensactModelGeneratedDir;
		var roots = new[]
		{
			Path.Combine(model, "common"),
			Path.Combine(model, nodeId),
			SensactBuildContext.GeneratedWsProtocolCppDir,
		};
		using var sha = IncrementalHash.CreateHash(HashAlgorithmName.SHA256);
		foreach (var root in roots)
		{
			if (!Directory.Exists(root))
			{
				throw new InvalidOperationException($"{root} fehlt -- GenerateModelFiles und GenerateBestBinaryBufferFiles muessen vorher laufen.");
			}
			foreach (var file in Directory.EnumerateFiles(root, "*", SearchOption.AllDirectories)
				.Where(f => Path.GetFileName(f) is not ("nodeDescription.inc" or "node_descriptor.json"))
				.OrderBy(f => Path.GetRelativePath(root, f).Replace('\\', '/'), StringComparer.Ordinal))
			{
				sha.AppendData(Encoding.UTF8.GetBytes(Path.GetFileName(root) + "/" + Path.GetRelativePath(root, file).Replace('\\', '/') + "\0"));
				// Zeilenenden normalisieren: dieselben Quellen sollen unabhaengig von Git-Einstellungen dieselbe Kennung geben
				var text = File.ReadAllBytes(file);
				sha.AppendData(NormalizeNewlines(text));
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

	/// <summary>Schreibt ein Paket mit der Web-UI (index.compressed.br) fuer eine Station.</summary>
	public static void Write(string path, string nodeId, byte[] compatId, byte[] webUiBr, string label)
	{
		var nodeBytes = Encoding.ASCII.GetBytes(nodeId);
		if (nodeBytes.Length > 31) throw new ArgumentException($"NODE_ID {nodeId} laenger als 31 Zeichen.");
		var labelBytes = Encoding.UTF8.GetBytes(label);
		if (labelBytes.Length > 39) labelBytes = labelBytes[..39];

		int headerSize = HeaderSize + SectionSize;
		using var ms = new MemoryStream();
		using var w = new BinaryWriter(ms); // little endian
		w.Write(Magic);
		w.Write(FormatVersion);
		w.Write((ushort)1);                 // sectionCount
		w.Write((uint)headerSize);
		w.Write(Pad(nodeBytes, 32));
		w.Write(compatId);
		w.Write((ulong)DateTimeOffset.UtcNow.ToUnixTimeSeconds());
		w.Write(Pad(labelBytes, 40));
		// Abschnitt 1: Web-UI
		w.Write(SectionWebUiHtmlBr);
		w.Write((ushort)0);
		w.Write((uint)headerSize);
		w.Write((uint)webUiBr.Length);
		w.Write((uint)0);
		w.Write(SHA256.HashData(webUiBr));
		if (ms.Length != headerSize) throw new InvalidOperationException("Paketkopf hat falsche Groesse.");
		w.Write(webUiBr);
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

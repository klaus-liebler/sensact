// Portierung von parsePartitionsCSVFromFile() (espidf.ts) -- liest partitions.csv (ESP-IDF-Format:
// erste zwei Zeilen Kommentare/Header, danach "Name,Type,SubType,Offset,Size,Flags" je Zeile).
// Wird nur fuer die NVS-Partition gebraucht (deren Offset ist NICHT Teil von flasher_args.json,
// s. FlasherArgs.cs).
namespace Builder;

public sealed record PartitionEntry(string Name, string Type, string SubType, long? Offset, long Size, string? Flags);

public static class PartitionsCsv
{
	public static List<PartitionEntry> Parse(string filePath)
	{
		var lines = File.ReadAllLines(filePath);
		var entries = new List<PartitionEntry>();
		// Erste zwei Zeilen sind Kommentare/Header, s. TS-Original.
		for (var i = 2; i < lines.Length; i++)
		{
			var line = lines[i].Trim();
			if (line.Length == 0) continue;
			var values = line.Split(',');
			entries.Add(new PartitionEntry(
				Name: values[0],
				Type: values[1],
				SubType: values[2],
				Offset: values.Length > 3 && values[3].Length > 0 ? Convert.ToInt64(values[3], 16) : null,
				Size: Convert.ToInt64(values[4], 16),
				Flags: values.Length > 5 && values[5].Length > 0 ? values[5] : null));
		}
		return entries;
	}

	public static PartitionEntry Find(string filePath, string name) =>
		Parse(filePath).FirstOrDefault(e => e.Name == name)
			?? throw new InvalidOperationException($"Partition \"{name}\" nicht in {filePath} gefunden.");
}

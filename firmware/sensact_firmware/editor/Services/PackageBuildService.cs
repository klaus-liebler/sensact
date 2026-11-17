using System.Diagnostics;
using System.Text.RegularExpressions;

namespace SensactEditor.Services;

/// <summary>
/// Baut das Stationspaket einer Station aus dem Editor heraus: startet den Builder
/// (BuildPackageFor --nodeId &lt;Station&gt; --model &lt;Haus&gt;) als eigenen Prozess -- derselbe Weg wie auf
/// der Kommandozeile, keine zweite Implementierung. Immer nur ein Lauf gleichzeitig; das Protokoll
/// erscheint live im Editor, das fertige Paket ist unter /download/&lt;Station&gt; abrufbar.
/// </summary>
public sealed partial class PackageBuildService(HouseModelService model)
{
	private readonly object sync = new();
	private readonly List<string> log = [];

	public bool Running { get; private set; }
	public string? NodeId { get; private set; }
	public bool? Success { get; private set; }
	public DateTime? Finished { get; private set; }

	public IReadOnlyList<string> Log
	{
		get { lock (sync) return log.ToList(); }
	}

	public event Action? Changed;

	public string FirmwareRoot => Path.GetDirectoryName(model.ModelDir)!;

	public string PackagePath(string nodeId) => Path.Combine(FirmwareRoot, "deploy", nodeId, nodeId + ".pkg");

	[GeneratedRegex(@"\x1B\[[0-9;]*[A-Za-z]")]
	private static partial Regex AnsiEscape();

	[GeneratedRegex("^[A-Z][A-Za-z0-9_]*$")]
	public static partial Regex NodeIdPattern();

	/// <summary>Startet den Bau; false, wenn schon einer laeuft oder das Modell ungespeicherte Aenderungen/Fehler hat.</summary>
	public bool Start(string nodeId, out string? reason)
	{
		reason = null;
		if (!NodeIdPattern().IsMatch(nodeId)) { reason = "ungueltige Station"; return false; }
		if (model.Dirty) { reason = "Erst speichern -- gebaut wird die gespeicherte Datei."; return false; }
		if (model.Issues.Count > 0) { reason = "Das Modell hat Fehler."; return false; }
		lock (sync)
		{
			if (Running) { reason = "Es laeuft schon ein Bau."; return false; }
			Running = true;
			NodeId = nodeId;
			Success = null;
			Finished = null;
			log.Clear();
		}
		Changed?.Invoke();
		_ = Task.Run(() => Run(nodeId, model.HouseName));
		return true;
	}

	private async Task Run(string nodeId, string house)
	{
		bool ok = false;
		try
		{
			var psi = new ProcessStartInfo("dotnet")
			{
				WorkingDirectory = FirmwareRoot,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
			};
			foreach (var a in new[] { "run", "--project", Path.Combine(FirmwareRoot, "builder"), "--", "BuildPackageFor", "--nodeId", nodeId, "--model", house })
			{
				psi.ArgumentList.Add(a);
			}
			Append($"> dotnet run --project builder -- BuildPackageFor --nodeId {nodeId} --model {house}");
			using var p = Process.Start(psi)!;
			p.OutputDataReceived += (_, e) => { if (e.Data != null) Append(e.Data); };
			p.ErrorDataReceived += (_, e) => { if (e.Data != null) Append(e.Data); };
			p.BeginOutputReadLine();
			p.BeginErrorReadLine();
			await p.WaitForExitAsync();
			ok = p.ExitCode == 0 && File.Exists(PackagePath(nodeId));
			Append(ok ? $"Fertig: {PackagePath(nodeId)}" : $"Fehlgeschlagen (Exit-Code {p.ExitCode}).");
		}
		catch (Exception ex)
		{
			Append("Builder konnte nicht gestartet werden: " + ex.Message);
		}
		lock (sync)
		{
			Running = false;
			Success = ok;
			Finished = DateTime.Now;
		}
		Changed?.Invoke();
	}

	private void Append(string line)
	{
		lock (sync)
		{
			log.Add(AnsiEscape().Replace(line, ""));
			if (log.Count > 2000) log.RemoveRange(0, log.Count - 2000);
		}
		Changed?.Invoke();
	}
}

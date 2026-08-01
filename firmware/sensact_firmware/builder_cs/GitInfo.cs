// Git-Versionsinformationen, portiert (fast 1:1) aus GitInfo.cs im Referenzprojekt -- reiner
// "git"-CLI-Aufruf ohne jede STM32-/Board-Spezifika, deshalb unveraendert wiederverwendbar. Anders
// als in der Vorlage (dort eigene Phase "ReadGitStatusAndGenerateFiles" mit 4 Ausgabedateien inkl.
// board-archiv-spezifischer firmware_constants.hh/device-ids.json) nutzt sensact_firmware Git-Infos
// aktuell nur als EINEN Eintrag (GIT_SHORT_HASH) unter vielen im gemeinsamen "defines"-Dictionary
// des Config-/Runtimeconfig-Writers (gulpfile.ts: createObjectWithDefines() -> cfg.create*) --
// deshalb hier bewusst nur die wiederverwendbare Lese-Logik, keine eigene Dateigenerierung. Wird
// vom spaeteren Config-/Runtimeconfig-Writer-Schritt (s. Migrationsstrategie) konsumiert.
using System.Diagnostics;
using System.Text.Json.Serialization;

namespace Builder;

public sealed class GitInfo
{
	[JsonPropertyName("commitHash")]
	public required string CommitHash { get; init; }

	[JsonPropertyName("branch")]
	public required string Branch { get; init; }

	[JsonPropertyName("tag")]
	public required string Tag { get; init; }

	// Unix-Epoch-Sekunden statt vorformatierter Strings -- Formatierung/Zeitzone passiert
	// ausschliesslich am Anzeigeort (Browser), analog zur Vorlage.
	[JsonPropertyName("commitDateEpoch")]
	public required long CommitDateEpoch { get; init; }

	[JsonPropertyName("commitAuthor")]
	public required string CommitAuthor { get; init; }

	[JsonPropertyName("commitMessage")]
	public required string CommitMessage { get; init; }

	[JsonPropertyName("isDirty")]
	public required bool IsDirty { get; init; }

	[JsonPropertyName("buildTimestampEpoch")]
	public required long BuildTimestampEpoch { get; init; }

	[JsonPropertyName("version")]
	public required string Version { get; init; }
}

public static class GitInfoReader
{
	// WorkingDirectory ist die Repo-Wurzel von sensact_firmware -- git findet das umschliessende
	// sensact-Repo trotzdem automatisch (laeuft die Verzeichnisse hoch, sensact_firmware ist ein
	// Unterordner davon, kein eigenes Repo, s. repo-map.md).
	private static string Git(string arguments, string fallback = "unknown")
	{
		try
		{
			var psi = new ProcessStartInfo("git", arguments)
			{
				WorkingDirectory = Paths.RootDir,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				UseShellExecute = false,
			};
			using var process = Process.Start(psi)!;
			var output = process.StandardOutput.ReadToEnd();
			process.WaitForExit();
			var trimmed = output.Trim();
			return process.ExitCode == 0 && trimmed.Length > 0 ? trimmed : fallback;
		}
		catch
		{
			return fallback;
		}
	}

	public static GitInfo ReadGitInfo()
	{
		var commitHash = Git("rev-parse --short HEAD");
		var branch = Git("rev-parse --abbrev-ref HEAD");
		var tag = Git("describe --tags --always");
		// %at statt %ai: liefert direkt Unix-Epoch-Sekunden, kein Parsen eines formatierten Datums noetig.
		var commitDateEpoch = long.Parse(Git("log -1 --format=%at", "0"));
		var commitAuthor = Git("log -1 --format=%an");
		var commitMessage = Git("log -1 --format=%s");
		var isDirty = Git("status --porcelain", "") != "";
		var buildTimestampEpoch = DateTimeOffset.UtcNow.ToUnixTimeSeconds();

		return new GitInfo
		{
			CommitHash = commitHash,
			Branch = branch,
			Tag = tag,
			CommitDateEpoch = commitDateEpoch,
			CommitAuthor = commitAuthor,
			CommitMessage = commitMessage,
			IsDirty = isDirty,
			BuildTimestampEpoch = buildTimestampEpoch,
			Version = $"{tag}-{commitHash}",
		};
	}
}

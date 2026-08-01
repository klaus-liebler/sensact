// Portierung von npm.ts (CreateAndInstallNpmProjectLazily) -- schreibt ein package.json fuer ein
// generiertes npm-Projekt und ruft "npm install" nur auf, wenn sich der Inhalt tatsaechlich
// geaendert hat oder node_modules/ noch fehlt (kein unnoetiger Netzwerkzugriff bei jedem Build-Lauf).
using System.Diagnostics;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Builder;

// Nur die tatsaechlich von den generierten Projekten genutzten Felder aus IPackageJson
// (package_json.ts) uebernommen -- die vollstaendige npm-package.json-Struktur hat weit mehr
// optionale Felder, die hier nie gebraucht werden.
public sealed class PackageJson
{
	[JsonPropertyName("name")]
	public required string Name { get; init; }

	[JsonPropertyName("version")]
	public string Version { get; init; } = "0.0.1";

	[JsonPropertyName("description")]
	public string? Description { get; init; }

	[JsonPropertyName("author")]
	public string? Author { get; init; }

	[JsonPropertyName("license")]
	public string? License { get; init; }

	[JsonPropertyName("dependencies")]
	public Dictionary<string, string>? Dependencies { get; init; }
}

public static class NpmProject
{
	public static void CreateAndInstallLazily(string projectRoot, PackageJson pj)
	{
		var pjContent = JsonSerializer.Serialize(pj);
		var pjPath = Path.Combine(projectRoot, "package.json");
		var needToCallNpmInstall = false;

		if (!File.Exists(pjPath) || File.ReadAllText(pjPath) != pjContent)
		{
			Directory.CreateDirectory(projectRoot);
			File.WriteAllText(pjPath, pjContent);
			Console.WriteLine($"package.json in {projectRoot} neu geschrieben ({(File.Exists(pjPath) ? "geaendert" : "existierte nicht")}) -> npm install");
			needToCallNpmInstall = true;
		}

		var nodeModulesPath = Path.Combine(projectRoot, "node_modules");
		if (pj.Dependencies is not null && !Directory.Exists(nodeModulesPath))
		{
			Console.WriteLine($"{nodeModulesPath} existiert nicht -> npm install");
			needToCallNpmInstall = true;
		}

		if (!needToCallNpmInstall) return;

		var psi = new ProcessStartInfo("npm.cmd", "install")
		{
			WorkingDirectory = projectRoot,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			UseShellExecute = false,
		};
		Console.WriteLine($"Fuehre \"npm install\" in {projectRoot} aus...");
		using var process = Process.Start(psi)!;
		var stdout = process.StandardOutput.ReadToEnd();
		var stderr = process.StandardError.ReadToEnd();
		process.WaitForExit();
		if (stdout.Length > 0) Console.WriteLine(stdout);
		if (process.ExitCode != 0)
		{
			throw new InvalidOperationException($"npm install in {projectRoot} fehlgeschlagen (Exit {process.ExitCode}):\n{stderr}");
		}
	}
}

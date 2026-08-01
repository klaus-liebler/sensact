// Kleiner Kindprozess-Helfer, von allen Phasen genutzt, die externe Tools aufrufen (npm/node/vite,
// idf.py, esptool.py, git). Fast 1:1 aus dem Referenzprojekt uebernommen (vollstaendig generisch,
// keine STM32-Spezifika) -- faengt ENOENT/nicht-0-Exit in eine lesbare Exception statt .NET's
// Standard-Win32Exception/nicht abgefangenen Exit-Code durchzureichen.
using System.Diagnostics;

namespace Builder;

public sealed class ProcessException(string message, int exitCode, string stdOut, string stdErr) : Exception(message)
{
	public int ExitCode { get; } = exitCode;
	public string StdOut { get; } = stdOut;
	public string StdErr { get; } = stdErr;
}

public static class ProcessRunner
{
	private static ProcessStartInfo BuildStartInfo(string fileName, IEnumerable<string> arguments, string? workingDirectory, bool redirect)
	{
		var psi = new ProcessStartInfo(fileName)
		{
			WorkingDirectory = workingDirectory ?? Paths.RootDir,
			RedirectStandardOutput = redirect,
			RedirectStandardError = redirect,
			UseShellExecute = false,
		};
		foreach (var arg in arguments) psi.ArgumentList.Add(arg);
		return psi;
	}

	private static Process StartOrThrow(ProcessStartInfo psi)
	{
		try
		{
			return Process.Start(psi)!;
		}
		catch (System.ComponentModel.Win32Exception ex) when (ex.NativeErrorCode == 2)
		{
			throw new ProcessException($"Datei nicht gefunden: {psi.FileName}", -1, "", "");
		}
	}

	// Faehrt einen Kindprozess hoch, gibt stdout als Text zurueck. Wirft bei nicht-0-Exit.
	public static string Run(string fileName, IEnumerable<string> arguments, string? workingDirectory = null)
	{
		var psi = BuildStartInfo(fileName, arguments, workingDirectory, redirect: true);
		using var process = StartOrThrow(psi);
		var stdoutTask = process.StandardOutput.ReadToEndAsync();
		var stderrTask = process.StandardError.ReadToEndAsync();
		Task.WaitAll(stdoutTask, stderrTask);
		process.WaitForExit();
		if (process.ExitCode != 0)
		{
			throw new ProcessException(
				$"{fileName} beendet mit Exit-Code {process.ExitCode}.\n{(stderrTask.Result.Length > 0 ? stderrTask.Result : stdoutTask.Result)}",
				process.ExitCode, stdoutTask.Result, stderrTask.Result);
		}
		return stdoutTask.Result;
	}

	// Wie Run(), aber stdout als rohe Bytes (fuer Binaerausgabe von Kindprozessen).
	public static byte[] RunBinary(string fileName, IEnumerable<string> arguments, string? workingDirectory = null)
	{
		var psi = BuildStartInfo(fileName, arguments, workingDirectory, redirect: true);
		using var process = StartOrThrow(psi);
		using var stdout = new MemoryStream();
		var copyTask = process.StandardOutput.BaseStream.CopyToAsync(stdout);
		var stderrTask = process.StandardError.ReadToEndAsync();
		Task.WaitAll(copyTask, stderrTask);
		process.WaitForExit();
		if (process.ExitCode != 0)
		{
			throw new ProcessException($"{fileName} beendet mit Exit-Code {process.ExitCode}.\n{stderrTask.Result}", process.ExitCode, "", stderrTask.Result);
		}
		return stdout.ToArray();
	}

	// Wie Run(), aber stdout/stderr werden direkt an die Konsole durchgereicht (fuer node/vite,
	// idf.py-Aufrufe, deren Live-Ausgabe der Nutzer sehen soll -- Pendant zu execFileSync's
	// stdio:"inherit").
	public static void RunInherit(string fileName, IEnumerable<string> arguments, string? workingDirectory = null)
	{
		var psi = BuildStartInfo(fileName, arguments, workingDirectory, redirect: false);
		using var process = StartOrThrow(psi);
		process.WaitForExit();
		if (process.ExitCode != 0)
		{
			throw new ProcessException($"{fileName} beendet mit Exit-Code {process.ExitCode}.", process.ExitCode, "", "");
		}
	}
}

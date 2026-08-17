// Portierung von gulpfile.ts:buildFirmware() (espidf.ts:buildFirmware()/exec_in_idf_terminal()).
// idf.py braucht eine per export.bat aktivierte Python-Umgebung (setzt PATH/PYTHONPATH etc.) --
// diese Umgebungsvariablen gelten nur innerhalb DESSELBEN Prozesses, deshalb export.bat und
// "idf.py build" ALS EIN EINZIGER, per "&&" verketteter Befehl ueber cmd.exe ausgefuehrt (analog zu
// espidf.ts: `"${exportBat}" && ${command}`, dort per execSync/cmd.exe unter Windows), nicht als
// zwei getrennte ProcessRunner-Aufrufe. Ueber ProcessRunner.RunInheritShellCommand() (nicht
// RunInherit()!), weil "export.bat" selbst gequotet ist -- s. Kommentar dort, warum eine
// ArgumentList das kaputt escaped haette.
namespace Builder.Phases;

public static class BuildFirmware
{
	public static void Run()
	{
		var idfPath = Environment.GetEnvironmentVariable("IDF_PATH");
		if (string.IsNullOrWhiteSpace(idfPath))
		{
			throw new InvalidOperationException(
				"IDF_PATH ist nicht gesetzt -- muss auf die ESP-IDF-Installationswurzel zeigen (enthaelt export.bat).");
		}
		var exportBat = Path.Combine(idfPath, "export.bat");
		if (!File.Exists(exportBat))
		{
			throw new InvalidOperationException(
				$"IDF_PATH ist auf \"{idfPath}\" gesetzt, aber \"{exportBat}\" existiert nicht -- IDF_PATH korrigieren.");
		}

		ProcessRunner.RunInheritShellCommand($"\"{exportBat}\" && idf.py build", Paths.RootDir);
		Console.WriteLine("Firmware gebaut.");
	}
}

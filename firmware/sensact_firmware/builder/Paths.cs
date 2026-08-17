// Zentrale Pfad-Konstanten, analog zu Paths.cs im Referenzprojekt (s. Kommentar dort zur
// AppContext.BaseDirectory-Problematik bei dotnet run/build/publish).
namespace Builder;

public static class Paths
{
	public static readonly string RootDir = FindRootDir();

	// Klettert von AppContext.BaseDirectory (z.B. builder/bin/Debug/net10.0/) aufwaerts, bis ein
	// Verzeichnis namens "builder" gefunden wird -- dieser Teil des Aufstiegs ist noetig, weil die
	// bin/obj-Verschachtelungstiefe je nach Debug/Release/TargetFramework variiert. Das Projekt-
	// Wurzelverzeichnis ist danach ausschliesslich das UNMITTELBAR uebergeordnete Verzeichnis von
	// "builder" -- kein weiteres Aufwaertssuchen darueber hinaus (anders als zuvor, das generisch bis
	// zur Wurzel des Dateisystems haette weitersuchen koennen und dabei z.B. einen entfernten
	// Vorfahren mit zufaellig gleichnamigem "builder"-Ordner faelschlich als Repo-Wurzel haette
	// akzeptieren koennen). Fehlt dort die CMakeLists.txt, wird sofort abgebrochen statt weiterzusuchen
	// -- wichtig, sobald "builder" (s. Zukunftsplan: Extraktion nach C:\repos\dotnet_libs\
	// firmware_builder o.ae.) in mehreren, unterschiedlich verschachtelten ESP32-Projekten eingebunden
	// wird und sich nicht auf eine bestimmte Aufstiegstiefe verlassen darf.
	private static string FindRootDir()
	{
		var dir = new DirectoryInfo(AppContext.BaseDirectory);
		while (dir is not null && !string.Equals(dir.Name, "builder", StringComparison.OrdinalIgnoreCase))
		{
			dir = dir.Parent;
		}
		var root = dir?.Parent;
		if (root is null || !File.Exists(Path.Combine(root.FullName, "CMakeLists.txt")))
		{
			throw new InvalidOperationException(
				"Konnte Repo-Wurzel nicht finden: das unmittelbar uebergeordnete Verzeichnis von 'builder/' " +
				$"(gesucht ab {AppContext.BaseDirectory} aufwaerts) enthaelt keine CMakeLists.txt.");
		}
		return root.FullName;
	}

	// Zwischengespeicherter Stand des zuletzt bekannten Boards (s. context.ts: Context.get() kopiert
	// die board-spezifische board_info.json hierher, sobald einmal ein Board verbunden war) --
	// KEINE Live-Abfrage eines angeschlossenen Boards, s. Phases/Info.cs.
	public static readonly string BoardInfoJsonPath = Path.Combine(RootDir, "board_info.json");

	public static readonly string WsProtocolDir = Path.Combine(RootDir, "ws-protocol");

	// Projektlokales Ausgabeverzeichnis fuer generierte Dateien (wsprotocol_cpp/_ts,
	// runtimeconfig_cpp/_ts, cmake/, web/ -- alles, was builder selbst erzeugt). Lag bis August 2026
	// unter dem repo-fremden "c:\repos\generated" (gemeinsames Scratch-Verzeichnis, unveraendert aus
	// gulpfile.ts uebernommen, GENERATED_ROOT) -- umgestellt auf RootDir/generated, weil das externe
	// Verzeichnis nirgends versioniert war und Konsumenten (npm-packages, CMake) nur ueber einen
	// impliziten, undokumentierten Pfad-Vertrag zusammenfanden. Cross-Repo-Konsumenten
	// (npm-packages/*, s. deren package.json "file:"-Abhaengigkeiten) zeigen seitdem per relativem
	// Sibling-Pfad hierher, statt auf ein drittes, repo-loses Verzeichnis.
	public static readonly string GeneratedRoot = Path.Combine(RootDir, "generated");

	public static readonly string GeneratedWsProtocolCppDir = Path.Combine(GeneratedRoot, "wsprotocol_cpp");

	public static readonly string GeneratedWsProtocolTsDir = Path.Combine(GeneratedRoot, "wsprotocol_ts");

	// Eigenstaendiges Repo (Remote klaus-liebler/npm-packages), von mehreren generierten npm-Projekten
	// unten per relativem "file:"-Pfad referenziert (s. RelativeFileDependency) -- die Pfadtiefe von
	// GeneratedRoot dorthin ist NICHT fest (haengt davon ab, wo GeneratedRoot gerade liegt), deshalb
	// nie als literaler "../../..."-String hartkodieren, sondern immer ueber RelativeFileDependency
	// berechnen. Maschinenabhaengiger absoluter Pfad -- kommt aus appsettings.json (s.
	// BuilderOptions), nicht hartkodiert, da jeder Entwickler seine Repos woanders auschecken kann.
	public static string NpmPackagesDir => BuilderOptions.Current.NpmPackagesDir;

	// "file:"-Abhaengigkeitswert fuer ein generiertes npm-Projekt unter "from" auf ein Zielverzeichnis
	// "to" (z.B. Paths.NpmPackagesDir + "@klaus-liebler/sensact-base") -- npm versteht sowohl "/" als
	// auch "\" in file:-Pfaden unter Windows, hier trotzdem einheitlich "/" fuer bessere Lesbarkeit/
	// Diff-Stabilitaet.
	public static string RelativeFileDependency(string from, string to) =>
		"file:" + Path.GetRelativePath(from, to).Replace('\\', '/');

	// Zweite ws-protocol-Quelle: die anderen 10 (von 12) Namespaces liegen im Nachbar-Repo
	// espidf-component-webmanager (s. Plan-Doc "Betroffene Repos"). Maschinenabhaengiger absoluter
	// Pfad -- kommt aus appsettings.json (s. BuilderOptions), nicht hartkodiert.
	public static string WebmanagerWsProtocolDir => BuilderOptions.Current.WebmanagerWsProtocolDir;

	// Ziele des Config-/Runtimeconfig-Writers (s. RuntimeConfigWriter.cs/Phases/GenerateRuntimeConfig.cs),
	// unveraendert aus paths.ts uebernommene Ordnernamen (GENERATED_RUNTIMECONFIG_CPP/_TS, GENERATED_CMAKE).
	public static readonly string GeneratedRuntimeConfigCppDir = Path.Combine(GeneratedRoot, "runtimeconfig_cpp");

	public static readonly string GeneratedRuntimeConfigTsDir = Path.Combine(GeneratedRoot, "runtimeconfig_ts");

	public static readonly string GeneratedCMakeDir = Path.Combine(GeneratedRoot, "cmake");

	// Sensact-spezifisch: pro-Node generierte Artefakte aus configware (s. Plan-Doc "configware wird
	// voll in den Orchestrator verschmolzen"), unveraendert aus gulpfile.ts uebernommen
	// (SENSACT_COMPONENT_GENERATED_PATH). Solange GenerateModelFiles (Schritt 7) noch nicht portiert
	// ist, ist dies weiterhin der Output-Ordner des bestehenden configware_*-Konsolenprogramms (dessen
	// appsettings.json "SourceCodeGenerator.BasePath" entsprechend hierher zeigen muss). Unter
	// GeneratedRoot, weil sensact-applicationmodel/CMakeLists.txt es ueber
	// "${GENERATED_DIR}/sensact_model" einbindet -- derselbe GENERATED_DIR wie fuer alle anderen
	// generierten Dateien, die Trennung "von builder vs. von configware erzeugt" spielt fuer die
	// Include-Pfad-Aufloesung keine Rolle.
	public static readonly string SensactModelGeneratedDir = Path.Combine(GeneratedRoot, "sensact_model");

	// TS-Template-Dateien fuer die Sensact-Codegenerierung (s. sensact_code_generator.ts:
	// P_WEB/"templates"), Phases/GenerateSensactFiles.cs.
	public static readonly string WebTemplatesDir = Path.Combine(RootDir, "web", "templates");

	// Web-Projekt (Vite) + dessen Ausgabeverzeichnis, s. Phases/BuildWebApp.cs. GENERATED_WEB aus
	// paths.ts uebernommen.
	public static readonly string WebDir = Path.Combine(RootDir, "web");

	public static readonly string GeneratedWebDir = Path.Combine(GeneratedRoot, "web");

	// ESP-IDF-eigenes, repo-lokales Build-Verzeichnis (bootloader.bin/sensact_firmware.bin/
	// flasher_args.json, von "idf.py build" erzeugt) -- bewusst NICHT unter GeneratedRoot, das ist
	// ESP-IDF's eigene, feste Konvention (s. Phases/BuildFirmware.cs/FlashFirmware.cs).
	public static readonly string BuildDir = Path.Combine(RootDir, "build");

	public static readonly string PartitionsCsvPath = Path.Combine(RootDir, "partitions.csv");
}

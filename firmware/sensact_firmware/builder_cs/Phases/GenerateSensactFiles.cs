// Portierung von sensact_code_generator.ts (Sensact.PrepareSensactFiles()) -- NUR der weiterhin
// benoetigte Teil: die zwei generierten npm-Projekte, die aus einem TS-Template
// (web/templates/*.template.ts) plus einem von configware erzeugten ".inc"-Schnipsel
// zusammengesetzt werden (sendCommandImplementation.ts fuer die Web-UI-Kommando-Funktionen,
// sensactapps.ts als "digitaler Zwilling" aller bekannten Apps in der Web-UI).
//
// BEWUSST WEGGELASSEN: die beiden applicationIds.fbs/commandTypes.fbs-Template-Expansionen aus dem
// TS-Original -- die sind Flatbuffers-spezifisch und dadurch obsolet, dass configware (s.
// ModelGeneration/Generator.cs) bereits direkt vollstaendige ws-protocol-"enums"-JSON-Dateien
// erzeugt (s. docs/plan_v2/02-builder-migration-csharp.md, "Flatbuffers wird durch ws-protocol
// ersetzt"), keine Template-Assemblierung aus Flatbuffers-Textschnipseln mehr noetig.
//
// Die beiden *.template.ts-Dateien importieren jetzt aus "@generated/wsprotocol_ts/ws-protocol"
// (Stufe 3, seit ws-protocol tatsaechlich in webmanager.hh/AppController verdrahtet ist) --
// entsprechend haengen die beiden generierten npm-Projekte unten von "@generated/wsprotocol_ts" ab,
// nicht mehr von "@generated/flatbuffers_ts".
using System.Text;

namespace Builder.Phases;

public static class GenerateSensactFiles
{
	private static void TemplateHere(string templatePath, string incPath, string destPath)
	{
		if (!File.Exists(templatePath))
		{
			throw new InvalidOperationException($"Keine Template-Basisdatei gefunden: {templatePath}.");
		}
		if (!File.Exists(incPath))
		{
			throw new InvalidOperationException($"Keine Template-Include-Datei gefunden: {incPath}.");
		}
		var content = File.ReadAllText(templatePath);
		Directory.CreateDirectory(Path.GetDirectoryName(destPath)!);
		File.WriteAllText(destPath, content.Replace("//TEMPLATE_HERE", File.ReadAllText(incPath)));
	}

	public static void Run()
	{
		var ctx = BoardContext.LoadCached();
		var nodeId = ctx.GetNodeId();

		// Sende-Funktionen, damit Applikationen ihre Commands verschicken koennen.
		var sendCommandProject = Path.Combine(Paths.GeneratedRoot, "sensact_sendCommandImplementation");
		TemplateHere(
			Path.Combine(Paths.WebTemplatesDir, "sendCommandImplementation.template.ts"),
			Path.Combine(Paths.SensactModelGeneratedDir, "common", "sendCommandImplementation.ts.inc"),
			Path.Combine(sendCommandProject, "sendCommandImplementation.ts"));
		NpmProject.CreateAndInstallLazily(sendCommandProject, new PackageJson
		{
			Name = "@generated/sensact_sendCommandImplementation",
			Description = "Generated during Build",
			Author = "Generated",
			License = "No License",
			Dependencies = new Dictionary<string, string>
			{
				["@generated/wsprotocol_ts"] = "file:../wsprotocol_ts",
				["@klaus-liebler/sensact-base"] = @"file:../../npm-packages/@klaus-liebler/sensact-base",
			},
		});

		// Digitaler Zwilling aller im Sensact-System bekannten Apps fuer die Web-UI.
		var appsBuilderProject = Path.Combine(Paths.GeneratedRoot, "sensact_appsbuilder");
		TemplateHere(
			Path.Combine(Paths.WebTemplatesDir, "sensactapps.template.ts"),
			Path.Combine(Paths.SensactModelGeneratedDir, nodeId, "sensactapps_local.ts.inc"),
			Path.Combine(appsBuilderProject, "sensactapps.ts"));
		NpmProject.CreateAndInstallLazily(appsBuilderProject, new PackageJson
		{
			Name = "@generated/sensact_appsbuilder",
			Description = "Generated during Build",
			Author = "Generated",
			License = "No License",
			Dependencies = new Dictionary<string, string>
			{
				["@generated/wsprotocol_ts"] = "file:../wsprotocol_ts",
				["@klaus-liebler/sensact-base"] = @"file:../../npm-packages/@klaus-liebler/sensact-base",
				["@klaus-liebler/web-components-sensact"] = @"file:../../npm-packages/@klaus-liebler/web-components-sensact",
			},
		});

		Console.WriteLine($"Sensact-Dateien -> {sendCommandProject}, {appsBuilderProject}");
	}
}

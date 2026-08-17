// Phase "GenerateModelFiles" -- Portierung von configware (ehemals zwei eigenstaendige
// Konsolenprogramme, configware_sattlerstrasse/configware_testmodel, s.
// docs/plan_v2/02-builder-migration-csharp.md, Abschnitt "configware wird voll in den
// Orchestrator verschmolzen"). Die eigentliche Generierungs-Engine (SourceCodeGenerator<T>,
// ModelBuilder<T>, Node, SensactApplication, ...) liegt unveraendert unter ModelGeneration/ (aus
// configware/common verschoben, KEINE Namensraum-Aenderungen noetig -- dort gab es nie eine
// direkte Abhaengigkeit von der modellspezifischen ApplicationId, alles laeuft ueber den
// generischen Typparameter). Die beiden Haus-Modelle liegen unter Models/<Name>/, jeweils mit
// eigenem, disambiguiertem Namespace fuer ApplicationId (Klli.Sensact.Model.Sattlerstrasse16 bzw.
// .Testmodel) -- der einzige Grund fuer die urspruengliche Aufteilung in zwei separate Projekte war
// eine Namensraum-Kollision (beide definierten Klli.Sensact.Model.ApplicationId), kein
// generics-/Typsystem-Problem (s. "Geklaerte Generics-Frage" im Plan-Doc).
//
// Modell-Auswahl per CLI-Flag (--model Sattlerstrasse16|Testmodel, s. "Entschieden" im Plan-Doc) --
// da C#-Generics zur Kompilierzeit aufgeloest werden muessen, dispatcht Run() manuell auf die
// passende geschlossene generische Instanziierung von SourceCodeGenerator<T>, statt Reflection zu
// bemuehen (nur 2 Modelle, ein switch ist hier klarer als generischer Overhead).
//
// Ersetzt configware_*'s Program.cs (DI-Container, NLog, blockierendes Console.ReadKey() am Ende)
// durch eine einfache, aufrufbare Methode mit Standard-Konsolen-Logging -- kein Subprozess, keine
// zweite .exe, wie im Plan gefordert.
using Klli.Sensact.Config;
using Klli.Sensact.Model.Common;
using Microsoft.Extensions.Logging;

namespace Builder.Phases;

public static class GenerateModelFiles
{
	public static void Run(string modelName)
	{
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Information));

		switch (modelName)
		{
			case "Sattlerstrasse16":
				RunFor<Klli.Sensact.Model.Sattlerstrasse16.ApplicationId>(
					new Sattlerstrasse16Builder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			case "Testmodel":
				RunFor<Klli.Sensact.Model.Testmodel.ApplicationId>(
					new TestModelBuilder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			default:
				throw new ArgumentException($"Unbekanntes Modell \"{modelName}\" (gueltig: Sattlerstrasse16, Testmodel).");
		}
	}

	private static void RunFor<ApplicationIdType>(ModelContainer mc, ILoggerFactory loggerFactory) where ApplicationIdType : struct, Enum
	{
		var appSettings = new AppSettings
		{
			SourceCodeGenerator = new SourceCodeGeneratorOptions { BasePath = Paths.SensactModelGeneratedDir },
		};
		var logger = loggerFactory.CreateLogger<SourceCodeGenerator<ApplicationIdType>>();
		var generator = new SourceCodeGenerator<ApplicationIdType>(appSettings, logger);

		if (!generator.CheckAndPrepare(mc))
		{
			throw new InvalidOperationException("CheckAndPrepare() fehlgeschlagen -- s. Log-Ausgabe oben fuer Details.");
		}
		generator.DeleteBaseDirectory();
		generator.generateAll(mc);
		Console.WriteLine($"Modell-Dateien -> {Paths.SensactModelGeneratedDir}");

		// Die beiden ws-protocol-Enum-Quellen (ApplicationId/Command, generiert nach
		// SensactModelGeneratedDir/common/, s. Generator.GenerateApplicationIds/GenerateCommandTypes)
		// muessen zusaetzlich im git-getrackten BestBinaryBuffersSchemaDir liegen, damit
		// GenerateWsProtocolFiles sie als Schema-Quelle findet -- das ist bewusst kein Kopieren "ins
		// Leere": beide Dateien bleiben git-getrackte Dateien, eine echte Modelaenderung zeigt sich
		// also weiterhin als normaler, review-barer "git diff" hier, nur ohne den manuellen
		// Kopier-Schritt davor.
		foreach (var enumFile in new[] { "applicationIds.enums.cs", "commandTypes.enums.cs" })
		{
			var source = Path.Combine(Paths.SensactModelGeneratedDir, "common", enumFile);
			var dest = Path.Combine(Paths.BestBinaryBuffersSchemaDir, $"sensact_{enumFile}");
			File.Copy(source, dest, overwrite: true);
		}
		Console.WriteLine($"ws-protocol-Enum-Quellen aktualisiert -> {Paths.BestBinaryBuffersSchemaDir}");
	}
}

// Konfiguration ueber appsettings.json -> typsichere POCO-Klasse, analog zu BuilderOptions.cs im
// Referenzprojekt. appsettings.json selbst ist gitignored (persoenliche Maschinenpfade), getrackt
// ist nur appsettings.json.template (s. builder.csproj, EnsureAppSettings-Target).
using Microsoft.Extensions.Configuration;

namespace Builder;

public sealed class BuilderOptions
{
	// Board-Archiv (s. paths.ts: BOARDS_BASE_DIR = OneDrive - HSOS\esp32_boards) -- pro Board ein
	// Unterverzeichnis "<mac6hex>_<macDecimal>_<mac12hex>" (s. IBoardInfo.BoardDirectoryName).
	public string BoardsDir { get; set; } = "";

	// Private CA + Board-/Server-Zertifikate (s. gulpfile.ts: CERTIFICATES = OneDrive - HSOS\certificates).
	public string CertsDir { get; set; } = "";

	// Zertifikats-Subject-Praefix (Country/State/Locality/Organization), OpenSSL-"-subj"-Notation
	// ("/C=.../ST=.../L=.../O=..."), 1:1 aus dem Referenzprojekt uebernommenes Feld/Format
	// (BuilderOptions.cs dort: SubjectPrefix, genutzt fuer "openssl req -subj") -- hier NICHT an
	// openssl uebergeben (Certificates.cs baut nativ ein X500DistinguishedName), aber bewusst
	// dasselbe appsettings.json-Feld/Format wie in der Vorlage, statt personenbezogene Werte im
	// Code zu hinterlegen (s. Certificates.cs: BuildSubject() parst diesen String).
	public string SubjectPrefix { get; set; } = "";

	// Eigenstaendiges Repo (Remote klaus-liebler/npm-packages), von mehreren generierten npm-Projekten
	// per relativem "file:"-Pfad referenziert (s. Paths.RelativeFileDependency). Maschinenabhaengig
	// (jeder Entwickler kann seine Repos woanders auschecken) -- deshalb appsettings.json statt
	// hartkodiert in Paths.cs.
	public string NpmPackagesDir { get; set; } = "";

	// Zweite BestBinaryBuffers-Schema-Quelle: die anderen 10 (von 12) Namespaces liegen im
	// Nachbar-Repo espidf-component-webmanager. Ebenfalls maschinenabhaengig.
	public string WebmanagerBestBinaryBuffersSchemaDir { get; set; } = "";

	private static readonly Lazy<BuilderOptions> LazyCurrent = new(Load);

	public static BuilderOptions Current => LazyCurrent.Value;

	private static BuilderOptions Load()
	{
		var config = new ConfigurationBuilder()
			.SetBasePath(AppContext.BaseDirectory)
			.AddJsonFile("appsettings.json", optional: false)
			.Build();
		return config.Get<BuilderOptions>()
			?? throw new InvalidOperationException("appsettings.json konnte nicht gebunden werden.");
	}
}

// Portierung von gulpfile.ts:buildAndCompressWebProject() (vite_helper.ts:buildAndCompressWebProject).
// Vite hat keine C#-API, deshalb Aufruf ueber die Vite-CLI (node web/node_modules/vite/bin/vite.js
// build ...) statt der JS-API, die das TS-Original nutzt -- analog zum Referenzprojekt
// (BuildWebApp.cs dort), das aus demselben Grund denselben Weg geht.
//
// cssCodeSplit:false/esbuild.legalComments:'none' aus vite_helper.ts's Inline-Overrides sind NICHT
// als CLI-Flags verfuegbar, muessen hier deshalb nicht separat gesetzt werden: web/vite.config.mts
// setzt beide bereits selbst identisch (redundante Doppelsetzung im TS-Original) -- das Ergebnis
// der Konfigurations-Zusammenfuehrung ist unabhaengig vom CLI- vs. JS-API-Weg dasselbe.
//
// Inlining, Lit-Template-bewusste Minifizierung UND Brotli-Kompression (-> index.compressed.br)
// laufen jetzt VOLLSTAENDIG innerhalb des Vite-Builds selbst, s.
// web/vite-plugin-single-file-firmware-asset.ts (aus dem Referenzprojekt uebernommen, s.
// docs/plan_v2/02-builder-migration-csharp.md, Abschnitt "BuildWebApp") -- kein separater
// C#-seitiger Brotli-Nachbearbeitungsschritt mehr noetig (anders als in einer frueheren Version
// dieser Phase, die die Kompression noch selbst per BrotliStream nachbildete).
namespace Builder.Phases;

public static class BuildWebApp
{
	public static void Run()
	{
		var viteEntry = Path.Combine(Paths.WebDir, "node_modules", "vite", "bin", "vite.js");
		if (!File.Exists(viteEntry))
		{
			throw new InvalidOperationException($"Vite nicht gefunden unter {viteEntry} -- zuerst \"npm install\" in {Paths.WebDir} ausfuehren.");
		}

		ProcessRunner.RunInherit("node", [
			viteEntry, "build", Paths.WebDir,
			"--outDir", Paths.GeneratedWebDir,
			"--sourcemap", "true",
			"--emptyOutDir",
		], Paths.WebDir);

		var compressedPath = Path.Combine(Paths.GeneratedWebDir, "index.compressed.br");
		if (!File.Exists(compressedPath))
		{
			throw new InvalidOperationException($"Vite-Build fertig, aber {compressedPath} wurde nicht erzeugt -- singleFileFirmwareAssetPlugin nicht aktiv?");
		}
		var compressedSize = new FileInfo(compressedPath).Length;
		Console.WriteLine($"Web-App gebaut, {compressedPath} geschrieben. Brotli-komprimierte Groesse = {compressedSize} Byte = {compressedSize / 1024.0:F2} kiB");
	}
}

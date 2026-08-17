// Portierung des "Certificates (lazy)"-Abschnitts von gulpfile.ts:createFiles(). "Lazy": erzeugt
// nur ein neues Board-Zertifikat, wenn noch keins im Board-Archiv liegt -- ein bereits vorhandenes
// wird NIE überschrieben (ein Board behaelt sein Zertifikat ueber beliebig viele weitere Builds
// hinweg, s. docs/build-process.md-Aequivalent im Referenzprojekt zum selben Muster bei
// ReadHardwareIds/CertsLazy). Nutzt die gemeinsame, bereits produktiv im Einsatz befindliche
// Root-CA unter CertsDir (OneDrive - HSOS\certificates\rootCA.pem.{crt,key}) -- wird nur GELESEN,
// nie neu erzeugt (CreateRootCA() in Certificates.cs ist fuer den einmaligen, manuellen CA-Setup
// gedacht, nicht Teil dieser Phase).
namespace Builder.Phases;

public static class GenerateCertificates
{
	public static void Run()
	{
		var ctx = BoardContext.LoadCached();
		var boardDir = ctx.BoardDirectory;
		var nodeId = ctx.GetNodeId();

		var keyPath = Path.Combine(boardDir, "certificates", "esp32.pem.key");
		var crtPath = Path.Combine(boardDir, "certificates", "esp32.pem.crt");
		if (File.Exists(keyPath) && File.Exists(crtPath))
		{
			Console.WriteLine($"Board-Zertifikat existiert bereits ({crtPath}) -- nichts zu tun (lazy).");
			return;
		}

		var caCertPath = Path.Combine(BuilderOptions.Current.CertsDir, "rootCA.pem.crt");
		var caKeyPath = Path.Combine(BuilderOptions.Current.CertsDir, "rootCA.pem.key");
		var cert = Certificates.CreateAndSignCert(nodeId, "192.168.4.1", [nodeId, $"{nodeId}.local", $"{nodeId}.fritz.box"], caCertPath, caKeyPath);

		Directory.CreateDirectory(Path.GetDirectoryName(keyPath)!);
		File.WriteAllText(keyPath, cert.PrivateKey);
		File.WriteAllText(crtPath, cert.Certificate);
		Console.WriteLine($"Neues Board-Zertifikat erzeugt -> {crtPath}");
	}
}

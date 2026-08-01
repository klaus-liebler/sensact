// Einstiegspunkt fuer alle Build-Phasen, analog zu Program.cs im Referenzprojekt.
//
// Aufruf:
//   dotnet run --project builder_cs -- <Phase> [--model Sattlerstrasse16|Testmodel]
//
// Einzelne Phasen (s. docs/plan_v2/02-builder-migration-csharp.md, "Migrationsstrategie" fuer die
// geplante Reihenfolge):
//   Info
//   GitStatus
//   GenerateWsProtocolFiles
//   GenerateRuntimeConfig
//   GenerateCertificates
//   GenerateModelFiles --model <Name>
//   GenerateSensactFiles
//   BuildWebApp
//   BuildFirmware
//   ReadHardwareIds
//   FlashFirmware
using Builder;
using Builder.Phases;

if (args.Length == 0)
{
	throw new ArgumentException("Kein Phasenname angegeben. Beispiel: dotnet run --project builder_cs -- Info");
}

static string GetRequiredArgValue(string[] args, string flag)
{
	var i = Array.IndexOf(args, flag);
	if (i < 0 || i + 1 >= args.Length)
	{
		throw new ArgumentException($"Fehlendes Argument \"{flag} <Wert>\".");
	}
	return args[i + 1];
}

switch (args[0])
{
	case "Info":
		Info.Run();
		break;
	case "GitStatus":
		GitStatus.Run();
		break;
	case "GenerateWsProtocolFiles":
		// Beide Quellen explizit (s. Kommentar bei GenerateWsProtocolFiles.Run): sensacts eigene
		// ws-protocol/-Dateien (sensact.json/nodemaster.json + configware-Enums) UND die 10
		// Namespaces aus dem Nachbar-Repo espidf-component-webmanager.
		GenerateWsProtocolFiles.Run([Paths.WsProtocolDir, Paths.WebmanagerWsProtocolDir]);
		break;
	case "GenerateRuntimeConfig":
		GenerateRuntimeConfig.Run();
		break;
	case "GenerateCertificates":
		GenerateCertificates.Run();
		break;
	case "GenerateModelFiles":
		GenerateModelFiles.Run(GetRequiredArgValue(args, "--model"));
		break;
	case "GenerateSensactFiles":
		GenerateSensactFiles.Run();
		break;
	case "BuildWebApp":
		BuildWebApp.Run();
		break;
	case "BuildFirmware":
		BuildFirmware.Run();
		break;
	case "ReadHardwareIds":
		ReadHardwareIds.Run();
		break;
	case "FlashFirmware":
		FlashFirmware.Run();
		break;
	default:
		throw new ArgumentException($"Unbekannte Phase \"{args[0]}\".");
}

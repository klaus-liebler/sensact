using System.Text.Json;
using Klli.Sensact.Config;
using Klli.Sensact.Model.Common;
using Microsoft.Extensions.Logging;

namespace Builder;

internal static class Program
{
	public static void Main(string[] args)
	{
		if (args.Length == 0)
		{
			throw new ArgumentException("Kein Phasenname angegeben. Beispiel: dotnet run --project builder -- Info");
		}

		var model = GetOptionalArgValue(args, "--model");
		var resetNvsPartition = HasFlag(args, "--resetNVSPartition");
		var registry = new CommandPipelineRegistry(StringComparer.Ordinal);
		registry.AddCommand("Info", RunInfo);
		registry.AddCommand("GitStatus", RunGitStatus);
		registry.AddCommand("GenerateBestBinaryBufferFiles", () => RunGenerateBestBinaryBufferFiles([Paths.BestBinaryBuffersSchemaDir, Paths.WebmanagerBestBinaryBuffersSchemaDir]));
		registry.AddCommand("GenerateRuntimeConfig", RunGenerateRuntimeConfig);
		registry.AddCommand("GenerateCertificates", RunGenerateCertificates);
		registry.AddCommand("GenerateModelFiles", () => RunGenerateModelFiles(model ?? throw new ArgumentException("Fehlendes Argument \"--model <Wert>\".")));
		registry.AddCommand("GenerateSensactFiles", RunGenerateSensactFiles);
		registry.AddCommand("BuildWebApp", RunBuildWebApp);
		registry.AddCommand("BuildFirmware", RunBuildFirmware);
		registry.AddCommand("ReadHardwareIds", RunReadHardwareIds);
		registry.AddCommand("FlashFirmware", () => RunFlashFirmware(resetNvsPartition));
		registry.AddPipeline("Pipeline",
		[
			"GenerateModelFiles",
			"GenerateBestBinaryBufferFiles",
			"GenerateRuntimeConfig",
			"GenerateCertificates",
			"GenerateSensactFiles",
			"BuildWebApp",
			"BuildFirmware",
			"ReadHardwareIds",
			"FlashFirmware",
		]);

		registry.Run(args[0]);
	}

	private static string GetRequiredArgValue(string[] args, string flag)
	{
		var i = Array.IndexOf(args, flag);
		if (i < 0 || i + 1 >= args.Length)
		{
			throw new ArgumentException($"Fehlendes Argument \"{flag} <Wert>\".");
		}
		return args[i + 1];
	}

	private static string? GetOptionalArgValue(string[] args, string flag)
	{
		var i = Array.IndexOf(args, flag);
		if (i < 0 || i + 1 >= args.Length)
		{
			return null;
		}
		return args[i + 1];
	}

	private static bool HasFlag(string[] args, string flag) => Array.IndexOf(args, flag) >= 0;

	private static void RunInfo()
	{
		if (!File.Exists(Paths.BoardInfoJsonPath))
		{
			Console.WriteLine($"Keine board_info.json unter {Paths.BoardInfoJsonPath} gefunden -- noch kein Board jemals verbunden gewesen (oder Cache geloescht).");
			return;
		}

		var cached = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(Paths.BoardInfoJsonPath))
			?? throw new InvalidOperationException($"{Paths.BoardInfoJsonPath} konnte nicht gelesen werden.");

		Console.WriteLine("              MAC: " + BoardPaths.Mac6Char(cached.Mac) + " (decimal: " + cached.Mac + ")");
		try
		{
			var hw = EspToolService.ReadHardwareIds(Paths.RootDir);
			Console.WriteLine("       Chip type: " + hw.ChipType + " (live)");
			Console.WriteLine(" Is current board: " + (hw.Mac == cached.Mac ? "yes" : "no"));
		}
		catch (Exception ex)
		{
			Console.WriteLine($" Is current board: (unbekannt -- Live-Board-Abfrage nicht moeglich: {ex.Message})");
		}

		var boardsDir = BuilderOptions.Current.BoardsDir;
		var boardDir = BoardPaths.BoardSpecificPath(boardsDir, cached.Mac);
		if (!Directory.Exists(boardDir))
		{
			Console.WriteLine($"Kein Board-Archiv unter {boardDir} gefunden -- dieses Board ist noch nicht bekannt.");
			return;
		}

		var boardInfoJsonPath = BoardPaths.BoardSpecificPath(boardsDir, cached.Mac, "board_info.json");
		if (!File.Exists(boardInfoJsonPath))
		{
			Console.WriteLine($"Board-Archiv {boardDir} existiert, enthaelt aber keine board_info.json.");
			return;
		}

		var b = JsonSerializer.Deserialize<IBoardInfo>(File.ReadAllText(boardInfoJsonPath))
			?? throw new InvalidOperationException($"{boardInfoJsonPath} konnte nicht gelesen werden.");

		Console.WriteLine("       Board Name: " + b.BoardName);
		Console.WriteLine("    Board Version: " + b.BoardVersion);
		Console.WriteLine("      Board Roles: " + b.BoardRolesDisplay);
		Console.WriteLine("   Board Settings: " + b.BoardSettingsDisplay);
		Console.WriteLine("  First connected: " + IBoardInfo.FromUnixMillis(b.FirstConnectedDt));
		Console.WriteLine("   Last connected: " + IBoardInfo.FromUnixMillis(b.LastConnectedDt));
		Console.WriteLine("Encryption active: " + (b.FlashEncryptionKeyBurnedAndActivated ? "yes" : "no"));
	}

	private static void RunGitStatus()
	{
		var info = GitInfoReader.ReadGitInfo(Paths.RootDir);
		Console.WriteLine("      Commit Hash: " + info.CommitHash);
		Console.WriteLine("            Branch: " + info.Branch);
		Console.WriteLine("               Tag: " + info.Tag);
		Console.WriteLine("       Commit Date: " + DateTimeOffset.FromUnixTimeSeconds(info.CommitDateEpoch).ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss"));
		Console.WriteLine("     Commit Author: " + info.CommitAuthor);
		Console.WriteLine("    Commit Message: " + info.CommitMessage);
		Console.WriteLine("         Is Dirty?: " + (info.IsDirty ? "yes" : "no"));
		Console.WriteLine("           Version: " + info.Version);
	}

	private static void RunGenerateBestBinaryBufferFiles(IReadOnlyList<string>? sources = null)
	{
		var files = BestBinaryBuffersProtocolGenerator.Generate(new BestBinaryBuffersGenerateParams(
			RootDir: Paths.RootDir,
			Sources: sources ?? [],
			DefaultSource: Paths.BestBinaryBuffersSchemaDir,
			IdMapPath: Path.Combine(Paths.BestBinaryBuffersSchemaDir, "ids.txt"),
			CppOutputFilePath: Path.Combine(Paths.GeneratedWsProtocolCppDir, "ws_protocol.hh"),
			TsOutputFilePath: Path.Combine(Paths.GeneratedWsProtocolTsDir, "ws-protocol.ts"),
			TsPackageJsonOutputPath: Path.Combine(Paths.GeneratedWsProtocolTsDir, "package.json"),
			TsPackageJsonContent: """{"name":"@generated/wsprotocol_ts","version":"1.0.0","main":"ws-protocol.ts","author":"BestBinaryBuffers","license":"No License","description":"Generated during build from BestBinaryBuffers (s. C:\\repos\\dotnet_libs\\best_binary_buffers)."}"""));

		var effectiveSources = (sources is { Count: > 0 }) ? sources : [Paths.BestBinaryBuffersSchemaDir];
		var sourcesLabel = string.Join(", ", effectiveSources);
		Console.WriteLine($"{files.Count} Datei(en) aus {effectiveSources.Count} Quelle(n) ({sourcesLabel}) -> {Paths.GeneratedWsProtocolCppDir} / {Paths.GeneratedWsProtocolTsDir}");
	}

	private static object JsonElementToObject(JsonElement el) => el.ValueKind switch
	{
		JsonValueKind.String => el.GetString()!,
		JsonValueKind.Number => el.TryGetInt64(out var l) ? l : el.GetDouble(),
		JsonValueKind.True => true,
		JsonValueKind.False => false,
		JsonValueKind.Array => el.EnumerateArray().Select(e => e.GetString() ?? "").ToList(),
		_ => el.GetRawText(),
	};

	private static void RunGenerateRuntimeConfig()
	{
		var appName = "sensact";
		var appVersion = "1.0";
		var defines = new Dictionary<string, object>();
		var clientDefines = new Dictionary<string, object>();

		var ctx = BoardContext.LoadCached();
		var b = ctx.Board;
		var boardDir = ctx.BoardDirectory;
		var nodeId = ctx.GetNodeId();

		var nodeDescriptorPath = Path.Combine(Paths.SensactModelGeneratedDir, nodeId, "node_descriptor.json");
		if (!File.Exists(nodeDescriptorPath))
		{
			throw new InvalidOperationException(
				$"Keine node_descriptor.json unter {nodeDescriptorPath} -- configware_sattlerstrasse/_testmodel muss vor dieser Phase einmal gelaufen sein.");
		}

		foreach (var prop in JsonDocument.Parse(File.ReadAllText(nodeDescriptorPath)).RootElement.EnumerateObject())
		{
			var value = JsonElementToObject(prop.Value);
			defines[prop.Name] = value;
			clientDefines[prop.Name] = value;
		}

		if (b.BoardSettings.ValueKind == JsonValueKind.Object)
		{
			if (b.BoardSettings.TryGetProperty("web", out var webEl) && webEl.ValueKind == JsonValueKind.Object)
			{
				foreach (var prop in webEl.EnumerateObject())
				{
					var value = JsonElementToObject(prop.Value);
					defines[prop.Name] = value;
					clientDefines[prop.Name] = value;
				}
			}
			if (b.BoardSettings.TryGetProperty("firmware", out var firmwareEl) && firmwareEl.ValueKind == JsonValueKind.Object)
			{
				foreach (var prop in firmwareEl.EnumerateObject())
				{
					defines[prop.Name] = JsonElementToObject(prop.Value);
				}
			}
		}

		var now = DateTimeOffset.UtcNow;
		var git = GitInfoReader.ReadGitInfo(Paths.RootDir);

		void SetShared(string key, object value)
		{
			defines[key] = value;
			clientDefines[key] = value;
		}

		defines["WEBMANAGER_AUTH_USERNAME"] = "admin";
		defines["WEBMANAGER_AUTH_PASSWORD"] = string.IsNullOrEmpty(b.WebAdminPassword)
			? $"{b.BoardName}_admin".ToLowerInvariant()
			: b.WebAdminPassword;

		SetShared("NODE_ID", nodeId);
		SetShared("HOSTNAME", nodeId);
		SetShared("BOARD_NAME", b.BoardName);
		SetShared("BOARD_VERSION", b.BoardVersion);
		SetShared("BOARD_ROLES", JsonElementToObject(b.BoardRoles));
		SetShared("BOARD_MAC", b.Mac);
		defines["BOARD_DIRECTORY"] = boardDir;
		SetShared("APP_NAME", appName);
		SetShared("APP_VERSION", appVersion);
		SetShared("CREATION_DT", now.ToUnixTimeSeconds());
		SetShared("CREATION_DT_STR", now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss"));
		SetShared("GIT_SHORT_HASH", git.CommitHash);
		SetShared("BANNER", $"{appName} {nodeId}");

		RuntimeConfigWriter.CreateCppConfigurationHeader(Paths.GeneratedRuntimeConfigCppDir, defines);
		RuntimeConfigWriter.CreateCMakeJsonConfigFile(Paths.GeneratedCMakeDir, defines);
		RuntimeConfigWriter.CreateTypeScriptRuntimeConfig(Paths.GeneratedRuntimeConfigTsDir, clientDefines);

		Console.WriteLine($"{defines.Count} Defines -> {Paths.GeneratedRuntimeConfigCppDir}, {Paths.GeneratedCMakeDir}; {clientDefines.Count} Defines -> {Paths.GeneratedRuntimeConfigTsDir}");
	}

	private static void RunGenerateCertificates()
	{
		var ctx = BoardContext.LoadCached();
		var boardDir = ctx.BoardDirectory;
		var nodeId = ctx.GetNodeId();
		var options = BuilderOptions.Current;
		var certOptions = options.ToCertificateAuthorityOptions();

		var keyPath = Path.Combine(boardDir, "certificates", "esp32.pem.key");
		var crtPath = Path.Combine(boardDir, "certificates", "esp32.pem.crt");
		if (File.Exists(keyPath) && File.Exists(crtPath))
		{
			Console.WriteLine($"Board-Zertifikat existiert bereits ({crtPath}) -- nichts zu tun (lazy).");
			return;
		}

		DotNetCertificateService.EnsureRootCertificateAuthority(certOptions, options.RootCaCommonName, force: false);

		var dnsEntries = certOptions.SanDnsEntries
			.Select(e => (e ?? string.Empty).Trim())
			.Where(e => e.Length > 0)
			.Select(e => e.Replace("{hostname}", nodeId, StringComparison.OrdinalIgnoreCase))
			.Distinct(StringComparer.OrdinalIgnoreCase)
			.ToArray();

		Directory.CreateDirectory(Path.GetDirectoryName(keyPath)!);
		DotNetCertificateService.EnsureBoardCertificate(
			certOptions,
			commonName: nodeId,
			boardDir: Path.GetDirectoryName(keyPath)!,
			force: false,
			ipAddress: certOptions.SanIpAddress,
			dnsHostnames: dnsEntries,
			outputFileBaseName: "esp32");
		Console.WriteLine($"Neues Board-Zertifikat erzeugt -> {crtPath}");
	}

	private static void RunGenerateModelFiles(string modelName)
	{
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Information));

		switch (modelName)
		{
			case "Sattlerstrasse16":
				RunModelFor<Klli.Sensact.Model.Sattlerstrasse16.ApplicationId>(
					new Sattlerstrasse16Builder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			case "Testmodel":
				RunModelFor<Klli.Sensact.Model.Testmodel.ApplicationId>(
					new TestModelBuilder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			default:
				throw new ArgumentException($"Unbekanntes Modell \"{modelName}\" (gueltig: Sattlerstrasse16, Testmodel).");
		}
	}

	private static void RunModelFor<ApplicationIdType>(ModelContainer mc, ILoggerFactory loggerFactory) where ApplicationIdType : struct, Enum
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

		foreach (var enumFile in new[] { "applicationIds.enums.cs", "commandTypes.enums.cs" })
		{
			var source = Path.Combine(Paths.SensactModelGeneratedDir, "common", enumFile);
			var dest = Path.Combine(Paths.BestBinaryBuffersSchemaDir, $"sensact_{enumFile}");
			File.Copy(source, dest, overwrite: true);
		}
		Console.WriteLine($"ws-protocol-Enum-Quellen aktualisiert -> {Paths.BestBinaryBuffersSchemaDir}");
	}

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

	private static void RunGenerateSensactFiles()
	{
		var ctx = BoardContext.LoadCached();
		var nodeId = ctx.GetNodeId();

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
				["@klaus-liebler/sensact-base"] = Paths.RelativeFileDependency(sendCommandProject, Path.Combine(Paths.NpmPackagesDir, "@klaus-liebler", "sensact-base")),
			},
		});

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
				["@klaus-liebler/sensact-base"] = Paths.RelativeFileDependency(appsBuilderProject, Path.Combine(Paths.NpmPackagesDir, "@klaus-liebler", "sensact-base")),
				["@klaus-liebler/web-components-sensact"] = Paths.RelativeFileDependency(appsBuilderProject, Path.Combine(Paths.NpmPackagesDir, "@klaus-liebler", "web-components-sensact")),
			},
		});

		Console.WriteLine($"Sensact-Dateien -> {sendCommandProject}, {appsBuilderProject}");
	}

	private static void RunBuildWebApp()
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

	private static void RunBuildFirmware()
	{
		var idfPath = Environment.GetEnvironmentVariable("IDF_PATH");
		if (string.IsNullOrWhiteSpace(idfPath))
		{
			throw new InvalidOperationException("IDF_PATH ist nicht gesetzt -- muss auf die ESP-IDF-Installationswurzel zeigen (enthaelt export.bat).");
		}
		var exportBat = Path.Combine(idfPath, "export.bat");
		if (!File.Exists(exportBat))
		{
			throw new InvalidOperationException($"IDF_PATH ist auf \"{idfPath}\" gesetzt, aber \"{exportBat}\" existiert nicht -- IDF_PATH korrigieren.");
		}

		ProcessRunner.RunInheritShellCommand($"\"{exportBat}\" && idf.py build", Paths.RootDir);
		Console.WriteLine("Firmware gebaut.");
	}

	private static void RunReadHardwareIds()
	{
		var hw = EspToolService.ReadHardwareIds(Paths.RootDir);
		Console.WriteLine("      Chip type: " + hw.ChipType);
		Console.WriteLine("            MAC: " + BoardPaths.Mac6Char(hw.Mac) + " (decimal: " + hw.Mac + ")");
		Console.WriteLine("Flash-Encryption-Key vorhanden: " + (hw.HasFlashEncryptionKey ? "yes" : "no"));

		var boardDir = BoardPaths.BoardSpecificPath(BuilderOptions.Current.BoardsDir, hw.Mac);
		var wasKnown = Directory.Exists(boardDir);

		var ctx = BoardContext.LoadFromLiveMac(hw.Mac);

		Console.WriteLine(wasKnown
			? $"Board bekannt ({ctx.Board.BoardName}) -- last_connected_dt aktualisiert."
			: "NEUES Board -- Eintrag mit Default-Werten angelegt.");
		Console.WriteLine($"Board-Verzeichnis: {ctx.BoardDirectory}");
	}

	private static void RunFlashFirmware(bool resetNvsPartition = false)
	{
		const bool writeStorage = true;

		var ctx = BoardContext.LoadCached();
		if (ctx.Board.FlashEncryptionKeyBurnedAndActivated)
		{
			throw new NotImplementedException("Verschluesselter Flash-Pfad ist bewusst nicht implementiert (brennt EFuses permanent).");
		}

		var f = EspIdfFlasherArgs.Load(Paths.BuildDir);
		List<(string Offset, string File)> sections =
		[
			(f.Bootloader.Offset, Path.Combine(Paths.BuildDir, f.Bootloader.File)),
			(f.App.Offset, Path.Combine(Paths.BuildDir, f.App.File)),
			(f.PartitionTable.Offset, Path.Combine(Paths.BuildDir, f.PartitionTable.File)),
			(f.Otadata.Offset, Path.Combine(Paths.BuildDir, f.Otadata.File)),
		];
		if (writeStorage && f.Storage is not null)
		{
			sections.Add((f.Storage.Offset, Path.Combine(Paths.BuildDir, f.Storage.File)));
		}

		foreach (var (_, file) in sections)
		{
			if (!File.Exists(file))
			{
				throw new InvalidOperationException($"Zu flashende Datei nicht gefunden: {file} -- zuerst BuildFirmware ausfuehren.");
			}
		}

		Console.WriteLine($"Flashe {sections.Count} Sektionen (unverschluesselt) auf Board {ctx.Board.BoardName}...");
		EspToolService.WriteFlash(sections, Paths.RootDir);
		Console.WriteLine("Flash (nicht verschluesselt) abgeschlossen.");

		if (resetNvsPartition)
		{
			var nvs = PartitionsCsv.Find(Path.Combine(Paths.RootDir, "partitions.csv"), "nvs");
			if (nvs.Offset is null)
			{
				throw new InvalidOperationException("nvs-Partition hat kein Offset in partitions.csv -- kann nicht geloescht werden.");
			}
			Console.WriteLine($"--resetNVSPartition: loesche nvs-Partition (Offset 0x{nvs.Offset:X}, Groesse 0x{nvs.Size:X}) -- WLAN-/Usersettings gehen verloren.");
			EspToolService.EraseRegion($"0x{nvs.Offset:X}", nvs.Size, Paths.RootDir);
			Console.WriteLine("nvs-Partition geloescht.");
		}
	}
}

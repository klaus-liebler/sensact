using System.Text.Json;
using FirmwareBuilder.Common;
using FirmwareBuilder.Common.Esp32;
using Klli.Sensact.Config;
using Klli.Sensact.Model.Common;
using Microsoft.Extensions.Logging;

namespace Builder;

internal static class Program
{
	public static void Main(string[] args) =>
		BuildStepRunner.Run(args, a => new SensactBuildContext(a), typeof(Program));

	[BuildStep]
	public static void Info(IBuildContextEsp32 ctx) =>
		Esp32ConsoleReport.WriteBoardInfo(ctx, "board_info.json", () =>
		{
			try
			{
				return [("NodeId", SensactContextLoader.Load(ctx.BoardArchiveDir).NodeId)];
			}
			catch (Exception ex)
			{
				return [("NodeId", $"(unbekannt -- {ex.Message})")];
			}
		});

	[BuildStep]
	public static void GitStatus(IBuildContextEsp32 ctx) => BuilderConsoleReport.WriteGitStatus(ctx.Git);

	// Nachfolger von ReadHardwareIds: liest die echte MAC vom angeschlossenen Board. Analog zu
	// STM32s PrepareContextWithRealHardware ein einzelner Aufruf in die (geteilte) Buildercommons.
	[BuildStep]
	public static void PrepareContextWithRealHardware(IBuildContextEsp32 ctx) =>
		Esp32BoardProvisioningService.PrepareContextWithRealHardware(
			ctx,
			createDefaultRecord: mac => SensactBuildContext.NewDefaultBoardRecord(mac),
			onBoardArchiveReady: recordPath => File.Copy(recordPath, ctx.BoardInfoJsonPath, overwrite: true));

	// Nachfolger von SelectOfflineBoard: baut die Board-Identitaet ohne angeschlossene Hardware rein
	// aus --nodeId auf (deterministisch abgeleitete synthetische MAC, s. SensactBuildContext).
	[BuildStep]
	public static void PrepareContextWithCommandLineArguments(IBuildContextEsp32 ctx)
	{
		var nodeId = Cli.GetRequiredArgValue(ctx.Args, "--nodeId");
		var board = SensactBuildContext.EnsureBoardForOfflineNodeId(nodeId);
		var mac = board.Mac ?? throw new InvalidOperationException("EnsureBoardForOfflineNodeId hat kein Mac-Feld gesetzt (unerwartet).");
		var boardDir = BoardPaths.BoardSpecificPath(ctx.BoardsDir, mac);
		Console.WriteLine($"Offline-Board fuer NODE_ID \"{nodeId}\" ausgewaehlt (synthetische MAC, kein Board angeschlossen).");
		Console.WriteLine("              MAC: " + BoardPaths.Mac6Char(mac) + " (decimal: " + mac + ")");
		Console.WriteLine("Board-Verzeichnis: " + boardDir);
	}

	[BuildStep]
	public static void GenerateBestBinaryBufferFiles(IBuildContextEsp32 ctx) => WsProtocolBuildService.Generate(
		ctx,
		sourceDirs: [ctx.BestBinaryBuffersSchemaDir, ctx.WebmanagerBestBinaryBuffersSchemaDir],
		cppOutputDir: SensactBuildContext.GeneratedWsProtocolCppDir,
		tsOutputDir: SensactBuildContext.GeneratedWsProtocolTsDir,
		tsPackageName: "@generated/wsprotocol_ts");

	private static object JsonElementToObject(JsonElement el) => el.ValueKind switch
	{
		JsonValueKind.String => el.GetString()!,
		JsonValueKind.Number => el.TryGetInt64(out var l) ? l : el.GetDouble(),
		JsonValueKind.True => true,
		JsonValueKind.False => false,
		JsonValueKind.Array => el.EnumerateArray().Select(e => e.GetString() ?? "").ToList(),
		_ => el.GetRawText(),
	};

	[BuildStep]
	public static void GenerateRuntimeConfig(IBuildContextEsp32 ctx)
	{
		var appName = "sensact";
		var appVersion = "1.0";
		var defines = new Dictionary<string, object>();
		var clientDefines = new Dictionary<string, object>();

		var board = SensactBuildContext.LoadBoardRecord(ctx.BoardArchiveDir);
		var sensact = SensactContextLoader.Load(ctx.BoardArchiveDir);
		var nodeId = sensact.NodeId;

		var nodeDescriptorPath = Path.Combine(SensactBuildContext.SensactModelGeneratedDir, nodeId, "node_descriptor.json");
		if (!File.Exists(nodeDescriptorPath))
		{
			throw new InvalidOperationException(
				$"Keine node_descriptor.json unter {nodeDescriptorPath} -- GenerateModelFiles muss vor diesem Schritt einmal gelaufen sein.");
		}

		foreach (var prop in JsonDocument.Parse(File.ReadAllText(nodeDescriptorPath)).RootElement.EnumerateObject())
		{
			var value = JsonElementToObject(prop.Value);
			defines[prop.Name] = value;
			clientDefines[prop.Name] = value;
		}

		foreach (var (key, value) in sensact.WebSettings)
		{
			defines[key] = value;
			clientDefines[key] = value;
		}
		foreach (var (key, value) in sensact.FirmwareSettings)
		{
			defines[key] = value;
		}

		var now = DateTimeOffset.UtcNow;

		void SetShared(string key, object value)
		{
			defines[key] = value;
			clientDefines[key] = value;
		}

		defines["WEBMANAGER_AUTH_USERNAME"] = "admin";
		defines["WEBMANAGER_AUTH_PASSWORD"] = string.IsNullOrEmpty(ctx.WebAdminPassword)
			? $"{ctx.Hostname}_admin".ToLowerInvariant()
			: ctx.WebAdminPassword;

		SetShared("NODE_ID", nodeId);
		SetShared("HOSTNAME", ctx.Hostname);
		SetShared("BOARD_NAME", ctx.Hostname);
		SetShared("BOARD_VERSION", board.BoardVersion ?? 0);
		SetShared("BOARD_MAC", ctx.ChipId.ToEsp32Mac48());
		defines["BOARD_DIRECTORY"] = ctx.BoardArchiveDir;
		SetShared("APP_NAME", appName);
		SetShared("APP_VERSION", appVersion);
		SetShared("CREATION_DT", now.ToUnixTimeSeconds());
		SetShared("CREATION_DT_STR", now.ToLocalTime().ToString("yyyy-MM-dd HH:mm:ss"));
		SetShared("GIT_SHORT_HASH", ctx.Git.CommitHash);
		SetShared("BANNER", $"{appName} {nodeId}");

		RuntimeConfigWriter.CreateCppConfigurationHeader(SensactBuildContext.GeneratedRuntimeConfigCppDir, defines);
		RuntimeConfigWriter.CreateCMakeJsonConfigFile(SensactBuildContext.GeneratedCMakeDir, defines);
		RuntimeConfigWriter.CreateTypeScriptRuntimeConfig(SensactBuildContext.GeneratedRuntimeConfigTsDir, clientDefines);

		Console.WriteLine($"{defines.Count} Defines -> {SensactBuildContext.GeneratedRuntimeConfigCppDir}, {SensactBuildContext.GeneratedCMakeDir}; {clientDefines.Count} Defines -> {SensactBuildContext.GeneratedRuntimeConfigTsDir}");
	}

	[BuildStep]
	public static void GenerateCertificates(IBuildContextEsp32 ctx) => BoardCertificateService.EnsureBoardCertificate(
		ctx,
		rootCaCommonName: BuilderOptions.Current.Certificates.RootCaCommonName,
		hostname: ctx.Hostname,
		outputFileBaseName: "esp32");

	[BuildStep]
	public static void GenerateModelFiles(IBuildContextEsp32 ctx)
	{
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Information));

		switch (modelName)
		{
			case "Sattlerstrasse16":
				RunModelFor<Klli.Sensact.Model.Sattlerstrasse16.ApplicationId>(
					ctx, new Sattlerstrasse16Builder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			case "Testmodel":
				RunModelFor<Klli.Sensact.Model.Testmodel.ApplicationId>(
					ctx, new TestModelBuilder().BuildAndFinalizeAndReturnModelContainer(), loggerFactory);
				break;
			default:
				throw new ArgumentException($"Unbekanntes Modell \"{modelName}\" (gueltig: Sattlerstrasse16, Testmodel).");
		}
	}

	private static void RunModelFor<ApplicationIdType>(IBuildContextEsp32 ctx, ModelContainer mc, ILoggerFactory loggerFactory) where ApplicationIdType : struct, Enum
	{
		var appSettings = new AppSettings
		{
			SourceCodeGenerator = new SourceCodeGeneratorOptions { BasePath = SensactBuildContext.SensactModelGeneratedDir },
		};
		var logger = loggerFactory.CreateLogger<SourceCodeGenerator<ApplicationIdType>>();
		var generator = new SourceCodeGenerator<ApplicationIdType>(appSettings, logger);

		if (!generator.CheckAndPrepare(mc))
		{
			throw new InvalidOperationException("CheckAndPrepare() fehlgeschlagen -- s. Log-Ausgabe oben fuer Details.");
		}
		generator.DeleteBaseDirectory();
		generator.generateAll(mc);
		Console.WriteLine($"Modell-Dateien -> {SensactBuildContext.SensactModelGeneratedDir}");

		foreach (var enumFile in new[] { "applicationIds.enums.cs", "commandTypes.enums.cs" })
		{
			var source = Path.Combine(SensactBuildContext.SensactModelGeneratedDir, "common", enumFile);
			var dest = Path.Combine(ctx.BestBinaryBuffersSchemaDir, $"sensact_{enumFile}");
			File.Copy(source, dest, overwrite: true);
		}
		Console.WriteLine($"ws-protocol-Enum-Quellen aktualisiert -> {ctx.BestBinaryBuffersSchemaDir}");
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

	[BuildStep]
	public static void GenerateSensactFiles(IBuildContextEsp32 ctx)
	{
		var nodeId = SensactContextLoader.Load(ctx.BoardArchiveDir).NodeId;

		var sendCommandProject = Path.Combine(SensactBuildContext.GeneratedRoot, "sensact_sendCommandImplementation");
		TemplateHere(
			Path.Combine(SensactBuildContext.WebTemplatesDir, "sendCommandImplementation.template.ts"),
			Path.Combine(SensactBuildContext.SensactModelGeneratedDir, "common", "sendCommandImplementation.ts.inc"),
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
				["@klaus-liebler/sensact-base"] = SensactBuildContext.RelativeFileDependency(sendCommandProject, Path.Combine(ctx.NpmPackagesDir, "@klaus-liebler", "sensact-base")),
			},
		});

		var appsBuilderProject = Path.Combine(SensactBuildContext.GeneratedRoot, "sensact_appsbuilder");
		TemplateHere(
			Path.Combine(SensactBuildContext.WebTemplatesDir, "sensactapps.template.ts"),
			Path.Combine(SensactBuildContext.SensactModelGeneratedDir, nodeId, "sensactapps_local.ts.inc"),
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
				["@klaus-liebler/sensact-base"] = SensactBuildContext.RelativeFileDependency(appsBuilderProject, Path.Combine(ctx.NpmPackagesDir, "@klaus-liebler", "sensact-base")),
				["@klaus-liebler/web-components-sensact"] = SensactBuildContext.RelativeFileDependency(appsBuilderProject, Path.Combine(ctx.NpmPackagesDir, "@klaus-liebler", "web-components-sensact")),
			},
		});

		Console.WriteLine($"Sensact-Dateien -> {sendCommandProject}, {appsBuilderProject}");
	}

	[BuildStep]
	public static void BuildWebApp(IBuildContextEsp32 ctx) => WebAppBuildService.Run(
		ctx,
		["--outDir", ctx.WebGeneratedDir, "--sourcemap", "true", "--emptyOutDir"],
		Path.Combine(ctx.WebGeneratedDir, "index.compressed.br"));

	[BuildStep]
	public static void BuildFirmware(IBuildContextEsp32 ctx) => Esp32FirmwareBuildService.Run(ctx);

	[BuildStep]
	public static void FlashFirmware(IBuildContextEsp32 ctx) => Esp32FlashService.Run(ctx);

	[BuildStep]
	public static void Pipeline(IBuildContextEsp32 ctx)
	{
		BuildStepRunner.Invoke(ctx, PrepareContextWithRealHardware);
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);
		BuildStepRunner.Invoke(ctx, GenerateRuntimeConfig);
		BuildStepRunner.Invoke(ctx, GenerateCertificates);
		BuildStepRunner.Invoke(ctx, GenerateSensactFiles);
		BuildStepRunner.Invoke(ctx, BuildWebApp);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
		BuildStepRunner.Invoke(ctx, FlashFirmware);
	}

	// Wie Pipeline, aber PrepareContextWithCommandLineArguments (--nodeId) statt
	// PrepareContextWithRealHardware, und ohne FlashFirmware -- fuer Builds ohne angeschlossenes
	// Board. Beispiel: PipelineOffline --nodeId SNSCT_NODE_SIDEDOOR --model Sattlerstrasse16.
	[BuildStep]
	public static void PipelineOffline(IBuildContextEsp32 ctx)
	{
		BuildStepRunner.Invoke(ctx, PrepareContextWithCommandLineArguments);
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);
		BuildStepRunner.Invoke(ctx, GenerateRuntimeConfig);
		BuildStepRunner.Invoke(ctx, GenerateCertificates);
		BuildStepRunner.Invoke(ctx, GenerateSensactFiles);
		BuildStepRunner.Invoke(ctx, BuildWebApp);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
	}
}

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
		BuildStepRunner.Run(args, a => Esp32Toolchain.CreateBuildContext(a, x => new SensactBuildContext(x)), typeof(Program));

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
		var boardId = ChipIdEsp32.FromEsp32Mac48(mac).ToBoardUid();
		var boardDir = BoardArchiveContext.BoardArchiveDir(ctx.BoardStorage, boardId);
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

		// Default: WiFi aktiv (webmanager::M). Boards mit funktionierendem Ethernet koennen WiFi
		// per sensact.json ("board_settings": { "firmware": { "WIFI_ENABLED": false } }) abschalten --
		// main.cc waehlt dann webmanager_eth_only::M statt webmanager::M, und das Web-Frontend blendet
		// die Wifimanager-Seite aus (s. WIFI_ENABLED in runtimeconfig_ts).
		var wifiEnabled = defines.TryGetValue("WIFI_ENABLED", out var wifiEnabledRaw) && wifiEnabledRaw is bool wifiEnabledBool
			? wifiEnabledBool
			: true;
		SetShared("WIFI_ENABLED", wifiEnabled);

		RuntimeConfigWriter.CreateCppConfigurationHeader(SensactBuildContext.GeneratedRuntimeConfigCppDir, defines);
		RuntimeConfigWriter.CreateCMakeJsonConfigFile(SensactBuildContext.GeneratedCMakeDir, defines);
		RuntimeConfigWriter.CreateTypeScriptRuntimeConfig(SensactBuildContext.GeneratedRuntimeConfigTsDir, clientDefines);

		Console.WriteLine($"{defines.Count} Defines -> {SensactBuildContext.GeneratedRuntimeConfigCppDir}, {SensactBuildContext.GeneratedCMakeDir}; {clientDefines.Count} Defines -> {SensactBuildContext.GeneratedRuntimeConfigTsDir}");
	}

	[BuildStep]
	public static void GenerateCertificates(IBuildContextEsp32 ctx) => BoardCertificateService.EnsureBoardCertificate(
		ctx,
		hostname: ctx.Hostname,
		outputFileBaseName: "esp32");

	// Quelle des Hausmodells: "cs" = C#-Hausmodell (builder/Models/*, bisher), "xml" = model/<Modell>.xml
	// (s. model/README.md). Beide liefern dieselben Modellobjekte; VerifyModelXml prueft, dass die
	// daraus generierten Dateien byte-gleich sind.
	private static (ModelContainer Mc, AppIdTable Ids) LoadModel(string modelName, string source)
	{
		switch (source)
		{
			case "cs":
				return modelName switch
				{
					"Sattlerstrasse16" => (new Sattlerstrasse16Builder().BuildAndFinalizeAndReturnModelContainer(), AppIdTable.FromEnum<Klli.Sensact.Model.Sattlerstrasse16.ApplicationId>()),
					"Testmodel" => (new TestModelBuilder().BuildAndFinalizeAndReturnModelContainer(), AppIdTable.FromEnum<Klli.Sensact.Model.Testmodel.ApplicationId>()),
					_ => throw new ArgumentException($"Unbekanntes Modell \"{modelName}\" (gueltig: Sattlerstrasse16, Testmodel)."),
				};
			case "xml":
				var dir = ModelXmlDir();
				var path = Path.Combine(dir, modelName + ".xml");
				if (!File.Exists(path)) throw new ArgumentException($"Kein XML-Hausmodell {path}.");
				var result = new Klli.Sensact.Model.Xml.HouseXmlImporter(LoadCatalog()).Import(path, Path.Combine(dir, "schema", "sensact-house.xsd"));
				return (new ModelContainer(result.Model), result.Ids);
			default:
				throw new ArgumentException($"Unbekannte Modellquelle \"{source}\" (gueltig: cs, xml).");
		}
	}

	private static string ModelXmlDir() => Path.Combine(Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!, "model");

	private static Klli.Sensact.Model.Xml.Catalog LoadCatalog()
	{
		var modelDir = ModelXmlDir();
		var catalogPath = Path.Combine(modelDir, "catalog.xml");
		var catalogDoc = System.Xml.Linq.XDocument.Load(catalogPath);
		var catalogSchemas = new System.Xml.Schema.XmlSchemaSet();
		catalogSchemas.Add(null, Path.Combine(modelDir, "schema", "sensact-catalog.xsd"));
		var catalogErrors = new List<string>();
		System.Xml.Schema.Extensions.Validate(catalogDoc, catalogSchemas, (_, e) => catalogErrors.Add(e.Message));
		if (catalogErrors.Count > 0)
		{
			throw new InvalidOperationException("catalog.xml verletzt das Schema:\n" + string.Join('\n', catalogErrors));
		}
		return Klli.Sensact.Model.Xml.Catalog.Load(catalogPath);
	}

	[BuildStep]
	public static void GenerateModelFiles(IBuildContextEsp32 ctx)
	{
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		var source = ModelSource(ctx);
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Information));
		var (mc, ids) = LoadModel(modelName, source);
		Console.WriteLine($"Hausmodell {modelName} aus Quelle \"{source}\"");
		RunModelFor(ctx, mc, ids, loggerFactory);
	}

	// --source cs|xml, Vorgabe cs (bis die C#-Hausmodelle entfallen)
	private static string ModelSource(IBuildContextEsp32 ctx)
	{
		int i = ctx.Args.ToList().IndexOf("--source");
		return i >= 0 && i + 1 < ctx.Args.Count ? ctx.Args[i + 1] : "cs";
	}

	// Uebergang zum XML-Hausmodell (docs/concept-modular-firmware.md): schreibt beide C#-Modelle
	// nach model/<Modell>.xml, prueft Katalog und Ergebnis gegen die XSDs und jede
	// Ein-/Ausgangsadresse per Rueckuebersetzung. Aendert nichts an Firmware oder Codegenerierung.
	[BuildStep]
	public static void ExportModelXml(IBuildContextEsp32 ctx)
	{
		var firmwareRoot = Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!;
		var modelDir = ModelXmlDir();
		var catalog = LoadCatalog();
		var houseXsd = Path.Combine(modelDir, "schema", "sensact-house.xsd");

		void Export<TId>(string name, Klli.Sensact.Model.Common.Model model) where TId : struct, Enum
		{
			var exporter = new Klli.Sensact.Model.Xml.HouseXmlExporter<TId>(catalog, firmwareRoot, NodeIdBoardTypeCatalog.TryResolveBoardType);
			var doc = exporter.Export(model);
			Klli.Sensact.Model.Xml.HouseXmlExporter<TId>.Validate(doc, houseXsd);
			var path = Path.Combine(modelDir, name + ".xml");
			Klli.Sensact.Model.Xml.HouseXmlExporter<TId>.Save(doc, path);
			Console.WriteLine($"{path}: {doc.Root!.Elements("station").Count()} Stationen, {doc.Root!.Elements("station").SelectMany(s => s.Elements()).Count(e => e.Attribute("number") != null)} Apps, gueltig.");
		}

		Export<Klli.Sensact.Model.Sattlerstrasse16.ApplicationId>("Sattlerstrasse16", new Sattlerstrasse16Builder().BuildAndFinalizeAndReturnModelContainer().Model);
		Export<Klli.Sensact.Model.Testmodel.ApplicationId>("Testmodel", new TestModelBuilder().BuildAndFinalizeAndReturnModelContainer().Model);
	}

	// Rueckweg pruefen: generiert jedes Hausmodell einmal aus dem C#-Modell und einmal aus dem XML
	// (in temporaere Verzeichnisse) und vergleicht alle erzeugten Dateien zeilengenau.
	[BuildStep]
	public static void VerifyModelXml(IBuildContextEsp32 ctx)
	{
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Warning));
		var root = Path.Combine(Path.GetTempPath(), "sensact_verify_model_xml");
		var failures = new List<string>();
		foreach (var modelName in new[] { "Sattlerstrasse16", "Testmodel" })
		{
			var dirs = new Dictionary<string, string>();
			foreach (var source in new[] { "cs", "xml" })
			{
				var dir = Path.Combine(root, modelName, source);
				var (mc, ids) = LoadModel(modelName, source);
				GenerateModelInto(mc, ids, dir, loggerFactory);
				dirs[source] = dir;
			}
			var files = Directory.EnumerateFiles(dirs["cs"], "*", SearchOption.AllDirectories).Select(f => Path.GetRelativePath(dirs["cs"], f))
				.Union(Directory.EnumerateFiles(dirs["xml"], "*", SearchOption.AllDirectories).Select(f => Path.GetRelativePath(dirs["xml"], f)))
				.OrderBy(f => f, StringComparer.Ordinal).ToList();
			int same = 0;
			foreach (var rel in files)
			{
				var a = Path.Combine(dirs["cs"], rel);
				var b = Path.Combine(dirs["xml"], rel);
				if (!File.Exists(a) || !File.Exists(b))
				{
					failures.Add($"{modelName}/{rel}: nur in {(File.Exists(a) ? "cs" : "xml")}");
					continue;
				}
				var la = File.ReadAllLines(a);
				var lb = File.ReadAllLines(b);
				int line = Enumerable.Range(0, Math.Max(la.Length, lb.Length)).FirstOrDefault(i => i >= la.Length || i >= lb.Length || la[i] != lb[i], -1);
				if (line < 0)
				{
					same++;
					continue;
				}
				failures.Add($"{modelName}/{rel}, Zeile {line + 1}:\n  cs : {(line < la.Length ? la[line] : "<Ende>")}\n  xml: {(line < lb.Length ? lb[line] : "<Ende>")}");
			}
			Console.WriteLine($"{modelName}: {same} von {files.Count} Dateien gleich.");
		}
		if (failures.Count > 0)
		{
			throw new InvalidOperationException("Generierung aus XML weicht ab:\n" + string.Join('\n', failures));
		}
		Console.WriteLine("Generierung aus XML und aus C# ist identisch.");
	}

	// Erzeugt die Modell-Dateien fuer die Host-Unit-Tests der Sensact-Apps
	// (sensact/espidf-components/sensact-applicationmodel/host_test) in ein frei waehlbares
	// Verzeichnis (--out). Immer aus dem Testmodel und -- anders als GenerateModelFiles -- OHNE die
	// Enum-Quellen nach best_binary_buffers_schema/ zu kopieren: das wuerde die dort git-getrackten
	// Schema-Dateien des echten Hausmodells mit denen des Testmodells ueberschreiben. Wird vom
	// CMake-Projekt der Host-Tests automatisch aufgerufen, s. dortige README.md.
	[BuildStep]
	public static void GenerateModelFilesForHostTests(IBuildContextEsp32 ctx)
	{
		var outDir = Path.GetFullPath(Cli.GetRequiredArgValue(ctx.Args, "--out"));
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Warning));
		var (mc, ids) = LoadModel("Testmodel", ModelSource(ctx));
		GenerateModelInto(mc, ids, outDir, loggerFactory);
	}

	private static void GenerateModelInto(ModelContainer mc, AppIdTable ids, string outDir, ILoggerFactory loggerFactory)
	{
		var appSettings = new AppSettings
		{
			SourceCodeGenerator = new SourceCodeGeneratorOptions { BasePath = outDir },
		};
		var logger = loggerFactory.CreateLogger<SourceCodeGenerator>();
		var generator = new SourceCodeGenerator(appSettings, ids, logger);

		if (!generator.CheckAndPrepare(mc))
		{
			throw new InvalidOperationException("CheckAndPrepare() fehlgeschlagen -- s. Log-Ausgabe oben fuer Details.");
		}
		generator.DeleteBaseDirectory();
		generator.generateAll(mc);
		Console.WriteLine($"Modell-Dateien -> {outDir}");
	}

	private static void RunModelFor(IBuildContextEsp32 ctx, ModelContainer mc, AppIdTable ids, ILoggerFactory loggerFactory)
	{
		GenerateModelInto(mc, ids, SensactBuildContext.SensactModelGeneratedDir, loggerFactory);

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

	// Host-Unit-Tests der Sensact-Apps (sensact/espidf-components/sensact-applicationmodel/host_test).
	// Teil von Pipeline und PipelineOffline: ein roter Test bricht den Build ab, bevor gebaut und
	// geflasht wird. Braucht CMake, Ninja und GCC im PATH (MSYS2 UCRT64), s. host_test/README.md --
	// aus PowerShell/cmd starten, nicht aus Git-Bash.
	[BuildStep]
	public static void RunHostTests(IBuildContextEsp32 ctx)
	{
		var dir = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!, "..", "..", "espidf-components", "sensact-applicationmodel", "host_test"));
		var build = Path.Combine(dir, "build");
		if (!File.Exists(Path.Combine(build, "CMakeCache.txt")))
		{
			ProcessRunner.RunInherit("cmake", ["-S", dir, "-B", build, "-G", "Ninja"], dir);
		}
		ProcessRunner.RunInherit("cmake", ["--build", build], dir);
		ProcessRunner.RunInherit("ctest", ["--test-dir", build, "--output-on-failure"], dir);
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
		BuildStepRunner.Invoke(ctx, RunHostTests);
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
		BuildStepRunner.Invoke(ctx, RunHostTests);
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);
		BuildStepRunner.Invoke(ctx, GenerateRuntimeConfig);
		BuildStepRunner.Invoke(ctx, GenerateCertificates);
		BuildStepRunner.Invoke(ctx, GenerateSensactFiles);
		BuildStepRunner.Invoke(ctx, BuildWebApp);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
	}
}

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

		// Kennung fuer das Stationspaket: die Firmware nimmt nur Pakete mit derselben Kennung an
		// (s. StationPackage.ComputeCompatId).
		defines["STATION_COMPAT_ID"] = StationPackage.Hex(StationPackage.ComputeCompatId());
		// Board-Initialisierung je Board-TYP (main/hal/<HAL_DIR>/board_init.hh); MONITORING ist keine
		// Station eines Hauses und hat eine eigene.
		defines["HAL_DIR"] = nodeId == "MONITORING" ? "MONITORING" : ctx.BoardTypeName;

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

	// Hausmodell aus model/<Modell>.xml (s. model/README.md): Stationen, Apps und App-IDs.
	private static (ModelContainer Mc, AppIdTable Ids) LoadModel(string modelName)
	{
		var dir = ModelXmlDir();
		var path = Path.Combine(dir, modelName + ".xml");
		if (!File.Exists(path))
		{
			var known = string.Join(", ", Directory.EnumerateFiles(dir, "*.xml").Select(Path.GetFileNameWithoutExtension).Where(n => n != "catalog"));
			throw new ArgumentException($"Kein Hausmodell {path} (vorhanden: {known}).");
		}
		var result = new Klli.Sensact.Model.Xml.HouseXmlImporter(LoadCatalog()).Import(path, Path.Combine(dir, "schema", "sensact-house.xsd"));
		return (new ModelContainer(result.Model), result.Ids);
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

	// STM32F103-Stationen (Katalog-Firmware "stm32f103", firmware/sensact_firmware_f103 auf rtoos):
	// dieselben Apps wie die ESP32-Basis, die Stationskonfiguration aber einkompiliert (kein Upload,
	// nur CAN). Erzeugt die modellunabhaengigen .inc-Dateien und den BestBinaryBuffers-Decoder (wie
	// fuer die ESP32-Firmware, nach generated/) und die Stationskonfiguration nach
	// ../sensact_firmware_f103/generated/<NODE_ID>/station_config.bin. Danach:
	//   cmake --preset lvng && cmake --build --preset lvng     (in firmware/sensact_firmware_f103)
	// Beispiel: GenerateStm32F103Files --model Sattlerstrasse16 --nodeId SNSCT_L1_LVNG_UP
	[BuildStep]
	public static void GenerateStm32F103Files(IBuildContextEsp32 ctx)
	{
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		var nodeId = Cli.GetRequiredArgValue(ctx.Args, "--nodeId");
		var catalog = LoadCatalog();
		var (mc, ids) = LoadModel(modelName);
		var station = mc.Model.Nodes.OfType<Klli.Sensact.Model.Xml.XmlStation>().FirstOrDefault(s => s.NodeName == nodeId)
			?? throw new InvalidOperationException($"Station {nodeId} gibt es im Hausmodell {modelName} nicht.");
		if (!catalog.Boards.TryGetValue(station.BoardType, out var board) || board.Firmware != "stm32f103")
		{
			throw new InvalidOperationException($"Station {nodeId} (Board {station.BoardType}) ist keine STM32F103-Station.");
		}
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);

		var schemaDirs = new[] { ctx.BestBinaryBuffersSchemaDir, ctx.WebmanagerBestBinaryBuffersSchemaDir };
		var idMap = BestBinaryBuffers.IdMap.Load(Path.Combine(ctx.BestBinaryBuffersSchemaDir, "ids.txt"));
		var config = StationConfigWriter.Encode(StationConfigWriter.ParseSchema(schemaDirs, idMap), mc.Model, ids, catalog, nodeId);
		var firmwareRoot = Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!;
		var outDir = Path.GetFullPath(Path.Combine(firmwareRoot, "..", "sensact_firmware_f103", "generated", nodeId));
		Directory.CreateDirectory(outDir);
		File.WriteAllBytes(Path.Combine(outDir, "station_config.bin"), config);
		// Nummer der Station (= der Knoten-App): kommt in den Image-Kopf, der Bootloader meldet sich
		// darunter am CAN-Bus (sensact_firmware_f103/boot/boot_protocol.hpp)
		File.WriteAllText(Path.Combine(outDir, "station_number.txt"), station.NodeId.ToString(System.Globalization.CultureInfo.InvariantCulture));
		Console.WriteLine($"Stationskonfiguration {nodeId} (Station {station.NodeId}, {config.Length} Byte) -> {outDir}");
	}

	[BuildStep]
	public static void GenerateModelFiles(IBuildContextEsp32 ctx)
	{
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		using var loggerFactory = LoggerFactory.Create(b => b.AddConsole().SetMinimumLevel(LogLevel.Information));
		var (mc, ids) = LoadModel(modelName);
		RunModelFor(ctx, mc, ids, loggerFactory);
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
		var (mc, ids) = LoadModel("Testmodel");
		GenerateModelInto(mc, ids, outDir, loggerFactory);

		// Fuer die Tests der App-Fabrik: C++-Decoder (ws_protocol.hh) und je Station die
		// Stationskonfiguration, mit derselben (nur gelesenen, nicht gespeicherten) ids.txt.
		var schemaDirs = new[] { ctx.BestBinaryBuffersSchemaDir, ctx.WebmanagerBestBinaryBuffersSchemaDir };
		var idMap = BestBinaryBuffers.IdMap.Load(Path.Combine(ctx.BestBinaryBuffersSchemaDir, "ids.txt"));
		var schemaFiles = schemaDirs.SelectMany(d => Directory.GetFiles(d, "*.cs")).ToList();
		var (cpp, _) = BestBinaryBuffers.SchemaCompiler.Generate(schemaFiles, idMap);
		Directory.CreateDirectory(Path.Combine(outDir, "wsprotocol_cpp"));
		File.WriteAllText(Path.Combine(outDir, "wsprotocol_cpp", "ws_protocol.hh"), cpp);
		var schema = StationConfigWriter.ParseSchema(schemaDirs, idMap);
		var catalog = LoadCatalog();
		void WriteConfigs(string modelName, string dir)
		{
			var (freshMc, freshIds) = LoadModel(modelName);
			var configDir = Path.Combine(dir, "station_config");
			Directory.CreateDirectory(configDir);
			foreach (var node in freshMc.Model.Nodes.OfType<Klli.Sensact.Model.Xml.XmlStation>()
				.Where(s => catalog.Boards.TryGetValue(s.BoardType, out var b) && b.Firmware == "esp32"))
			{
				File.WriteAllBytes(Path.Combine(configDir, node.NodeName + ".bin"),
					StationConfigWriter.Encode(schema, freshMc.Model, freshIds, catalog, node.NodeName));
			}
		}
		WriteConfigs("Testmodel", outDir);
		// Zusaetzlich das echte Hausmodell (unter <out>/Sattlerstrasse16): es nutzt App-Typen, die das
		// Testmodel nicht hat -- die Fabrik-Tests vergleichen auch dessen ESP32-Stationen mit dem
		// generierten Code.
		var houseDir = Path.Combine(outDir, "Sattlerstrasse16");
		var (houseMc, houseIds) = LoadModel("Sattlerstrasse16");
		GenerateModelInto(houseMc, houseIds, houseDir, loggerFactory);
		WriteConfigs("Sattlerstrasse16", houseDir);
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

		foreach (var enumFile in new[] { "commandTypes.enums.cs" }) // App-IDs bewusst nicht: die Basis ist stationsneutral (s. sensact.cs)
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

		// App-Nummern und -Namen des Hausmodells fuer die Web-UI. Frueher Teil des ws-Protokolls; das ist
		// seit der stationsneutralen Basis modellunabhaengig (App-IDs als ushort), die Namen braucht nur
		// noch die (je Station gebaute, im Stationspaket ausgelieferte) Web-UI.
		var idsProject = Path.Combine(SensactBuildContext.GeneratedRoot, "sensact_applicationids");
		Directory.CreateDirectory(idsProject);
		var idLines = File.ReadAllLines(Path.Combine(SensactBuildContext.SensactModelGeneratedDir, "common", "applicationIds.inc"))
			.Select(l => l.Trim().TrimEnd(','))
			.Where(l => l.Length > 0 && !l.StartsWith("CNT=", StringComparison.Ordinal));
		File.WriteAllText(Path.Combine(idsProject, "applicationIds.ts"),
			"// Generiert (GenerateSensactFiles) aus dem Hausmodell -- nicht bearbeiten.\n" +
			"export enum ApplicationId {\n" + string.Concat(idLines.Select(l => $"\t{l.Replace("=", " = ")},\n")) + "}\n");
		NpmProject.CreateAndInstallLazily(idsProject, new PackageJson
		{
			Name = "@generated/sensact_applicationids",
			Description = "Generated during Build",
			Author = "Generated",
			License = "No License",
			Dependencies = new Dictionary<string, string>(),
		});

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
				["@generated/sensact_applicationids"] = "file:../sensact_applicationids",
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
				["@generated/sensact_applicationids"] = "file:../sensact_applicationids",
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
	public static void BuildFirmware(IBuildContextEsp32 ctx)
	{
		// Wartungsseite (main/maintenance/index.html) brotli-komprimiert einbetten -- die eigentliche Web-UI
		// ist stationsspezifisch und kommt mit dem Stationspaket (s. main/CMakeLists.txt, EMBED_FILES)
		var firmwareRoot = Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!;
		var maintenanceDir = Path.Combine(SensactBuildContext.GeneratedRoot, "maintenance");
		Directory.CreateDirectory(maintenanceDir);
		var html = File.ReadAllBytes(Path.Combine(firmwareRoot, "main", "maintenance", "index.html"));
		using (var output = File.Create(Path.Combine(maintenanceDir, "index.compressed.br")))
		using (var brotli = new System.IO.Compression.BrotliStream(output, System.IO.Compression.CompressionLevel.SmallestSize))
		{
			brotli.Write(html);
		}
		Esp32FirmwareBuildService.Run(ctx);
	}

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
		BuildStepRunner.Invoke(ctx, BuildStationPackage);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
		BuildStepRunner.Invoke(ctx, CollectDeployFiles);
		BuildStepRunner.Invoke(ctx, FlashFirmware);
	}

	// Macht das ECHTE Board einer Station zum aktiven Board (board_info.json), ohne dass es
	// angeschlossen ist: sucht im Board-Archiv (BoardStorage) die Boards mit dieser node_id
	// (sensact.json), laesst synthetische (PrepareContextWithCommandLineArguments, "locally
	// administered"-MAC) weg und nimmt bei mehreren das zuletzt angeschlossene. Voraussetzung: das
	// Board war mindestens einmal per USB angeschlossen (Pipeline/PrepareContextWithRealHardware).
	// Beispiel: SelectBoard --nodeId SNSCT_L0_TECH_HS_1
	[BuildStep]
	public static void SelectBoard(IBuildContextEsp32 ctx)
	{
		var nodeId = Cli.GetRequiredArgValue(ctx.Args, "--nodeId");
		var root = BuilderOptions.Current.BoardStorage.BoardsDir;
		var candidates = new List<(string Dir, BoardRecord Record)>();
		foreach (var dir in Directory.Exists(root) ? Directory.EnumerateDirectories(root) : [])
		{
			var sensactJson = Path.Combine(dir, "sensact.json");
			if (!File.Exists(sensactJson) || !File.Exists(Path.Combine(dir, "board_info.json"))) continue;
			string? id;
			try
			{
				id = JsonSerializer.Deserialize<Dictionary<string, JsonElement>>(File.ReadAllText(sensactJson))?
					.GetValueOrDefault("node_id").GetString();
			}
			catch (JsonException)
			{
				continue;
			}
			if (id != nodeId) continue;
			var record = SensactBuildContext.LoadBoardRecord(dir);
			bool synthetic = record.Mac is long mac && (((mac >> 40) & 0x02) != 0);
			if (!synthetic) candidates.Add((dir, record));
		}
		if (candidates.Count == 0)
		{
			throw new InvalidOperationException(
				$"Kein echtes Board fuer {nodeId} im Board-Archiv ({root}). Das Board muss einmal per USB " +
				"angeschlossen und mit Pipeline bzw. PrepareContextWithRealHardware vorbereitet werden.");
		}
		var (chosenDir, chosen) = candidates.OrderByDescending(c => c.Record.LastConnectedAtEpoch).First();
		File.Copy(Path.Combine(chosenDir, "board_info.json"), ctx.BoardInfoJsonPath, overwrite: true);
		Console.WriteLine($"Aktives Board: {nodeId} ({chosenDir}){(candidates.Count > 1 ? $", zuletzt angeschlossenes von {candidates.Count}" : "")}");
	}

	// Fuer den Editor: Board der Station waehlen und ihr Stationspaket bauen. Das vorher aktive Board
	// wird danach wiederhergestellt -- ein spaeterer BuildForOta auf der Kommandozeile baut also nicht
	// ueberraschend fuer die zuletzt im Editor gebaute Station.
	// Beispiel: BuildPackageFor --nodeId SNSCT_L0_TECH_HS_1 --model Sattlerstrasse16
	[BuildStep]
	public static void BuildPackageFor(IBuildContextEsp32 ctx)
	{
		var previous = File.Exists(ctx.BoardInfoJsonPath) ? File.ReadAllBytes(ctx.BoardInfoJsonPath) : null;
		try
		{
			BuildStepRunner.Invoke(ctx, SelectBoard);
			BuildStepRunner.Invoke(ctx, BuildPackage);
		}
		finally
		{
			if (previous != null)
			{
				File.WriteAllBytes(ctx.BoardInfoJsonPath, previous);
				Console.WriteLine("Vorher aktives Board wiederhergestellt.");
			}
		}
	}

	// Nach einer Aenderung am Hausmodell (Editor): nur das Stationspaket (Konfiguration + Web-UI) fuer
	// das AKTIVE Board neu bauen, KEINE Firmware -- die Basis-Firmware ist stationsneutral. Ergebnis:
	// deploy/<NODE_ID>/<NODE_ID>.pkg, hochladen ueber die Weboberflaeche (System, "Station Package").
	// Beispiel: BuildPackage --model Sattlerstrasse16
	[BuildStep]
	public static void BuildPackage(IBuildContextEsp32 ctx)
	{
		RequireActiveBoardIsStation(ctx);
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);
		BuildStepRunner.Invoke(ctx, GenerateRuntimeConfig);
		BuildStepRunner.Invoke(ctx, GenerateSensactFiles);
		BuildStepRunner.Invoke(ctx, BuildWebApp);
		BuildStepRunner.Invoke(ctx, BuildStationPackage);
	}

	// Nach einer Aenderung an der Basis-Firmware (C++-Code, Schema): Firmware UND Paket fuer das
	// AKTIVE Board (board_info.json -- das zuletzt per Pipeline/PrepareContextWithRealHardware
	// vorbereitete, mit seinen echten Zertifikaten), ohne zu flashen, nach deploy/<NODE_ID>/.
	// Anders als PipelineOffline (synthetisches Board-Archiv, falsche Zertifikate) fuer echte Geraete.
	// Beispiel: BuildForOta --model Sattlerstrasse16
	[BuildStep]
	public static void BuildForOta(IBuildContextEsp32 ctx)
	{
		RequireActiveBoardIsStation(ctx);
		BuildStepRunner.Invoke(ctx, RunHostTests);
		BuildStepRunner.Invoke(ctx, GenerateModelFiles);
		BuildStepRunner.Invoke(ctx, GenerateBestBinaryBufferFiles);
		BuildStepRunner.Invoke(ctx, GenerateRuntimeConfig);
		BuildStepRunner.Invoke(ctx, GenerateCertificates);
		BuildStepRunner.Invoke(ctx, GenerateSensactFiles);
		BuildStepRunner.Invoke(ctx, BuildWebApp);
		BuildStepRunner.Invoke(ctx, BuildStationPackage);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
		BuildStepRunner.Invoke(ctx, CollectDeployFiles);
	}

	private static void RequireActiveBoardIsStation(IBuildContextEsp32 ctx)
	{
		var nodeId = SensactContextLoader.Load(ctx.BoardArchiveDir).NodeId;
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		var (mc, _) = LoadModel(modelName);
		if (!mc.Model.Nodes.Any(n => n.NodeName == nodeId))
		{
			throw new InvalidOperationException(
				$"Das aktive Board ist {nodeId}, das ist keine Station von {modelName}. Erst das richtige Board " +
				"vorbereiten (einmal per USB: Pipeline bzw. PrepareContextWithRealHardware).");
		}
		Console.WriteLine($"Aktives Board {nodeId} ({ctx.BoardArchiveDir})");
	}

	// Stationspaket fuer das aktive Board: Abschnitt 1 Web-UI (aus BuildWebApp), Abschnitt 2
	// Stationskonfiguration (StationConfigWriter, aus model/<Modell>.xml). Nach deploy/<NODE_ID>/ und
	// zusaetzlich ins LittleFS-Quellverzeichnis, damit ein USB-Flash (Pipeline) das Geraet gleich mit
	// Konfiguration startet. Braucht --model.
	[BuildStep]
	public static void BuildStationPackage(IBuildContextEsp32 ctx)
	{
		var nodeId = SensactContextLoader.Load(ctx.BoardArchiveDir).NodeId;
		var modelName = Cli.GetRequiredArgValue(ctx.Args, "--model");
		var firmwareRoot = Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!;
		var webUi = Path.Combine(ctx.WebGeneratedDir, "index.compressed.br");
		if (!File.Exists(webUi))
		{
			throw new InvalidOperationException($"{webUi} fehlt -- erst BuildWebApp.");
		}
		if (nodeId == "MONITORING")
		{
			Console.WriteLine("MONITORING ist keine Station eines Hauses -- kein Stationspaket.");
			return;
		}

		var (mc, ids) = LoadModel(modelName);
		var schemaDirs = new[] { ctx.BestBinaryBuffersSchemaDir, ctx.WebmanagerBestBinaryBuffersSchemaDir };
		var idMap = BestBinaryBuffers.IdMap.Load(Path.Combine(ctx.BestBinaryBuffersSchemaDir, "ids.txt"));
		var config = StationConfigWriter.Encode(StationConfigWriter.ParseSchema(schemaDirs, idMap), mc.Model, ids, LoadCatalog(), nodeId);
		var webUiBytes = File.ReadAllBytes(webUi);

		var compatId = StationPackage.ComputeCompatId();
		var deployDir = Path.Combine(firmwareRoot, "deploy", nodeId);
		Directory.CreateDirectory(deployDir);
		var pkgPath = Path.Combine(deployDir, nodeId + ".pkg");
		var label = $"{nodeId} {DateTime.Now:yyyy-MM-dd HH:mm}";
		StationPackage.Write(pkgPath, nodeId, compatId,
			[(StationPackage.SectionWebUiHtmlBr, webUiBytes), (StationPackage.SectionStationConfig, config)], label);
		Console.WriteLine($"Stationspaket -> {pkgPath} (Konfiguration {config.Length} Byte, Web-UI {webUiBytes.Length} Byte, compatId {StationPackage.Hex(compatId)[..16]}...)");

		// fuer den ersten USB-Flash: Paket und Web-UI ins LittleFS-Abbild (dort erwartet sie der Webmanager)
		var littlefs = Path.Combine(firmwareRoot, "littlefs_filesystem");
		Directory.CreateDirectory(littlefs);
		File.Copy(pkgPath, Path.Combine(littlefs, "station.pkg"), overwrite: true);
		File.WriteAllBytes(Path.Combine(littlefs, "webui.br"), webUiBytes);
	}

	// Kopiert die zuletzt gebaute Firmware nach deploy/<NODE_ID>/ und schreibt eine Anleitung dazu.
	[BuildStep]
	public static void CollectDeployFiles(IBuildContextEsp32 ctx)
	{
		var nodeId = SensactContextLoader.Load(ctx.BoardArchiveDir).NodeId;
		var firmwareRoot = Path.GetDirectoryName(SensactBuildContext.GeneratedRoot)!;
		var deployDir = Path.Combine(firmwareRoot, "deploy", nodeId);
		Directory.CreateDirectory(deployDir);
		var firmwareBin = Path.Combine(firmwareRoot, "build", "sensact_firmware.bin");
		if (File.Exists(firmwareBin))
		{
			File.Copy(firmwareBin, Path.Combine(deployDir, "sensact_firmware.bin"), overwrite: true);
			Console.WriteLine($"Firmware      -> {Path.Combine(deployDir, "sensact_firmware.bin")}");
		}
		File.WriteAllText(Path.Combine(deployDir, "README.txt"),
			$"Erzeugt {DateTime.Now:yyyy-MM-dd HH:mm:ss} fuer {nodeId}, Basis-Kennung {StationPackage.Hex(StationPackage.ComputeCompatId())}\n" +
			"Update ueber die Weboberflaeche (System):\n" +
			$"- Nur das Hausmodell geaendert: {nodeId}.pkg unter \"Station Package\" (Geraet startet neu).\n" +
			"- Basis-Firmware geaendert: zuerst sensact_firmware.bin unter \"Over-the-air Firmware Update\" (Geraet startet neu;\n" +
			$"  ein Paket einer anderen Basis wird dabei verworfen -> Sicherheitsmodus), danach {nodeId}.pkg.\n");
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
		BuildStepRunner.Invoke(ctx, BuildStationPackage);
		BuildStepRunner.Invoke(ctx, BuildFirmware);
		BuildStepRunner.Invoke(ctx, CollectDeployFiles);
	}
}

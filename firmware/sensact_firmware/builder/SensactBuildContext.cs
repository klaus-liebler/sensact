using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using FirmwareBuilder.Common;
using FirmwareBuilder.Common.Esp32;

namespace Builder;

// Implementiert IBuildContextEsp32 auf Basis von AbstractBuildContext (geteilte Lib) -- ersetzt
// BoardContext.cs. Board-Identitaets-Properties (BoardUid/ChipId/BoardArchiveDir/WebAdminPassword/
// BoardSettings/BoardTypeName/BoardTypeVersion/Hostname) lesen bei JEDEM Zugriff frisch von der
// Platte (kein gecachtes Feld), damit ein PrepareContextWithRealHardware/
// PrepareContextWithCommandLineArguments-Aufruf FRUEHER in derselben Pipeline auch fuer spaeter
// aufgerufene Schritte sofort sichtbar ist.
public sealed class SensactBuildContext : AbstractBuildContext, IBuildContextEsp32
{
	// --- Sensact-eigene Pfade, NICHT Teil von AbstractBuildContext (andere generated/-Unterstruktur,
	// sensact_model, npm-Pakete, Web-Templates...). Diejenigen, die nur von dieser Klasse selbst
	// gebraucht werden (Backing fuer die Properties unten), sind privat; die von Program.cs direkt
	// referenzierten bleiben public static.
	private static readonly string _buildDir = BuildContextPaths.BuildDir(RootDirStatic);
	private static readonly string _partitionsCsvPath = Path.Combine(RootDirStatic, "partitions.csv");

	// Projektlokales Ausgabeverzeichnis fuer generierte Dateien (wsprotocol_cpp/_ts,
	// runtimeconfig_cpp/_ts, cmake/, web/ -- alles, was builder selbst erzeugt). Lag bis August 2026
	// unter dem repo-fremden "c:\repos\generated" (gemeinsames Scratch-Verzeichnis, unveraendert aus
	// gulpfile.ts uebernommen, GENERATED_ROOT) -- umgestellt auf RootDir/generated, weil das externe
	// Verzeichnis nirgends versioniert war und Konsumenten (npm-packages, CMake) nur ueber einen
	// impliziten, undokumentierten Pfad-Vertrag zusammenfanden. Cross-Repo-Konsumenten
	// (npm-packages/*, s. deren package.json "file:"-Abhaengigkeiten) zeigen seitdem per relativem
	// Sibling-Pfad hierher, statt auf ein drittes, repo-loses Verzeichnis.
	//
	// Bewusst NICHT auf STM32s Konvention "alles flach direkt in Core/generated/" umgestellt: sensact
	// gruppiert nach Erzeugungszweck (wsprotocol_cpp/, wsprotocol_ts/, runtimeconfig_cpp/, ...) unter
	// einem gemeinsamen GeneratedRoot, waehrend STM32 gar kein gemeinsames Wurzelverzeichnis hat
	// (Core/generated/ und web/generated/ sind bei STM32 voneinander unabhaengig). Deshalb bleibt
	// GeneratedRoot sensact-spezifisch statt Teil von AbstractBuildContext.
	public static readonly string GeneratedRoot = Path.Combine(RootDirStatic, "generated");

	// Muss NACH GeneratedRoot deklariert sein -- C# initialisiert statische Felder in Deklarationsreihenfolge.
	private static readonly string _generatedWebDir = Path.Combine(GeneratedRoot, "web");

	public static readonly string GeneratedWsProtocolCppDir = Path.Combine(GeneratedRoot, "wsprotocol_cpp");

	public static readonly string GeneratedWsProtocolTsDir = Path.Combine(GeneratedRoot, "wsprotocol_ts");

	public static readonly string GeneratedRuntimeConfigCppDir = Path.Combine(GeneratedRoot, "runtimeconfig_cpp");

	public static readonly string GeneratedRuntimeConfigTsDir = Path.Combine(GeneratedRoot, "runtimeconfig_ts");

	public static readonly string GeneratedCMakeDir = Path.Combine(GeneratedRoot, "cmake");

	// Sensact-spezifisch: pro-Node generierte Artefakte aus configware (s. Plan-Doc "configware wird
	// voll in den Orchestrator verschmolzen"), unveraendert aus gulpfile.ts uebernommen
	// (SENSACT_COMPONENT_GENERATED_PATH). Unter GeneratedRoot, weil sensact-applicationmodel/CMakeLists.txt
	// es ueber "${GENERATED_DIR}/sensact_model" einbindet -- derselbe GENERATED_DIR wie fuer alle
	// anderen generierten Dateien.
	public static readonly string SensactModelGeneratedDir = Path.Combine(GeneratedRoot, "sensact_model");

	// TS-Template-Dateien fuer die Sensact-Codegenerierung (s. sensact_code_generator.ts:
	// P_WEB/"templates"), Phases/GenerateSensactFiles.cs.
	public static readonly string WebTemplatesDir = Path.Combine(RootDirStatic, "web", "templates");

	public SensactBuildContext(string[] args) : base(args)
	{
	}

	protected override IBuilderAppSettings Settings => BuilderOptions.Current;

	public string NpmPackagesDir => BuilderOptions.Current.NpmPackagesDir;
	public string WebmanagerBestBinaryBuffersSchemaDir => BuilderOptions.Current.WebmanagerBestBinaryBuffersSchemaDir;

	public override string WebGeneratedDir => _generatedWebDir;
	public override string FirmwareGeneratedDir => GeneratedRoot;
	public string BuildDir => _buildDir;
	public string PartitionsCsvPath => _partitionsCsvPath;

	public string IdfPath
	{
		get
		{
			var idfPath = Environment.GetEnvironmentVariable("IDF_PATH");
			if (string.IsNullOrWhiteSpace(idfPath))
			{
				throw new InvalidOperationException("IDF_PATH ist nicht gesetzt -- muss auf die ESP-IDF-Installationswurzel zeigen (enthaelt export.bat).");
			}
			if (!File.Exists(Path.Combine(idfPath, "export.bat")))
			{
				throw new InvalidOperationException($"IDF_PATH ist auf \"{idfPath}\" gesetzt, aber \"export.bat\" existiert dort nicht -- IDF_PATH korrigieren.");
			}
			return idfPath;
		}
	}

	// Liest die gecachte board_info.json aus dem Repo-Root frisch (kein gecachtes Feld). Nutzt den
	// geteilten BoardRecord-Typ (FirmwareBuilder.Common) -- gleiches JSON-Schema wie STM32s
	// board_info.json (s. BoardRecord-Klassenkommentar dort).
	public BoardRecord Board
	{
		get
		{
			if (!File.Exists(BoardInfoJsonPath))
			{
				throw new InvalidOperationException(
					$"Keine board_info.json unter {BoardInfoJsonPath} -- noch kein Board jemals verbunden gewesen " +
					"(fuer eine Live-Board-Abfrage s. PrepareContextWithRealHardware, fuer Offline-Arbeit s. PrepareContextWithCommandLineArguments).");
			}
			return JsonSerializer.Deserialize<BoardRecord>(File.ReadAllText(BoardInfoJsonPath))
				?? throw new InvalidOperationException($"{BoardInfoJsonPath} konnte nicht gelesen werden.");
		}
	}

	// Liest board_info.json direkt aus dem Board-Archiv (nicht aus dem Repo-Root-Cache) -- fuer
	// [BuildStep]-Methoden, die nur IBuildContextEsp32 (nicht die konkrete SensactBuildContext-
	// Klasse) entgegennehmen und trotzdem an Felder wollen, die nicht Teil des Interfaces sind
	// (BoardVersion, Zeitstempel). Board-Archiv-Kopie und Repo-Root-Cache werden von EnsureBoard
	// immer synchron gehalten.
	public static BoardRecord LoadBoardRecord(string boardArchiveDir)
	{
		var path = Path.Combine(boardArchiveDir, "board_info.json");
		if (!File.Exists(path))
		{
			throw new InvalidOperationException($"Keine board_info.json im Board-Archiv gefunden ({path}).");
		}
		return JsonSerializer.Deserialize<BoardRecord>(File.ReadAllText(path))
			?? throw new InvalidOperationException($"{path} konnte nicht gelesen werden.");
	}

	// Privat statt oeffentliches IBuildContextEsp32-Property (das gibt es nicht mehr) -- nur noch
	// intern gebraucht, um BoardUid/ChipId aus dem persistierten board_info.json-Feld "mac"
	// aufzuloesen. Aufrufer verwenden stattdessen ctx.ChipId.ToEsp32Mac48().
	private long BoardMac => Board.Mac ?? throw new InvalidOperationException($"{BoardInfoJsonPath}: Feld \"mac\" fehlt (unerwartet fuer ein sensact-Board-Archiv).");

	public override ChipId ChipId => ChipId.FromEsp32Mac48(BoardMac);

	// Identisches Konzept wie bei STM32 (dort: ChipUid) -- der Unterordnername unter BoardsDir, ueber
	// den BoardArchiveContext.BoardArchiveDir (geteilte Lib) genau wie bei STM32 aufgeloest wird.
	public override string BoardUid => BoardPaths.BoardDirectoryName(BoardMac);
	public override string BoardArchiveDir => BoardArchiveContext.BoardArchiveDir(BuilderOptions.Current.BoardStorage, BoardUid);

	public override string? WebAdminPassword => Board.WebAdminPassword;
	public override IReadOnlyDictionary<string, string> BoardSettings => Board.BoardSettings ?? [];
	public bool FlashEncryptionKeyBurnedAndActivated => Board.FlashEncryptionKeyBurnedAndActivated ?? false;

	// node_id (aus sensact.json) bestimmt indirekt, welche hal_sensact*.hh eingebunden wird -- DAS
	// ist der eigentliche Board-Typ (physische Hardware-Familie). NICHT dasselbe wie node_id selbst
	// (Software-Funktionsumfang, s. ISensactContext.NodeId) -- s. NodeIdBoardTypeCatalog.
	public override string BoardTypeName => NodeIdBoardTypeCatalog.Resolve(SensactContextLoader.Load(BoardArchiveDir).NodeId).BoardTypeName;

	public override string BoardTypeVersion => NodeIdBoardTypeCatalog.Resolve(SensactContextLoader.Load(BoardArchiveDir).NodeId).BoardTypeVersion;

	// BoardSettingsKeys.OverrideHostname ist der bereits bestehende, generische BoardSettings-
	// Schluessel (s. IBuildContext.BoardSettings) -- kein zusaetzliches sensact-eigenes Feld noetig.
	// Ohne explizites Override faellt sensact auf die NodeId zurueck (ersetzt das vormalige eigene
	// ISensactContext.BoardName, das intern bereits genau dasselbe tat).
	public override string Hostname => BoardSettings.GetValueOrDefault(BoardSettingsKeys.OverrideHostname) is { Length: > 0 } name
		? name
		: SensactContextLoader.Load(BoardArchiveDir).NodeId;

	// --- Board-Archiv vorbereiten (genutzt von PrepareContextWithRealHardware/
	// PrepareContextWithCommandLineArguments in Program.cs) ---

	// Default-Feldwerte fuer ein NEU angelegtes Board -- von Esp32BoardProvisioningService.
	// PrepareContextWithRealHardware (geteilte Lib) via CreateDefaultRecord-Hook aufgerufen, ebenso
	// vom Offline-Pendant EnsureBoardForOfflineNodeId unten.
	public static BoardRecord NewDefaultBoardRecord(long mac)
	{
		var now = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
		return new BoardRecord(
			FirstConnectedAtEpoch: now,
			LastConnectedAtEpoch: now,
			WebAdminPassword: null,
			BoardSettings: [],
			Mac: mac,
			BoardVersion: 0,
			FlashEncryptionKeyBurnedAndActivated: false);
	}

	// Offline-Pendant: fuer main/hal/<NODE_ID>/, fuer das noch NIE ein physisches Board
	// angeschlossen wurde/dessen MAC unbekannt ist. Statt einer echten MAC wird eine aus der
	// NODE_ID deterministisch abgeleitete "MAC" verwendet (Bit 0x02 im ersten Byte gesetzt =
	// "locally administered" -- nach IEEE 802 niemals eine echte, von einem Hersteller vergebene
	// Adresse, macht das Board-Archiv also klar als synthetisch erkennbar). Gleiche NODE_ID ->
	// gleiche synthetische MAC -> gleiches Board-Archiv, damit Zertifikate etc. ueber mehrere
	// Offline-Laeufe hinweg erhalten bleiben.
	public static BoardRecord EnsureBoardForOfflineNodeId(string nodeId)
	{
		if (string.IsNullOrWhiteSpace(nodeId))
		{
			throw new ArgumentException("nodeId darf nicht leer sein.", nameof(nodeId));
		}

		var mac = ComputeOfflineMac(nodeId);
		var board = EnsureBoard(mac, updateLastConnected: false);
		var boardDir = BoardPaths.BoardSpecificPath(BuilderOptions.Current.BoardStorage.BoardsDir, mac);

		var sensactConfigPath = Path.Combine(boardDir, "sensact.json");
		if (!File.Exists(sensactConfigPath))
		{
			File.WriteAllText(sensactConfigPath, JsonSerializer.Serialize(new Dictionary<string, string>
			{
				["node_id"] = nodeId,
			}));
		}
		else
		{
			var existingNodeId = JsonSerializer.Deserialize<Dictionary<string, string>>(File.ReadAllText(sensactConfigPath))?.GetValueOrDefault("node_id");
			if (!string.Equals(existingNodeId, nodeId, StringComparison.Ordinal))
			{
				throw new InvalidOperationException(
					$"Offline-Board-Archiv {boardDir} enthaelt bereits eine sensact.json mit abweichender node_id (\"{existingNodeId}\" statt \"{nodeId}\").");
			}
		}

		return board;
	}

	// Kernlogik (lesen-oder-mit-Defaults-anlegen, optional last-connected auffrischen) ist
	// BoardStateStore.EnsureBoard (geteilte Lib) -- hier bleibt nur sensact-Spezifisches: die
	// Verzeichnisnamens-Konvention (BoardPaths), die Default-Feldwerte, und der Repo-Root-Cache.
	private static BoardRecord EnsureBoard(long mac, bool updateLastConnected)
	{
		var boardsDir = BuilderOptions.Current.BoardStorage.BoardsDir;
		var boardInfoJsonPath = BoardPaths.BoardSpecificPath(boardsDir, mac, "board_info.json");

		var board = BoardStateStore.EnsureBoard(boardInfoJsonPath, updateLastConnected, _ => NewDefaultBoardRecord(mac));

		// Haelt den Repo-Root-Cache aktuell, falls der board-spezifische Stand (z.B. von Hand
		// editierte board_settings, oder gerade aktualisiertes last_connected_dt) neuer ist als der Cache.
		// BoardInfoJsonPathStatic ist von AbstractBuildContext geerbt (protected static) -- selber
		// Pfad, den die Instanz-Property BoardInfoJsonPath liefert, hier aber aus statischem Kontext
		// zugreifbar (EnsureBoard/EnsureBoardForOfflineNodeId laufen schon VOR einer fertigen
		// SensactBuildContext-Instanz, s. PrepareContextWithRealHardware/PrepareContextWithCommandLineArguments).
		File.Copy(boardInfoJsonPath, BoardInfoJsonPathStatic, overwrite: true);

		return board;
	}

	private static long ComputeOfflineMac(string nodeId)
	{
		var hash = SHA256.HashData(Encoding.UTF8.GetBytes(nodeId));
		long mac = 0;
		for (var i = 0; i < 6; i++)
		{
			var b = i == 0 ? (byte)((hash[i] & 0xFE) | 0x02) : hash[i];
			mac = (mac << 8) | b;
		}
		return mac;
	}
}

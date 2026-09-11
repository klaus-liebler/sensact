# Board-Provisioning

Ablauf, um ein neues physisches Board mit dieser Firmware in Betrieb zu nehmen.

## 1. Board anschließen und auslesen

```
cd builder
dotnet run -- PrepareContextWithRealHardware
```

Liest Chip-Typ und MAC-Adresse vom per USB verbundenen Board aus (über `esptool`)
und legt/aktualisiert die board-spezifische `board_info.json` unter
`%USERPROFILE%\OneDrive - HSOS\esp32_boards\<mac-dir>\board_info.json` an. Diese
Datei wird außerdem als "aktuell verbundenes Board" in den Projekt-Root kopiert
(`board_info.json` neben dieser Datei). Dieser Schritt ist auch Teil von `Pipeline`
und muss nicht separat aufgerufen werden, außer man will nur den Board-Status
prüfen/aktualisieren.

`board_info.json`-Schema (`BoardRecord`) – generischer, projektübergreifender Teil
des Board-Zustands (Backing für `IBuildContext.WebAdminPassword`/`BoardSettings`,
s. `dotnet_libs/firmware_builder_common`). Epoch-Zeitstempel in Sekunden,
camelCase-Feldnamen:

```json
{
  "firstConnectedAtEpoch": 1789053000,
  "lastConnectedAtEpoch": 1789053835,
  "webAdminPassword": null,
  "boardSettings": {},
  "chipUid": null,
  "hostname": null,
  "mcuType": null,
  "boardTypeName": null,
  "boardTypeVersion": null,
  "mac": 57746244706752,
  "boardVersion": 0,
  "flashEncryptionKeyBurnedAndActivated": false
}
```

`boardSettings` ist ein flaches String-Dictionary (generische Overrides, projekt-
übergreifend). Wohlbekannte Schlüssel (`FirmwareBuilder.Common.BoardSettingsKeys`):
`overrideHostname`, `overrideWifiApSsid`, `lastDebugProbeId`.

## 2. Node-ID / Board-Rolle festlegen

Nicht `board_info.json`, sondern das board-spezifische `sensact.json` (s. Abschnitt 3) legt per
`node_id` fest, welches Unterverzeichnis unter `main/hal/<NODE_ID>/` zur Build-Zeit eingebunden
wird (`main/CMakeLists.txt`: `INCLUDE_DIRS "hal/${NODE_ID}"`). `boardVersion` ist rein
beschreibend und beeinflusst die `NODE_ID` nicht. `board_name` (Anzeige, Default-Web-Passwort)
lebt in `sensact.json`, s. Abschnitt 3 – sensact-spezifisch, nicht Teil des
projektübergreifenden `board_info.json`-Vertrags.

**`main/hal/<NODE_ID>/` wird von Hand gepflegt, es gibt keinen Generator dafür:**
das Verzeichnis enthält genau eine Datei, `board_init.hh`, die eine Klasse
`sensact::BoardInit : IBoardInit` (s. `main/hal/board_init_interface.hh`)
definiert. Sie legt fest, welche `iHAL`-Implementierung (`hal_sensact*.hh`)
verwendet wird, wie die `AbstractBusmaster`-Instanzen (I2C-Busmaster,
GPIO-Busmaster, …) mit der physischen Hardware verdrahtet sind, und welche
`iHost`-Instanzen (`cApplicationHost`, `cGatewayHost`, `cIoHost`) für diesen Node
aktiv sind. Details siehe [architecture.md](architecture.md#board-init).

Existiert für die gewünschte `NODE_ID` noch kein solches Verzeichnis, muss es manuell
neu angelegt werden (bestehendes `main/hal/<ähnliches-Board>/board_init.hh` als
Vorlage nehmen). Zusätzlich muss `builder/NodeIdBoardTypeCatalog.cs` einen Eintrag
für die neue `NODE_ID` bekommen (welche `hal_sensact*.hh`-Familie dahintersteckt).

## 3. Sensact-spezifisches `sensact.json`

Zusätzlich zu `board_info.json` liest der Builder (`ISensactContext`/`SensactContextLoader.cs`)
ein board-spezifisches `sensact.json` (`<boardsDir>\<mac-dir>\sensact.json`):

```json
{
  "node_id": "SNSCT_L3_WORK_HS",
  "board_settings": { "web": {}, "firmware": {} }
}
```

`node_id` wird als HAL-Include-Verzeichnisname *und* (sofern nicht per `board_settings.overrideHostname`
überschrieben) als Hostname verwendet. `board_settings.web`/`board_settings.firmware` sind sensact-
spezifische Key-Value-Overrides (App-/Web-Defines) – nicht Teil des projektübergreifenden
`board_info.json`-Vertrags. Muss beim erstmaligen Anlegen eines Boards von Hand angelegt werden.

## 4. Generieren, bauen, flashen

```
cd builder
dotnet run -- Pipeline --model Sattlerstrasse16
```

Führt nacheinander Hausmodell-Codegenerierung, ws-protocol-/Runtimeconfig-Generierung,
Zertifikatserzeugung (falls nötig), Web-Build und ESP-IDF-Build/Flash aus (vollständige
Phasenliste s. [build-process.md](build-process.md)). Muss aus einer echten
`cmd.exe`/PowerShell-Umgebung laufen, nicht aus git-bash (s. Troubleshooting in
[build-process.md](build-process.md)).

## 5. Zertifikate & Flash-Encryption

- Board-Zertifikat (`esp32.pem.crt`/`esp32.pem.key`) wird beim ersten
  `GenerateCertificates`-Lauf automatisch erzeugt (selbstsigniert, signiert von
  der Root-CA unter `OneDrive - HSOS\certificates`), falls noch nicht vorhanden.
- Ein Flash-Encryption-Key wird ebenfalls lazy erzeugt, falls noch nicht vorhanden.
  Der verschlüsselte Flash-Pfad selbst ist aber bewusst **nicht implementiert**:
  `FlashFirmware` bricht mit einer klaren Fehlermeldung ab, falls
  `flashEncryptionKeyBurnedAndActivated` im Board-Archiv gesetzt ist. Geflasht
  wird ausschließlich unverschlüsselt.

## 6. Build ohne physisches Board (offline)

Um für eine beliebige `NODE_ID` (z.B. `SNSCT_NODE_SIDEDOOR`) einen kompletten Build
durchzuführen, ohne dass das Board angeschlossen ist und ohne dessen echte MAC zu
kennen, ruft `PipelineOffline --nodeId <NODE_ID>` intern zuerst
`PrepareContextWithCommandLineArguments` auf – wählt ein synthetisches Board-Archiv
aus (eine aus der `NODE_ID` deterministisch abgeleitete, als "locally administered"
markierte Fake-MAC – niemals eine echte Hersteller-MAC, s.
`SensactBuildContext.EnsureBoardForOfflineNodeId`). Danach laufen die übrigen
Schritte wie gewohnt:

```
dotnet run --project builder -- PipelineOffline --nodeId SNSCT_NODE_SIDEDOOR --model Sattlerstrasse16
# oder einzeln:
dotnet run --project builder -- PrepareContextWithCommandLineArguments --nodeId SNSCT_NODE_SIDEDOOR
dotnet run --project builder -- PipelineOffline --model Sattlerstrasse16
```

Legt bei Bedarf ein neues Board-Archiv unter `<boardsDir>\<fake-mac-dir>\` mit
Default-`board_info.json` und passender `sensact.json` an (inkl. eigenem Zertifikat,
s. Abschnitt 5) und überschreibt den Repo-Root-Cache (`board_info.json`), genau wie
`PrepareContextWithRealHardware` es für ein echtes Board täte. Gleiche `NODE_ID` ->
gleiche Fake-MAC -> gleiches Archiv, das Zertifikat wird also nicht bei jedem Lauf
neu erzeugt. Flashen (`FlashFirmware`) ergibt für ein solches Board naturgemäß
keinen Sinn und ist nicht Teil von `PipelineOffline`.

## Offene Punkte

- Es gibt keinen automatisierten Weg, ein komplett neues `main/hal/<NODE_ID>/`-
  Verzeichnis aus einer Vorlage zu erzeugen – das ist reine Handarbeit.

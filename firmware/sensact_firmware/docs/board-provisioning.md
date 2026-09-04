# Board-Provisioning

Ablauf, um ein neues physisches Board mit dieser Firmware in Betrieb zu nehmen.

## 1. Board anschließen und auslesen

```
cd builder
npm run u    # addOrUpdateConnectedBoard
```

Liest MAC-Adresse und Chip-Infos vom per USB verbundenen Board aus (eigene
serielle SLIP/ROM-Loader-Implementierung in `esp32.ts`, kein `esptool`-Aufruf) und
legt/aktualisiert die board-spezifische `board_info.json` unter
`%USERPROFILE%\OneDrive - HSOS\esp32_boards\<mac-dir>\board_info.json` an. Diese
Datei wird außerdem als "aktuell verbundenes Board" in den Projekt-Root kopiert
(`board_info.json` neben dieser Datei).

`board_info.json`-Schema (`SensactBoardRecord`) -- generischer, projektübergreifender Teil des
Board-Zustands (Backing für `IBuildContext.WebAdminPassword`/`BoardSettings`, s.
`dotnet_libs/firmware_builder_common`):

```json
{
  "mac": "...",
  "board_version": 10000,
  "board_settings": { "override_hostname": "...", "last_debug_probe_id": "..." },
  "first_connected_dt": "...",
  "last_connected_dt": "...",
  "web_admin_password": null,
  "flash_encryption_key_burned_and_activated": false
}
```

`board_settings` ist ein flaches String-Dictionary (generische Overrides, projektübergreifend).
Wohlbekannte Schlüssel: `"override_hostname"`, `"override_wifi_ap_ssid"`, `"last_debug_probe_id"`.

## 2. Node-ID / Board-Rolle festlegen

Nicht `board_info.json`, sondern das board-spezifische `sensact.json` (s. Abschnitt 3) legt per
`node_id` fest, welches Unterverzeichnis unter `main/hal/<NODE_ID>/` zur Build-Zeit eingebunden
wird (`main/CMakeLists.txt`: `INCLUDE_DIRS "hal/${NODE_ID}"`). `board_version` ist rein
beschreibend und beeinflusst die `NODE_ID` nicht. `board_name` (Anzeige, Default-Web-Passwort)
lebt jetzt ebenfalls in `sensact.json`, s. Abschnitt 3 -- sensact-spezifisch, nicht Teil des
projektübergreifenden `board_info.json`-Vertrags.

**Wichtig (bestätigt von Klaus, 2026-07-31): die drei Dateien pro Node-Verzeichnis
werden von Hand gepflegt, es gibt aktuell keinen Generator dafür:**

- `hal.inc` – welche `iHAL`-Implementierung (`hal_sensact*.hh`) für dieses Board
  verwendet wird.
- `station_config_hardware.inc` – Verdrahtung der `AbstractBusmaster`-Instanzen
  (I2C-Busmaster, GPIO-Busmaster, …) auf die physische Hardware.
- `station_config_hosts.inc` – welche `iHost`-Instanzen (`cApplicationHost`,
  `cGatewayHost`, `cIoHost`) für diesen Node aktiv sind.

Existiert für die gewünschte `NODE_ID` noch kein solches Verzeichnis, muss es manuell
neu angelegt werden (bestehendes `main/hal/<ähnliches-Board>/` als Vorlage nehmen).

## 3. Sensact-spezifisches `sensact.json`

Zusätzlich zu `board_info.json` liest der Builder (`ISensactContext`/`SensactContextLoader.cs`)
ein board-spezifisches `sensact.json` (`<boardsDir>\<mac-dir>\sensact.json`):

```json
{
  "node_id": "SNSCT_L3_WORK_HS",
  "board_name": "SENSACT_L3_WORK",
  "board_settings": { "web": {}, "firmware": {} }
}
```

`node_id` wird als HAL-Include-Verzeichnisname *und* (sofern nicht per `board_settings.override_hostname`
überschrieben) als Hostname verwendet. `board_name`/`board_settings` sind sensact-spezifisch (Anzeige,
Default-Web-Passwort-Fallback, App-/Web-Defines) -- nicht Teil des projektübergreifenden
`board_info.json`-Vertrags. Muss beim erstmaligen Anlegen eines Boards von Hand angelegt werden.

## 4. Generieren, bauen, flashen

```
npm run c     # buildForCurrent: createFiles + Web-Build + idf.py build
# oder die Vollkette inkl. Flash:
npm start     # addOrUpdateConnectedBoard -> buildForCurrent -> encrypt (falls nötig) -> flash
```

## 5. Zertifikate & Flash-Encryption

- Board-Zertifikat (`esp32.pem.crt`/`esp32.pem.key`) wird beim ersten `createFiles`-Lauf
  automatisch erzeugt (selbstsigniert, signiert von der Root-CA unter
  `OneDrive - HSOS\certificates`), falls noch nicht vorhanden.
- Flash-Encryption-Key wird ebenfalls lazy erzeugt, falls noch nicht vorhanden.
  Ob tatsächlich verschlüsselt geflasht wird, hängt von `DEFAULT_ENCRYPTION_MODE`
  (`gulpfile.ts`) und dem gespeicherten
  `flash_encryption_key_burned_and_activated`-Flag ab.

## 6. Build ohne physisches Board (offline)

Um für eine beliebige `NODE_ID` (z.B. `SNSCT_NODE_SIDEDOOR`) einen kompletten Build
durchzuführen, ohne dass das Board angeschlossen ist und ohne dessen echte MAC zu
kennen, ruft `PipelineOffline --nodeId <NODE_ID>` intern zuerst `PrepareContextWithCommandLineArguments`
auf (Nachfolger von `SelectOfflineBoard`) -- wählt ein synthetisches Board-Archiv aus (eine
aus der `NODE_ID` deterministisch abgeleitete, als "locally administered" markierte
Fake-MAC -- niemals eine echte Hersteller-MAC, s. `SensactBuildContext.EnsureBoardForOfflineNodeId`).
Danach laufen die uebrigen Schritte wie gewohnt:

```
dotnet run --project builder -- PipelineOffline --nodeId SNSCT_NODE_SIDEDOOR --model Sattlerstrasse16
# oder einzeln:
dotnet run --project builder -- PrepareContextWithCommandLineArguments --nodeId SNSCT_NODE_SIDEDOOR
dotnet run --project builder -- PipelineOffline --model Sattlerstrasse16
```

Legt bei Bedarf ein neues Board-Archiv unter `<boardsDir>\<fake-mac-dir>\` mit
Default-`board_info.json` und passender `sensact.json` an (inkl. eigenem Zertifikat,
s. Abschnitt 5) und überschreibt den Repo-Root-Cache (`board_info.json`), genau wie
`PrepareContextWithRealHardware` es für ein echtes Board täte. Gleiche `NODE_ID` -> gleiche Fake-MAC
-> gleiches Archiv, das Zertifikat wird also nicht bei jedem Lauf neu erzeugt. Flashen
(`FlashFirmware`) ergibt für ein solches Board naturgemäß keinen Sinn und ist nicht
Teil von `PipelineOffline`.

## Offene Punkte (siehe auch [plan_v2/01-entwicklerdokumentation-und-build.md](plan_v2/01-entwicklerdokumentation-und-build.md))

- Es gibt keinen automatisierten Weg, ein komplett neues `main/hal/<NODE_ID>/`-Verzeichnis
  aus einer Vorlage zu erzeugen – das ist reine Handarbeit und ein möglicher, aber
  aktuell nicht geplanter Verbesserungspunkt für die C#-Builder-Migration (Stufe 2).

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

`board_info.json`-Schema (`IBoardInfo`):

```json
{
  "mac": "...",
  "board_name": "SENSACT_L3_WORK",
  "board_version": 10000,
  "board_roles": "...",
  "board_settings": { "web": {}, "firmware": {} },
  "first_connected_dt": "...",
  "last_connected_dt": "...",
  "flash_encryption_key_burned_and_activated": false
}
```

## 2. Node-ID / Board-Rolle festlegen

`board_name`/`board_roles` bestimmen die `NODE_ID`, die wiederum bestimmt, welches
Unterverzeichnis unter `main/hal/<NODE_ID>/` zur Build-Zeit eingebunden wird
(`main/CMakeLists.txt`: `INCLUDE_DIRS "hal/${NODE_ID}"`).

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

Zusätzlich zu `board_info.json` liest `sensact_code_generator.ts` ein
board-spezifisches `sensact.json` (`<boardsDir>\<mac-dir>\sensact.json`) mit
mindestens dem Feld `node_id` – dieser Wert wird als HAL-Include-Verzeichnisname
*und* als Hostname verwendet. Muss beim erstmaligen Anlegen eines Boards ebenfalls
von Hand angelegt werden.

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

## Offene Punkte (siehe auch [plan_v2/01-entwicklerdokumentation-und-build.md](plan_v2/01-entwicklerdokumentation-und-build.md))

- Es gibt keinen automatisierten Weg, ein komplett neues `main/hal/<NODE_ID>/`-Verzeichnis
  aus einer Vorlage zu erzeugen – das ist reine Handarbeit und ein möglicher, aber
  aktuell nicht geplanter Verbesserungspunkt für die C#-Builder-Migration (Stufe 2).

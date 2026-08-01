# Stufe 3 – Wifimanager-Review + konfigurierbares AP-Fallback

**Status (2026-08-01): umgesetzt.** Ueber den urspruenglichen Scope dieser Stufe
hinaus wurde auf Nutzerwunsch gleichzeitig die komplette ws-protocol-Migration
(BestBinaryBuffers statt Flatbuffers) fuer alle 8 Namespaces mit echter Server-
Implementierung durchgezogen (wifimanager, scheduler, systeminfo, usersettings,
nodemaster, sensact, journal, fingerprint) -- s. eigener Abschnitt "ws-protocol-
Migration (ueber den urspruenglichen Scope hinaus)" am Ende dieser Datei fuer Details,
betroffene Dateien und offene Punkte (v.a. Firmware-Build-Verifikation, s.u.).

**Repo-Hinweis:** Der Server-Code liegt in `C:\repos\espidf-component-webmanager`
(eigenes Repo, Remote `klaus-liebler/espidf-component-webmanager`), der Client-Code in
`C:\repos\npm-packages\@klaus-liebler\web-components` (eigenes Repo, Remote
`klaus-liebler/npm-packages`). Beide waren in dieser Session (anders als die hier
urspruenglich vermerkte Annahme) tatsaechlich als Working Directories zugaenglich,
ebenso `C:\repos\npm-packages\@klaus-liebler\web-components-sensact` und
`sensact-base` (weitere, urspruenglich nicht erwaehnte Consumer-Pakete, s.u.).

## Ist-Zustand: WLAN-Statemachine (Server, `cpp/webmanager.hh`)

Die gesamte Logik steckt in einer einzigen Klasse `webmanager::M` (Singleton,
~1430 Zeilen, `cpp/webmanager.hh`), die gleichzeitig WLAN-State-Machine,
HTTP-Fileserver, OTA-Update, Login/Session-Verwaltung und den Wifimanager-Websocket-
Handler enthält (siehe auch Stufe 4 – das ist auch ein Architektur-Fund).

Zustände: `WorkingState::AP_STARTED` / `KEEP_CONNECTION`
(`webmanager_workingstate.inc`). Drei Timer (`tTimeout_us`, `tShutdownAp_us`,
`tReconnect_us`), die per `Supervise()` (alle 4s, eigener FreeRTOS-Task) gepollt
werden (`webmanager.hh:1401-1430`).

**Aktuelles Reconnect-/Fallback-Verhalten** (`wifi_event_handler`,
`webmanager.hh:273-337`, Konstanten in `webmanager_constants.hh`):
- Bei Verbindungsabbruch (`WIFI_EVENT_STA_DISCONNECTED`) wird ein Zähler
  `remainingAttempsToConnectAsSTA` runtergezählt.
- Beim Boot startet dieser Zähler bei `RECONNECTS_ON_STARTUP = 3` – nach 3
  gescheiterten Versuchen (~24s) fällt das Gerät zurück in den Access-Point-Modus.
- Nach dem **ersten erfolgreichen** Connect wird der Zähler auf
  `RECONNECTS_ON_OPERATION = UINT32_MAX` gesetzt (`webmanager.hh:307`) – das
  bedeutet: **sobald das Gerät einmal erfolgreich verbunden war, wird praktisch nie
  wieder ein Access Point geöffnet**, das Gerät versucht für immer (alle 8s,
  `RECONNECT_TIMEOUT_US`) erneut zu verbinden.
- Das ist **das Gegenteil** dessen, was gewünscht ist: aktuell gibt es keine
  Zeit-basierte Konfiguration, sondern zwei fest kodierte, inkonsistente
  Attempt-Zähler (3 vs. "unendlich"), die zudem Boot-Fall und Betriebs-Fall
  unterschiedlich behandeln, ohne dass das irgendwo konfigurierbar wäre.

## Gewünschte neue Funktionalität

Konfigurierbare Zeitdauer, nach der (bei durchgehend fehlgeschlagenen
Reconnect-Versuchen) ein eigener Access Point geöffnet wird, um Rekonfiguration zu
ermöglichen. Ein Sentinel-Wert (z.B. `INT64_MAX`, passend zur bereits existierenden
Konstante `FAR_FUTURE` in `webmanager_constants.hh:8`) soll bedeuten: **niemals**
einen Access Point öffnen, stattdessen für immer weitere Verbindungsversuche starten.

### Vorgeschlagenes Design

1. Attempt-Zähler-Modell (`remainingAttempsToConnectAsSTA`,
   `RECONNECTS_ON_STARTUP`/`RECONNECTS_ON_OPERATION`) durch ein
   **Deadline-Modell** ersetzen:
   - Neuer Zustand: `giveUpAt_us` (Zeitpunkt, ab dem – falls immer noch nicht
     verbunden – der AP geöffnet wird). Wird gesetzt, sobald die STA-Verbindung
     verloren geht bzw. beim Boot, falls von Anfang an keine Verbindung zustande
     kommt: `giveUpAt_us = now_us + WIFIMANAGER_AP_FALLBACK_TIMEOUT_US`.
   - Wird auf `FAR_FUTURE` zurückgesetzt, sobald `IP_EVENT_STA_GOT_IP` feuert
     (erfolgreich verbunden).
   - `Supervise()` bzw. der Disconnect-Handler prüfen `now_us > giveUpAt_us` statt
     eines Zählers – das vereinheitlicht Boot- und Betriebsverhalten unter einer
     einzigen, konfigurierbaren Zeitspanne statt zwei verschiedenen Konstanten.
   - Retry-Abstand (aktuell `RECONNECT_TIMEOUT_US = 8s`) bleibt eine **feste,
     nicht konfigurierbare** `constexpr` (Reconnect-Intervall ≠ Gesamt-Timeout bis
     AP-Fallback – nur Letzterer wird konfigurierbar, siehe "Entschieden" unten).
   - Sentinel: `WIFIMANAGER_AP_FALLBACK_TIMEOUT_US == INT64_MAX` (bzw. wiederverwendbar
     als `FAR_FUTURE`) ⇒ `giveUpAt_us` wird nie erreicht ⇒ Gerät versucht für immer,
     nie AP. Das deckt sich mit dem bestehenden `FAR_FUTURE`-Konzept, das im Code
     schon für "kein Timeout" verwendet wird – konsistente Wiederverwendung statt
     neuer Sonderwert.

2. **Wo die Konfiguration herkommt:** aktuell ist die Zeitkonstante in
   `webmanager_constants.hh` fest einkompiliert (`constexpr`). Für Konfigurierbarkeit
   pro Board muss sie zu einem Parameter von `M::Begin(...)` werden (die Methode nimmt
   bereits mehrere optionale Parameter entgegen, z.B. `wifiLogLevel`,
   `auth_username_param` – das neue Verhalten reiht sich dort ein:
   `time_t apFallbackTimeout_us = FAR_FUTURE`). `RECONNECT_TIMEOUT_US` bleibt
   unverändert eine `constexpr` in `webmanager_constants.hh` (siehe "Entschieden").
   Aufrufseitig (`sensact_firmware/main/main.cc`, wo `webmanager::M::GetSingleton()->Begin(...)`
   aufgerufen wird) wird der Wert aus dem bereits bestehenden Build-Define-Mechanismus
   bezogen (`board_settings.firmware` in `board_info.json` → `createObjectWithDefines()`
   im Builder (Stufe 1/2) → generiertes `runtimeconfig.hh`, Namespace `cfg::`) – das ist
   exakt der Mechanismus, der für alle anderen board-spezifischen Firmware-Einstellungen
   schon existiert, hier lediglich um einen neuen Key erweitert
   (`WIFIMANAGER_AP_FALLBACK_TIMEOUT_MS`).
3. Optionale, spätere Erweiterung (nicht Teil des MVP): exponentielles Backoff statt
   fixem Reconnect-Intervall; UI-Anzeige "verbleibende Zeit bis AP-Fallback" im
   Wifimanager-Web-Client.

## Konkrete Bugs/Inkonsistenzen, die im Zuge dieser Stufe mitgeprüft/behoben werden sollten

1. **Inkonsistente Argumentanzahl bei `CreateResponseWifiConnectDirect`**:
   - `sendWifiConnectionNotSuccessfulMessage()` (`webmanager.hh:157-166`) ruft
     `CreateResponseWifiConnectDirect(b, false, "", 0, 0, 0)` (bool + 4 weitere Werte).
   - `handleRequestWifiConnect()`s `negativeresponse`-Label (`webmanager.hh:587-594`)
     ruft dieselbe Funktion mit `(b, false, ssid, 0, 0, 0, 0)` (bool + 5 weitere Werte).
   - Das Flatbuffers-Schema `ResponseWifiConnect` hat die Felder
     `success, ssid, ip, netmask, gateway, rssi` (6 Felder,
     `ns01wifimanager.fbs:39-45`) – die beiden Call-Sites sind demnach
     unterschiedlich (und mindestens eine davon vermutlich falsch/inkonsistent).
     **Muss zuerst gegen die tatsächlich generierte Signatur
     (`ns01wifimanager_generated.h`, wird zur Build-Zeit erzeugt) geprüft und dann
     vereinheitlicht werden.**
2. **Fehlerhafter `Set-Cookie`-Header beim Login** (`handle_login_post`,
   `webmanager.hh:~1082-1090`): zwei Cookies werden in *einen* Aufruf von
   `httpd_resp_set_hdr(req, "Set-Cookie", cookie_header)` gepackt, wobei
   `cookie_header` selbst bereits den literalen Text `"Set-Cookie: "` und ein
   eingebettetes `\r\n` enthält. Das ist kein valides HTTP (ein Header-Wert darf
   keinen zweiten Header-Namen + Zeilenumbruch enthalten). Der Beweis, dass das ein
   echter Bug ist: der Dev-Proxy (`web/vite.config.mts`, aktuell in Bearbeitung im
   Arbeitsverzeichnis) muss den Client-seitigen Workaround
   `c.replace(/^Set-Cookie:\s*/i, "")` einbauen, weil sonst das Cookie nicht
   erkannt wird. **Fix:** `httpd_resp_set_hdr` zweimal aufrufen (einmal pro Cookie),
   oder – falls die HTTP-Server-API in ESP-IDF Mehrfach-Header nicht sauber
   unterstützt – nur ein Cookie mit beiden Infos kombiniert übertragen, statt zwei
   separate `Set-Cookie`-Zeilen ineinander zu verschachteln.
3. **Default-Zugangsdaten** `"admin"`/`"password"` als Fallback-Parameter von
   `Begin(...)` – prüfen, ob das in der Praxis immer überschrieben wird (Board-
   Provisioning) oder ob hier ein reales Risiko besteht, dass ein Board mit
   Standard-Login im Feld landet. Falls ja: in Stufe 1 (Board-Provisioning-Doku)
   und/oder Stufe 4 (Architektur-Backlog) verpflichtend machen, dass eigene
   Zugangsdaten gesetzt werden.
4. **`validate_credentials`** vergleicht Strings mit `==` (nicht
   konstant-zeitig) – Timing-Seitenkanal. Angesichts des Einsatzzwecks (lokales
   Access Point, physischer Zugriff nötig) niedrige Priorität, aber als Fund im
   Architektur-Backlog vermerken.

## Client-Seite

- Wifimanager-UI: `C:\repos\npm-packages\@klaus-liebler\web-components\typescript\screen_controller\wifimanager_controller.ts`
  (`WifimanagerController`) – zeigt STA-Status, AP-Konfiguration, Netzwerkliste,
  Connect/Disconnect-Dialoge. Transport über
  `.../typescript/screen_controller/app_controller.ts` (`AppController`,
  WebSocket mit Namespace-Präfix-Framing).
- **Es gibt aktuell keine automatische Reconnect-Logik im Client** – nur eine
  statische Fehler-Snackbar bei `onclose`/`onerror`. Das ist prinzipiell in Ordnung
  (Reconnect ist Firmware-Sache), aber es gibt auch **keine UI-Anzeige, dass/wann**
  das Gerät gerade in den AP-Fallback wechselt. Optionale Erweiterung (nicht MVP):
  die neue `ResponseNetworkInformation` könnte um ein Feld
  "verbleibende Zeit bis AP-Fallback" ergänzt werden, das der Client anzeigt.

## Vorgehen

- [x] Bug #1 (Argumentanzahl bei `CreateResponseWifiConnectDirect`) – **erledigt sich
      automatisch**: die komplette Flatbuffers-Funktion entfaellt durch die
      ws-protocol-Migration (s.u.), `ResponseWifiConnect::Payload` wird jetzt an
      genau einer Stelle je Call-Site befuellt (`webmanager.hh`,
      `sendWifiConnectionNotSuccessfulMessage`/`sendWifiConnectionSuccessfulMessage`/
      `handleRequestWifiConnect`), kein Uneinigkeits-Risiko mehr moeglich. Dabei
      wurde nebenbei auch der schon damals vermerkte Bug behoben, dass `ap.rssi` in
      `sendWifiConnectionSuccessfulMessage` zwar geholt, aber nie in die Response
      geschrieben wurde.
- [x] Bug #2 (Set-Cookie) behoben: `handle_login_post` ruft jetzt zweimal
      `httpd_resp_set_hdr(req, "Set-Cookie", ...)` (einmal pro Cookie) statt eines
      kombinierten, malformten Header-Werts. Client-seitiger Workaround in
      `web/vite.config.mts` (`c.replace(/^Set-Cookie:\s*/i, "")`) entfernt.
      **Noch offen:** echte End-to-End-Verifikation (Login-Flow gegen ein Board)
      steht aus, s. "Offene Punkte" unten.
- [x] Bug #3 (Default-Zugangsdaten): `Begin(...)` bekommt weiterhin `"admin"`/
      `"password"` als Fallback, aber `main.cc`s Aufruf reicht jetzt optional
      `cfg::WEBMANAGER_AUTH_USERNAME`/`cfg::WEBMANAGER_AUTH_PASSWORD` durch (per
      `#ifdef`-Praesenzpruefung gegen `runtimeconfig_defines.hh`, analog zum neuen
      AP-Fallback-Timeout-Mechanismus) – pro Board ueberschreibbar, kein Board setzt
      die Keys aktuell, also unveraendertes Verhalten bis explizit konfiguriert.
- [x] Bug #4 (nicht-konstantzeitiger Credential-Vergleich): `validate_credentials`
      nutzt jetzt `constant_time_equals` (Laengen-Check + XOR-Akkumulation ohne
      Early-Exit).
- [x] Deadline-Modell implementiert: `giveUpAt_us`/`apFallbackTimeout_us` ersetzen
      `remainingAttempsToConnectAsSTA`/`RECONNECTS_ON_STARTUP`/
      `RECONNECTS_ON_OPERATION` (aus `webmanager_constants.hh` entfernt). Neuer
      `Begin(...)`-Parameter `time_t apFallbackTimeout_us_param = FAR_FUTURE`.
      `giveUpAt_us` wird nur beim ERSTEN Disconnect einer Serie scharfgeschaltet
      (nicht bei jedem Retry neu), bei erfolgreichem Connect auf `FAR_FUTURE`
      zurueckgesetzt.
- [x] Konfigurationswert durch die Build-Kette durchgereicht: `board_settings.firmware`
      → (generischer, unveraenderter Mechanismus in `GenerateRuntimeConfig.cs`) →
      `runtimeconfig.hh`/`runtimeconfig_defines.hh` → `main.cc`s `Begin(...)`-Aufruf
      (`#ifdef __WIFIMANAGER_AP_FALLBACK_TIMEOUT_MS__` mit `FAR_FUTURE`-Fallback,
      d.h. kein Board-Verhalten aendert sich, bis ein Board den Key explizit setzt).
- [ ] Kurzer Testplan (manuell): **noch nicht durchgefuehrt**, da kein Firmware-Build
      moeglich war (s. "Offene Punkte" unten) – AP-Fallback nach konfigurierter Zeit
      auslösen (z.B. durch Router ausschalten), Sentinel-Wert testen (Gerät soll
      dauerhaft weiterprobieren, nie AP öffnen), sobald ein Build moeglich ist.
- [ ] Optional/Stretch: `webmanager::M` in kleinere Klassen aufteilen – weiterhin
      nicht Teil dieser Stufe, Eintrag fuer Stufe 4 (Architektur-Backlog) bleibt
      bestehen. wifimanager wird weiterhin inline in `M` behandelt (nicht als
      eigenstaendiges Plugin extrahiert), s. "ws-protocol-Migration" unten fuer die
      Begruendung.

## Offene Punkte (2026-08-01)

- **Kein Firmware-Build in dieser Session moeglich**: die ESP-IDF-Python-venv auf
  dieser Maschine ist kaputt (vorbestehendes, aus Stufe 2 bereits dokumentiertes
  Problem, nicht durch diese Aenderungen verursacht). Alle C++-Aenderungen (Server,
  beide Repos) sind dadurch **nicht compile-getestet** – Absicherung erfolgte
  ausschliesslich durch genaues Nachvollziehen der generierten
  `WsProtocol::<namespace>::*`-API und Feld-fuer-Feld-Diff gegen die bisherige
  Flatbuffers-Logik. Vor dem naechsten echten Flash-Vorgang: `idf.py build`
  (sobald die venv repariert ist) einmal vollstaendig durchlaufen lassen und alle
  Compile-Fehler beheben, bevor auf einem echten Board getestet wird.
- **TypeScript-Seite ist verifiziert**: `tsc --noEmit` in `web/` (deckt
  `web-components`, `web-components-sensact`, `sensact-base`,
  `usersettings_runtime` ueber die npm-`file:`-Abhaengigkeiten mit ab) laeuft
  fehlerfrei durch (Exit Code 0) – das prueft aber nur Typkorrektheit, nicht
  tatsaechliches Laufzeitverhalten gegen einen echten Server.
- **Kein End-to-End-Test gegen ein echtes Board** fuer keinen der 8 migrierten
  Namespaces (wifimanager, scheduler, systeminfo, usersettings, nodemaster,
  sensact, journal, fingerprint) – steht aus, sobald ein Firmware-Build wieder
  moeglich ist. Das in einer frueheren Session verbundene `SENSACT_L3_WORK`-Board
  (physisch weiterhin angeschlossen gewesen) eignet sich dafuer.

## Entschieden

- **Reconnect-Intervall bleibt eine feste Konstante** (nicht konfigurierbar) –
  nur der **AP-Fallback-Timeout wird konfigurierbar**. Das vereinfacht Punkt 2 im
  Design oben: `M::Begin(...)` bekommt nur einen neuen Parameter
  (`time_t apFallbackTimeout_us = FAR_FUTURE`), `RECONNECT_TIMEOUT_US` bleibt wie
  bisher eine `constexpr` in `webmanager_constants.hh`. Entsprechend gibt es auch nur
  **einen** neuen Build-Define-Key, nicht zwei (`WIFIMANAGER_AP_FALLBACK_TIMEOUT_MS`).
- **Sentinel-Kodierung**: numerischer Wert in Millisekunden im Board-Settings-JSON,
  `INT64_MAX`-Millisekunden-Äquivalent als Sentinel für "nie AP öffnen" – wie
  vorgeschlagen.

## ws-protocol-Migration (ueber den urspruenglichen Scope hinaus, 2026-08-01)

Auf expliziten Nutzerwunsch ("Jetzt Stufe 3! und dabei die neue Serialisierung des
Websockets einbinden!", spaeter praezisiert zu "kein Nebeneinander, keine
Kompatibilitaetsschicht") wurde diese Stufe genutzt, um Flatbuffers durch den neuen
`BestBinaryBuffers`-Serializer (Schema als annotierter C#-Code statt `.fbs`/JSON, s.
`C:\repos\dotnet_libs\best_binary_buffers`) zu ersetzen – nicht nur fuer wifimanager,
sondern fuer alle Namespaces mit echter Server-Implementierung.

### Scope

Von den 12 Wire-Namespaces haben **8 eine echte Server-Implementierung** und wurden
vollstaendig migriert (Server + Client, soweit ein Client existiert):
`wifimanager`, `scheduler`, `systeminfo`, `usersettings` (alle in
`espidf-component-webmanager/cpp/`), sowie `nodemaster`, `sensact`, `journal`,
`fingerprint` (alle in `sensact_firmware/main/`).

- `canmonitor`, `chatbot`: keine Server-Implementierung irgendwo – Schema existiert,
  aber nichts zu migrieren (canmonitor hat einen toten Client, s.u.).
- `functionblock`, `heaterexperiment`: echter TS-Client existiert
  (`Flowchart.ts`/`develop_cfc_controller.ts`, `heater_experiment_controller.ts`),
  aber keine Server-Implementierung im `sensact`-Monorepo – der Server lebt in einem
  anderen, hier nicht betrachteten Firmware-Projekt (das `web-components`-Paket wird
  projektuebergreifend genutzt). **Bewusst nicht migriert** – bleiben auf Flatbuffers,
  s. "Uebergangsregel" unten.

### Teil A: Einheitlicher Transport-Mechanismus (statt Parallelbetrieb)

- **Server** (`espidf-component-webmanager/cpp/webmanager_interfaces.hh`):
  `iWebmanagerCallback::WrapAndSendAsync(uint32_t, FlatBufferBuilder&)` komplett
  ersetzt durch `SendRawAsync(const uint8_t* data, size_t len)` (erwartet einen
  bereits vollstaendig ws-protocol-gerahmten Frame). `iWebmanagerPlugin::
  ProvideWebsocketMessage(...)` liest jetzt `uint16_t namespaceId, uint16_t
  messageTypeId, const uint8_t *frame, size_t frameLen` statt `uint32_t ns, uint8_t*
  buf` (kein Vorab-Slicing durch den Dispatcher mehr). `webmanager_async_response.hh`s
  `AsyncResponse` hat entsprechend nur noch einen Konstruktor (roher Byte-Puffer,
  kein Namespace-Praefix-Voranstellen mehr). wifimanager bleibt bewusst **inline** in
  `M` (keine Extraktion in eine eigene `WifimanagerPlugin`-Klasse, da eng mit der
  WLAN-State-Machine verzahnt) und implementiert `ProvideWebsocketMessage` einfach
  mit derselben Signatur, direkt vom Dispatcher aufgerufen (kein Sonderfall im
  Dispatcher-Code selbst mehr, anders als zuvor).
- **Client** (`web-components/typescript/`): `IWebsocketMessageListener`/
  `IAppManagement` in `utils/interfaces.ts` ersetzt durch `IMessageListener.
  OnMessage(namespaceId, messageTypeId, view: DataView)` und `IAppManagement.
  RegisterNamespace(...)`/`SendFrame(namespaceId, bytes, maxLockingTimeMs?)`.
  `AppController` kennt kein Flatbuffers-Konzept mehr, hat nur noch eine Map/ein
  Interface/eine Sende-Methode, haengt selbst keinen Praefix mehr an (jeder Aufrufer
  liefert einen fertigen Frame).
- **Uebergangsregel fuer functionblock/heaterexperiment** (bewusst NICHT migriert):
  kein Widerspruch zu "keine Kompatibilitaetsschicht" – `AppController` bleibt bei
  genau einem Mechanismus, die 2 Controller (`Flowchart.ts`/
  `develop_cfc_controller.ts`, `heater_experiment_controller.ts`) adaptieren sich
  selbst mechanisch (eigenes Slicen/`ByteBuffer`-Wrap in ihrem `OnMessage`, eigenes
  Voranstellen des alten 4-Byte-uint32-Praefix vor `SendFrame`) – ihre Flatbuffers-
  Payload-Logik selbst bleibt unangetastet.

### Teil B: Migrierte Namespaces (Server + Client)

Gemeinsames Muster: `#include "flatbuffers_cpp/nsNN..._generated.h"` →
`#include "wsprotocol_cpp/ws_protocol.hh"`; `flatbuffers::GetRoot<...RequestWrapper>`/
`request_type()`-Switch → `if (namespaceId != WsProtocol::<ns>::NAMESPACE_ID) return
NOT_FOR_ME;` + `switch (messageTypeId)` gegen generierte `TYPE_ID`-Konstanten;
`CreateResponseWrapper(...).Union()` + `WrapAndSendAsync` → `WsProtocol::<ns>::
ResponseXxx::Payload` befuellen + `Encode(...)` in einen Stack-/`static`-Scratch-Puffer
+ `SendRawAsync(...)`. `requestId` wird aus dem jeweils dekodierten Request
uebernommen und in der Response zurueckgegeben (bei asynchron gepushten Notifications
– wifimanager `ResponseWifiConnect`, sensact `NotifyStatus`, fingerprint
`NotifyFingerDetected`/`NotifyEnrollNewFinger` – ueber ein gemerktes Member-Feld bzw.
`requestId=0` fuer echte Server-Pushes ohne zugehoerigen Request).

- **wifimanager**: `webmanager.hh` (inline, s.o.). `AccessPoint`-Liste jetzt ueber
  `AppendResponseNetworkInformationAccesspointsAccessPointElement` in einen Scratch-
  Puffer statt Flatbuffers-Vector. Nebenbei behobener Bug: `ap.rssi` wird jetzt
  tatsaechlich in `ResponseWifiConnect` geschrieben (vorher geholt, aber verworfen).
- **systeminfo**: `cpp/webmanager_plugins/systeminfo_plugin.hh`. `Mac6`/`PartitionInfo`
  ueber generierte Struct-/Class-Encode-Funktionen, `partitions`-Liste ueber
  `AppendResponseSystemDataPartitionsPartitionInfoElement`.
- **usersettings**: `cpp/webmanager_plugins/usersettings_plugin.hh`. Design-Aenderung
  im Schema selbst (bewusst, s. `ws-protocol/usersettings.cs`-Kommentar): statt einer
  Flatbuffers-Union `Setting{Integer,Enum,Boolean,String}` jetzt 4 eigenstaendige
  getaggte Wrapper-Klassen (`IntegerSettingWrapper` etc.), decodiert per
  `DecodeRequestSetUserSettingsSettingsElements`-Visitor mit `if constexpr`-
  Typunterscheidung. `usersettings::Setting`-Enum (Flatbuffers) durch neues,
  protokoll-unabhaengiges `SettingKind`-Enum in `usersettings_plugin.hh` ersetzt;
  `usersettings/nvs_accessor.hh.inc` (generierte, aber nicht durch `builder_cs`
  erzeugte Konfigurationsdatei unter `C:\repos\generated\usersettings\`) direkt
  angepasst. Client: `usersettings_runtime`-Paket (separates npm-Paket,
  `ConfigItemRT.WriteToFlatbufferBufferAndReturnSettingWrapperOffset`/
  `ReadFlatbuffersObjectAndSetValueInDom` → `BuildSettingsElement`/
  `ReadSettingsElementAndSetValueInDom`) mitmigriert.
- **scheduler**: `cpp/scheduler.hh` + `cpp/scheduler_timers.hh`. Polymorphe
  `Schedule`-Varianten (`Predefined`/`OneWeekIn15Minutes`/`SunRandom`) jetzt ueber
  `[BinaryUnion] IScheduleVariant` statt Flatbuffers-Union `uSchedule`. `aTimer`
  bekommt neue virtuelle Methoden `GetScheduleType()`/`EncodeScheduleVariant(...)`
  statt `FillListOfResponseSchedulerListItems`/`CreateFlatbufferScheduleOffset`; die
  5 Predefined-Singletons (`ALWAYS`/`NEVER`/...) teilen sich jetzt eine gemeinsame
  Basisklasse `aPredefinedTimer` (kleine, durch die Migration motivierte
  Konsolidierung). **NVS-Format wechselt ersatzlos** (Nutzer-Entscheidung: keine
  bestehenden Zeitplaene vorhanden, kein Migrations-/Kompat-Code fuer alte
  Flatbuffers-NVS-Blobs). `iScheduler::FillFlatbufferWithAvailableNames` →
  `FillAvailableScheduleNames(std::vector<std::string>&)` (protokoll-unabhaengig,
  betrifft auch den Aufrufer in `fingerprint_webmanager_sensact_adapter.hh`).
- **nodemaster**: `main/nodemaster.hh`. Kein TS-Consumer gefunden (in
  `web-components/typescript/`; s. aber "Zusaetzlich entdeckte Consumer" unten –
  `web/nodemaster_controller.ts` existiert doch und wurde ebenfalls migriert).
- **sensact**: `main/node_applicationhost.hh`/`.cc`. `NotifyStatus` (asynchroner
  Push aus `PublishApplicationStatus()`) nutzt `requestId=0`. Kein TS-Consumer in
  `web-components/typescript/` gefunden (s. aber "Zusaetzlich entdeckte Consumer" –
  `web-components-sensact/sensact_controller.ts` existiert und wurde migriert).
- **journal**: `main/journal_plugin.hh`. Singleton-Logging-Sink (`LogJournal`/
  `ResetJournal`, aufgerufen aus `main/logger.hh`/`nodemaster.hh`) bleibt
  namespace-agnostisch unangetastet, nur `ProvideWebsocketMessage` migriert.
- **fingerprint**: `main/fingerprint/fingerprint_webmanager_sensact_adapter.hh`.
  Komplexester Namespace (10 Request-/12 Response-Typen). Board-bedingt: nur auf
  `SNSCT_NODE_SIDEDOOR` tatsaechlich instanziiert.

### Zusaetzlich entdeckte Consumer (nicht im urspruenglichen TS-Consumer-Survey erfasst)

Eine erste Recherche hatte nur `web-components/typescript/` durchsucht und dabei
uebersehen, dass `nodemaster`/`sensact`/`canmonitor` **doch** TS-Consumer haben – nur
in anderen Paketen: `web/nodemaster_controller.ts` (im Firmware-Repo selbst),
`web-components-sensact/sensact_controller.ts` + `sensactapps_base.ts` (separates
Paket), `web-components-sensact/canmonitor_controller.ts` (canmonitor hat also einen
Client, obwohl es nie einen Server gab – der Client war/ist daher schon immer
funktionslos, aber musste trotzdem migriert werden, damit er weiter kompiliert).
Gefunden erst durch `tsc --noEmit` in `web/` (schlug zunaechst mit ca. 15 Fehlern in
4 Dateien fehl) – **Lektion**: bei TS-Consumer-Recherchen kuenftig immer das ganze
Monorepo durchsuchen, nicht nur ein einzelnes Paket, und/oder `tsc --noEmit` als
Verifikationsschritt nicht erst am Ende, sondern schon waehrend der Migration laufen
lassen. Alle 4 Dateien wurden nachtraeglich auf die neue `wsprotocol_ts`-API
umgestellt (inkl. `sensact-base/can_message_utils.ts`,
`application_id_utils.ts`, `interfaces.ts` und der generierten Vorlagen
`web/templates/sendCommandImplementation.template.ts`/`sensactapps.template.ts` in
`builder_cs`), `tsc --noEmit` in `web/` laeuft jetzt fehlerfrei durch (Exit Code 0).

### Echter Bug gefunden und gefixt: Namespace-ID-Kollision

Die von `BestBinaryBuffers`' `IdMap` (alphabetisch bei Erstauftreten) automatisch
vergebenen neuen `NAMESPACE_ID`s kollidierten real mit den alten, **weiterhin aktiven**
Flatbuffers-`Namespace.Value`s der nicht migrierten Namespaces: neues `fingerprint`
(NAMESPACE_ID=3) kollidierte mit dem alten, weiterhin von `functionblock` benutzten
Wert 3. Da `AppController` nach der Migration nur noch EINE Dispatch-Map hat (per
Absicht, s.o.), haette das dazu gefuehrt, dass `FingerprintScreenController` und
`DevelopCFCController` (functionblock) sich gegenseitig Nachrichten "gestohlen"
haetten. Behoben durch manuelle Korrektur in `ws-protocol/ids.txt`
(`namespace fingerprint 3` → `namespace fingerprint 13`, einmalig vertretbar, da
diese ID nie ausgeliefert wurde) und Neu-Generierung via
`dotnet run -- GenerateWsProtocolFiles`. **Wichtig fuer zukuenftige Migrationen**:
vor jeder weiteren Namespace-Migration pruefen, ob die neue `NAMESPACE_ID` mit einem
noch aktiven alten Flatbuffers-`Namespace.Value` kollidiert (aktuell noch aktiv:
functionblock=3, heaterexperiment=4 im alten Schema).

### Betroffene Dateien (Gesamtuebersicht)

**`espidf-component-webmanager`**: `cpp/webmanager_interfaces.hh`,
`cpp/webmanager_async_response.hh`, `cpp/webmanager.hh`, `cpp/webmanager_constants.hh`,
`cpp/webmanager_plugins/systeminfo_plugin.hh`,
`cpp/webmanager_plugins/usersettings_plugin.hh`, `cpp/scheduler.hh`,
`cpp/scheduler_timers.hh`.

**`sensact_firmware`**: `main/main.cc`, `main/nodemaster.hh`,
`main/node_applicationhost.hh`/`.cc`, `main/journal_plugin.hh`,
`main/fingerprint/fingerprint_webmanager_sensact_adapter.hh`, `web/vite.config.mts`,
`web/nodemaster_controller.ts`, `web/templates/sendCommandImplementation.template.ts`,
`web/templates/sensactapps.template.ts`, `builder_cs/Phases/GenerateWsProtocolFiles.cs`
(package.json-Generierung fuer `@generated/wsprotocol_ts` ergaenzt),
`builder_cs/Phases/GenerateSensactFiles.cs` (Abhaengigkeiten umgestellt),
`ws-protocol/ids.txt` (Kollisionsfix).

**`npm-packages`**: `@klaus-liebler/web-components/typescript/utils/interfaces.ts`,
`.../screen_controller/screen_controller.ts`, `.../app_controller.ts`,
`.../screen_controller/wifimanager_controller.ts`,
`.../screen_controller/systeminfo_controller.ts`,
`.../screen_controller/usersettings_controller.ts`,
`.../screen_controller/scheduler_controller.ts`,
`.../screen_controller/journal_controller.ts`,
`.../screen_controller/fingerprint_controller.ts`,
`.../flowchart/Flowchart.ts` + `.../screen_controller/develop_cfc_controller.ts`
(mechanisch angepasst, s.o.),
`.../screen_controller/heater_experiment_controller.ts` (mechanisch angepasst),
`@klaus-liebler/usersettings_runtime/index.ts`,
`@klaus-liebler/sensact-base/{can_message_utils,application_id_utils,interfaces}.ts`,
`@klaus-liebler/web-components-sensact/{canmonitor_controller,sensact_controller,
sensactapps_base}.ts`, sowie diverse `package.json`-Anpassungen (Abhaengigkeit von
`@generated/flatbuffers_ts` auf `@generated/wsprotocol_ts` umgestellt).

**Neu**: `C:\repos\generated\wsprotocol_ts\package.json` (macht `wsprotocol_ts` als
npm-Paket konsumierbar, analog zu `flatbuffers_ts`).

Kein Code-Change in `builder_cs/ModelGeneration/` noetig.

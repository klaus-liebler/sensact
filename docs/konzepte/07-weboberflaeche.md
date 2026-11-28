# 7 Weboberfläche

Jede ESP32-Station liefert eine eigene Weboberfläche aus. Sie dient dem Bedienen (Apps schalten), der
Diagnose (Ein-/Ausgänge, CAN, Log) und der Wartung (Paket, Firmware, WLAN).

![Startseite einer Station mit den Arbeitsbereichen](../bilder/web-startseite.png)

## Aufbau

```mermaid
flowchart TB
    subgraph Shell["Anwendungs-Shell @klaus-liebler/ui-shell (gemeinsam mit factory_in_a_box, labathome)"]
        TL["Titelleiste: Verbindung, Glocke (Warnungen/Fehler), Hell/Dunkel, Vollbild, Benutzer"]
        RB["Arbeitsbereich-Umschalter + Ribbon"]
        SL["Statusleiste: Verbindung, Zustand, Build und Modell"]
    end
    subgraph WS["Arbeitsbereiche (ScreenController)"]
        O["Übersicht"]
        S["Apps der Station<br/>(SensactController)"]
        N["Ein-/Ausgänge"]
        C["CAN-Monitor"]
        J["Journal"]
        T["Terminal"]
        Y["System"]
        W["WLAN"]
    end
    subgraph Gen["aus dem Hausmodell erzeugt"]
        Z["sensactapps.ts<br/>digitaler Zwilling: eine Bedien-Klasse je App"]
        I["App-Nummern und -Namen"]
    end
    Shell --> WS
    Z --> S
    I --> S
    WS <-- "WebSocket, BestBinaryBuffers" --> Dev["Station"]
```

| Arbeitsbereich | Gruppe | Wofür |
|---|---|---|
| Übersicht | – | Kacheln aller Arbeitsbereiche, Startseite |
| Apps der Station | Haus | Lampen, Rollläden, Signale bedienen; Status live |
| Fingerabdruck | Haus | nur an der Seitentür: Finger anlernen und verwalten |
| Ein-/Ausgänge | Diagnose | Eingänge der PCA9555 lesen, Ausgänge direkt schalten |
| CAN-Monitor | Diagnose | Nachrichten auf dem CAN-Bus mitlesen |
| Journal | Diagnose | Ereignisse der Station (überstehen Neustarts) |
| Terminal | Diagnose | Firmware-Log mit Filtern und Export, Konsole (`help`, `loglevel`, `restart` …) |
| System | Gerät | Partitionen, Kennwerte, **Stationspaket**, **Firmware-Update**, Neustart |
| WLAN | Gerät | Netze suchen und verbinden (nur Stationen mit WLAN) |

## Der digitale Zwilling

Die Liste der Apps einer Station wird **nicht** zur Laufzeit aus der Konfiguration zusammengesucht,
sondern vom Builder aus dem Hausmodell als TypeScript erzeugt (`sensactapps.ts`) und mit der Oberfläche
ins Stationspaket gepackt. Für jeden App-Typ gibt es eine Bedienklasse
(`npm-packages/@klaus-liebler/web-components-sensact/sensactapps_base.ts`): Rollladen mit Auf/Stopp/Ab
und Positionsbalken, Dimmer mit Schalter und Schieberegler, Schaltausgang mit Ein/Aus usw.

Warum erzeugt statt zur Laufzeit gebaut: Es gibt nur eine Abbildung „Modell → App-Objekte“ (im
Generator), keine zweite in TypeScript, die mit ihr auseinanderlaufen könnte. Weil Oberfläche und
Konfiguration gemeinsam im Paket reisen, passen sie immer zusammen.

![Apps eines Raums: Rollläden und Lampen](../bilder/web-apps-raum.png)

Gruppiert wird nach Technik (Typ-Präfix des Namens, angezeigt als „Dimmer“, „Rollläden und Markisen“ …) oder
nach Raum (Raum und Ebene aus dem Namen, angezeigt als „Wohnen · EG“); die Klartexte stehen in
`sensact-base/application_id_utils.ts`. Filter für die Ebenen K (Keller), E (Erdgeschoss), O (Obergeschoss),
D (Dachgeschoss), A (außen). „Nur diese Station“ zeigt nur die Apps dieser Station; ausgeschaltet lassen sich alle Apps des Hauses
schalten (die Kommandos gehen dann über CAN).

## Mobile Bedienung (`/mobil`)

Für das Telefon gibt es unter `/mobil` eine zweite Oberfläche ohne Anwendungs-Shell
(`firmware/sensact_firmware/web/mobile/`). Sie steckt im selben Bundle (die Station liefert für jeden Pfad
dieselbe Seite aus); `app.ts` startet bei diesem Pfad statt der Shell nur das Element `<sensact-mobile>`.
Datenquelle ist dieselbe erzeugte App-Liste wie bei „Apps der Station“: daraus werden Dimmer
(`SinglePwmApplication`), Schaltausgänge `POWIT_*` (`OnOffApplication`) und Rollläden `BLIND_*` genommen und
über Ebene und Raum im App-Namen nach Stockwerk und Raum geordnet. Gesendet wird über die erzeugten
`Send*Command`-Funktionen, Zustände kommen per `RequestStatus`/`NotifyStatus` wie gehabt.

Gestaltungsentscheidungen:

- **Stockwerkswechsel per Ein-Finger-Wischen** (natives CSS-Scroll-Snap) plus Stockwerksleiste unten.
  Mehrfinger-Gesten wurden verworfen: Zwei Finger lösen im mobilen Browser Pinch-Zoom aus, iOS-Safari
  reserviert Mehrfinger-Gesten teils für sich, sie sind schwer zu entdecken und einhändig kaum zu machen.
  Scroll-Snap übernimmt Richtungserkennung (waagrecht = Stockwerk, senkrecht = Seite scrollen) und
  Schwung vom Browser selbst und ist damit robuster als eine eigene Gestenerkennung.
- **Dimmer = ein runder Knopf**: Antippen schaltet (`TOGGLE`); nach kurzem Halten (280 ms) gehört der
  Finger dem Knopf, ein Schieber erscheint, und Hoch-/Runterziehen sendet `SET_VERTICAL_TARGET`
  (höchstens alle 120 ms, Endwert beim Loslassen). Die Haltezeit ist nötig, weil sonst jede senkrechte
  Bewegung auf einem Knopf dimmen würde und die Seite nicht mehr scrollbar wäre. Der Wert ändert sich
  relativ zur Startposition (kein Sprung beim Aufsetzen), 220 px Fingerweg = 0…100 %.
- Rollläden: dieselbe Geste auf dem Rollladensymbol stellt eine Teilöffnung ein
  (`SET_VERTICAL_TARGET` 16384…49151, an den Enden der Überfahrbereich).
- Shadow DOM, damit die globalen Stile der Shell (im selben Bundle) nicht hineinwirken.

Ohne Station und Builder lässt sich die Oberfläche nicht starten (sie braucht die erzeugten Pakete);
mit `npm run it` (Testserver) unter `https://localhost:5173/mobil`.

## Wartungsseite

Fest in der Firmware steckt nur eine kleine Wartungsseite. Sie wird ausgeliefert, solange kein gültiges
Stationspaket vorhanden ist (Sicherheitsmodus), und bietet Paket-Upload, Firmware-Upload und Log.

## Entwickeln ohne Gerät

`npm run wss` und `npm run dev:testserver` in `firmware/sensact_firmware/web/` starten einen Testserver,
der die Station mit allen Namensräumen nachbildet (inkl. simuliertem Firmware-Log), und den
Vite-Dev-Server unter `https://localhost:5173`. Die Bilder dieser Dokumentation stammen von dort.
Gegen ein echtes Gerät leitet `npm run dev` die WebSocket-Verbindung per Proxy weiter (eigenes Login).

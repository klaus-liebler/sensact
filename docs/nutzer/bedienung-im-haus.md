# Bedienung im Haus

Diese Seite beschreibt, wie sich Taster, Drehknöpfe und Lichter im Haus verhalten. Welcher Taster
welche Lampe schaltet, ist für jeden Raum festgelegt und kann vom Betreuer der Anlage geändert werden
(s. [Betrieb und Änderungen](betrieb-und-aenderungen.md)).

## Kurz, lang, doppelt

Die meisten Taster unterscheiden, **wie** man drückt:

| Bedienung | Dauer |
|---|---|
| **kurz** | loslassen vor 0,4 s |
| **lang (halten)** | länger als 0,4 s gedrückt halten |
| **doppelt** | zweimal kurz innerhalb von 0,6 s |

Ein Taster, der beim Einschalten der Anlage schon gedrückt ist oder klemmt, löst nichts aus, bis er
einmal losgelassen wurde.

## Licht

### Lichttaster (ein/aus)

Ein Druck schaltet um: an ↔ aus. Manche Taster schalten mehrere Lampen gleichzeitig.

### Dimmtaster (eine Taste)

| Bedienung | Wirkung |
|---|---|
| kurz | an (mit der zuletzt eingestellten Helligkeit) / aus |
| halten | dimmen, bis man loslässt; beim nächsten Halten in die andere Richtung |
| doppelt | sicher an |

Nach 5 Sekunden Pause beginnt das Dimmen wieder mit „heller“. Die Lampe merkt sich die Helligkeit für das
nächste Einschalten.

### Dimmtaster (zwei Tasten: heller/dunkler)

| Bedienung | Wirkung |
|---|---|
| kurz, eine der Tasten | an / aus |
| obere Taste halten | heller, bis zum Loslassen |
| untere Taste halten | dunkler, bis zum Loslassen; bei ausgeschalteter Lampe: schwach einschalten |

### Drehknopf (Wohnzimmer, Küche)

| Bedienung | Wirkung |
|---|---|
| drücken | an / aus |
| drehen | in die eine Richtung heller, in die andere dunkler |

Die **erste Rastung** nach einer Pause weckt den Knopf nur auf und ändert noch nichts. Direkt nach dem
Drücken wird kurz nicht gedreht, damit ein Wackeln beim Drücken die Helligkeit nicht verstellt.

### Automatisch aus

Viele Lampen schalten sich nach einer festen Zeit selbst aus, damit kein Licht tagelang brennt –
z. B. Kellerräume nach 30 Minuten, Flure nach einigen Stunden, das Außenlicht am Eingang nach
2 Minuten. Erneutes Bedienen startet die Zeit neu.

## Rollläden und Markisen

Rollladentaster haben eine Taste „auf“ und eine Taste „ab“.

| Bedienung | Wirkung |
|---|---|
| kurz | fährt ganz auf bzw. ganz zu; ein fahrender Rollladen **stoppt** |
| halten | fährt, solange man hält; beim Loslassen Stopp |
| kurz, wenn der Rollladen schon ganz oben/unten steht | fährt in die Gegenrichtung (Komfortfunktion, sobald die Anlage die Position kennt) |

Gut zu wissen:

- Nach einem Stromausfall kennt die Anlage die Position nicht. Die erste Fahrt in eine Endlage dauert dann
  etwas länger (zur Sicherheit über den ganzen Weg), danach ist die Position wieder bekannt.
- Beim Richtungswechsel steht der Motor kurz still – das schont Motor und Relais und ist Absicht.
- Einige Rollläden fahren **automatisch**: morgens zur Dämmerung auf, abends zur Dämmerung zu, jeden Tag
  ein paar Minuten unterschiedlich (wirkt bewohnt). Die Automatik lässt sich in der Weboberfläche ein-
  und ausschalten; der Zustand bleibt auch nach einem Stromausfall erhalten.

## Türklingel

- Die Klingel spielt tagsüber laut, nachts leise (Werkseinstellung: laut von 7 bis 22 Uhr).
- Zusätzlich blinken im **Wohnzimmer und in der Küche** die Eck-Leuchten der Bedienstellen dreimal weiß.

## Leuchten an den Bedienstellen (Wohnzimmer, Küche)

Die Bedienstellen im Wohnzimmer und in der Küche haben eine kleine Leuchte am Drehknopf und vier
Leuchten in den Ecken, die gedämpft durch die Abdeckung scheinen.

### Leuchte am Drehknopf – Rückmeldung

| Farbe | Bedeutung |
|---|---|
| warmweiß, 2,5 s, dann ausblendend | die gedimmte Lampe hat jetzt **diese Helligkeit** (je heller die Leuchte, desto heller die Lampe) |
| kurz gedimmtes Orange | Lampe ausgeschaltet |
| kurz grün | Befehl an ein anderes Gerät dieser Bedienstelle ausgeführt |
| kurz blau | Befehl an ein Gerät in einem anderen Teil des Hauses geschickt |
| **zweimal kurz rot alle 5 s** | **Störung**: eine angeschlossene Lampensteuerung antwortet nicht oder die Verbindung zum Haus ist gestört → Betreuer informieren |

### Eck-Leuchten – Signale

| Muster | Bedeutung |
|---|---|
| dauerhaft (warmweiß oder eingestellte Farbe) | Orientierungslicht |
| dreimal kurz weiß | es hat geklingelt |
| rot blinkend | Alarm (endet nach spätestens 60 s) |
| blau an- und abschwellend | Hinweis |
| 1 s grün / 1 s rot | Bestätigung / Fehler einer Aktion |
| **alle Leuchten blau blinkend** | jemand sucht gerade diese Bedienstelle (Wartung) – kein Fehler |

## Weitere Automatiken

- **Netzteile** der Lampen laufen nur, solange eine Lampe leuchtet, und gehen etwa 20 s nach dem letzten
  Ausschalten aus.
- **Hebepumpe** im Keller: läuft automatisch in festen Abständen.
- **Seitentür:** ein angelernter Finger am Fingerabdruckleser öffnet die Tür.
- **Terrasse:** die Funk-Fernbedienung schaltet und dimmt die Spots am Terrassendach und fährt die
  Markisen.

## Wenn etwas nicht reagiert

| Beobachtung | Was tun |
|---|---|
| ein Taster reagiert nicht, andere im Raum schon | einmal kurz drücken und loslassen (ein klemmender Taster wird ignoriert); bleibt es so: Betreuer |
| eine Lampe geht nicht an, die Rückmeldeleuchte blinkt zweimal rot | Störung der Lampensteuerung; Betreuer informieren |
| mehrere Taster in verschiedenen Räumen reagieren nicht | Verbindung zwischen den Stationen gestört; Betreuer informieren |
| Rollladen fährt nach Stromausfall länger als sonst | normal, die erste Fahrt sucht die Endlage |
| Licht geht „von selbst“ aus | automatisches Ausschalten nach der eingestellten Zeit |

Licht und Rollläden lassen sich bei einer Störung meist auch über die
[Weboberfläche](weboberflaeche.md) einer anderen Station schalten.

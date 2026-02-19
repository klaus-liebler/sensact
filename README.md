# sensact
Smarthome system - hardware, firmware, configware, visualization

## Architektur
* Verteiltes System. Modellbasierte Codeerzeugung.
* Grundlage ist sensact/configware. Hier ein Projekt analog Beispielprojekt "sattlerstraße" anlegen. 
* Kleinste Einheit ist eine "Application", die eine eindeutige ID besitzt. Beispiele für Applications sind "Taster", "Drehencoder", "Jalousie" oder "PWM-Licht"


## Hardware
1. SensactHS: Hutschienenmontage
1. SensactUP: Unterputzmontage



## Kommunikation zwischen den Knoten
Die Knoten sind über einen gemeinsamen CAN-Bus verbunden.
### Altes Nachrichtenformat ()
TBD

### Neues Nachrichtenformat

Es gibt vier Namespaces für Nachrichten "REQUEST" und "RESPONSE", "EVENT" und "ACKNOWLEDGE". Dies wird mit den beiden LSB des Versions-Namespace-Bytes erklärt (Somit bleiben 64 Versionierungsmöglichkeiten)
Alle vier Namespaces haben ihren eigenen counter. Dieser beginnt bei 0x0 mit der ersten versendeten Nachricht und läuft nach 255 (0xFF) wieder auf 0. Die Ready-Nachricht wird immer mit counter=0x0 übertragen. Sie darf verwendet werden, um die Kommunikations-Session abzubrechen und die counter neu zu starten. Die counter representieren gemeinsames Wissen von Master und Slave  
REQEST (0b00): Vom Master zum Slave.
RESPONSE (0b01): Antwort auf REQUEST
EVENT (0b10): Vom Slave zum Master
ACKNOWLEDGE (0b11): Antwort des Masters auf ein Event

REQUEST
0x00: "HEARTBEAT": Board gibt Heartbeat zurück
0x01: "IDENTIFY: 
0x02: "SEND_CAN":

RESPONSE
In der Response wird einfach die ID des Requests übernommen. Häufig wird dann ein REQUEST-spezifischer Fehlercode als uint32-t zurück geliefert. Eine 0x0 steht dann per Konvention für eine fehlerfreie Verarbeitung. Ggf. werden in Zukunft WELL_KNOWN_ERROR_CODES definiert.
0x01: UNEXPECTED_COUNTER
0x02: CRC_ERROR


Alle Responses, die nicht nur einen reinen Error-Code, sondern spezifischere Daten zurück liefern, werden im folgenden beschrieben  
0x01: "IDENTIFY": Board gibt Board-Typ u16. NodeID u16 und ID des Chips u96 (?) zurück.

EVENTS
00 "READY" Slave meldet sich nach dem Booten und gibt an, dass er jetzt für Befehle aller Art bereit steht. Beginn einer Session. Alle counter werden auf 0 gesetzt, außer der EVENT_COUNTER - der muss nun auf 1 stehen
02 "CAN_RECEIVED": Slave zeigt an, dass er eine CAN-Nachricht empfangen (besser: auf dem Bus mitgelesen) hat

ACKNOWLEDGES:
Der Slave kann bei ausbleibendem Acknowledge die Nachricht nochmals versenden

## WebUI

### Konzept

Auf einer Website sollen alle Apps einer Node mit commands versehen werden können
Jeder Host hat eine Bedienoberfläche
Problem: Wird die Bedienoberfläche zur Laufzeit von den Hosts und ihren Unterobjekten generiert (a) HTML oder b)JSON) oder kennt die WebUI bereits alle Objekte
Kennt die WebUI die Objekte auch semantisch oder kann Sie nur eine HTML-GUI aufbauen zur Erzeugung von CAN-Bus-Nachrichten 
Denkweise: Die WebUI kann CAN-Nachrichten erzeugen und so an die Node senden, wie sie auch über den CAN-Bus an der Node ankommen würden. Es muss ein JSON generiert werden, das zur Erzeugung einer HTML-UI verwendet werden kann, die dann eben genau diese Nachrichten sendet
Die WebUI kennt bereits alle Applications, die in der Node vorhanden sind. Dazu wird neben der C++-Instanzierung auch TypeScript-Code aufgebaut, der die entsprechenden Objekte aufbaut
Für Buttons und RotEnc gibt es die Möglichkeit, die Beträtigung zu simulieren. Dazu wird dann einfach jene Nachricht auf den MessageBus gelegt, die auch in Wirklichkeit vrsendet wird
An Aktoren können jene Commands versendet werden mitsamt notwendiger Parameter
Für die Erstellung eines Command 

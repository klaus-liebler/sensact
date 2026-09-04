// ws-protocol-Schema fuer den 'hoymiles'-Namespace. Liefert Live-Daten eines per nRF24L01+
// angebundenen Hoymiles-Wechselrichters (HM-1200/1500, 4 DC-Strings) an die Web-UI, in Anlehnung
// an OpenDTUs LiveView->Main. ResponseLiveData beantwortet RequestLiveData mit dem zuletzt
// gecachten Snapshot (fuer sofortige Anzeige beim Oeffnen der Seite); NotifyLiveData ist der
// periodische Server-Push (alle paar Sekunden) mit denselben Feldern -- MessageKind.Event, da es
// kein "laufender Request" ist, den die Notify begleitet (analog canmonitor.NotifyCanMessage).
using BestBinaryBuffers;

namespace hoymiles;

[BinaryType]
public struct DcChannel
{
	public float Udc;
	public float Idc;
	public float Pdc;
	public float YieldDay;
	public float YieldTotal;
}

[BinaryType]
public struct AcPhase
{
	public float Uac;
	public float Iac;
	public float Pac;
	public float ReactivePower;
	public float Frequency;
	public float PowerFactor;
}

[BinaryType]
public struct InverterTotals
{
	public float TemperatureC;
	public ushort EventLogCount;
	public float YieldDay;
	public float YieldTotal;
	public float Pdc;
	public float EfficiencyPct;
}

[BinaryMessage(MessageKind.Request)]
public class RequestLiveData
{
}

[BinaryMessage(MessageKind.Response)]
public class ResponseLiveData
{
	public bool Reachable;
	public bool Producing;
	public uint DataAgeMs;
	[BinaryCount(4)] public DcChannel[] Dc;
	public AcPhase Ac;
	public InverterTotals Totals;
}

[BinaryMessage(MessageKind.Event)]
public class NotifyLiveData
{
	public bool Reachable;
	public bool Producing;
	public uint DataAgeMs;
	[BinaryCount(4)] public DcChannel[] Dc;
	public AcPhase Ac;
	public InverterTotals Totals;
}

// ws-protocol-Schema fuer den 'sensact'-Namespace (ehemals ns20sensact.fbs). ApplicationId/Command
// werden von configware generiert (s. sensact_applicationIds.enums.cs/sensact_commandTypes.enums.cs,
// configware/common/Generator.cs) -- selbes Schema-Namespace "sensact", deshalb hier unqualifiziert
// referenzierbar. NotifyStatus war im Original Teil der 'Responses'-Union (nicht ausserhalb wie
// canmonitor.NotifyCanMessage), deshalb 'Response' statt 'Event'.
using BestBinaryBuffers;

namespace sensact;

/// Rohes Kommando-Payload, feste 8 Byte + Laenge (ehemals Flatbuffers-struct Payload{data:[uint8:8], len:int}).
[BinaryType]
public struct Payload
{
	[BinaryCount(8)] public byte[] Data;
	public int Len;
}

/// Fester Status-Payload, 4x uint16 (ehemals Flatbuffers-struct StatusPayload{data:[uint16:4]}).
[BinaryType]
public struct StatusPayload
{
	[BinaryCount(4)] public ushort[] Data;
}

[BinaryType]
public struct ApplicationIdValue
{
	public ApplicationId Value;
}

[BinaryType]
public struct ResponseStatusItem
{
	public ApplicationId Id;
	public StatusPayload Status;
}

[BinaryMessage(MessageKind.Request)]
public class RequestCommand
{
	public ApplicationId Id;
	public Command Cmd;
	public Payload Payload;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseCommand
{
}

/// ids:[ApplicationId] im Original -- nackte Enum-Werte muessen wie bei Bare-Skalar-Arrays ueber einen
/// Einzelfeld-Wrapper (ApplicationIdValue) in ein Array-Element verpackt werden.
[BinaryMessage(MessageKind.Request)]
public class RequestStatus
{
	// 128: groesster aktuell generierter Knoten (SNSCT_L0_TECH_HS_1) hat 89 Applikationen, s.
	// generated/sensact_model/SNSCT_L0_TECH_HS_1/applicationInitializers.inc -- Rundung mit Puffer.
	[BinaryMaxItemCount(128)] public ApplicationIdValue[] Ids;
}

/// Server-Push (Statusaenderung einer Applikation) -- Teil der urspruenglichen 'Responses'-Union.
[BinaryMessage(MessageKind.Response)]
public class NotifyStatus
{
	public ApplicationId Id;
	public StatusPayload Status;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseStatus
{
	[BinaryMaxItemCount(128)] public ResponseStatusItem[] States;
}

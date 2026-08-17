// ws-protocol-Schema fuer den 'nodemaster'-Namespace (ehemals ns21nodemaster.fbs). Range hat ein
// String-Feld (name) und wird deshalb zur Class statt zum Struct, referenziert als
// Ein-Varianten-Union (klassische Vorgehensweise fuer eine getaggte Liste eines einzelnen Klassentyps,
// s. BestBinaryBuffers/README.md).
using BestBinaryBuffers;

namespace nodemaster;

[BinaryUnion]
public interface IRange
{
}

[BinaryType]
public class Range : IRange
{
	public ushort Start;
	public ushort End;
	public string Name;
}

[BinaryMessage(MessageKind.Request)]
public class RequestOutputs
{
}

[BinaryMessage(MessageKind.Request)]
public class RequestInputs
{
}

[BinaryMessage(MessageKind.Request)]
public class RequestSetOutput
{
	public ushort Index;
	public ushort Value;
}

[BinaryMessage(MessageKind.Request)]
public class RequestGetInput
{
	public ushort Index;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseOutputs
{
	public IRange[] Ranges;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseInputs
{
	public IRange[] Ranges;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseGetInput
{
	public ushort Index;
	public ushort Value;
}

[BinaryMessage(MessageKind.Response)]
public class ResponseSetOutput
{
	public bool Success;
	public ushort Index;
	public ushort Value;
}

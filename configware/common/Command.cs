using Klli.Sensact.Model.Common;

public class Command
    {
        public ushort TargetAppId;
        public CommandType CommandType; //TODO: Prüfung: Kann die DestinationApp auf dieses Event reagieren?
        public byte[] Payload=new byte[0];
    }
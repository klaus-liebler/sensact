using System.Text;


namespace Klli.Sensact.UdpMonitor
{
enum ErrorCode
{
    OK = 0,
    
    GENERIC_ERROR = 1,
    QUEUE_OVERLOAD = 2,
    NONE_AVAILABLE=3,
    INDEX_OUT_OF_BOUNDS=4,
    INVALID_NEW_FBD=5,
    NOT_YET_IMPLEMENTED = 6,
    TEMPORARYLY_NOT_AVAILABLE = 7,
    FILE_SYSTEM_ERROR=8,
    INCOMPATIBLE_VERSION=9,
    EMERGENCY_SHUTDOWN=10,
    INVALID_ARGUMENT_COUNT=11,
    INVALID_ARGUMENT_VALUES=12,
    OBJECT_NOT_CHANGED=13,
    INVALID_APPLICATION_ID=14,
    INVALID_EVENT_ID=15,
    INVALID_STATE=16,
    INVALID_COMMAND=17,
    NO_CONFIGURATION_FOUND=18,
    PIN_NOT_AVAILABLE,
    PIN_DOES_NOT_SUPPORT_MODE,
    PIN_MODE_DOES_NOT_ALLOW_THIS_SETTING,
    SEMAPHORE_NOT_AVAILABLE,
    DEVICE_NOT_RESPONDING,
    HARDWARE_NOT_INITIALIZED,
    PAYLOAD_TOO_LARGE,
    OK_BUT_NOT_NEEDED,
    FUNCTION_NOT_AVAILABLE,
    
};



	enum eCanMessageType
	{
		// There is no "node application", all node-related messages are in a separate namespace
		// Reason: Node-messages can be send/received from the bootloader and there
		// hence, the node ID has to be burned in the bootloader of a node
		// the bootloader places this information in the RTC_RAM
		NodeEvent = 0x00000000,
		// Messages from an application (which it does not matter, on which node the app is running)
		ApplicationEvent = 0x01000000,
		NodeCommand = 0x02000000,
		NodeCommandAcknowledge = 0x03000000,
		ApplicationCommand = 0x04000000,
		CommandAcknowledge = 0x05000000,
		ApplicationStatus = 0x06000000,
		Payload = 0x1F000000,

	};

	// as long as not all nodes speak the new CAN IDs, this assures compatibility, if only Commands are used
	enum eCanMessageTypeOld
	{
		ApplicationCommand = 0x00000000,
		ApplicationEvent = 0x01000000,
		NodeCommand = 0x02000000,
		NodeCommandAcknowledge = 0x03000000,
		NodeEvent = 0x04000000,
		CommandAcknowledge = 0x05000000,
		ApplicationStatus = 0x06000000,
		ApplicationStatus1 = 0x07000000,		// if 8bytes for status are not enough, this may address the second 8 bytes
		ApplicationStatusSplitted = 0x08000000, // first Byte of Payload is Index ==>Status may have 256*7Bytes =1792bytes
		Payload = 0x1F000000,

	};

	enum eNodeCommandType
	{
		NOC = 0,
		RESET = 1,
		PAYLOAD = 2,
		COPY_SCRATCH_TO_FLASH = 3,
		WRITE_SCRATCH = 4,
		CNT,
	};

	enum eNodeEventType
	{
		NOE = 0,
		NODE_STARTED = 1,
		NODE_STATUS = 2,
		NODE_STOPPED = 3,
		NODE_READY = 4,
		BOOTLOADER_READY = 5,
		// APPLICATION_STARTED=5, NO!!! THIS IS APPLICATION STATUS
		// APPLICATION_STATUS=6,
		// APPLICATION_STOPPED=7,
		CNT,
	};
	
        struct CANMessage
    {
        public UInt32 Id;
        public byte[] Data;
        public int DataLen;
    };
	
	abstract class aCANMessageBuilderParser{
        protected UInt32 MessageTypeMask=0x1F000000U;
		public virtual ErrorCode BuildApplicationCommandMessage(UInt16 destinationAppId, byte commandId, byte[] payload, byte payloadLength, CANMessage m){m.DataLen=0; return ErrorCode.INVALID_STATE;}
		public virtual ErrorCode ParseApplicationCommandMessageId(CANMessage m, ref UInt16 destinationAppId, ref byte commandId){return ErrorCode.INVALID_STATE;}
		public abstract void TraceCommandMessage(CANMessage m);

	};

	class cCANMessageBuilderParserOld:aCANMessageBuilderParser{
		public override ErrorCode BuildApplicationCommandMessage(UInt16 destinationAppId, byte commandId, byte[] payload, byte payloadLength, CANMessage m)
		{
			if (payloadLength > 7) return ErrorCode.PAYLOAD_TOO_LARGE;
			m.Id = (UInt32)eCanMessageTypeOld.ApplicationCommand | ((UInt32)destinationAppId);
            m.Data[0] = (byte)commandId;
            for (int i = 0; i < payloadLength; i++){
				m.Data[i + 1] = payload[i];
			}
			m.DataLen=payloadLength+1;
            return ErrorCode.OK;
		}

		public override ErrorCode ParseApplicationCommandMessageId(CANMessage m, ref UInt16 destinationAppId, ref byte commandId)
		{
			destinationAppId = (UInt16)((m.Id & 0x3FF));
			commandId = m.Data[0];
			return ErrorCode.OK;
		}

        private static string ByteArrayToString(IEnumerable<byte> ba)
        {
        StringBuilder hex = new StringBuilder(ba.Count() * 2);
        foreach (byte b in ba)
            hex.AppendFormat("{0:x2}", b);
        return hex.ToString();
        }

		public override void TraceCommandMessage(CANMessage m){
			UInt16 appId=0;
			byte commandId=0;
			//assert that it is really a command message
			UInt32 type = (m.Id & MessageTypeMask);   
			if(type!=0){
				Console.WriteLine($"There is a message with id 0x{m.Id:08X} which is not allowed");
			}
			ParseApplicationCommandMessageId(m, ref appId, ref commandId);
			int payloadLen=m.DataLen-1;
            String payloadString = ByteArrayToString(new ArraySegment<byte>(m.Data, 1, m.DataLen-1));
			string appName = Enum.GetName<Model.ApplicationId>((Klli.Sensact.Model.ApplicationId)appId)??"?";
			string commandName = Enum.GetName<Klli.Sensact.Model.Common.CommandType>((Klli.Sensact.Model.Common.CommandType)commandId)??"?";
            
            Console.WriteLine($"ApplicationCommand (old CAN-ID) to id 0x{appId:x2} ({appName}); command:0x{commandId:x2} ({commandName}); len:{payloadLen}; payload: 0x{payloadString}");
		}
	};


	
}


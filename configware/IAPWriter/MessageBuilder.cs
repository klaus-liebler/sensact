using MiscUtil.Conversion;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;



namespace IAPWriter
{
    enum eCanMessageTypes : UInt32
    {
        //There is no "node application", all node-related messages are in a seperate namespace
        //Reason: Node-messages can be send/received from the bootloader and there
        //hence, the node ID has to be burned in the bootloader of a node
        //this very basic information is burned in the bootloader
        //the bootloader places this information in the RTC_RAM
        NodeEvent   = 0x00000000, //it is an event from the node itself and not from an application
	NodeCommand = 0x01000000,
	NodeCommandAcknowledge=0x02000000,
	Event		= 0x03000000,
	Command		= 0x04000000,
	CommandAcknowledge=0x05000000,
	Payload     = 0x1F000000,
};

    enum eNodeCommandType : byte
    {
        NOP=0 ,
        RESET=1 ,
        PAYLOAD=2 ,
        COPY_SCRATCH_TO_FLASH=3 ,
		WRITE_SCRATCH=4,
        CNT
    };

    enum  eNodeEventType : byte
    {
        NOE=0 ,
        BOOTLOADER_READY=1 ,
        CNT
    };

    //32bit message id
    //64bit payload
    public class MessageBuilder
    {
        static UInt32 CreateNodeEventMessageId(byte sourceNodeId, byte eventId)
        {
            return (UInt32)eCanMessageTypes.NodeEvent | ((UInt32)sourceNodeId << 16) | ((UInt32)eventId) << 8; ;
        }
        static UInt32 CreateNodeCommandMessageId(byte destinationNodeId, eNodeCommandType commandId)
        {
            return (UInt32)eCanMessageTypes.NodeCommand | ((UInt32)destinationNodeId << 16) | ((UInt32)commandId) << 8; ;
        }
        static UInt32 CreateNodeCommandAcknowledgeMessageId(byte acknowledgingNodeId, byte commandId)
        {
            return (UInt32)eCanMessageTypes.NodeCommandAcknowledge | ((UInt32)acknowledgingNodeId) << 16 | ((UInt32)commandId) << 8;
        }
        static UInt32 CreateEventMessageId(UInt16 sourceAppId, byte eventId)
        {
            return (UInt32)eCanMessageTypes.Event | ((UInt32)sourceAppId) << 8 | ((UInt32)eventId) << 0;
        }
        static UInt32 CreateCommandMessageId(UInt16 destinationAppId, byte commandId)
        {
            return (UInt32)eCanMessageTypes.Command | ((UInt32)destinationAppId) << 8 | ((UInt32)commandId) << 0;
        }
        static UInt32 CreateCommandAcknowledgeMessageId(UInt16 acknowledgingdestinationAppId, byte commandId)
        {
            return (UInt32)eCanMessageTypes.CommandAcknowledge | ((UInt32)acknowledgingdestinationAppId) << 8 | ((UInt32)commandId) << 0;
        }
        static UInt32 CreatePayloadMessageId(byte destinationNodeId, UInt16 chunkIndex)
        {
            return (UInt32)eCanMessageTypes.Payload | ((UInt32)destinationNodeId) << 16 | ((UInt32)chunkIndex) << 0;
        }

        public static byte[] BootToBootloader(byte nodeId)
        {
            byte[] retval = new byte[12];
            EndianBitConverter.Little.CopyBytes(CreateNodeCommandMessageId(nodeId, eNodeCommandType.RESET), retval, 0);
            return retval;
        }
    }
}

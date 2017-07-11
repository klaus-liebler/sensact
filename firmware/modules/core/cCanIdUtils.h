#include <inttypes.h>

namespace sensact
{

enum struct eCanMessageType:uint32_t
{
	//There is no "node application", all node-related messages are in a seperate namespace
	//Reason: Node-messages can be send/received from the bootloader and there
	//hence, the node ID has to be burned in the bootloader of a node
	//this very basic information is burned in the bootloader
	//the bootloader places this information in the RTC_RAM
	///Messages from the Node itself (either the bootloader or the main application)
	NodeEvent   			= 0x00000000,
	//Messages from an application (which it does not matter, on which node the app is running)
	ApplicationEvent		= 0x01000000,
	NodeCommand 			= 0x02000000,
	NodeCommandAcknowledge	= 0x03000000,
	ApplicationCommand		= 0x04000000,
	CommandAcknowledge		= 0x05000000,
	ApplicationStatus 		= 0x06000000,
	Payload           		= 0x1F000000,

};

enum struct eNodeCommandType:uint8_t{
	    NOC=0 ,
        RESET=1 ,
        PAYLOAD=2 ,
        COPY_SCRATCH_TO_FLASH=3 ,
		WRITE_SCRATCH=4,
        CNT,
};

enum struct eNodeEventType:uint8_t{
	    NOE=0 ,
        NODE_STARTED=1 ,
		NODE_STATUS=2,
		NODE_STOPPED=3,
		BOOTLOADER_READY=4,
		APPLICATION_STARTED=5,
		APPLICATION_STATUS=6,
		APPLICATION_STOPPED=7,
        CNT,
};


class cCanIdUtils
	{
		static uint32_t MessageTypeMask;
public:
		static uint32_t CreateNodeEventMessageId(uint8_t sourceNodeId, uint8_t eventId);
		static uint32_t CreateNodeCommandMessageId(uint8_t destinationNodeId, uint8_t commandId);
		static uint32_t CreateNodeCommandAcknowledgeMessageId(uint8_t acknowledgingNodeId, uint8_t commandId);
		static uint32_t CreateEventMessageId(uint16_t sourceAppId, uint8_t eventId);
		static uint32_t CreateApplicationStatusMessageId(uint16_t sourceAppId, uint8_t statusId);
		static uint32_t CreateCommandMessageId(uint16_t destinationAppId, uint8_t commandId);
		static uint32_t CreateCommandAcknowledgeMessageId(uint16_t acknowledgingdestinationAppId, uint8_t commandId);
		static uint32_t CreatePayloadMessageId(uint8_t destinationNodeId, uint16_t chunkIndex);

		static eCanMessageType ParseCanMessageType(uint32_t messageId);
		static void ParseNodeEventMessageId(uint32_t messageId, uint8_t *sourceNodeId, uint8_t *eventId);
		static void ParseNodeCommandMessageId(uint32_t messageId, uint8_t *destinationNodeId, uint8_t *commandId);
		static void ParseNodeCommandAcknowledgeMessageId(uint32_t messageId, uint8_t *acknowledgingNodeId, uint8_t *commandId);
		static void ParseEventMessageId(uint32_t messageId, uint16_t *sourceAppId, uint8_t *eventId);
		static void ParseApplicationStatusMessageId(uint32_t messageId, uint16_t *sourceAppId, uint8_t *statusType);

		static void ParseCommandMessageId(uint32_t messageId, uint16_t *destinationAppId, uint8_t *commandId);
		static void ParseCommandAcknowledgeMessageId(uint32_t messageId, uint16_t *acknowledgingdestinationAppId, uint8_t *commandId);
		static void ParsePayloadMessageId(uint32_t messageId, uint8_t *destinationNodeId, uint16_t *chunkIndex);
	};
}

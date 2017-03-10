#include <inttypes.h>

namespace sensact
{

enum struct eCanMessageTypes:uint32_t
{
	NodeEvent   = 0x00000000,
	NodeCommand = 0x01000000,
	NodeCommandAcknowledge=0x02000000,
	Event		= 0x03000000,
	Command		= 0x04000000,
	CommandAcknowledge=0x05000000,
	Payload     = 0x1F000000,
};

enum struct eNodeCommandType:uint8_t{
	    NOP=0 ,
        RESET=1 ,
        PAYLOAD=2 ,
        WRITE_SCRATCH_TO_FLASH=3 ,
        CNT
};

enum struct eNodeEventType:uint8_t{
	    NOE=0 ,
        BOOTLOADER_READY=1 ,
        CNT
};

class cCanIdUtils
	{
		static uint32_t MessageTypeMask;
public:
		static uint32_t CreateNodeEventMessageId(uint8_t sourceNodeId, uint8_t eventId);
		static uint32_t CreateNodeCommandMessageId(uint8_t destinationNodeId, uint8_t commandId);
		static uint32_t CreateNodeCommandAcknowledgeMessageId(uint8_t acknowledgingNodeId, uint8_t commandId);
		static uint32_t CreateEventMessageId(uint16_t sourceAppId, uint8_t eventId);
		static uint32_t CreateCommandMessageId(uint16_t destinationAppId, uint8_t commandId);
		static uint32_t CreateCommandAcknowledgeMessageId(uint16_t acknowledgingdestinationAppId, uint8_t commandId);
		static uint32_t CreatePayloadMessageId(uint8_t destinationNodeId, uint16_t chunkIndex);

		static eCanMessageTypes ParseCanMessageType(uint32_t messageId);
		static void ParseNodeEventMessageId(uint32_t messageId, uint8_t *sourceNodeId, uint8_t *eventId);
		static void ParseNodeCommandMessageId(uint32_t messageId, uint8_t *destinationNodeId, uint8_t *commandId);
		static void ParseNodeCommandAcknowledgeMessageId(uint32_t messageId, uint8_t *acknowledgingNodeId, uint8_t *commandId);
		static void ParseEventMessageId(uint32_t messageId, uint16_t *sourceAppId, uint8_t *eventId);
		static void ParseCommandMessageId(uint32_t messageId, uint16_t *destinationAppId, uint8_t *commandId);
		static void ParseCommandAcknowledgeMessageId(uint32_t messageId, uint16_t *acknowledgingdestinationAppId, uint8_t *commandId);
		static void ParsePayloadMessageId(uint32_t messageId, uint8_t *destinationNodeId, uint16_t *chunkIndex);
	};
}

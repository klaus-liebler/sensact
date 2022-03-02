#include <inttypes.h>
#include <esp_log.h>
#include "common_in_project.hh"
#define TAG "CAN"
namespace sensact
{
	/*
	Der Status einer Applikation wird beim Start und bei jedem Applikationszyklus ermittelt
	Der Status wird publiziert, wann immer sich der Status �ndert und auch regelm��ig
	Statusnachrichten
	ApplicationStartedMessages, erstes Byte ist das eAppCallResult vom Setup
	ApplicationErrorMessages, erstes Byte ist das eAppCallResult
	Der Master hat einen statischen Buffer mit fester L�nge statisch alloziiert, der die Statusnachricht der Application aufnehmen kann
	Die anderen Nodes senden die Statusnachrichten
	Der Server hat ein array id->eAppType und wei� deshalb, welchen Typs die Nachricht ist.
	Man braucht dann auch ein Array (uint8_t)eAppType->"stringNameDerApp beziehungsweise der Queue"
	Au�erdem muss auch irgendwo die L�nge stehen,
	damit klar st, wie viel Bytes �ber MQTT gesendet werden
	*/

#ifndef NEW_CANID
	// as long as not all nodes speak the new CAN IDs, this assures compatibility, if only Commands are used
	enum struct eCanMessageType : uint32_t
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
#else
	enum struct eCanMessageType : uint32_t
	{
		// There is no "node application", all node-related messages are in a separate namespace
		// Reason: Node-messages can be send/received from the bootloader and there
		// hence, the node ID has to be burned in the bootloader of a node
		// this very basic information is burned in the bootloader
		// the bootloader places this information in the RTC_RAM
		/// Messages from the Node itself (either the bootloader or the main application)
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
#endif

	enum struct eNodeCommandType : uint8_t
	{
		NOC = 0,
		RESET = 1,
		PAYLOAD = 2,
		COPY_SCRATCH_TO_FLASH = 3,
		WRITE_SCRATCH = 4,
		CNT,
	};

	enum struct eNodeEventType : uint8_t
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
	constexpr uint32_t MessageTypeMask{(uint32_t)0x1F00000000U};
	class CANMessenger
	{
	private:
		
		HAL *hal;

	public:
		CANMessenger(HAL *hal) : hal(hal) {}
		static u32 CreateNodeEventMessageId(uint8_t sourceNodeId, uint8_t eventId)
		{
			return (uint32_t)eCanMessageType::NodeEvent | ((uint32_t)sourceNodeId << 16) | ((uint32_t)eventId) << 8;
		}
		ErrorCode SendNodeCommandMessage(uint8_t destinationNodeId, uint8_t commandId)
		{
			u32 canid = (uint32_t)eCanMessageType::NodeCommand | ((uint32_t)destinationNodeId << 16) | ((uint32_t)commandId) << 8;
		}
		ErrorCode SendNodeCommandAcknowledgeMessage(uint8_t acknowledgingNodeId, uint8_t commandId)
		{
			u32 canid = (uint32_t)eCanMessageType::NodeCommandAcknowledge | ((uint32_t)acknowledgingNodeId) << 16 | ((uint32_t)commandId) << 8;
		}
		ErrorCode SendEventMessage(uint16_t sourceAppId, uint8_t eventId)
		{
			u32 canid = (uint32_t)eCanMessageType::ApplicationEvent | ((uint32_t)sourceAppId) << 8 | ((uint32_t)eventId) << 0;
		}
		ErrorCode SendApplicationStatusMessage(uint16_t sourceAppId, uint8_t statusId, const uint8_t *const payload, uint8_t payloadLength)
		{
			u32 canid = (uint32_t)eCanMessageType::ApplicationStatus | ((uint32_t)sourceAppId) << 8 | ((uint32_t)statusId) << 0;
			return hal->TrySendCanMessage(canid, payload, payloadLength);
		}

#ifdef NEW_CANID
		ErrorCode SendCommandMessage(uint16_t destinationAppId, uint8_t commandId, const uint8_t *const payload, uint8_t payloadLength)
		{
			u32 canid =  (uint32_t)eCanMessageType::ApplicationCommand | ((uint32_t)destinationAppId) << 8 | ((uint32_t)commandId) << 0;
			return hal->TrySendCanMessage(canid, payload, payloadLength);
		}
#else
		ErrorCode SendCommandMessage(uint16_t destinationAppId, uint8_t commandId, const uint8_t *const payload, uint8_t payloadLength)
		{
			if (payloadLength > 7) return ErrorCode::PAYLOAD_TOO_LARGE;
			u32 canid = (uint32_t)eCanMessageType::ApplicationCommand | ((uint32_t)destinationAppId);
            uint8_t buf[8];
            buf[0] = (uint8_t)commandId;
            for (int i = 0; i < payloadLength; i++) buf[i + 1] = payload[i];
            return hal->TrySendCanMessage(canid, buf, payloadLength + 1);
		}
#endif

		ErrorCode SendCommandAcknowledgeMessageId(uint16_t acknowledgingdestinationAppId, uint8_t commandId)
		{
			u32 id = (uint32_t)eCanMessageType::CommandAcknowledge | ((uint32_t)acknowledgingdestinationAppId) << 8 | ((uint32_t)commandId) << 0;
		}
		ErrorCode SendPayloadMessageId(uint8_t destinationNodeId, uint16_t chunkIndex)
		{
		}

		static eCanMessageType ParseCanMessageType(uint32_t messageId)
		{
			return (eCanMessageType)(messageId & MessageTypeMask);
		}

		static ErrorCode ParseApplicationStatusMessageId(uint32_t messageId, uint16_t *sourceAppId, uint8_t *statusType)
		{
			*sourceAppId = (uint16_t)((messageId & 0x00FFFF00) >> 8);
			*statusType = (uint8_t)((messageId & 0x000000FF) >> 0);
		}

		static ErrorCode ParseNodeEventMessageId(uint32_t messageId, uint8_t *sourceNodeId, uint8_t *eventId)
		{
			*sourceNodeId = (uint8_t)((messageId & 0x00FF0000) >> 16);
			*eventId = (uint8_t)((messageId & 0x0000FF00) >> 8);
		}
		static ErrorCode ParseNodeCommandMessageId(uint32_t messageId, uint8_t *destinationNodeId, uint8_t *commandId)
		{
			*destinationNodeId = (uint8_t)((messageId & 0x00FF0000) >> 16);
			*commandId = (uint8_t)((messageId & 0x0000FF00) >> 8);
		}
		static ErrorCode ParseNodeCommandAcknowledgeMessageId(uint32_t messageId, uint8_t *acknowledgingNodeId, uint8_t *commandId)
		{
			*acknowledgingNodeId = (uint8_t)((messageId & 0x00FF0000) >> 16);
			*commandId = (uint8_t)((messageId & 0x0000FF00) >> 8);
		}
		static ErrorCode ParseEventMessageId(uint32_t messageId, uint16_t *sourceAppId, uint8_t *eventId)
		{
			*sourceAppId = (uint16_t)((messageId & 0x00FFFF00) >> 8);
			*eventId = (uint8_t)((messageId & 0x000000FF) >> 0);
		}
#ifdef NEW_CANID
		static ErrorCode ParseCommandMessageId(uint32_t messageId, uint16_t *destinationAppId, uint8_t *commandId, uint8_t firstByteOfMessage)
		{
			(void)firstByteOfMessage;
			*destinationAppId = (uint16_t)((messageId & 0x00FFFF00) >> 8);
			*commandId = (uint8_t)((messageId & 0x000000FF) >> 0);
		}
#else
		static ErrorCode ParseCommandMessageId(uint32_t messageId, uint16_t *destinationAppId, uint8_t *commandId, uint8_t firstByteOfMessage)
		{
			*destinationAppId = (uint16_t)((messageId & 0x3FF));
			*commandId = firstByteOfMessage;
		}

#endif
		static ErrorCode ParseCommandAcknowledgeMessageId(uint32_t messageId, uint16_t *acknowledgingdestinationAppId, uint8_t *commandId)
		{
			*acknowledgingdestinationAppId = (uint16_t)((messageId & 0x00FFFF00) >> 8);
			*commandId = (uint8_t)((messageId & 0x000000FF) >> 0);
		}

		static void TraceCommandMessage(CANMessage *m){
			u16 appId;
			u8 commandId;        
			CANMessenger::ParseCommandMessageId(rcvMessage.Id, &appId, &commandId, rcvMessage.Data[0]);
			char charbuf[2*rcvMessage.Length+1];
            for(int i=0;i<rcvMessage.Length;i++)
            {
                sprintf(&charbuf[2*i], "%02X, ", rcvMessage.Data[i]);
            }
            charbuf[2*rcvMessage.Length]='\0';
#ifdef NEW_CANID
            LOGI(TAG, "ApplicationCommand to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length, charbuf);
#else
            LOGI(TAG, "ApplicationCommand (old CAN-ID) to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandId, rcvMessage.Length, charbuf);
#endif
		}

		static void TraceEventMessage(CANMessage *m){
			u16 appId;
			u8 eventId; 
			char charbuf[2*rcvMessage.Length+1];
			for(int i=0;i<rcvMessage.Length;i++)
			{
				sprintf(&charbuf[2*i], "%02X, ", rcvMessage.Data[i]);
			}
            charbuf[2*rcvMessage.Length]='\0';
#ifdef NEW_CANID
            LOGD(TAG, "ApplicationEvent from id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], eventId, rcvMessage.Length, charbuf);
#else
            LOGD(TAG, "ApplicationEvent (old CAN-ID) from id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], eventId, rcvMessage.Length, charbuf);
#endif
		}
	};
}
#undef TAG

#pragma once
#include <inttypes.h>
#include <esp_log.h>
#include "common_in_project.hh"
#include "model_applications.hh"
#define TAG "CAN"
namespace sensact
{
	/*
	Der Status einer Applikation wird beim Start und bei jedem Applikationszyklus ermittelt
	Der Status wird publiziert, wann immer sich der Status ändert und auch regelmäßig
	Statusnachrichten
	ApplicationStartedMessages, erstes Byte ist das eAppCallResult vom Setup
	ApplicationErrorMessages, erstes Byte ist das eAppCallResult
	Der Master hat einen statischen Buffer mit fester Länge statisch alloziiert, der die Statusnachricht der Application aufnehmen kann
	Die anderen Nodes senden die Statusnachrichten
	Der Server hat ein array id->eAppType und weiß deshalb, welchen Typs die Nachricht ist.
	Man braucht dann auch ein Array (uint8_t)eAppType->"stringNameDerApp beziehungsweise der Queue"
	Außerdem muss auch irgendwo die Länge stehen,
	damit klar ist, wie viel Bytes über MQTT gesendet werden
	*/


	enum struct eCanMessageType : uint32_t
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
	enum struct eCanMessageTypeOld : uint32_t
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
	constexpr uint32_t MessageTypeMask{(uint32_t)0x1F000000U};
	
	class aCANMessageBuilderParser{
	public:
		virtual ErrorCode BuildNodeCommandMessage(uint8_t destinationNodeId, uint8_t commandId, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildNodeEventMessage(uint8_t sourceNodeId, uint8_t eventId, uint8_t * payload, uint8_t payloadLength, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildNodeCommandAcknowledgeMessage(uint8_t acknowledgingNodeId, uint8_t commandId, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildApplicationEventMessage(uint16_t sourceAppId, uint8_t eventId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildApplicationStatusMessage(uint16_t sourceAppId, uint8_t statusId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildApplicationCommandMessage(uint16_t destinationAppId, uint8_t commandId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode BuildCommandAcknowledgeMessage(uint16_t acknowledgingdestinationAppId, uint8_t commandId, CANMessage &m){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseCanMessageType(CANMessage& m, eCanMessageType& type){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseApplicationStatusMessageId(CANMessage& m, uint16_t &sourceAppId, uint8_t &statusType){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseNodeEventMessageId(CANMessage& m, uint8_t &sourceNodeId, uint8_t &eventId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseNodeCommandMessageId(CANMessage& m, uint8_t &destinationNodeId, uint8_t &commandId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseNodeCommandAcknowledgeMessageId(CANMessage& m, uint8_t &acknowledgingNodeId, uint8_t &commandId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseApplicationEventMessageId(CANMessage& m, uint16_t &sourceAppId, uint8_t &eventId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseApplicationCommandMessageId(CANMessage& m, uint16_t &destinationAppId, uint8_t &commandId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual ErrorCode ParseApplicationCommandAcknowledgeMessageId(CANMessage& m, uint16_t &acknowledgingdestinationAppId, uint8_t &commandId){m.DataLen=0; return ErrorCode::INVALID_STATE;}
		virtual void TraceCommandMessage(CANMessage &m){return;}
		virtual void TraceEventMessage(CANMessage &m){return;}
		virtual uint8_t* GetPayloadStart(CANMessage &m)=0;
		virtual uint8_t GetPayloadLen(CANMessage &m)=0;
	};

	class cCANMessageBuilderParserOld:public aCANMessageBuilderParser{
		ErrorCode BuildApplicationCommandMessage(uint16_t destinationAppId, uint8_t commandId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m) override
		{
			if (payloadLength > 7) return ErrorCode::PAYLOAD_TOO_LARGE;
			m.Id = (uint32_t)eCanMessageTypeOld::ApplicationCommand | ((uint32_t)destinationAppId);
            m.Data[0] = (uint8_t)commandId;
            for (int i = 0; i < payloadLength; i++){
				m.Data[i + 1] = payload[i];
			}
			m.DataLen=payloadLength+1;
            return ErrorCode::OK;
		}

		ErrorCode ParseApplicationCommandMessageId(CANMessage& m, uint16_t &destinationAppId, uint8_t &commandId) override
		{
			destinationAppId = (uint16_t)((m.Id & 0x3FF));
			commandId = m.Data[0];
			return ErrorCode::OK;
		}

		void TraceCommandMessage(CANMessage &m) override{
			u16 appId;
			u8 commandId;
			//assert that it is really a command message
			u32 type = (m.Id & MessageTypeMask);   
			if(type!=0){
				LOGE(TAG, "There is a message with id 0x%08X which is not allowed", (unsigned int)m.Id);
			}
			ParseApplicationCommandMessageId(m, appId, commandId);
			char charbuf[2*m.DataLen+1];
            for(int i=1;i<m.DataLen;i++)
            {
                sprintf(&charbuf[2*i], "%02X, ", m.Data[i]);
            }
            charbuf[2*m.DataLen]='\0';
            LOGI(TAG, "ApplicationCommand (old CAN-ID) to id:%s; command:%d; len:%d; payload: 0x%s", sensact::model::applications::ApplicationNames[appId], commandId, m.DataLen, charbuf);
		}

		uint8_t* GetPayloadStart(CANMessage &m) override{
			return &m.Data[1];
		}

		uint8_t GetPayloadLen(CANMessage &m) override{
			return m.DataLen-1;
		}
	};


	class cCANMessageBuilderParserNew:public aCANMessageBuilderParser
	{
	public:
		uint8_t* GetPayloadStart(CANMessage &m) override{
			return &m.Data[0];
		}

		uint8_t GetPayloadLen(CANMessage &m) override{
			return m.DataLen;
		}
		
		ErrorCode BuildNodeCommandMessage(uint8_t destinationNodeId, uint8_t commandId, CANMessage &m)override
		{
			m.Id = (uint32_t)eCanMessageType::NodeCommand | ((uint32_t)destinationNodeId << 16) | ((uint32_t)commandId) << 8;
			m.DataLen=0;
			return ErrorCode::OK;
		}

		ErrorCode BuildNodeEventMessage(uint8_t sourceNodeId, uint8_t eventId, uint8_t * payload, uint8_t payloadLength, CANMessage &m)override
		{
			m.Id =  (uint32_t)eCanMessageType::NodeEvent | ((uint32_t)sourceNodeId << 16) | ((uint32_t)eventId) << 8;
			for(int i=0;i<payloadLength;i++){
				m.Data[i]=payload[i];
			}
			m.DataLen=payloadLength;
			return ErrorCode::OK;
		}

		ErrorCode BuildNodeCommandAcknowledgeMessage(uint8_t acknowledgingNodeId, uint8_t commandId, CANMessage &m)override
		{
			m.Id = (uint32_t)eCanMessageType::NodeCommandAcknowledge | ((uint32_t)acknowledgingNodeId) << 16 | ((uint32_t)commandId) << 8;
			m.DataLen=0;
			return ErrorCode::OK;
		}
		ErrorCode BuildApplicationEventMessage(uint16_t sourceAppId, uint8_t eventId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m)override
		{
			m.Id = (uint32_t)eCanMessageType::ApplicationEvent | ((uint32_t)sourceAppId) << 8 | ((uint32_t)eventId) << 0;
			for(int i=0;i<payloadLength;i++){
				m.Data[i]=payload[i];
			}
			m.DataLen=payloadLength;
			return ErrorCode::OK;
		}
		ErrorCode BuildApplicationStatusMessage(uint16_t sourceAppId, uint8_t statusId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m)override
		{
			m.Id = (uint32_t)eCanMessageType::ApplicationStatus | ((uint32_t)sourceAppId) << 8 | ((uint32_t)statusId) << 0;
			for(int i=0;i<payloadLength;i++){
				m.Data[i]=payload[i];
			}
			m.DataLen=payloadLength;
			return ErrorCode::OK;
		}

		ErrorCode BuildApplicationCommandMessage(uint16_t destinationAppId, uint8_t commandId, const uint8_t *const payload, uint8_t payloadLength, CANMessage &m)override
		{
			m.Id =  (uint32_t)eCanMessageType::ApplicationCommand | ((uint32_t)destinationAppId) << 8 | ((uint32_t)commandId) << 0;
			for(int i=0;i<payloadLength;i++){
				m.Data[i]=payload[i];
			}
			m.DataLen=payloadLength;
			return ErrorCode::OK;
		}

		ErrorCode BuildCommandAcknowledgeMessage(uint16_t acknowledgingdestinationAppId, uint8_t commandId, CANMessage &m)override
		{
			m.Id = (uint32_t)eCanMessageType::CommandAcknowledge | ((uint32_t)acknowledgingdestinationAppId) << 8 | ((uint32_t)commandId) << 0;
			m.DataLen=0;
			return ErrorCode::OK;
		}

		ErrorCode ParseCanMessageType(CANMessage& m, eCanMessageType& type)override
		{
			type = (eCanMessageType)(m.Id & MessageTypeMask);
			return ErrorCode::OK;
		}

		ErrorCode ParseApplicationStatusMessageId(CANMessage& m, uint16_t &sourceAppId, uint8_t &statusType)override
		{
			sourceAppId = (uint16_t)((m.Id & 0x00FFFF00) >> 8);
			statusType = (uint8_t)((m.Id & 0x000000FF) >> 0);
			return ErrorCode::OK;
		}

		ErrorCode ParseNodeEventMessageId(CANMessage& m, uint8_t &sourceNodeId, uint8_t &eventId)override
		{
			sourceNodeId = (uint8_t)((m.Id & 0x00FF0000) >> 16);
			eventId = (uint8_t)((m.Id & 0x0000FF00) >> 8);
			return ErrorCode::OK;
		}
		ErrorCode ParseNodeCommandMessageId(CANMessage& m, uint8_t &destinationNodeId, uint8_t &commandId)override
		{
			destinationNodeId = (uint8_t)((m.Id & 0x00FF0000) >> 16);
			commandId = (uint8_t)((m.Id & 0x0000FF00) >> 8);
			return ErrorCode::OK;
		}
		ErrorCode ParseNodeCommandAcknowledgeMessageId(CANMessage& m, uint8_t &acknowledgingNodeId, uint8_t &commandId)override
		{
			acknowledgingNodeId = (uint8_t)((m.Id & 0x00FF0000) >> 16);
			commandId = (uint8_t)((m.Id & 0x0000FF00) >> 8);
			return ErrorCode::OK;
		}
		ErrorCode ParseApplicationEventMessageId(CANMessage& m, uint16_t &sourceAppId, uint8_t &eventId) override
		{
			sourceAppId = (uint16_t)((m.Id & 0x00FFFF00) >> 8);
			eventId = (uint8_t)((m.Id & 0x000000FF) >> 0);
			return ErrorCode::OK;
		}

		ErrorCode ParseApplicationCommandMessageId(CANMessage& m, uint16_t &destinationAppId, uint8_t &commandId)
		{
			destinationAppId = (uint16_t)((m.Id & 0x00FFFF00) >> 8);
			commandId = (uint8_t)((m.Id & 0x000000FF) >> 0);
			return ErrorCode::OK;
		}

		ErrorCode ParseCommandAcknowledgeMessageId(CANMessage& m, uint16_t &acknowledgingdestinationAppId, uint8_t &commandId)
		{
			acknowledgingdestinationAppId = (uint16_t)((m.Id & 0x00FFFF00) >> 8);
			commandId = (uint8_t)((m.Id & 0x000000FF) >> 0);
			return ErrorCode::OK;
		}

		void TraceCommandMessage(CANMessage &m){
			u16 appId;
			u8 commandId;        
			ParseApplicationCommandMessageId(m, appId, commandId);
			char charbuf[2*m.DataLen+1];
            for(int i=0;i<m.DataLen;i++)
            {
                sprintf(&charbuf[2*i], "%02X, ", m.Data[i]);
            }
            charbuf[2*m.DataLen]='\0';
            LOGI(TAG, "ApplicationCommand to id:%s; command:%d; len:%d; payload: 0x%s", sensact::model::applications::ApplicationNames[appId], commandId, m.DataLen, charbuf);
		}

		void TraceEventMessage(CANMessage &m){
			u16 appId;
			u8 eventId; 
			ParseApplicationEventMessageId(m, appId, eventId);
			char charbuf[2*m.DataLen+1];
			for(int i=0;i<m.DataLen;i++)
			{
				sprintf(&charbuf[2*i], "%02X, ", m.Data[i]);
			}
            charbuf[2*m.DataLen]='\0';
            LOGI(TAG, "ApplicationEvent from id:%s; command:%d; len:%d; payload: 0x%s", sensact::model::applications::ApplicationNames[appId], eventId, m.DataLen, charbuf);
		}
	};
}
#undef TAG

#include "nodemaster.hh"

#include <esp_log.h>
#define TAG "NDMSTR"



namespace sensact {

enum struct eMQTTState
{
	DEFAULT,
	CONNECTION_STARTED,
	REGISTRATION_STARTED,
	READY,
};

enum struct eMqttTopic
{
	APP_EVENT=0,
	NODE_EVENT=1,
	APP_STATUS = 2
};

const char * const cMaster::mqttTopicNames[]={
		"APP_EVENT",
		"NODE_EVENT",
		"APP_STATUS",
};

const char * const incomingMqttTopicNames[]={
		"APP_COMMAND",
		"NODE_COMMAND",
};

volatile eMQTTState mqttState = eMQTTState::DEFAULT;
CANMessage cMaster::rcvMessage;
eApplicationID cMaster::heartbeatBuffer[3] = {eApplicationID::NO_APPLICATION, eApplicationID::NO_APPLICATION, eApplicationID::NO_APPLICATION, };
Time_t cMaster::lastSentCANMessage = 0;


/*
 * Avoids sending the save heartbeat message over and over if the same standby controller is requested from several apps of this node
 */
void Nodemaster::BufferHeartbeat(eApplicationID target, Time_t now)
{
	for(uint8_t i=0;i<COUNTOF(heartbeatBuffer);i++)
	{
		if(heartbeatBuffer[i] == eApplicationID::NO_APPLICATION)
		{
			heartbeatBuffer[i]=target;
			return;
		}
		if(heartbeatBuffer[i] == target)
		{
			return;
		}

	}
	LOGW("Heartbeat buffer overflow. Sending heartbeat directly...");
	cApplication::SendHEARTBEATCommand(target, (uint32_t)MODEL::NodeMasterApplication, now);
}


void Nodemaster::Run(HAL& hal, MODEL& model, MQTTClient& mqtt) {
	BSP::Init();
	Time_t now = BSP::GetSteadyClock();

	PublishNodeEvent(now, MODEL::NodeID, eNodeEventType::NODE_STARTED, 0, 0);

	uint16_t appId = 0;
	uint16_t appCnt=0;
	uint16_t nextStatusApp=0;
	uint32_t statusBufferU32[STATUSBUFFER_LENGTH_BYTES/4]; //use u32 to have it 4byte-aligned
	uint8_t* statusBuffer = (uint8_t*)statusBufferU32;
	size_t statusBufferLength=0;
	eAppCallResult appResult;


	//start at "1" here, because "0" is the global master application, then starting at 1, the node applications start
	for (appId = 1; appId < (uint16_t) eApplicationID::CNT; appId++) {
		cApplication * const ap = MODEL::Glo2locCmd[appId];
		if (!ap) continue;
		appResult = ap->Setup();
		if((uint8_t)appResult<(uint8_t)eAppCallResult::ERROR_GENERIC)
		{
			LOGI(TAG, "App %s successfully configured", MODEL::ApplicationNames[appId]);
		}
		else
		{
			LOGE("Error while configuring App %s. Error is %u", MODEL::ApplicationNames[appId], appResult);
		}
		statusBuffer[0]=(uint8_t)appResult;
		PublishApplicationStatus(now, (eApplicationID)appId, eApplicationStatus::STARTED, statusBuffer, 8);
		appCnt++;
	}
	LOGI("%u local applications have been configured. Now, %s is pleased to be at your service.\r\n", appCnt, MODEL::ModelString);
	PublishNodeEvent(now, MODEL::NodeID, eNodeEventType::NODE_READY, 0, 0);

	while(MODEL::Glo2locCmd[nextStatusApp]==0)
	{
		nextStatusApp++;
	}
	while (true) {

		now = BSP::GetSteadyClock();
		for (appId = 0; appId < (uint16_t) eApplicationID::CNT; appId++) {
			cApplication * const ap = MODEL::Glo2locCmd[appId];
			if (!ap) continue;
			appResult = ap->DoEachCycle(now, statusBuffer, &statusBufferLength);
#ifdef MASTERNODE
			for(int i = 0;i<statusBufferLength && i <STATUSBUFFER_LENGTH_BYTES;i++)
			{
				MODEL::applicationStatus[appId][i]=rcvMessage.Data[i];
			}
#endif

			if((uint8_t)appResult<(uint8_t)eAppCallResult::ERROR_GENERIC)//means: no error
			{
				if(appResult != eAppCallResult::OK)
				{
					if(statusBufferLength>0)
					{
						PublishApplicationStatus(now, (eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
					}
				}
				if(now-lastSentCANMessage>1000 && appId == nextStatusApp)
				{
					if(statusBufferLength>0)
					{
						PublishApplicationStatus(now, (eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
					}

					do{
						nextStatusApp++;
						if(nextStatusApp==(uint16_t) eApplicationID::CNT)
						{
							nextStatusApp=0;
						}
					}
					while(MODEL::Glo2locCmd[nextStatusApp]==0);
				}

			}
			else
			{
				PublishApplicationStatus(now, (eApplicationID)appId, eApplicationStatus::ERROR_ON_CYCLIC, statusBuffer, 8);
			}


		}

#ifdef MASTERNODE
		MX_LWIP_Process();
#endif


		if(BufferHasMessage)
		{
			cShell::processCmds((uint8_t*)UART_cmdBuffer, UART_buffer_pointer);
			for(uint16_t i=0;i<UART_buffer_pointer;i++)
			{
				UART_cmdBuffer[i]='\0';
			}
			UART_buffer_pointer=0;
			BufferHasMessage=false;
		}
		CanBusProcess();
		BSP::DoEachCycle(now);
		//Reset heartbeat buffer;
		for(uint8_t i=0;i<COUNTOF(heartbeatBuffer);i++)
		{
			if(heartbeatBuffer[i]== eApplicationID::NO_APPLICATION) break;
			LOGD("Flushing heartbeat buffer with target %s", cApplication::N4I((uint32_t const)heartbeatBuffer[i]));
			cApplication::SendHEARTBEATCommand(heartbeatBuffer[i], (uint32_t)MODEL::NodeMasterApplication, now);
			heartbeatBuffer[i] = eApplicationID::NO_APPLICATION;
		}
		while (BSP::GetSteadyClock()-now < 20) {
			CanBusProcess();
		}
#ifdef MASTERNODE
		MX_LWIP_Process();
#endif
	}
}
#ifdef MASTERNODE
void cMaster::mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
	err_t err;
	if(status == MQTT_CONNECT_ACCEPTED) {
		LOGI("mqtt_connection_cb: Successfully connected\n");

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, cMaster::mqtt_incoming_publish_cb, cMaster::mqtt_incoming_data_cb, arg);
		mqttState=eMQTTState::REGISTRATION_STARTED;
		subscriberIndex=0;
		if(COUNTOF(incomingMqttTopicNames)>subscriberIndex)
		{
			LOGI("mqtt_connection_cb: Requesting subscription for '%s'", incomingMqttTopicNames[subscriberIndex]);
			err = mqtt_subscribe(client, incomingMqttTopicNames[subscriberIndex], 1, cMaster::mqtt_sub_request_cb, arg);
			if(err != ERR_OK) {
				LOGE("first mqtt_subscribe for '%s' return: %d\n", incomingMqttTopicNames[subscriberIndex], err);
			}
			subscriberIndex++;
		}
	} else {
		LOGE("mqtt_connection_cb: Disconnected, reason: %d\n", status);
	}
}

void cMaster::mqtt_sub_request_cb(void *arg, err_t result)
{
	err_t err;
	if(COUNTOF(incomingMqttTopicNames)>subscriberIndex)
	{
		LOGI("mqtt_sub_request_cb: Requesting subscription for '%s'", incomingMqttTopicNames[subscriberIndex]);
		err = mqtt_subscribe(&client, incomingMqttTopicNames[subscriberIndex], 1, cMaster::mqtt_sub_request_cb, arg);
		if(err != ERR_OK) {
			LOGE("subsequent mqtt_subscribe for '%s' return: %d\n", incomingMqttTopicNames[subscriberIndex], err);
		}
		subscriberIndex++;
	}
	else
	{
		mqttState=eMQTTState::READY;
		LOGI("%d Subscriptions completed\n", (subscriberIndex));
	}
}


/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
 */
uint32_t cMaster::inpub_id=0;
void cMaster::mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
	LOGD("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);
	for(int i=0; i<COUNTOF(incomingMqttTopicNames); i++)
	{
		if(strcmp(topic, incomingMqttTopicNames[i]) == 0) {
			inpub_id=i;
		}
	}
}

void cMaster::mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	LOGI("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

	if(flags & MQTT_DATA_FLAG_LAST) {
		/* Last fragment of payload received (or whole part if payload fits receive buffer
       See MQTT_VAR_HEADER_BUFFER_LEN)  */

		/* Call function or do action depending on reference, in this case inpub_id */
		if(inpub_id == 0) {
			/* Don't trust the publisher, check zero termination */
			if(data[len-1] == 0) {
				LOGI("mqtt_incoming_data_cb: %s\n", (const char *)data);
			}
		} else if(inpub_id == 1) {
			/* Call an 'A' function... */
		} else {
			LOGI("mqtt_incoming_data_cb: Ignoring payload...\n");
		}
	} else {
		/* Handle fragmented payload, store in buffer, write to file or whatever */
	}
}


//qos At most once (0), At least once (1), Exactly once (2).
void cMaster::mqtt_publishOnTopic(eMqttTopic topic, u8 *buf, size_t len, void *arg, uint8_t qos)
{
	err_t err;
	u8_t retain = 0;
	char charbuf[2*len+1];
	for(int i=0;i<len;i++)
	{
		sprintf(&charbuf[2*i], "%02X, ", buf[i]);
	}
	charbuf[2*len]='\0';

	LOGI("Publishing on topic %s with payload %s", mqttTopicNames[(u8)topic], charbuf);
	err = mqtt_publish(&cMaster::client, mqttTopicNames[(u8)topic], buf, len, qos, retain, cMaster::mqtt_pub_request_cb, arg);
	if(err != ERR_OK) {
		LOGE("Publish err: %d\n", err);
	}
}

/* Called when publish is complete either with sucess or failure */
void cMaster::mqtt_pub_request_cb(void *arg, err_t result)
{
		LOGI("Publish result: %d\n", result);

}
#endif
void cMaster::OnCommand(eCommandType command, uint8_t *data, uint8_t dataLenght, Time_t now)
{
	UNUSED(command);
	UNUSED(data);
	UNUSED(dataLenght);
	UNUSED(now);
}


bool cMaster::SendCommandToMessageBus(Time_t now, eApplicationID destinationApp, eCommandType cmd,
		const uint8_t * const payload, const uint8_t payloadLength) {
	if(destinationApp==eApplicationID::NO_APPLICATION)
	{
		return true;
	}
	if ((uint16_t) destinationApp < (uint16_t)eApplicationID::CNT) {
		cApplication * const app = MODEL::Glo2locCmd[(uint16_t) destinationApp];
		if (app != NULL) {
			//only in this case, the message can be processed local; no need to send it to can
			app->OnCommand(cmd, payload, payloadLength, now);
			return true;
		} else {
			lastSentCANMessage=now;
			uint32_t canid=cCanIdUtils::CreateCommandMessageId((u16)destinationApp, (u8)cmd);
#ifdef NEW_CANID
			return BSP::SendCANMessage(canid, payload, payloadLength);
#else
			if(payloadLength>7)
			{
				LOGE("PAYLOAD TOO LARGE");
			}
			uint8_t buf[8];
			buf[0]=(uint8_t)cmd;
			for(int i=0;i<payloadLength;i++)
			{
				buf[i+1]=payload[i];
			}
			return BSP::SendCANMessage(canid, buf, payloadLength+1);
#endif

		}
	}
	LOGE("Trying to send to an invalid application id %i", (uint16_t)destinationApp);
	return false;
}


void cMaster::PublishNodeEvent(Time_t now, eNodeID sourceNode, eNodeEventType event, uint8_t * payload, uint8_t payloadLength)
{
	lastSentCANMessage=now;
	BSP::SendCANMessage(cCanIdUtils::CreateNodeEventMessageId((u8)sourceNode, (u8)event), payload, payloadLength);
#ifdef MASTERNODE
	cMaster::mqtt_publishOnTopic(eMqttTopic::NODE_EVENT, payload, payloadLength, 0, 1);
#endif
}

void cMaster::PublishApplicationEvent(Time_t now, eApplicationID sourceApp, eEventType event, uint8_t * payload, uint8_t payloadLength)
{
	lastSentCANMessage=now;
	BSP::SendCANMessage(cCanIdUtils::CreateEventMessageId((u16)sourceApp, (u8)event), payload, payloadLength);
#ifdef MASTERNODE
	uint8_t buf[11];
	Common::WriteInt16((u16)sourceApp, buf, 0);
	buf[2]=(u8)event;
	for(int i=0;i<payloadLength;i++)
	{
		buf[i+3]=payload[i];
	}
	cMaster::mqtt_publishOnTopic(eMqttTopic::APP_EVENT, buf, 3+payloadLength, 0, 1);
#endif
}

void cMaster::PublishApplicationStatus(Time_t now, eApplicationID sourceApp, eApplicationStatus statusType, uint8_t * payload, uint8_t payloadLength)
{
	lastSentCANMessage=now;
	BSP::SendCANMessage(cCanIdUtils::CreateApplicationStatusMessageId((u16)sourceApp, (u8)statusType), payload, payloadLength);
#ifdef MASTERNODE
	uint8_t buf[11];
	Common::WriteInt16((u16)sourceApp, buf, 0);
	buf[2]=(u8)statusType;
	for(int i=0;i<payloadLength;i++)
	{
		buf[i+3]=payload[i];
	}
	cMaster::mqtt_publishOnTopic(eMqttTopic::APP_STATUS, buf, 3+payloadLength, 0, 1);
#endif
}


void cMaster::PublishApplicationEventFiltered(Time_t now, const eApplicationID sourceApp, const eEventType evt, const eEventType *const localEvts, const uint8_t localEvtsLength, const eEventType *const busEvts, const uint8_t busEvtsLength, uint8_t * payload, uint8_t payloadLength)
{
	uint8_t i;

	for(i=0;i<localEvtsLength;i++)
	{
		eEventType test = localEvts[i];
		if(evt==test)
		{
			cApplication *app = MODEL::Glo2locEvt[(uint16_t)sourceApp];
			if (app != NULL) {
				app->OnEvent(sourceApp, evt, payload, payloadLength, now);
			}
			break;
		}
	}
	for(i=0;i<busEvtsLength;i++)
	{
		eEventType test = busEvts[i];
		if(evt==test)
		{
			PublishApplicationEvent(now, sourceApp, evt, payload, payloadLength);
			break;
		}
	}

}

void cMaster::CanBusProcess() {

	uint16_t appId;
	uint8_t commandOrEventId;
	Time_t now = BSP::GetSteadyClock();
	cApplication *app;
	while (BSP::ReceiveCANMessage(&rcvMessage))
	{
		eCanMessageType type = cCanIdUtils::ParseCanMessageType(rcvMessage.Id);
		switch(type)
		{
		case eCanMessageType::ApplicationCommand:
			cCanIdUtils::ParseCommandMessageId(rcvMessage.Id, &appId, &commandOrEventId, rcvMessage.Data[0]);
			if(appId >= (uint32_t)eApplicationID::CNT)
			{
				LOGW("Received ApplicationCommand. Unknown applicationID %i", appId);
				return;
			}
			if(MODEL::TRACE_COMMANDS)
			{
				char charbuf[2*rcvMessage.Length+1];
				for(int i=0;i<rcvMessage.Length;i++)
				{
					sprintf(&charbuf[2*i], "%02X, ", rcvMessage.Data[i]);
				}
				charbuf[2*rcvMessage.Length]='\0';
#ifdef NEW_CANID
				LOGX("ApplicationCommand to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length, charbuf);
#else
				LOGX("ApplicationCommand (old CAN-ID) to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length, charbuf);
#endif
			}
			app = MODEL::Glo2locCmd[appId];
			if (app != NULL) {
#ifdef NEW_CANID
				app->OnCommand((eCommandType)commandOrEventId, rcvMessage.Data, rcvMessage.Length, now);
#else
				app->OnCommand((eCommandType)commandOrEventId, &rcvMessage.Data[1], rcvMessage.Length-1, now);
#endif
			}
			break;
		case eCanMessageType::ApplicationEvent:
			cCanIdUtils::ParseEventMessageId(rcvMessage.Id, &appId, &commandOrEventId);
			if(appId >= (uint32_t)eApplicationID::CNT)
			{
				LOGW("Unknown applicationID %i", appId);
				return;
			}
			if(MODEL::TRACE_EVENTS)
			{
				char charbuf[2*rcvMessage.Length+1];
				for(int i=0;i<rcvMessage.Length;i++)
				{
					sprintf(&charbuf[2*i], "%02X, ", rcvMessage.Data[i]);
				}
				charbuf[2*rcvMessage.Length]='\0';
#ifdef NEW_CANID
				LOGD("ApplicationEvent to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length, charbuf);
#else
				LOGD("ApplicationEvent (old CAN-ID) to id:%s; command:%d; len:%d; payload: 0x%s", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length, charbuf);
#endif
			}
			app = MODEL::Glo2locEvt[appId];
			if (app != NULL) {
				app->OnEvent((eApplicationID) appId, (eEventType)commandOrEventId,  rcvMessage.Data, rcvMessage.Length, now);
			}
			break;
		case eCanMessageType::ApplicationStatus:
#ifdef MASTERNODE
			cCanIdUtils::ParseApplicationStatusMessageId(rcvMessage.Id, &appId, &commandOrEventId);
			if(appId >= (uint32_t)eApplicationID::CNT)
			{
				LOGW("Received ApplicationStatus; unknown applicationID %i", appId);
				return;
			}

			Common::WriteInt16(appId, buf, 0);
			buf[2]=commandOrEventId;
			for(int i=0;i<rcvMessage.Length;i++)
			{
				MODEL::applicationStatus[appId][i]=rcvMessage.Data[i];
				buf[i+3]=rcvMessage.Data[i];
			}
			cMaster::mqtt_publishOnTopic(eMqttTopic::APP_STATUS, buf, 3+rcvMessage.Length, 0, 1);

#endif
			break;
		default: LOGW("Unknown application type %d", type); break;
		}
	}
}







}

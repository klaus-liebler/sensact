/*
 * homeconrol_master.c
 *
 *  Created on: 31.03.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */
#include <cMaster.h>
#include "console.h"
#include "cModel.h"
#define LOGLEVEL LEVEL_DEBUG
#include "shell.h"
#define LOGNAME "MASTR"
#include "cLog.h"
#include "date.h"
#ifdef STM32F1
#include "flash_info.h"
#endif
#ifdef MASTERNODE
#include "lwip.h"
#include "lwip/apps/httpd.h"
#include "mqtt.h"
#include <string.h>
#endif

volatile uint8_t UART_buffer_pointer=0;
volatile uint64_t UART_cmdBuffer64[16];
volatile uint8_t *UART_cmdBuffer = (uint8_t *)UART_cmdBuffer64;
volatile bool BufferHasMessage=false;

using namespace date;
using namespace std::chrono;

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

namespace sensact {

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


CANMessage cMaster::rcvMessage;
pIapPseudoFunction cMaster::JumpToApplication;
uint32_t cMaster::JumpAddress;
eApplicationID cMaster::heartbeatBuffer = eApplicationID::NO_APPLICATION;
Time_t cMaster::lastSentCANMessage = 0;

#ifdef MASTERNODE
struct mqtt_connect_client_info_t cMaster::ci;
mqtt_client_t cMaster::client;

#endif

/*
 * Avoids sending the save heartbeat message over and over if the same standby controller is requested from several apps of this node
 */
void cMaster::BufferHeartbeat(eApplicationID target, Time_t now)
{
	if(heartbeatBuffer != eApplicationID::NO_APPLICATION && heartbeatBuffer != target)
	{
		cApplication::SendHEARTBEATCommand(heartbeatBuffer, (uint32_t)MODEL::NodeMasterApplication, now);
	}
	heartbeatBuffer=target;
}


void cMaster::StartIAP()
{
#ifdef STM32F1
	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	if (((*(__IO uint32_t*)IAP_APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (IAP_APPLICATION_ADDRESS + 4);
		JumpToApplication = (pIapPseudoFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) IAP_APPLICATION_ADDRESS);
		JumpToApplication();
	}
#endif
}

void cMaster::Run(void) {
	BSP::Init();
#ifdef MASTERNODE
	MX_LWIP_Init();
	httpd_init();
	ip4_addr_t ip_addr;
	IP4_ADDR(&ip_addr, 192, 168, 1, 1);
	err_t err;

	// Setup an empty client info structure
	memset(&ci, 0, sizeof(ci));
	ci.client_id = "lwip_test";
	err = mqtt_client_connect(&client, &ip_addr, MQTT_PORT, cMaster::mqtt_connection_cb, 0, &ci);
#endif
	uint16_t i = 0;
	uint16_t appCnt=0;
	uint16_t statusApp=0;
	uint32_t statusBufferU32[16]; //use u32 to have it 4byte-aligned
	uint8_t* statusBuffer = (uint8_t*)statusBufferU32;
	size_t statusBufferLength=0;
	eAppResult appResult;
	Time_t now = BSP::GetSteadyClock();

	//start at "1" here, because "0" is the local/global master
	for (i = 1; i < (uint16_t) eApplicationID::CNT; i++) {
		cApplication * const ap = MODEL::Glo2locCmd[i];
		if (ap) {
			appResult = ap->Setup();
			if(appResult==eAppResult::OK)
			{
				LOGI("App %s successfully configured", MODEL::ApplicationNames[i]);
			}
			else
			{
				LOGE("Error while configuring App %s. Error is %u", MODEL::ApplicationNames[i], appResult);
			}
			statusBuffer[0]=(u8)appResult;
			PublishApplicationStatus(now, (eApplicationID)i, eApplicationStatus::STARTED, statusBuffer, 1);
			appCnt++;
		}
	}
	LOGI("%u local applications have been configured. Now, %s is pleased to be at your service.\r\n", appCnt, MODEL::ModelString);
	bool published=false;
	while (true) {

		now = BSP::GetSteadyClock();
		MX_LWIP_Process();

		if(mqtt_client_is_connected(&client) && !published)
		{
			uint8_t text[] = {1,2,3,4,5};
			mqtt_publishOnTopic(eMqttTopic::NODE_EVENT, text, 5, 0);
			published=true;
		}

		for (i = 0; i < (uint16_t) eApplicationID::CNT; i++) {
			cApplication * const ap = MODEL::Glo2locCmd[i];
			if (ap) {
				appResult = ap->DoEachCycle(now, statusBuffer, &statusBufferLength);
				//CanBusProcess();
			}
#ifndef MASTERNODE1
			if(now-lastSentCANMessage>1000 && i > statusApp && statusBufferLength>0)
			{
				PublishApplicationStatus(now, (eApplicationID)i, eApplicationStatus::REGULAR_STATUS, statusBuffer, statusBufferLength);
				statusApp=i;
			}
#endif
		}

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
#ifdef DUMP_FRONTEND
		static uint32_t lastAllInputs = 0;
		static uint16_t lastRot1Value = 0;
		static uint16_t lastRot2Value=0;
		uint32_t inputs = BSP::GetAllOnboardInputsLowLevel();
		if(inputs != lastAllInputs)
		{
			SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::INPUT_CHANGED, (uint8_t*)&inputs, 4);
		}
		lastAllInputs=inputs;

		uint16_t rot1val = BSP::GetRotaryEncoderValue(eRotaryEncoder::ROTARYENCODER_1);
		if(rot1val!=lastRot1Value)
		{
			int16_t change =  rot1val-lastRot1Value;
			cMaster::SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::TURNED, (uint8_t*)&change ,2);
		}
		lastRot1Value=rot1val;

		uint16_t rot2val = BSP::GetRotaryEncoderValue(eRotaryEncoder::ROTARYENCODER_2);
		if(rot2val!=lastRot2Value)
		{
			int16_t change =  rot2val-lastRot2Value;
			cMaster::SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::TURNED, (uint8_t*)&change ,2);
		}
		lastRot2Value=rot2val;
#endif
		BSP::DoEachCycle(now);
		//special call to release and reset message
		BufferHeartbeat(eApplicationID::NO_APPLICATION, now);
		while (BSP::GetSteadyClock()-now < 20) {
			CanBusProcess();
		}
	}
}

void cMaster::mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
	err_t err;
	if(status == MQTT_CONNECT_ACCEPTED) {
		LOGI("mqtt_connection_cb: Successfully connected\n");

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, cMaster::mqtt_incoming_publish_cb, cMaster::mqtt_incoming_data_cb, arg);

		/* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
		err = mqtt_subscribe(client, "subtopic", 1, cMaster::mqtt_sub_request_cb, arg);

		if(err != ERR_OK) {
			LOGE("mqtt_subscribe return: %d\n", err);
		}
	} else {
		LOGE("mqtt_connection_cb: Disconnected, reason: %d\n", status);

	}
}

void cMaster::mqtt_sub_request_cb(void *arg, err_t result)
{
	/* Just print the result code here for simplicity,
     normal behaviour would be to take some action if subscribe fails like
     notifying user, retry subscribe or disconnect from server */
	LOGI("Subscribe result: %d\n", result);
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
				printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
			}
		} else if(inpub_id == 1) {
			/* Call an 'A' function... */
		} else {
			printf("mqtt_incoming_data_cb: Ignoring payload...\n");
		}
	} else {
		/* Handle fragmented payload, store in buffer, write to file or whatever */
	}
}


void cMaster::mqtt_publishOnTopic(eMqttTopic topic, u8 *buf, size_t len, void *arg)
{
	err_t err;
	u8_t qos = 2; /* 0 1 or 2, see MQTT specification */
	u8_t retain = 0; /* No don't retain such crappy payload... */
	err = mqtt_publish(&cMaster::client, mqttTopicNames[(u8)topic], buf, len, qos, retain, cMaster::mqtt_pub_request_cb, arg);
	if(err != ERR_OK) {
		printf("Publish err: %d\n", err);
	}
}

/* Called when publish is complete either with sucess or failure */
void cMaster::mqtt_pub_request_cb(void *arg, err_t result)
{
	if(result != ERR_OK) {
		printf("Publish result: %d\n", result);
	}
}

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
			return BSP::SendCANMessage(canid, payload, payloadLength);
		}
	}
	LOGE("Trying to send to an invalid application id %i", (uint16_t)destinationApp);
	return false;
}

/*
Download zur MASTERNODE: Commands an die Node (insbes Abfrage von Statuus), Command an Apps
Upload von der MASTERNODE: Returns von Commands, Events von Nodes und Apps
 *
 *
 */

//Das sind einfach nur AppEvents
//SLAVE-Nodes write it to the CAN, Masternode writes it to can and MQTT
void cMaster::PublishNodeEvent(Time_t now, eNodeID sourceNode, eNodeEventType event, uint8_t * payload, uint8_t payloadLength)
{
	lastSentCANMessage=now;
	BSP::SendCANMessage(cCanIdUtils::CreateNodeEventMessageId((u8)sourceNode, (u8)event), payload, payloadLength);
#ifdef MASTERNODE
	cMaster::mqtt_publishOnTopic(eMqttTopic::NODE_EVENT, payload, payloadLength, 0);
#endif
}

void cMaster::PublishApplicationEvent(Time_t now, eApplicationID sourceApp, eEventType event, uint8_t * payload, uint8_t payloadLength)
{
	lastSentCANMessage=now;
	BSP::SendCANMessage(cCanIdUtils::CreateEventMessageId((u16)sourceApp, (u8)event), payload, payloadLength);
#ifdef MASTERNODE
	cMaster::mqtt_publishOnTopic(eMqttTopic::APP_EVENT, payload, payloadLength, 0);
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
	for(int i=0;i<8;i++)
	{
		buf[i+3]=rcvMessage.Data[i];
	}
	cMaster::mqtt_publishOnTopic(eMqttTopic::APP_STATUS, payload, payloadLength, 0);
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

#define HIGHEST_CAN_BIT 0x10000000
void cMaster::CanBusProcess() {

	uint16_t appId;
	uint8_t commandOrEventId;
	Time_t now = BSP::GetSteadyClock();
	cApplication *app;
	uint8_t buf[11];
	while (BSP::ReceiveCANMessage(&rcvMessage))
	{
		eCanMessageType type = cCanIdUtils::ParseCanMessageType(rcvMessage.Id);
		switch(type)
		{
		case eCanMessageType::ApplicationCommand:
			cCanIdUtils::ParseCommandMessageId(rcvMessage.Id, &appId, &commandOrEventId);
			if(appId >= (uint32_t)eApplicationID::CNT)
			{
				LOGW("Unknown applicationID %i", appId);
				return;
			}
			if(MODEL::TRACE_COMMANDS)
			{
				LOGX("Command to id:%s; command:%d; len:%d; payload:", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length);
				for (int i = 0; i < rcvMessage.Length; i++)
				{
					Console::Write("0x%02X, ", rcvMessage.Data[i]);
				}
				Console::Writeln("");
			}
			app = MODEL::Glo2locCmd[appId];
			if (app != NULL) {
				app->OnCommand((eCommandType)commandOrEventId, rcvMessage.Data, rcvMessage.Length, now);
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
				LOGX("Event from id:%s; event:%d; len:%d; payload:", MODEL::ApplicationNames[appId], commandOrEventId, rcvMessage.Length);
				for (int i = 0; i < rcvMessage.Length; i++)
				{
					Console::Write("0x%02X, ", rcvMessage.Data[i]);
				}
				Console::Writeln("");
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
				LOGW("Unknown applicationID %i", appId);
				return;
			}

			Common::WriteInt16(appId, buf, 0);
			buf[2]=commandOrEventId;
			for(int i=0;i<8;i++)
			{
				MODEL::applicationStatus[appId][i]=rcvMessage.Data[i];
				buf[i+3]=rcvMessage.Data[i];
			}
			cMaster::mqtt_publishOnTopic(eMqttTopic::APP_STATUS, buf, 10, 0);

#endif
			break;
		default: LOGW("Unknown application type %d", type); break;
		}
	}
}

std::chrono::hours
cMaster::utc_offset_cet(std::chrono::system_clock::time_point tp)
{
	constexpr auto CET = hours(1);
	constexpr auto CEST = hours(2);
	const auto y = year_month_day(floor<days>(tp)).year();
	const auto begin = day_point(sun[last]/mar/y) + hours(1); // CEST begins at this UTC time
	const auto end   = day_point(sun[last]/10/y) + hours(1); // CEST ends at this UTC time
	if (tp < begin || end <= tp)
		return CET;
	return CEST;
}

std::chrono::hours
cMaster::cet_offset_utc(std::chrono::system_clock::time_point tp)
{

	constexpr auto CET = -hours(1);
	constexpr auto CEST = -hours(2);
	const auto y = year_month_day(floor<days>(tp)).year();
	const auto begin = day_point(sun[last]/mar/y) + hours(2); // CEST begins at this CET time
	const auto end   = day_point(sun[last]/10/y) + hours(3); // CEST ends at this CET time
	if (tp < begin || end <= tp)
		return CET;
	return CEST;
}


Time_t cMaster::Date2unixtimeMillis(uint16_t jahr, uint8_t monat, uint8_t tag, uint8_t stunde, uint8_t minute, uint8_t sekunde)
{
	const uint16_t tage_seit_jahresanfang[12] = /* Anzahl der Tage seit Jahresanfang ohne Tage des aktuellen Monats und ohne Schalttag */
	{0,31,59,90,120,151,181,212,243,273,304,334};

	int schaltjahre = ((jahr-1)-1968)/4 /* Anzahl der Schaltjahre seit 1970 (ohne das evtl. laufende Schaltjahr) */
			- ((jahr-1)-1900)/100
			+ ((jahr-1)-1600)/400;

	long tage_seit_1970 = (jahr-1970)*365 + schaltjahre
			+ tage_seit_jahresanfang[monat-1] + tag-1;

	if ( (monat>2) && (jahr%4==0 && (jahr%100!=0 || jahr%400==0)) )
		tage_seit_1970 += 1; /* +Schalttag, wenn jahr Schaltjahr ist */
	return (sekunde + 60 * ( minute + 60 * (stunde + 24*tage_seit_1970) ))*1000;
}
}

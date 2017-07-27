/*
 * homecontrol_master.h
 *
 *  Created on: 31.03.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#pragma once
#include "common.h"
#include "stdint.h"
#include "stdbool.h"
#include "cBsp.h"
#include "cCanIdUtils.h"
#include <chrono>
#include "cModel.h"
#ifdef MASTERNODE
#include "mqtt.h"
#endif

namespace sensact {

enum struct eMqttTopic;

typedef  void (*pIapPseudoFunction)(void);

class cMaster {
private:
	static const char * const mqttTopicNames[];
	static CANMessage rcvMessage;
	static pIapPseudoFunction JumpToApplication;
	static uint32_t JumpAddress;
	static eApplicationID heartbeatBuffer;
	static Time_t lastSentCANMessage;

	static uint32_t inpub_id;
#ifdef MASTERNODE
	static mqtt_client_t client;
	static struct mqtt_connect_client_info_t ci;
	static uint32_t subscriberIndex;


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
	static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
	static void mqtt_sub_request_cb(void *arg, err_t result);
	static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
	static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
	static void mqtt_publishOnTopic(eMqttTopic topic, uint8_t *buf, size_t len, void *arg, uint8_t qos);
	static void mqtt_pub_request_cb(void *arg, err_t result);
#endif
public:
	static void MasterControlLoop(void);
	static bool SendCommandToMessageBus(Time_t now, eApplicationID destinationApp, eCommandType cmd, const uint8_t * const payload, uint8_t payloadLength);
	static void PublishApplicationEventFiltered(Time_t now, const eApplicationID, const eEventType evt, const eEventType *const localEvts, const uint8_t localEvtsLength, const eEventType *const busEvts, const uint8_t busEvtsLength, uint8_t* payload, uint8_t payloadLenght);
	static void PublishNodeEvent(Time_t now, eNodeID sourceNode, eNodeEventType event, uint8_t * payload, uint8_t payloadLength);
	static void PublishApplicationEvent(Time_t now, eApplicationID sourceNode, eEventType event, uint8_t * payload, uint8_t payloadLength);
	static void PublishApplicationStatus(Time_t now, eApplicationID sourceApp, eApplicationStatus statusType, uint8_t * payload, uint8_t payloadLength);
	static void CanBusProcess();
	static void Run();
	static Time_t Date2unixtimeMillis(uint16_t jahr, uint8_t monat, uint8_t tag, uint8_t stunde, uint8_t minute, uint8_t sekunde);
	static std::chrono::hours cet_offset_utc(std::chrono::system_clock::time_point tp);
	static std::chrono::hours utc_offset_cet(std::chrono::system_clock::time_point tp);
	static void StartIAP(void);
	static void BufferHeartbeat(eApplicationID target, Time_t now);
	static void OnCommand(eCommandType command, uint8_t *data, uint8_t dataLenght, Time_t now);
};
}

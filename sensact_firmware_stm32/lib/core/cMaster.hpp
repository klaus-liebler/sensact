#pragma once
#include "common.hpp"
#include "stdint.h"
#include "stdbool.h"
#include "cCanIdUtils.hpp"
#include <chrono>
#include <array>
#include "cModel.hpp"
#include <cApplication.hpp>
#include <node.hpp>
#ifdef MASTERNODE
#include "mqtt.h"
#endif

namespace sensactcore {

enum struct eMqttTopic;

typedef  void (*pIapPseudoFunction)(void);

class cMaster:public IMasterAsApplicationsParent {
private:
	std::array<const char*, 3> mqttTopicNames{"APP_EVENT", "NODE_EVENT", "APP_STATUS"};
	CANMessage rcvMessage;
	pIapPseudoFunction JumpToApplication;
	uint32_t JumpAddress;
	std::array<eApplicationID,10> heartbeatBuffer{eApplicationID::NO_APPLICATION};
	Time_t lastSentCANMessage{0};
	sensacthal::SensactHAL *hal;
	SensactContext currentCtx;

	uint32_t inpub_id;
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
	cMaster(SensactHAL *hal):hal(hal){}
	void MasterControlLoop(void);
	bool SendCommandToMessageBus(eApplicationID destinationApp, eCommandType cmd, const uint8_t * const payload, uint8_t payloadLength);
	void PublishApplicationEventFiltered(const eApplicationID, const eEventType evt, const eEventType *const localEvts, const uint8_t localEvtsLength, const eEventType *const busEvts, const uint8_t busEvtsLength, uint8_t* payload, uint8_t payloadLenght);
	void PublishNodeEvent(eNodeID sourceNode, eNodeEventType event, uint8_t * payload, uint8_t payloadLength);
	void PublishApplicationEvent(eApplicationID sourceNode, eEventType event, uint8_t * payload, uint8_t payloadLength);
	void PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, uint8_t * payload, uint8_t payloadLength);
	void CanBusProcess();
	void Run(sensactcore::INodeAsPhaseTrigger *nodePhase, sensactcore::INodeAsIOProvider *nodeIO);
	Time_t Date2unixtimeMillis(uint16_t jahr, uint8_t monat, uint8_t tag, uint8_t stunde, uint8_t minute, uint8_t sekunde);
	//static std::chrono::hours cet_offset_utc(std::chrono::system_clock::time_point tp);
	//static std::chrono::hours utc_offset_cet(std::chrono::system_clock::time_point tp);
	void StartIAP(void);
	void BufferHeartbeat(eApplicationID target);
	void OnCommand(eCommandType command, uint8_t *data, uint8_t dataLenght);
};
}

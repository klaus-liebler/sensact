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



void Nodemaster::Run(HAL& hal, MODEL& model, MQTTClient& mqtt) {

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

















}

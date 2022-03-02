#pragma once
#include "common.hh"
#include "busmaster.hh"
#include "stdint.h"
#include "stdbool.h"
#include <stdio.h>
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "can_id_utils.hh"
#include <chrono>
#include <vector>
#include "apps/sensactContext.hh"
#include "hal.hh"
#include "node_applicationHost.hh"
#include "node_gateway.hh"
#include "node_iohost.hh"

//Frage: Wie kann eine Statusnachricht, die vom ApplicationHost per CAN versendet wird, noch vom Gateway abgefangen werden. Flag "localDistributionToOtherRoles"?

namespace sensact
{
	class Nodemaster;
	class NodemasterRoleRunner{
		virtual ErrorCode Setup(tms_t now)=0;
		virtual ErrorCode Loop(tms_t now)=0;
		virtual ErrorCode OfferMessage(tms_t now, CANMessage* m)=0;
	}

	enum struct eMqttTopic;

	extern "C" static void NodemasterTask(void *params);

	class Nodemaster
	{
	private:
		std::vector<NodeRole> nodeRoles;
		HAL *const hal;
		std::vector<AbstractBusmaster *> busmasters;
		std::vector<NodemasterRoleRunner*> roleRunners;
		const char *const mqttTopicNames[];
		CANMessage rcvMessage;

		ErrorCode PublishNodeEvent(Time_t now, eNodeID sourceNode, eNodeEventType event, uint8_t * payload, uint8_t payloadLength)
		{
			hal->TrySendCanMessage(CANMessenger::CreateNodeEventMessageId((u8)sourceNode, (u8)event), payload, payloadLength);
		#ifdef MASTERNODE
			cMaster::mqtt_publishOnTopic(eMqttTopic::NODE_EVENT, payload, payloadLength, 0, 1);
		#endif
		}
			

	public:
		Nodemaster(std::vector<NodeRole> nodeRoles, HAL *hal, std::vector<AbstractBusmaster *> busmasters) : nodeRoles(nodeRoles), hal(hal), busmasters(busmasters) {

		}
		void RunEternalLoopInTask(void)
		{
			TaskHandle_t *const taskHandle = NULL;
			xTaskCreate(NodemasterTask, "NodemasterTask", 4096 * 4, this, 6, taskHandle);
		}

		void EternalLoop()
		{
			tms_t now = hal->GetMillisS64();
			hal->Setup();
			PublishNodeEvent(now, MODEL::NodeID, eNodeEventType::NODE_STARTED, 0, 0);
			for(auto& role:nodeRoles){
				switch (role)
				{
				case NodeRole::APPLICATION_HOST:
					roleRunners.push_back(new ApplicationHostRunner());
					break;
				case NodeRole::GATEWAY:
					roleRunners.push_back(new GatewayRunner());
					break;
				case NodeRole::IO_HOST:
					roleRunners.push_back(new IoHostRunner());
					break;
				default:
					LOGE(TAG, "NodeRole has no implementation");
					break;
				}
			}
			for(auto& rr:roleRunners){
				rr->Setup(this);
			}
			PublishNodeEvent(now, MODEL::NodeID, eNodeEventType::NODE_READY, 0, 0);
			
			while(true){
				now = = hal->GetMillisS64();
				hal->BeforeAppLoop();
				CANMessage message;
				while(hal->TryReceiveCANMessage(message)==ErrorCode::OK){
					for(auto& rr:roleRunners) rr->OfferMessage(this, &message);
				}
				// TODO CONSOLEMessage! ->entweder im HAL in CANMessage transformieren oder zweiten TryReceive-Prozess aufsetzen
				for(auto& rr:roleRunners){
					rr->Loop(this);
				}
				hal->AfterAppLoop();//schreibe die Outputs
			}
		}


		static void OnCommand(eCommandType command, uint8_t *data, uint8_t dataLenght, Time_t now);
	};
	static void NodemasterTask(void *params)
	{
		Nodemaster *nm = (Nodemaster *)params;
		nm->EternalLoop();
	}

}

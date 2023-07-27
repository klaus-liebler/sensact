#pragma once
#include <sensact_commons.hh>
#include "busmaster.hh"
#include "stdint.h"
#include <stdbool.h>
#include <stdio.h>
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <chrono>
#include <vector>

#include <hal.hh>
#include <sensact_logger.hh>
#include "node_applicationhost.hh"
#include "node_gatewayhost.hh"
#include "node_iohost.hh"
#include "interfaces.hh"
#include "model_node.hh"


#define TAG "NODEMSTR"

/**
 * @brief
 * Nodemaster ist die höchste Instanz. Weiß nichts von Apps
 * Nodemaster kennt die Busmaster (Bus-Zugriff läuft nicht bzw nicht direkt über die HAL, aber die Busmaster selbst können natürlich die HAL nutzen)
 * Nodemaster kennt die Hosts (ein Host ist der ApplicationHost, in dem die SensactApps laufen)
 * Nodemaster macht den großen Zyklus "HAL_Before" -->Empfang aller CAN-Nachrichten und Angebot an alle Hosts-->Verarbeitung in allen Hosts -->"HAL_After"
 * Nodemaster kann ausgehende CAN-Nachrichten an alle RoleRunner spiegeln.
 */

namespace sensact
{
	constexpr size_t STATUS_MESSAGE_BUFLEN{256};
	class cNodemaster : public iHostContext
	{
	private:
		std::vector<NodeRole> *nodeRoles;
		sensact::hal::iHAL *const hal;
		std::vector<AbstractBusmaster *> *busmasters;
		aCANMessageBuilderParser *canMBP;
		std::vector<iHost *> hosts;
		iHost *currentRoleRunner{nullptr};
		tms_t currentNow{0}; // das "jetzt" soll bei einem Aufruf konstant gehalten werden
		char statusMessageBuffer[STATUS_MESSAGE_BUFLEN]{0};


		ErrorCode PublishNodeEvent(tms_t now, eNodeID sourceNode, eNodeEventType event, uint8_t *payload, uint8_t payloadLength)
		{
			CANMessage m;
			ErrorCode err = canMBP->BuildNodeEventMessage((u8)sourceNode, (u8)event, payload, payloadLength, m);
			if (err != ErrorCode::OK)
			{
				return err;
			}
			return hal->TrySendCanMessage(m);
		}

		void Task()
		{
			TickType_t xLastWakeTime = xTaskGetTickCount();
			const TickType_t xFrequency = pdMS_TO_TICKS(100);
			while (true)
			{
				currentNow = hal->GetMillisS64();
				ESP_LOGD(TAG, "Before Apploop");
				hal->BeforeAppLoop();
				CANMessage message;
				while (hal->TryReceiveCANMessage(message) == ErrorCode::OK)
				{
					for (auto &rr : hosts)
					{
						this->currentRoleRunner = rr;
						rr->OfferMessage(*this, message);
					}
				}
				// TODO CONSOLEMessage! ->entweder im HAL in CANMessage transformieren oder zweiten TryReceive-Prozess aufsetzen
				for (auto &rr : hosts)
				{
					this->currentRoleRunner = rr;
					rr->Loop(*this);
				}
				ESP_LOGD(TAG, "hal->AfterAppLoop(); 1");
				hal->AfterAppLoop();
				ESP_LOGD(TAG, "hal->AfterAppLoop(); 2");
				//check problem and log them
				hal->CheckAndLogHealth();
				ESP_LOGD(TAG, "hal->CheckAndLogHealth();");
				xTaskDelayUntil(&xLastWakeTime, xFrequency);
			}
		}

		static void StaticTask(void *pvParameters)
		{
			cNodemaster *myself = static_cast<cNodemaster *>(pvParameters);
			ESP_LOGI(TAG, "cNodemaster started");
			myself->Task();
		}

	public:
		cNodemaster(std::vector<NodeRole> *nodeRoles, sensact::hal::iHAL *hal, std::vector<AbstractBusmaster *> *busmasters, aCANMessageBuilderParser *canMBP) : nodeRoles(nodeRoles), hal(hal), busmasters(busmasters), canMBP(canMBP)
		{
		}

		void Setup()
		{
			this->currentNow = hal->GetMillisS64();
			hal->Setup();
			for (auto &bm : *this->busmasters)
			{
				if (bm->Setup() != ErrorCode::OK)
				{
					LOGE(TAG, "Busmaster reported error on Setup");
				}
			}
			PublishNodeEvent(currentNow, sensact::model::node::NodeID, eNodeEventType::NODE_STARTED, 0, 0);
			for (auto &role : *nodeRoles)
			{
				iHost *h;
				switch (role)
				{
				case NodeRole::APPLICATION_HOST:
					h = new cApplicationHost(hal, this, this->canMBP);
					hosts.push_back(h);
					break;
				case NodeRole::GATEWAY:
					hosts.push_back(new cGatewayHost());
					break;
				case NodeRole::IO_HOST:
					hosts.push_back(new cIoHost());
					break;
				default:
					LOGE(TAG, "NodeRole has no implementation");
					break;
				}
			}
			this->currentNow = hal->GetMillisS64();
			for (auto &rr : hosts)
			{
				this->currentRoleRunner = rr;
				rr->Setup(*this);
			}
			PublishNodeEvent(currentNow, sensact::model::node::NodeID, eNodeEventType::NODE_READY, 0, 0);

			LOGI(TAG, "All Hosts have been configured. Now, %s is pleased to be at your service.\r\n", sensact::model::node::NodeDescription);
		}

		void RunEternalLoopInTask(void)
		{
			xTaskCreate(StaticTask, "NodemasterTask", 4096 * 4, this, 6, nullptr);
		}

		void PublishOnMessageBus(CANMessage &m, bool distributeLocally) override
		{
			ErrorCode err = hal->TrySendCanMessage(m);
			if (err != ErrorCode::OK)
			{
				LOGE(TAG, "CAN Message couln't be sent out %02X", (int)err);
			}
			else
			{
				LOGD(TAG, "Message successfully sent out %lu", m.Id);
			}
			if (!distributeLocally)
				return;
			for (auto &rr : hosts)
			{
				if (this->currentRoleRunner == rr)
					continue;
				rr->OfferMessage(*this, m);
			}
		}

		ErrorCode SetU16Output(u16 id, uint16_t value) override
		{
			u16 busmasterIndex = id >> 14;
			if (this->busmasters->size() <= busmasterIndex)
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			u16 localIndex = id & 0x3FFF;
			return this->busmasters->at(busmasterIndex)->SetOutput(localIndex, value);
		}
		ErrorCode GetU16Input(u16 id, u16 &value) override
		{
			u16 busmasterIndex = id >> 14;
			if (this->busmasters->size() <= busmasterIndex)
				return ErrorCode::INDEX_OUT_OF_BOUNDS;
			u16 localIndex = id & 0x3FFF;
			return this->busmasters->at(busmasterIndex)->GetInput(localIndex, value);
		}

		tms_t Now() override
		{
			return currentNow;
		}

		char *GetStatusMessage()
		{
			char *pointer = statusMessageBuffer;
			size_t remainingLen = STATUS_MESSAGE_BUFLEN;
			for (auto &host : hosts)
			{
				int size = host->GetStatusMessage(*this, pointer, remainingLen);
				pointer += size;
				remainingLen -= size;
			}
			*pointer = '\0';
			return statusMessageBuffer;
		}
	};
}

#undef TAG
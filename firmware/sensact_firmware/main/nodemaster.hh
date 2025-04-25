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

#include <driver/twai.h>

#include <hal.hh>
#include <led_animator.hh>
#include <sensact_logger.hh>
#include "node_applicationhost.hh"
#include "node_gatewayhost.hh"
#include "node_iohost.hh"
#include "interfaces.hh"
#include "model_node.hh"
#include "webmanager_interfaces.hh"
#include "flatbuffers/flatbuffers.h"


#include <algorithm>
#include <iterator>


#define TAG "NODEMSTR"

/**
 * @brief
 * Nodemaster ist die höchste Instanz. Weiß nichts von Apps
 * Nodemaster kennt die Busmaster (Bus-Zugriff läuft nicht bzw nicht direkt über die HAL, aber die Busmaster selbst können natürlich die HAL nutzen)
 * Nodemaster kennt die Hosts (einer dieser Hosts ist der ApplicationHost, in dem die SensactApps laufen)
 * Nodemaster macht den großen Zyklus "HAL_Before" -->Empfang aller CAN-Nachrichten und Angebot an alle Hosts-->Verarbeitung in allen Hosts -->"HAL_After"
 * Nodemaster kann ausgehende CAN-Nachrichten an alle Hosts spiegeln.
 * Nodemaster abstrahiert alle pyhsischen Inputs und Outputs auf 16bit-Index mit 16bit-Wert
 * 
 */

//TODO:
//- CAN-artige Nachrichten können auch vom Webinterface, von der Fernbedienung und vom Fingerprint-Sensor kommen
//NAchrichten können also von unterschiedlichen Quellen stammen, die 
//möglichst gleich verwaltet werden sollen
//while(source->HasMessage(&temp_message))
//eine Message hat ProcessingInstruction: ForceLocal (kein Versand), ForceCanBus (Versand auf den CAN-Bus), Auto (Prüft, ob die Nachricht lokal jemanden interessiert. Wenn ja: Kein Weiterversand, Wenn nein: Weiterleitung auf den CAN-Bus)

namespace sensact
{
	constexpr size_t STATUS_MESSAGE_BUFLEN{256};
	
	led::BlinkPattern SLOW(200, 1000);
	led::BlinkPattern FAST(200, 200);
	led::BlinkPattern FLASH(20, 3000);
	class cNodemaster : public iHostContext
	{
	private:
		sensact::hal::iHAL *const hal;
		std::vector<AbstractBusmaster *> *busmasters;
		aCANMessageBuilderParser *canMBP;
		std::vector<iHost *> hosts;
		iHost *currentHost{nullptr};
		tms_t currentNow{0}; // das "jetzt" soll bei einem Aufruf konstant gehalten werden
		char statusMessageBuffer[STATUS_MESSAGE_BUFLEN]{0};
		webmanager::iWebmanagerCallback * webmanager_callback_i_am_nullptr{nullptr};

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

		ErrorCode CheckAndLogHealth(){

			bool atLeastHealthWarning{false};
			bool healthError{false};
			static bool previousWifiState{false};
			//Temperatur
			float temperatureCelcius;
			hal->GetBoardTemperature(temperatureCelcius);
			if(temperatureCelcius>70){
				LOGGER::Journal(messagecodes::C::BOARD_HOT, temperatureCelcius);
				healthError=true;				
			}
			else if(temperatureCelcius>60){
				LOGGER::Journal(messagecodes::C::BOARD_WARM, temperatureCelcius);
				atLeastHealthWarning=true;
			}
			//CAN-Nachrichten
			twai_status_info_t status_info={};
			twai_get_status_info(&status_info);
			if(status_info.tx_error_counter>0){
				LOGGER::Journal(messagecodes::C::CAN_TX_ERROR_COUNTER, status_info.tx_error_counter);
				atLeastHealthWarning=true;
			}
			if(status_info.rx_error_counter>0){
				LOGGER::Journal(messagecodes::C::CAN_RX_ERROR_COUNTER, status_info.rx_error_counter);
				atLeastHealthWarning=true;
			}
			if(status_info.tx_failed_count>0){
				LOGGER::Journal(messagecodes::C::CAN_TX_FAILED_COUNTER, status_info.tx_failed_count);
				healthError=true;
			}
			if(status_info.rx_missed_count>0){
				LOGGER::Journal(messagecodes::C::CAN_RX_MISSED_COUNTER, status_info.rx_missed_count);
				healthError=true;
			}
			if(status_info.rx_overrun_count>0){
				LOGGER::Journal(messagecodes::C::CAN_RX_OVERRUN_COUNTER, status_info.rx_overrun_count);
				healthError=true;
			}
			if(status_info.arb_lost_count>0){
				LOGGER::Journal(messagecodes::C::CAN_ARBITRATION_LOST, status_info.arb_lost_count);
				healthError=true;
			}
			if(status_info.bus_error_count>0){
				LOGGER::Journal(messagecodes::C::CAN_BUS_ERROR, status_info.bus_error_count);
				healthError=true;
			}
			//Netzwerk
			webmanager::M* wm= webmanager::M::GetSingleton();
			bool state= wm->GetStaState();
			if(state!=previousWifiState){
				if(state){
					LOGGER::Journal(messagecodes::C::WIFI_CONNECTED, (uint32_t)state);
				}else{
					LOGGER::Journal(messagecodes::C::WIFI_NOT_CONNECTED, (uint32_t)state);
				}
				previousWifiState=state;
			}
			if(!state){
				atLeastHealthWarning=true;
			}
			static bool previousHasRealtime{false};
			bool hasRealtime=wm->HasRealtime();
			if(previousHasRealtime!=hasRealtime){
				LOGGER::Journal(messagecodes::C::GOT_REALTIME, esp_timer_get_time()/1000);
				previousHasRealtime=hasRealtime;
			}
			if(healthError){
				hal->SetInfoLed(&sensact::FAST);
			}else if(atLeastHealthWarning){
				hal->SetInfoLed(&sensact::SLOW);
			}else{
				hal->SetInfoLed(&sensact::FLASH);
			}

			return healthError?ErrorCode::HEALTH_ERROR:atLeastHealthWarning?ErrorCode::HEALTH_WARNING:ErrorCode::OK;
		}

		void task()
		{
			TickType_t xLastWakeTime = xTaskGetTickCount();
			const TickType_t xFrequency = pdMS_TO_TICKS(100);
			
			switch (auto reason = esp_reset_reason())
            {
            case ESP_RST_POWERON:
                ESP_LOGI(TAG, "Reset Reason is RESET %d. MessageLog Memory is reset", reason);
				webmanager::JournalPlugin::GetSingleton()->ResetJournal();
                break;
            case ESP_RST_EXT:
            case ESP_RST_SW:
                ESP_LOGI(TAG, "Reset Reason is EXT/SW %d. MessageLogEntry is added", reason);
                LOGGER::Journal(messagecodes::C::SW, reason);
                break;
            case ESP_RST_PANIC:
                ESP_LOGI(TAG, "Reset Reason is PANIC %d. MessageLogEntry is added", reason);
                LOGGER::Journal(messagecodes::C::PANIC, reason);
                break;
            case ESP_RST_BROWNOUT:
                ESP_LOGI(TAG, "Reset Reason is BROWNOUT %d. MessageLogEntry is added", reason);
                LOGGER::Journal(messagecodes::C::BROWNOUT, reason);
                break;
            case ESP_RST_TASK_WDT:
            case ESP_RST_INT_WDT:
                ESP_LOGI(TAG, "Reset Reason is WATCHDOG %d. MessageLogEntry is added", reason);
                LOGGER::Journal(messagecodes::C::WATCHDOG, 0);
                break;
            default:
                ESP_LOGI(TAG, "Reset Reason is %d. MessageLog Memory is reset", reason);
                webmanager::JournalPlugin::GetSingleton()->ResetJournal();
                break;
            }



			while (true)
			{
				currentNow = hal->GetMillisS64();
				ESP_LOGD(TAG, "Before Apploop");
				hal->BeforeAppLoop();
				CANMessage message;
				while (hal->TryReceiveCanMessage(message) == ErrorCode::OK)
				{
					for (auto &rr : hosts)
					{
						this->currentHost = rr;
						rr->OfferMessage(*this, message);
					}
				}

				for (auto &rr : hosts)
				{
					this->currentHost = rr;
					rr->Loop(*this);
				}
				ESP_LOGD(TAG, "hal->AfterAppLoop(); 1");
				hal->AfterAppLoop();
				ESP_LOGD(TAG, "hal->AfterAppLoop(); 2");
				//check problem and log them
				this->CheckAndLogHealth();
				ESP_LOGD(TAG, "hal->CheckAndLogHealth();");
				xTaskDelayUntil(&xLastWakeTime, xFrequency);
			}
		}

		public:
		cNodemaster(sensact::hal::iHAL *hal, std::vector<AbstractBusmaster *> *busmasters, aCANMessageBuilderParser *canMBP) : hal(hal), busmasters(busmasters), canMBP(canMBP)
		{
			
		}

		

		void Setup(std::vector<sensact::iHost *> hosts)
		{
			this->hosts = hosts;
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
			this->currentNow = hal->GetMillisS64();
			for (auto &rr : hosts)
			{
				this->currentHost = rr;
				rr->Setup(*this);
			}
			PublishNodeEvent(currentNow, sensact::model::node::NodeID, eNodeEventType::NODE_READY, 0, 0);

			LOGI(TAG, "All Hosts have been configured. Now, %s is pleased to be at your service.\r\n", sensact::model::node::NodeDescription);
		}

		void RunEternalLoopInTask(void)
		{
			xTaskCreate([](void *p){((cNodemaster*)p)->task(); }, "NodemasterTask", 4096*4, this, 6, nullptr);
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
				if (this->currentHost == rr)
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
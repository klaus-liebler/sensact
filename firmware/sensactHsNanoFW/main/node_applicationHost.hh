#pragma once

#include "nodemaster.hh"
#include "can_messenger.hh"
namespace sensact
{
    class ApplicationHostRunner : NodemasterRoleRunner, SensactContext
    {
    private:
        u8 *statusBuffer{0};
        u16 nextStatusApp{0};
        u16 appCnt{0};
        size_t statusBufferLength{0};
        HAL *hal;
        tms_t lastSentCANMessage;
		tms_t nowForCurrentLoop{0};
        CANMessenger *messenger;

        void FindNextStatusApp()
        {
            do
            {
                nextStatusApp++;
                if (nextStatusApp == (uint16_t)eApplicationID::CNT)
                {
                    nextStatusApp = 0;
                }
            } while (MODEL::Glo2locCmd[nextStatusApp] == 0);
        }

        ErrorCode PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, uint8_t * payload, uint8_t payloadLength)
        {
            lastSentCANMessage=now;
            messenger->SendApplicationStatusMessage((u16)sourceApp, (u8)statusType, payload, payloadLength);
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

        

    public:
        ApplicationHostRunner(HAL *hal) : hal(hal)
        {
            messenger=new CANMessenger(hal);
        }

        ErrorCode SetAmplifierVolume(uint8_t volume0_255) override
        {
            return hal->SetAmplifierVolume(volume0_255);
        }
        ErrorCode PlayMP3(uint8_t volume0_255, const uint8_t &buf, size_t len) override
        {
            return hat - PlayMP3(volume0_255, buf, len);
        }
        ErrorCode PlayRTTTL(uint8_t volume0_255, const uint8_t &buf, size_t len) override
        {
            return hat - PlayRTTTL(volume0_255, buf, len);
        }
        virtual ErrorCode StopSound() override
        {
            hal->StopSound();
        }
        tms_t GetNowMsS64() override
        {
            return nowForCurrentLoop;
        }
        void GetTimestamp(char *buf, size_t maxLen) override
        {
            return;
        }
        ErrorCode ColorizeLed(uint8_t index, CRGB color) override
        {
            return hal->PrepareColorizeLed(index, color);
        }
        ErrorCode UnColorizeAllLed() override
        {
            return hal->PrepareUnColorizeAllLed();
        }

        ErrorCode SetU16Output(InOutId pinId, u16 value) override
        {
            // TODO: Forward to correct busmaster
            return hal->SetU16Output(pinId, value);
        }
        // TODO : virtual ErrorCode GetBoolInputs(InOutId pinId_last5bitsAreNotCosidered, uint32_t &value) = 0;
        ErrorCode GetU16Input(InOutId input, u16 &inputState) override
        {
            // TODO: Forward to correct busmaster
            return hal->GetU16Input(input, inputState);
        }
        ErrorCode GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value) override
        {
            return hal->GetRotaryEncoderValue(re, value);
        }



        ErrorCode PostEventFiltered(const eApplicationID sourceApp, const eEventType evt, std::vector<eEventType> localEvts, std::vector<eEventType> busEvts, uint8_t * payload, uint8_t payloadLength)
        {
            for(auto& test:localEvents)
            {
                if(evt==test)
                {
                    cApplication *app = MODEL::Glo2locEvt[(uint16_t)sourceApp];
                    if (app != NULL) {
                        app->OnEvent(sourceApp, evt, payload, payloadLength);
                    }
                    break;
                }
            }
            for(auto& test:busEvents)
            {
                if(evt==test)
                {
                    PostEvent(sourceApp, evt, payload, payloadLength);
                    break;
                }
            }

        }

        ErrorCode PostCommand(eApplicationID destinationApp, eCommandType cmd, const uint8_t *const payload, uint8_t payloadLength)
        {
            if (destinationApp == eApplicationID::NO_APPLICATION)
            {
                return ErrorCode::OK;
            }
            if ((uint16_t)destinationApp >= (uint16_t)eApplicationID::CNT)
            {
                LOGE(TAG, "Trying to send to an invalid application id %i", (uint16_t)destinationApp);
                return ErrorCode::INVALID_APPLICATION_ID;
            }

            cApplication *const app = MODEL::Glo2locCmd[(uint16_t)destinationApp];
            if (app != NULL)
            {
                // only in this case, the message can be processed local; no need to send it to the CAN bus
                app->OnCommand(cmd, payload, payloadLength, now);
                return ErrorCode::OK;
            }
            lastSentCANMessage = now;
            return messenger->SendCommandMessage((u16)destinationApp, (u8)cmd, payload, payloadLength)
        }

        ErrorCode PostEvent(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength)
        {
	        lastSentCANMessage=now;
            messenger->SendEventMessage((u16)sourceApp, (u8)event, payload, payloadLength);
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
        
        ErrorCode Setup(s64 now) override
        {
            // TODO maybe, start at "1" here, because "0" was the global master application in early stages of this project
            for (u16 appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
            {
                cApplication *const app = MODEL::Glo2locCmd[appId];
                if (!app)
                    continue;
                appResult = app->Setup(this);
                if ((uint8_t)appResult < (uint8_t)eAppCallResult::ERROR_GENERIC)
                {
                    LOGI(TAG, "App %s successfully configured", MODEL::ApplicationNames[appId]);
                }
                else
                {
                    LOGE(TAG, "Error while configuring App %s. Error is %u", MODEL::ApplicationNames[appId], appResult);
                }
                statusBuffer = malloc(8); // only for one single Application
                statusBuffer[0] = (uint8_t)appResult;
                PublishApplicationStatus(now, (eApplicationID)appId, eApplicationStatus::STARTED, statusBuffer, 8);
                appCnt++;
            }
            FindNextStatusApp();
            LOGI("%u local applications have been configured. Now, %s is pleased to be at your service.\r\n", appCnt, MODEL::ModelString);
        }

        ErrorCode OfferMessage(CANMessage *rcvMessage) {

            uint16_t appId;
            uint8_t commandId;
            uint8_t eventId;
            Time_t now = BSP::GetSteadyClock();
            cApplication *app;
            eCanMessageType type = CANMessenger::ParseCanMessageType(rcvMessage.Id);
            switch(type)
            {
            case eCanMessageType::ApplicationCommand:
                if(MODEL::TRACE_COMMANDS) CANMessenger::TraceCommandMessage(rcvMessage);
                CANMessenger::ParseCommandMessageId(rcvMessage.Id, &appId, &commandId, rcvMessage.Data[0]);
                if(appId >= (uint32_t)eApplicationID::CNT)
                {
                    LOGE(TAG, "Received ApplicationCommand. Unknown target applicationID %i", appId);
                    return ErrorCode::INVALID_APPLICATION_ID;
                }
                app = MODEL::Glo2locCmd[appId];
                if (app != NULL) {
 #ifdef NEW_CANID
                    app->OnCommand((eCommandType)commandId, rcvMessage.Data, rcvMessage.Length);
#else
                    app->OnCommand((eCommandType)commandId, &rcvMessage.Data[1], rcvMessage.Length-1);
#endif
                }
                return ErrorCode::OK;
            case eCanMessageType::ApplicationEvent:
                if(MODEL::TRACE_EVENTS) CANMessenger::TraceEventMessage(rcvMessage);
                cCanIdUtils::ParseEventMessageId(rcvMessage.Id, &appId, &commandOrEventId);
                if(appId >= (uint32_t)eApplicationID::CNT)
                {
                    LOGE(TAG, "Received ApplicationEvent. Unknown source applicationID %i", appId);
                    return;
                }
                app = MODEL::Glo2locEvt[appId];
                if (app != NULL) {
                    app->OnEvent((eApplicationID) appId, (eEventType)commandOrEventId,  rcvMessage.Data, rcvMessage.Length);
                }
                return ErrorCode::OK;
            default: 
                return ErrorCode::OK_BUT_NOT_NEEDED;
            }
        }


        
        ErrorCode Loop(s64 now) override
        {
            this->nowForLoop=now;
            for (u16 appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
            {
                cApplication *const app = MODEL::Glo2locCmd[appId];
                if (!app)
                    continue;
                eAppCallResult appResult = app->Loop(this);
                if ((uint8_t)appResult >= (uint8_t)eAppCallResult::ERROR_GENERIC) // means: no severe error
                {
                    PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::ERROR_ON_CYCLIC, statusBuffer, 8);
                    continue;
                }
                if (appResult != eAppCallResult::OK && statusBufferLength > 0) // TODO: Weshalb muss statusBufferLength (auch 5 Zeilen drunter) geprüft werden. Wenn es etwas interessantes gibt, dann muss das doch veröffentlicht werden, auch wenn es keinen Payload gibt!
                {
                    // Something interesting happened -->inform everybody!
                    PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
                }
                if (now - lastSentCANMessage > 1000 && appId == nextStatusApp)
                {
                    if (statusBufferLength > 0)
                    {
                        PublishApplicationStatus( (eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
                    }
                    FindNextStatusApp();
                }
            }

            // Reset heartbeat buffer;
            for (uint8_t i = 0; i < COUNTOF(heartbeatBuffer); i++)
            {
                if (heartbeatBuffer[i] == eApplicationID::NO_APPLICATION)
                    break;
                LOGD("Flushing heartbeat buffer with target %s", cApplication::N4I((uint32_t const)heartbeatBuffer[i]));
                cApplication::SendHEARTBEATCommand(heartbeatBuffer[i], (uint32_t)MODEL::NodeMasterApplication, now);
                heartbeatBuffer[i] = eApplicationID::NO_APPLICATION;
            }
            while (BSP::GetSteadyClock() - now < 20)
            {
                CanBusProcess();
            }
        }
    }
}
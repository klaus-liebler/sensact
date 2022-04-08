
#include "common_in_project.hh"
#include "node_applicationHost.hh"
#include "model_applications.hh"
#include "application.hh"
#include "applicationcontext.hh"

#define TAG "APPHOST"

namespace sensact
{

#include <generated/common/sendCommandImplementation.inc>

    ErrorCode cApplicationHost::OnApplicationCommand(cApplication *app, eCommandType command, uint8_t *payload, uint8_t payloadLength)
    {
        SensactContext *ctx = this;
        switch (command)
        {
#include <common/parseCommand.inc>
        default:
            return ErrorCode::INVALID_COMMAND;
        }
        return ErrorCode::OK;
    }

    void cApplicationHost::FindNextStatusApp()
    {
        do
        {
            nextStatusApp++;
            if (nextStatusApp == (uint16_t)eApplicationID::CNT)
            {
                nextStatusApp = 0;
            }
        } while (sensact::model::applications::Glo2locCmd[nextStatusApp] == 0);
    }

    ErrorCode cApplicationHost::PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, uint8_t *payload, uint8_t payloadLength)
    {
        CANMessage m;
        canMBP->BuildApplicationStatusMessage((u16)sourceApp, (u8)statusType, payload, payloadLength, m);
        hostCtx->PublishOnMessageBus(m, true);
        return ErrorCode::OK;
    }

    void cApplicationHost::SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType commmandType, uint8_t *payload, uint8_t payloadLength)
    {
        if (destinationApp == eApplicationID::NO_APPLICATION)
        {
            return;
        }
        if ((uint16_t)destinationApp >= (uint16_t)eApplicationID::CNT)
        {
            LOGE(TAG, "Trying to send to an invalid application id %i", (uint16_t)destinationApp);
            return;
        }

        cApplication *const app = sensact::model::applications::Glo2locCmd[(uint16_t)destinationApp];
        if (app != NULL)
        {
            // only in this case, the message can be processed local; no need to send it to the CAN bus
            this->OnApplicationCommand(app, commmandType, payload, payloadLength);

            return;
        }
        this->lastSentCANMessage =  hal->GetMillisS64();
        CANMessage m;
        canMBP->BuildApplicationCommandMessage((u16)destinationApp, (u8)commmandType, payload, payloadLength, m);
        hostCtx->PublishOnMessageBus(m, true);
    }

    cApplicationHost::cApplicationHost(sensact::hal::iHAL *hal, iHostContext *hostCtx, aCANMessageBuilderParser *canMBP) : hal(hal), hostCtx(hostCtx), canMBP(canMBP)
    {
    }

    void cApplicationHost::SetAmplifierVolume(uint8_t volume0_255)
    {
        ErrorCode err = hal->SetAmplifierVolume(volume0_255);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "SetAmplifierVolume raised error %02X", (int)err);
        }
    }
    void cApplicationHost::PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len)
    {
        ErrorCode err = hal->PlayMP3(volume0_255, buf, len);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "PlayMP3 raised error %02X", (int)err);
        }
    }

    void cApplicationHost::StopSound()
    {
        ErrorCode err = hal->StopSound();
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "StopSound raised error %02X", (int)err);
        }
    }
    tms_t cApplicationHost::Now()
    {
        return nowForCurrentLoop;
    }

    void cApplicationHost::SetRGBLed(uint8_t index, CRGB color)
    {
        ErrorCode err = hal->StageRGBLed(index, color);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "SetRGBLed raised error %02X", (int)err);
        }
    }
    void cApplicationHost::UnColorizeAllRGBLed()
    {
        ErrorCode err = hal->StageUnColorizeAllRGBLed();
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "UnColorizeAllRGBLed raised error %02X", (int)err);
        }
    }

    void cApplicationHost::SetU16Output(InOutId id, u16 value)
    {
        // TODO: Forward to correct busmaster
        ErrorCode err = this->hostCtx->SetU16Output(id, value);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "SetU16Output raised error %02X", (int)err);
        }
    }

    void cApplicationHost::GetU16Input(InOutId id, u16 &value)
    {
        ErrorCode err = this->hostCtx->GetU16Input(id, value);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "GetU16Input raised error %02X", (int)err);
        }
    }
    void cApplicationHost::GetRotaryEncoderValue(eRotaryEncoder re, uint16_t &value, bool &isPressed)
    {
        ErrorCode err = hal->GetRotaryEncoderValue(re, value, isPressed);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "GetRotaryEncoderValue raised error %02X", (int)err);
        }
    }

   
    /*
    ErrorCode cApplicationHost::PostEventFiltered(const MODEL::eApplicationID sourceApp, const MODEL::eEventType evt, std::vector<eEventType> localEvts, std::vector<eEventType> busEvts, uint8_t *payload, uint8_t payloadLength)
    {
        for (auto &test : localEvents)
        {
            if (evt == test)
            {
                cApplication *app = MODEL::Glo2locEvt[(uint16_t)sourceApp];
                if (app != NULL)
                {
                    app->OnEvent(sourceApp, evt, payload, payloadLength);
                }
                break;
            }
        }
        for (auto &test : busEvents)
        {
            if (evt == test)
            {
                PostEvent(sourceApp, evt, payload, payloadLength);
                break;
            }
        }
    }




    ErrorCode cApplicationHost::PostEvent(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength)
    {
        lastSentCANMessage = now;
        messenger->SendEventMessage((u16)sourceApp, (u8)event, payload, payloadLength);
#ifdef MASTERNODE
        uint8_t buf[11];
        Common::WriteInt16((u16)sourceApp, buf, 0);
        buf[2] = (u8)event;
        for (int i = 0; i < payloadLength; i++)
        {
            buf[i + 3] = payload[i];
        }
        cMaster::mqtt_publishOnTopic(eMqttTopic::APP_EVENT, buf, 3 + payloadLength, 0, 1);
#endif
    }
*/
    ErrorCode cApplicationHost::Setup(iHostContext &ctx)
    {
        // TODO maybe, start at "1" here, because "0" was the global master application in early stages of this project
        for (uint16_t appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
        {
            cApplication *const app = sensact::model::applications::Glo2locCmd[appId];
            if (!app)
                continue;
            sensact::eAppCallResult appResult = app->Setup(this);
            if ((uint8_t)appResult < (uint8_t)eAppCallResult::ERROR_GENERIC)
            {
                LOGI(TAG, "App %s successfully configured", sensact::model::applications::ApplicationNames[appId]);
            }
            else
            {
                LOGE(TAG, "Error while configuring App %s. Error is %u", sensact::model::applications::ApplicationNames[appId], (int)appResult);
            }
            statusBuffer = (uint8_t *)malloc(8); // only for one single Application
            statusBuffer[0] = (uint8_t)appResult;
            PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::STARTED, statusBuffer, 8);
            appCnt++;
        }
        FindNextStatusApp();
        LOGI(TAG, "%u local applications have been configured.", appCnt);
        return ErrorCode::OK;
    }

    ErrorCode cApplicationHost::OfferMessage(iHostContext &ctx, CANMessage &m)
    {
        uint16_t appId;
        uint8_t commandType;
        uint8_t eventId;
        cApplication *app;
        eCanMessageType type;
        ErrorCode err = canMBP->ParseCanMessageType(m, type);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "canMBP->ParseCanMessageType(m) raised error %02X", (int)err);
            return err;
        }

        switch (type)
        {
        case eCanMessageType::ApplicationCommand:
            if (sensact::config::TRACE_COMMANDS)
                canMBP->TraceCommandMessage(m);
            canMBP->ParseApplicationCommandMessageId(m, appId, commandType);
            if (appId >= (uint16_t)eApplicationID::CNT)
            {
                LOGE(TAG, "Received ApplicationCommand. Unknown target applicationID %i", appId);
                return ErrorCode::INVALID_APPLICATION_ID;
            }
            app = sensact::model::applications::Glo2locCmd[appId];
            if (app != NULL)
            {
                // Es ist natürlich kein Fehler, wenn hier an der Node eine Nachricht ankommt, für die lokal keine App hinterlegt ist - eine andere Node wird die schon empfangen...
                this->OnApplicationCommand(app, (eCommandType)commandType, canMBP->GetPayloadStart(m), canMBP->GetPayloadLen(m));
            }
            return ErrorCode::OK;
        case eCanMessageType::ApplicationEvent:
            if (sensact::config::TRACE_EVENTS)
                canMBP->TraceEventMessage(m);
            canMBP->ParseApplicationEventMessageId(m, appId, eventId);
            if (appId >= (uint16_t)eApplicationID::CNT)
            {
                LOGE(TAG, "Received ApplicationEvent. Unknown source applicationID %i", appId);
                return ErrorCode::INVALID_APPLICATION_ID;
            }
            app = sensact::model::applications::Glo2locEvt[appId];
            if (app != NULL)
            {
                LOGE(TAG, "Received ApplicationEvent, but this is not yet implemented");
                return ErrorCode::NOT_YET_IMPLEMENTED;;
                //app->OnEvent((eApplicationID)appId, (eEventType)commandOrEventId, rcvMessage.Data, rcvMessage.Length);
            }
            return ErrorCode::OK;
        default:
            return ErrorCode::OK_BUT_NOT_NEEDED;
        }
    }

    ErrorCode cApplicationHost::Loop(iHostContext &ctx)
    {
        this->nowForCurrentLoop = ctx.Now();
        for (u16 appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
        {
            cApplication *const app = sensact::model::applications::Glo2locCmd[appId];
            if (!app)
                continue;
            eAppCallResult appResult = app->Loop(this);
            if ((uint8_t)appResult >= (uint8_t)sensact::eAppCallResult::ERROR_GENERIC) // means: no severe error
            {
                PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::ERROR_ON_CYCLIC, statusBuffer, 8);
                continue;
            }
            if (appResult != eAppCallResult::OK && statusBufferLength > 0) // TODO: Weshalb muss statusBufferLength (auch 5 Zeilen drunter) geprüft werden. Wenn es etwas interessantes gibt, dann muss das doch veröffentlicht werden, auch wenn es keinen Payload gibt!
            {
                // Something interesting happened -->inform everybody!
                PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
            }
            if (nowForCurrentLoop - lastSentCANMessage > 1000 && appId == nextStatusApp)
            {
                if (statusBufferLength > 0)
                {
                    PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, statusBuffer, 8);
                }
                FindNextStatusApp();
            }
        }
        return ErrorCode::OK;
    }

}
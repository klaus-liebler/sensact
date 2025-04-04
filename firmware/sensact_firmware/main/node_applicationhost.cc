
#include <sensact_commons.hh>
#include <sensact_projectconfig.hh>
#include "node_applicationHost.hh"
#include "cApplications.hh"
#include "cApplication.hh"
#include "iSensactContext.hh"
#include <sensact_logger.hh>
#define TAG "APPHOST"

namespace sensact
{

#include <common/sendCommandImplementation.inc>
#include <common/publishEventImplementation.inc>

void cApplicationHost::OnBegin(webmanager::iWebmanagerCallback *callback){
    this->webmanager_callback=callback;
}
void cApplicationHost::OnWifiConnect(webmanager::iWebmanagerCallback *callback){
    this->webmanager_callback=callback;
}
void cApplicationHost::OnWifiDisconnect(webmanager::iWebmanagerCallback *callback){
    this->webmanager_callback=callback;
}
void cApplicationHost::OnTimeUpdate(webmanager::iWebmanagerCallback *callback){
    this->webmanager_callback=callback;
}

webmanager::eMessageReceiverResult cApplicationHost::handleRequestCommand(const sensact::RequestCommand* req, webmanager::iWebmanagerCallback* callback){
    flatbuffers::FlatBufferBuilder b(1024);
    auto d = req->payload()->data()->data();
    CommandMessage message{
        (sensact::eApplicationID)req->id(), 
        (sensact::eCommandType)req->cmd(),
        {d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]},
        (uint8_t)req->payload()->len()
    };
    xQueueSend(this->webCommandQueue, (void *)&message, pdMS_TO_TICKS(100));
    b.Finish(
        sensact::CreateResponseWrapper(
            b,
            sensact::Responses::Responses_ResponseCommand,
            sensact::CreateResponseCommand(b).Union()
        )
    );
    return callback->WrapAndSendAsync(sensact::Namespace::Namespace_Value, b)==ESP_OK?webmanager::eMessageReceiverResult::OK:webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
}


webmanager::eMessageReceiverResult cApplicationHost::handleRequestStatus(const sensact::RequestStatus* req, webmanager::iWebmanagerCallback* callback){
    flatbuffers::FlatBufferBuilder b(1024);
    std::vector<sensact::ResponseStatusItem> states;
    for(int i=0;i<req->ids()->size();i++){
        auto id = req->ids()->Get(i);
        auto state=this->statusBuffer[id];
        states.push_back(sensact::ResponseStatusItem((sensact::ApplicationId)id, sensact::StatusPayload(state)));
    }
    b.Finish(
        sensact::CreateResponseWrapper(
            b,
            sensact::Responses::Responses_ResponseStatus,
            sensact::CreateResponseStatusDirect(b, &states).Union()
        )
    );
    return callback->WrapAndSendAsync(sensact::Namespace::Namespace_Value, b)==ESP_OK?webmanager::eMessageReceiverResult::OK:webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
}


webmanager::eMessageReceiverResult cApplicationHost::ProvideWebsocketMessage(webmanager::iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint32_t ns, uint8_t* buf){
    this->webmanager_callback=callback;
    if (ns != sensact::Namespace::Namespace_Value)
        return webmanager::eMessageReceiverResult::NOT_FOR_ME;
    
    auto rw = flatbuffers::GetRoot<sensact::RequestWrapper>(buf);
    switch (rw->request_type())
    {
    case sensact::Requests::Requests_RequestCommand:{
        return this->handleRequestCommand(rw->request_as_RequestCommand(), callback);
    }
    case sensact::Requests::Requests_RequestStatus:{
        return this->handleRequestStatus(rw->request_as_RequestStatus(), callback);
    }
    default:
        return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
    }
}    


ErrorCode cApplicationHost::OnApplicationCommand(sensact::apps::cApplication *app, eCommandType command, uint8_t *payload, uint8_t payloadLength)
    {
        iSensactContext *ctx = this;
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
        } while (sensact::apps::cApplications::Glo2locCmd[nextStatusApp] == 0);
    }

    ErrorCode cApplicationHost::PublishApplicationStatus(eApplicationID sourceApp, eApplicationStatus statusType, std::array<uint16_t,4> &payload)
    {
        uint8_t byteArray[8];
        for (size_t i = 0; i < payload.size(); ++i) {
            byteArray[2 * i] = static_cast<uint8_t>(payload[i] & 0xFF); // Niedrigwertiges Byte
            byteArray[2 * i + 1] = static_cast<uint8_t>((payload[i] >> 8) & 0xFF); // Hochwertiges Byte
        }
        CANMessage m;
        if(canMBP->BuildApplicationStatusMessage((u16)sourceApp, (u8)statusType, byteArray, 8, m)==ErrorCode::OK){
            hostCtx->PublishOnMessageBus(m, true);
        }
        if(this->webmanager_callback!=nullptr){
            flatbuffers::FlatBufferBuilder b(128);
            auto sp= sensact::StatusPayload(payload);
            b.Finish(
                sensact::CreateResponseWrapper(
                    b,
                    sensact::Responses::Responses_NotifyStatus,
                    sensact::CreateNotifyStatus(b, static_cast<sensact::ApplicationId>(sourceApp), &sp).Union()
                )
            );
            webmanager_callback->WrapAndSendAsync(sensact::Namespace::Namespace_Value, b);

        }
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

        sensact::apps::cApplication *const app = sensact::apps::cApplications::Glo2locCmd[(uint16_t)destinationApp];
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

    void cApplicationHost::PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength)
    {
        if (sourceApp == eApplicationID::NO_APPLICATION)
        {
            return;
        }
        if ((uint16_t)sourceApp >= (uint16_t)eApplicationID::CNT)
        {
            LOGE(TAG, "Trying to send to an invalid application id %i", (uint16_t)sourceApp);
            return;
        }
        this->lastSentCANMessage =  hal->GetMillisS64();
        CANMessage m;
        if(canMBP->BuildApplicationEventMessage((u16)sourceApp, (u8)event, payload, payloadLength,  m)!=ErrorCode::OK)
            return;
        hostCtx->PublishOnMessageBus(m, false);
    }

    cApplicationHost::cApplicationHost(sensact::hal::iHAL *hal, iHostContext *hostCtx, aCANMessageBuilderParser *canMBP) : hal(hal), hostCtx(hostCtx), canMBP(canMBP)
    {
        this->webCommandQueue = xQueueCreate(3, sizeof(CommandMessage));
    }

    void cApplicationHost::SetAmplifierVolume(uint8_t volume0_255)
    {
        ErrorCode err = hal->SetAmplifierVolume(volume0_255);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "SetAmplifierVolume raised error %s", ErrorCodeStr[(int)err]);
        }
    }
    void cApplicationHost::PlayMP3(uint8_t volume0_255, const uint8_t *buf, size_t len)
    {
        ErrorCode err = hal->PlayMP3(volume0_255, buf, len);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "PlayMP3 raised error %s", ErrorCodeStr[(int)err]);
        }
    }

    void cApplicationHost::StopSound()
    {
        ErrorCode err = hal->StopSound();
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "StopSound raised error %s", ErrorCodeStr[(int)err]);
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
            LOGE(TAG, "SetRGBLed raised error %s", ErrorCodeStr[(int)err]);
        }
    }
    void cApplicationHost::UnColorizeAllRGBLed()
    {
        ErrorCode err = hal->StageUnColorizeAllRGBLed();
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "UnColorizeAllRGBLed raised error %s", ErrorCodeStr[(int)err]);
        }
    }

    void cApplicationHost::SetU16Output(InOutId id, u16 value)
    {
        ErrorCode err = this->hostCtx->SetU16Output(id, value);
        if (err != ErrorCode::OK)
        {
            LOGE(TAG, "SetU16Output {'id':%d}  raised error %s",(int)id, ErrorCodeStr[(int)err]);
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
    */

    ErrorCode cApplicationHost::Setup(iHostContext &ctx)
    {
        // TODO maybe, start at "1" here, because "0" was the global master application in early stages of this project
        for (uint16_t appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
        {
            sensact::apps::cApplication *const app = sensact::apps::cApplications::Glo2locCmd[appId];
            if (!app)
                continue;
            sensact::eAppCallResult appResult = app->Setup(this);
            if ((uint8_t)appResult < (uint8_t)eAppCallResult::ERROR_GENERIC)
            {
                LOGI(TAG, "App %s successfully configured", sensact::ApplicationNames[appId]);
            }
            else
            {
                LOGE(TAG, "Error while configuring App %s. Error is %u", sensact::ApplicationNames[appId], (int)appResult);
            }
            //Too many messages! statusBuffer = new uint8_t[8]; // only for one single Application
            //statusBuffer[0] = (uint8_t)appResult;
            //PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::STARTED, statusBuffer, 8);
            //delete[] statusBuffer;
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
        sensact::apps::cApplication *app;
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
            app = sensact::apps::cApplications::Glo2locCmd[appId];
            if (app != nullprt)
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
            app = sensact::apps::cApplications::Glo2locEvt[appId];
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
        CommandMessage message;
        this->nowForCurrentLoop = ctx.Now();
        while (xQueueReceive(this->webCommandQueue, (void *)&message, 0)==pdTRUE)
        {
            sensact::apps::cApplication *const app = sensact::apps::cApplications::Glo2locCmd[message.target];
            if(!app) continue;
            this->OnApplicationCommand(app, message->command, message.payload, message.payloadLength);
        }
        for (u16 appId = 0; appId < (uint16_t)eApplicationID::CNT; appId++)
        {
            sensact::apps::cApplication *const app = sensact::apps::cApplications::Glo2locCmd[appId];
            if (!app)
                continue;
            eAppCallResult appResult = app->Loop(this);
            if ((uint8_t)appResult >= (uint8_t)sensact::eAppCallResult::ERROR_GENERIC) 
            {
                // means: an error occured. Report instantly
                app->FillStatus(*this, this->statusBuffer[appId]);
                PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::ERROR_ON_CYCLIC, this->statusBuffer[appId]);
                continue;
            }
            else if (appResult != eAppCallResult::OK)
            {
                // Something interesting happened. Report everybody!
                app->FillStatus(*this, this->statusBuffer[appId]);
                PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::CHANGED, this->statusBuffer[appId]);
            }
            else if (nowForCurrentLoop - lastSentCANMessage > 1000 && appId == nextStatusApp)
            {
                app->FillStatus(*this, this->statusBuffer[appId]);
                PublishApplicationStatus((eApplicationID)appId, eApplicationStatus::REGULAR_STATUS, this->statusBuffer[appId]);
                FindNextStatusApp();
            }
        }
        return ErrorCode::OK;
    }

    int cApplicationHost::GetStatusMessage(iHostContext& ctx, char* buffer, size_t remainingLen){
        return snprintf(buffer, remainingLen, "\tApplicationHost ok!\n");
    }

}
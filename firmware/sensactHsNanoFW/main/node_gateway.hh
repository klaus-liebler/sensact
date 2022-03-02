#pragma once

#include "nodemaster.hh"
#include "can_messenger.hh"
namespace sensact{
    class GatewayRunner:NodemasterRoleRunner{
        public:
        GatewayRunner(){
            
        }
        ErrorCode Setup(Nodemaster *nm) override
		{

        }

        ErrorCode Loop(Nodemaster *nm) override{
        {
            for (int i = 0; i < statusBufferLength && i < STATUSBUFFER_LENGTH_BYTES; i++)
            {
                MODEL::applicationStatus[appId][i] = rcvMessage.Data[i];
            }
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
            case eCanMessageType::ApplicationStatus:

                CANMessenger::ParseApplicationStatusMessageId(rcvMessage.Id, &appId, &commandOrEventId);
                if(appId >= (uint32_t)eApplicationID::CNT)
                {
                    LOGE(TAG, "Received ApplicationStatus; unknown applicationID %i", appId);
                    return ErrorCode::INVALID_APPLICATION_ID;
                }

                Common::WriteInt16(appId, buf, 0);
                buf[2]=commandOrEventId;
                for(int i=0;i<rcvMessage.Length;i++)
                {
                    MODEL::applicationStatus[appId][i]=rcvMessage.Data[i];
                    buf[i+3]=rcvMessage.Data[i];
                }
                //cMaster::mqtt_publishOnTopic(eMqttTopic::APP_STATUS, buf, 3+rcvMessage.Length, 0, 1);
                return ErrorCode::OK;
            default:
                return  ErrorCode::OK_BUT_NOT_NEEDED;
            }
        }

    };
}
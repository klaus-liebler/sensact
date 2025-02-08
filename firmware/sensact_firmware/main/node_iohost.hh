#pragma once

#include "nodemaster.hh"
#include "interfaces.hh"
namespace sensact
{
    class cIoHost : public iHost
    {
    public:
        cIoHost()
        {
        }
        ErrorCode Setup(iHostContext& ctx) override
        {
            return ErrorCode::OK;
        }

        ErrorCode Loop(iHostContext& ctx) override
        {
            return ErrorCode::OK;
        }

        ErrorCode OfferMessage(iHostContext& ctx, CANMessage& m) override
        {
            return ErrorCode::OK;
        }

        int GetStatusMessage(iHostContext& ctx, char* buffer, size_t remainingLen){
            return snprintf(buffer, remainingLen, "\tIoHost ok!\n");
        }
    };
}
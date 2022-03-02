#pragma once

#include "nodemaster.hh"
namespace sensact
{
    class IoHostRunner : NodemasterRoleRunner
    {
    public:
        IoHostRunner()
        {
        }
        ErrorCode Setup(Nodemaster *nm) override
        {
            return ErrorCode::OK;
        }

        ErrorCode Loop(Nodemaster *nm) override
        {
            return ErrorCode::OK;
        }

        ErrorCode OfferMessage(Nodemaster* nodemaster, CANMessage* m) override
        {
            return ErrorCode::OK;
        }
    };
}
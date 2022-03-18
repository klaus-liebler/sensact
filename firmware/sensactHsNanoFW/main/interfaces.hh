#pragma once

#include "common_in_project.hh"

namespace sensact{
    class iHostContext{
    public:
        virtual tms_t Now();
        virtual void PublishOnMessageBus(CANMessage& m, bool distributeLocally)=0;
        virtual ErrorCode SetU16Output(u16 id, u16 value) = 0;
        virtual ErrorCode GetU16Input(u16 id, u16 &value) = 0;
    };

    class iHost{
	public:
		virtual ErrorCode Setup(iHostContext& ctx)=0;
		virtual ErrorCode Loop(iHostContext& ctx)=0;
		virtual ErrorCode OfferMessage(iHostContext& ctx, CANMessage& m)=0;
	};
}

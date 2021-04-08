#pragma once
#include <inttypes.h>
#include "common.hh"
#include <cJSON.h>

class cApplication{
    public:
        uint32_t const id;
        cApplication(const uint32_t id):id(id){}
        virtual ErrorCode Setup(SensactContext *ctx) =0;
		virtual ErrorCode Loop(SensactContext *ctx) =0;
        virtual ErrorCode FillStatus(BinaryWriter* w)=0;
        virtual ErrorCode ProcessJsonCommand(cJSON * json)=0;
};


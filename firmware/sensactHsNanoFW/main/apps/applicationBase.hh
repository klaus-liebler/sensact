#pragma once
#include <inttypes.h>
#include "common_in_project.hh"
#include "sensactContext.hh"
#include "../generated/webui_core_comm_generated.h"
using namespace sensact::comm;

class cApplication{
    public:
        uint32_t const id;
        cApplication(const uint32_t id):id(id){}
        virtual eAppCallResult Setup(SensactContext *ctx) = 0;
		virtual eAppCallResult Loop(SensactContext *ctx) = 0;
        virtual eAppCallResult FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) = 0;
        virtual eAppCallResult ProcessCommand(const tCommand* cmd) = 0;
};


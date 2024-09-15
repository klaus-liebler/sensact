#pragma once
#include <inttypes.h>

#include "common_in_project.hh"
using namespace sensact::comm;

class cApplication{
    public:
        uint32_t const id;
        cApplication(const uint32_t id):id(id){}
        virtual ErrorCode Setup(SensactContext *ctx) = 0;
		virtual ErrorCode Loop(SensactContext *ctx) = 0;
        virtual ErrorCode FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) = 0;
        virtual ErrorCode ProcessCommand(const tCommand* cmd) = 0;
};


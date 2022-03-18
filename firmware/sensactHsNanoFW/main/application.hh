#pragma once
#include <inttypes.h>
#include "common_in_project.hh"
#include "model_applications_enums.hh"
#include "applicationcontext.hh"

namespace sensact
{
    struct Command
    {
        const eApplicationID target;
        const eCommandType command;
        const uint8_t *const payload;
        const uint8_t payloadLength;
    };
    
    
    class cApplication
    {
    public:
        eApplicationID const id;
        #include <common/commandHandlerDeclarationsVirtual.inc>
        cApplication(eApplicationID id) : id(id) {}
        virtual eAppCallResult Setup(SensactContext *ctx) = 0;
        virtual eAppCallResult Loop(SensactContext *ctx) = 0;
        //virtual eAppCallResult FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) = 0;
        //virtual eAppCallResult ProcessCommand(const tCommand *cmd) = 0;
    };
}
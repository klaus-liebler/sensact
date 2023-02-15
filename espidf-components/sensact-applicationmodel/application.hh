#pragma once
#include <inttypes.h>
#include "sensact_commons.hh"
#include "applicationmodel_enums_and_structs.hh"
#include "applicationcontext.hh"

namespace sensact::apps
{


    class cApplication
    {
    public:
        eApplicationID const id;
#include <common/commandHandlerDeclarationsVirtual.inc>
        cApplication(eApplicationID id) : id(id)
        {
        }
        virtual eAppType GetAppType() = 0;
        virtual eAppCallResult Setup(SensactContext *ctx) = 0;
        virtual eAppCallResult Loop(SensactContext *ctx) = 0;
        // virtual eAppCallResult FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector) = 0;
        // virtual eAppCallResult ProcessCommand(const tCommand *cmd) = 0;
    };
}
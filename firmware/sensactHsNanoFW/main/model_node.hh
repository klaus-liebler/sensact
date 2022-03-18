#pragma once
#include <stdint.h>

namespace sensact
{

    enum class eNodeID : uint16_t
    {
#include <common/nodeIds.inc>
    };

   
}

namespace sensact::model
{
    class node{
    public:
        static const char* const NodeDescription;
        static const sensact::eNodeID NodeID;
    };
}
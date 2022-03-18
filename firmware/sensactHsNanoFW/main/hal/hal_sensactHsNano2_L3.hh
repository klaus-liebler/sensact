#pragma once
#include "hal_sensactHsNano2.hh"
namespace sensact::hal::SensactHsNano2::L3{
    std::vector<NodeRole> nodeRoles{NodeRole::APPLICATION_HOST, NodeRole::GATEWAY,};
    
    class cHAL: public sensact::hal::SensactHsNano2::cHAL{

    };
}
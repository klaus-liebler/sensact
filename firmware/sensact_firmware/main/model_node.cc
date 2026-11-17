#include "model_node.hh"
#include <string>
#include <station_config.hh>
#include <runtimeconfig_cpp/runtimeconfig.hh>

namespace sensact::model
{
    const char* node::NodeDescription = "no station configuration (safe mode)";
    sensact::eNodeID node::NodeID = sensact::eNodeID::UNKNOWN;
    const char* node::NodeName = cfg::NODE_ID;

    void node::ApplyStationConfig()
    {
        static std::string description;
        if (!sensact::config::Station.loaded) return;
        NodeID = sensact::config::Station.nodeId;
        NodeName = sensact::config::Station.nodeName.c_str();
        description = "NodeId " + sensact::config::Station.nodeName + " configured on " + sensact::config::Station.created +
                      " using model " + sensact::config::Station.houseName;
        NodeDescription = description.c_str();
    }
}

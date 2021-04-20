#pragma once
#include "esp_log.h"
#include "common.hh"
#include "applicationBase.hh"
#include <vector>

class Manager:public PostOffice
{
private:
    HAL * const hal;
    SensactContext ctx;
    std::vector<cApplication *> apps;
public:
    Manager(HAL *hal);
    ErrorCode HandleCommandFromWebUI(const sensact::comm::tCommand *cmd);
    ErrorCode FillBuilderWithStateForWebUI(flatbuffers::FlatBufferBuilder *builder);
    ErrorCode Init();
    ErrorCode Loop();
    ErrorCode PostCommand(const tCommand* cmd);
};
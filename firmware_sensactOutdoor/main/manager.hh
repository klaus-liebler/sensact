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
    ErrorCode ParseConfigAndInitApps(const char *buf, size_t len);
public:
    Manager(HAL *hal);
    ErrorCode HandleWebUICommand(cJSON* json);
    ErrorCode WebUIData(BinaryWriter *sw);
    ErrorCode Init();
    ErrorCode Loop();
    ErrorCode PostCommand(uint32_t target, cJSON *json);
};
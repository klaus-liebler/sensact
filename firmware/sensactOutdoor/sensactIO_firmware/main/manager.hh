#pragma once
#include "esp_log.h"
#include "common.hh"
#include "applicationBase.hh"
#include <rx470c.hh>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>


class Manager:public PostOffice, public InputOutput
{
private:
    HAL * const hal;
    SensactContext ctx;
    std::vector<cApplication *> apps;
    std::vector<IOSource*> ioSources;
    std::vector<uint16_t> outputU16Buffer;
    uint32_t outputBufferChangedBits;
    uint32_t inputsBuffer;
    std::vector<IOMode> configurationBuffer;
    bool reconfigurationRequestOccured{false};
    SemaphoreHandle_t handleCommandSemaphore = NULL;
    EventGroupHandle_t eventGroup;
    EventBits_t stop_requestBit;
    void Task();
    static void Task_static(void *pvParameters);
public:
    Manager(HAL *hal, std::vector<IOSource *> ioSources, EventGroupHandle_t eventGroup, EventBits_t stop_requestBit);
    virtual ~Manager(){
    }
    ErrorCode HandleCommand(const sensact::comm::tCommand *cmd);
    ErrorCode FillBuilderWithStateForWebUI(flatbuffers::FlatBufferBuilder *builder);
    ErrorCode SetupAndRun();
    ErrorCode PostCommand(const tCommand* cmd) override;
    ErrorCode ConfigureIO(uint16_t index, IOMode mode) override;
    ErrorCode SetU16Output(uint16_t index, uint16_t state) override;
    ErrorCode GetBoolInputs(uint32_t *value) override;
};
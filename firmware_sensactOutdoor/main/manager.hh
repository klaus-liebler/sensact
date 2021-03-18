#pragma once
#include "esp_log.h"
#include "common.hh"
#include <vector>
#include "aBlind.hh"
#define TAG "manager"

static cBlind markise(eApplicationID::MARKISE, RELAY1, RELAY2);
static cBlind rollo(eApplicationID::ROLLO, RELAY1, RELAY2);

class Manager
{
private:
    HAL *hal;
public:
    Manager(HAL *hal) : hal(hal)
    {
    }

    ErrorCode WebUIEvent(uint32_t eApplicationId, uint32_t eventId, uint32_t* payload, size_t payloadLen)
    {
        return ErrorCode::OK;
    }

    ErrorCode Init()
    {
        //uint32_t nowMsSteady = hal->GetMillis();
        return ErrorCode::OK;
    }

    ErrorCode Loop()
    {
        //uint32_t nowMsSteady = hal->GetMillis();
        return ErrorCode::OK;
    }
};

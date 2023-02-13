#pragma once

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int64_t tms_t;

#define LOGI ESP_LOGI
#define LOGD ESP_LOGD
#define LOGE ESP_LOGE
#define LOGW ESP_LOGW

enum class ErrorCode:int
{
    OK = 0,
    
    GENERIC_ERROR = 1,
    QUEUE_OVERLOAD = 2,
    NONE_AVAILABLE=3,
    INDEX_OUT_OF_BOUNDS=4,
    INVALID_NEW_FBD=5,
    NOT_YET_IMPLEMENTED = 6,
    TEMPORARYLY_NOT_AVAILABLE = 7,
    FILE_SYSTEM_ERROR=8,
    INCOMPATIBLE_VERSION=9,
    EMERGENCY_SHUTDOWN=10,
    INVALID_ARGUMENT_COUNT=11,
    INVALID_ARGUMENT_VALUES=12,
    OBJECT_NOT_CHANGED=13,
    INVALID_APPLICATION_ID=14,
    INVALID_EVENT_ID=15,
    INVALID_STATE=16,
    INVALID_COMMAND=17,
    NO_CONFIGURATION_FOUND=18,
    PIN_NOT_AVAILABLE,
    PIN_DOES_NOT_SUPPORT_MODE,
    PIN_MODE_DOES_NOT_ALLOW_THIS_SETTING,
    SEMAPHORE_NOT_AVAILABLE,
    DEVICE_NOT_RESPONDING,
    HARDWARE_NOT_INITIALIZED,
    PAYLOAD_TOO_LARGE,
    OK_BUT_NOT_NEEDED,
    FUNCTION_NOT_AVAILABLE,
    
};

namespace sensact
{

    struct CANMessage
    {
        uint32_t Id;
        uint8_t Data[8];
        size_t DataLen;
    };
}


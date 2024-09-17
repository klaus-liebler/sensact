#pragma once
#include <stdint.h>
#include <ctime>
#include <common.hh>
#include <errorcodes.hh>
#include <esp_log.h>




typedef uint16_t InOutId;

namespace sensact::strings
{
    constexpr char SUCCESSFUL_STRING[] = "Initialization of %s was successful";
    constexpr char NOT_SUCCESSFUL_STRING[] = "Initialization of %s was NOT successful";
}

namespace sensact
{
    enum class eRotaryEncoder : uint8_t
    {
        ROTENC0,
        ROTENC1,
    };

    enum class ePushState : uint8_t
    {
        RELEASED = 0,
        PRESSED = 1
    };

    enum class ePowerState : uint16_t
    {
        INACTIVE = 0,
        ACTIVE = 0xFFFF,
    };

    enum class eDirection : uint8_t
    {
        STOP,
        UP,
        DOWN,
        PREPAREUP,
        PREPAREDOWN,
        PREPARESTOP
    };

    enum class eRelayInterlockMode{
		R1_POWER__R2_UP,
		R1_POWER__R2_DOWN,
		R1_UP__R2_POWER,
		R1_DOWN__R2_POWER,
		R1_DOWN__R2_UP,
		R1_UP__R2_DOWN,
	};

    struct CANMessage
    {
        uint32_t Id;
        uint8_t Data[8];
        size_t DataLen;
    };

    struct WebMessage
    {
        uint32_t Id;
        uint8_t Data[8];
        size_t DataLen;
        bool send2can;
    };
	
	class iWebsensact{
        public:
        virtual ErrorCode TryReceiveWebMessage(sensact::WebMessage &m)=0;
    };
}

namespace sensact::magic
{
    constexpr InOutId INPUT1 = 0x3FFF;      // steht für einen Input mit dauerhaft 1 (letzte Bits sind 1 (wie I=input)-1)
    constexpr InOutId INPUT0 = 0x3FFE;      // steht für einen Input mit dauerhaft 0 (letzte Bits sind 1 (wie I=input)-0)
    constexpr InOutId OUTPUT_NULL = 0x3FFC; // steht für einen unbeschalteten Output
    constexpr tms_t SHORT_PRESS = 400;
    constexpr tms_t DOUBLE_PRESS = 600;
    constexpr tms_t HEARTBEAT_STANDBY_SENDER{5555};
    constexpr tms_t HEARTBEAT_STANDBY_RECEIVER{30000};
    constexpr tms_t TIME_MAX = INT64_MAX;
    
    constexpr uint16_t ACTIVE = UINT16_MAX;
    constexpr uint16_t INACTIVE = 0;
}
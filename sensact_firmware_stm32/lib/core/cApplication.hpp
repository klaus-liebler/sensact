#pragma once
#include <cstddef>
#include <error.hpp>
#include <common.hpp>
#include <context.hpp>

namespace sensactcore
{
    enum class eApplicationID:uint16_t{
        #include <generated/appids.hinc>
    };

    enum struct eNodeID:uint16_t{
        #include <generated/nodeids.hinc>
    };
    enum class eCommandType:uint8_t{
        #include <generated/commandTypes.hinc>
    };
    enum class eEventType:uint8_t{
        #include <generated/eventTypes.hinc>
    };

    //used in multiple Apps...
    enum struct ePowerState : uint8_t
    {
        INACTIVE = 0,
        ACTIVE = 1
    };

    //used in multiple Apps
    enum struct ePushState : uint8_t
	{
		RELEASED = 0,
		PRESSED = 1
	};

    //used in multiple Apps
    enum struct eRelayMode : uint8_t
	{
		TWO_PHASES = 0,
		INTERLOCKED_ACTIVE_DOWN = 1,
		INTERLOCKED_ACTIVE_UP = 2
	};

    //used in multiple Apps...
    enum struct eDirection : uint8_t
	{
		STOP,
		UP,
		DOWN,
		PREPAREUP,
		PREPAREDOWN,
		PREPARESTOP
	};

    enum struct eAppCallResult : uint8_t
    {
        OK = 0,
        OK_CHANGED = 1,
        OK_CHANGEUP_START = 2,
        OK_CHANGEDOWN_START = 3,
        OK_CHANGE_END = 4,
        ERROR_GENERIC = 128,
        BUS_ERROR = 129,
        STATE_ERROR = 130,
        NOT_IMPLEMENTED = 255,
    };

    enum struct eApplicationStatus : uint8_t
    {
        STARTED,
        NEW_STATUS,
        REGULAR_STATUS,
        ERROR_ON_CYCLIC,
        TERMINATED,
    };

    enum struct eAppType : uint8_t
    {
        UNDEFINED,
        BLIND,
        POWIT,
        PWM,
        PSHBT,
        ROTAR,
        BMP180,
        BSENS,
        RGBW,
        STDBY,
        SOUND,
        MLTLC,
        RCEVT,
        LIBAR,
        SNSCT,
        FRWRD,
    };
    const char *const eAppType2Name[] = {
        "UNDEFINED",
        "BLIND",
        "POWIT",
        "PWM",
        "PSHBT",
        "ROTAR",
        "BMP180",
        "BSENS",
        "RGBW",
        "STDBY",
        "SOUND",
        "MLTLC",
        "RCEVT",
        "LIBAR",
        "SNSCT",
        "FRWRD",
    };

    struct Command
    {
        const eApplicationID target;
        const eCommandType command;
        const uint8_t *const payload;
        const uint8_t payloadLength;
    };


class cApplication {
public:
	eApplicationID Id;
    
    #include <generated/cApplication.hinc>

	virtual eAppType GetAppType()=0;
	virtual eAppCallResult Setup(SensactContext *ctx)=0;
	virtual eAppCallResult DoEachCycle(SensactContext *ctx, uint8_t *statusBuffer, size_t *statusBufferLength)=0; //has to be called cyclically
	eAppCallResult OnCommand(eCommandType cmd, const uint8_t * const payload, uint8_t payloadLength, SensactContext *ctx);
	virtual eAppCallResult OnEvent(eApplicationID sender, eEventType evt, const uint8_t * const payload, uint8_t payloadLength, SensactContext *ctx);
	cApplication(const eApplicationID id);
	static char const* NN(eApplicationID appID);
	char const* N();
	static char const* N4I(uint32_t appID);
	virtual ~cApplication() {
		return;
	}
};

} // namespace sensactcore

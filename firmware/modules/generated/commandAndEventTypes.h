/*
@file		CommandAndEventTypes
@author		autogen
@version	1.0
@date		06/04/2016 00:08:11 (Rendered)
@brief		Short
@details	Details
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */

#pragma once
#include <inttypes.h>
namespace sensact{
enum struct eCommandType:uint8_t{
	    RESET,
        START,
        STOP,
        UP,
        DOWN,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        ON,
        OFF,
        TOGGLE,
        TOGGLE_SPECIAL,
        SET_PARAM,
        SET_HORIZONTAL_TARGET,
        STEP_HORIZONTAL,
        SET_VERTICAL_TARGET,
        STEP_VERTICAL,
        SET_LATERAL_TARGET,
        STEP_LATERAL,
        HEARTBEAT,
        SEND_STATUS,
        SET_RGBW,
        SET_SIGNAL,
        CNT
};

enum struct eEventType:uint8_t{
	    PRESSED,
        RELEASED,
        PRESSED_SHORT_AND_HOLD,
        PRESSED_MEDIUM_AND_HOLD,
        RELEASED_SHORT,
        RELEASED_MEDIUM,
        RELEASED_LONG,
        TURNED,
        TIMER,
        STATUS,
        NEW_SENSOR_VALUE,
        CRITICAL_VALUE,
        ALARM,
        WARN,
        CNT
};
}
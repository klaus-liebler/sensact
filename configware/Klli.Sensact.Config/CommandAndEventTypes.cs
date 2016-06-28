﻿namespace Klli.Sensact.Config
{
    public enum CommandType
    {
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
        SET_HORIZONTAL_TARGET, //Param: the target as u16
        STEP_HORIZONTAL, //Param: the step size as i16
        SET_VERTICAL_TARGET,
        STEP_VERTICAL,
        SET_LATERAL_TARGET,
        STEP_LATERAL,
        HEARTBEAT,
        SEND_STATUS,
        SET_RGBW, //Params as u8
        SET_SIGNAL, //Param as u16 (mp3 song, well known color, light sequence)
    }

    public enum EventType
    {
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
    }
}
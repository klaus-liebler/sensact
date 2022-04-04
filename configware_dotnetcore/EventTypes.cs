namespace Klli.Sensact.Config
{
    public enum EventType
    {
        NOEVENT, //no event
        PONG, //as response to ping, just to test communication
        PRESSED,
        RELEASED,
        PRESSED_SHORT_AND_HOLD,
        PRESSED_MEDIUM_AND_HOLD,
        RELEASED_SHORT,
        RELEASED_MEDIUM,
        RELEASED_LONG,
        INPUT_CHANGED,
        TURNED,
        TIMER,
        STATUS,
        NEW_SENSOR_VALUE,
        CRITICAL_VALUE,
        ALARM,
        WARN,
        IAP_READY,
        DOUBLE_CLICKED,
    }
}

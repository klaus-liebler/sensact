#pragma once
namespace sensactcore
{
    enum Error
    {
        OK,
        UNSPECIFIED_ERROR,
        I2C_DeviceNotFound,
        I2C_BusError,
        CAN_BusError,
        OneWireDeviceNotFound,
        UNKNOWN_IO,
        NOT_YET_IMPLEMENTED,
        NO_MESSAGE_AVAILABLE,
    };
}
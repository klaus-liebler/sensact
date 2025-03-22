//defined: iHAL* halobj;
//defined: temperature_sensor_handle_t tempHandle;
//defined aCANMessageBuilderParser* canMBP;

halobj = new sensact::hal::SensactOutdoor::cHAL(tempHandle);
DirectGPIOBusmaster gpioBusmaster("GPIO", halobj);
std::vector<AbstractBusmaster*> busmasters{&gpioBusmaster};

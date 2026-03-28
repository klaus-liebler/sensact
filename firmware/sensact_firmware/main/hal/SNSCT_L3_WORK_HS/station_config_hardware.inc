//defined: iHAL* halobj;
    
    
    halobj = new sensact::hal::SensactUpHs::cHAL(tempHandle);


    PCA9685::M pca9685_0(PCA9685::Device::Dev01/*A0 is connected to VCC*/, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::TotemPole, PCA9685::OutputNotEn::OutputNotEn_0, PCA9685::Frequency::Frequency_400Hz);
    std::vector<PCA9555::M *> pca9555_vec{};
    std::vector<PCA9685::M *> pca9685_vec{&pca9685_0};
    DirectGPIOBusmaster gpioBusmaster("GPIO", halobj);
    I2CBusmaster i2cBusmaster("SensactBus", (size_t)SensactUpHs::R::I2C_INT_PORT, SensactUpHs::INTERRUPT_LINES, pca9555_vec, pca9685_vec);
    std::vector<AbstractBusmaster*> busmasters{&gpioBusmaster, &i2cBusmaster};
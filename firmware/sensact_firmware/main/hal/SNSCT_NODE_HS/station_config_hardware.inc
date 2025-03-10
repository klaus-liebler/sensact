//defined: iHAL* halobj;
    
    
    halobj = new sensact::hal::SensactHsNano3::cHAL(hostname);

    iI2CPort* i2c_external = halobj->GetI2CPort(SensactHsNano3::I2C_EXTERNAL);
    PCA9555::M pca9555_0(i2c_external, PCA9555::Device::Dev0, 0x0000, 0xFFFF, 0xFFFF);
    PCA9685::M pca9685_0(i2c_external, PCA9685::Device::Dev00, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::TotemPole, PCA9685::OutputNotEn::OutputNotEn_0, PCA9685::Frequency::Frequency_400Hz);
    std::vector<PCA9555::M *> pca9555_vec{&pca9555_0};
    std::vector<PCA9685::M *> pca9685_vec{&pca9685_0};
    DirectGPIOBusmaster gpioBusmaster("GPIO", halobj);
    I2CBusmaster i2cBusmaster("SensactBus", i2c_external, SensactHsNano3::INTERRUPT_LINES, pca9555_vec, pca9685_vec);
    std::vector<AbstractBusmaster*> busmasters{&gpioBusmaster, &i2cBusmaster};
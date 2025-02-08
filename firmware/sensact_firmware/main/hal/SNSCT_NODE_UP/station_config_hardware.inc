    halobj = new sensact::hal::SensactUp3::cHAL();
    iI2CPort* i2c_internal = halobj->GetI2CPort(SensactUp3::I2C_INTERNAL);
    PCA9555::M pca9555_0(i2c_internal, PCA9555::Device::Dev0, 0x0000, 0xFFFF, 0xFFFF);
    std::vector<PCA9555::M *> pca9555_vec{&pca9555_0};
    std::vector<PCA9685::M *> pca9685_vec{};
    DirectGPIOBusmaster gpioBusmaster("GPIO", halobj);
    I2CBusmaster i2cBusmaster("SensactBus", i2c_internal, SensactUp3::INTERRUPT_LINES, pca9555_vec, pca9685_vec);
    std::vector<AbstractBusmaster*> busmasters{&gpioBusmaster, &i2cBusmaster};

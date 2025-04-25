   //defined aCANMessageBuilderParser* canMBP;
   applicationHost=new cApplicationHost(halobj, nodemaster, canMBP);
   hosts.push_back(applicationHost);

   sensact::apps::cMilightController* milightControllerApp = static_cast<sensact::apps::cMilightController*>(sensact::apps::cApplications::Glo2locCmd[(int)sensact::eApplicationID::REMOT_LX_BACK]);
   
   const uint8_t CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT{0b00110000}; //mask TX_DS, mask MAX_RT, disable CRC, CRC data len=1;
   
   auto nrf24= new Nrf24Receiver(CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT, P::NRF_CE, P::NRF_IRQ);
   nrf24->SetupSpi(SPI2_HOST, P::NRF_MISO, P::NRF_MOSI, P::NRF_SCLK, P::NRF_CS);
   auto milight = new milight::Milight9BytePayload(nrf24, milightControllerApp);
   ESP_LOGI(TAG, "Init milight");
   ESP_ERROR_CHECK(milight->SetupAndRun());


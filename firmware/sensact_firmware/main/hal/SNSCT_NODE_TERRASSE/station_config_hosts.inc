   //defined aCANMessageBuilderParser* canMBP;
   applicationHost=new cApplicationHost(halobj, nodemaster, canMBP);
   hosts.push_back(applicationHost);

   m2h= new milight::Milight2HostAdapter(nodemaster, canMBP);
   auto milight = new milight::Milight(m2h);
   ESP_LOGI(TAG, "Init milight");
   ESP_ERROR_CHECK(milight->SetupAndRun(SPI2_HOST, SPI_DMA_CH_AUTO, P::NRF_CE, P::NRF_CS, P::NRF_MISO, P::NRF_MOSI, P::NRF_SCLK));

   sensact::apps::cMilightController* milightControllerApp = static_cast<sensact::apps::cMilightController*>(sensact::apps::cApplications::Glo2locCmd[(int)sensact::eApplicationID::REMOT_X1_XX1_42]);

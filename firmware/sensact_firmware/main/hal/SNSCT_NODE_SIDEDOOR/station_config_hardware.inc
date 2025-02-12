//defined: iHAL* halobj;
    

    sensact::apps::cFingerprint* fingerprintApp= (sensact::apps::cFingerprint*)sensact::apps::applications::Glo2locCmd[(int)sensact::eApplicationID::FINGR_X1_XX1_42];
    halobj = new sensact::hal::SensactUpControl::cHAL();
    DirectGPIOBusmaster gpioBusmaster("GPIO", halobj);
    std::vector<AbstractBusmaster*> busmasters{&gpioBusmaster};
    nvs_handle_t nvsFingerName2FingerIndex;
    nvs_handle_t nvsFingerIndex2SchedulerName;
    nvs_handle_t nvsFingerIndex2ActionIndex;
    nvs_handle_t nvsSchedulerName2SchedulerObjHandle;
    ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION_NAME, NVS_FINGER_NAME_2_FINGER_INDEX_NAMESPACE, NVS_READWRITE, &nvsFingerName2FingerIndex));
    ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION_NAME, NVS_FINGER_INDEX_2_SCHEDULER_NAME_NAMESPACE, NVS_READWRITE, &nvsFingerIndex2SchedulerName));
    ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION_NAME, NVS_FINGER_INDEX_2_ACTION_INDEX_NAMESPACE, NVS_READWRITE, &nvsFingerIndex2ActionIndex));
    ESP_ERROR_CHECK(nvs_open_from_partition(NVS_PARTITION_NAME, NVS_SCHEDULER_NAMESPACE, NVS_READWRITE, &nvsSchedulerName2SchedulerObjHandle));
    sched = new scheduler::Scheduler(nvsSchedulerName2SchedulerObjHandle);
    sched->Begin();
    fingerprintAdapter= new fingerprint::FingerprintWebmanagerSensactAdapter(halobj, nvsFingerName2FingerIndex, nvsFingerIndex2SchedulerName, nvsFingerIndex2ActionIndex);
    fpm = new fingerprint::R503ProManager(UART_NUM_1, sensact::hal::SensactUpControl::P::FINGER_IRQ, fingerprintAdapter, sched, nvsFingerName2FingerIndex, nvsFingerIndex2SchedulerName, nvsFingerIndex2ActionIndex);
    fpm->Begin(sensact::hal::SensactUpControl::P::FINGER_TX, sensact::hal::SensactUpControl::P::FINGER_RX);
    fingerprintAdapter->Begin(fingerprintApp, fpm, sched);
    plugins.push_back(fingerprintAdapter);

    journalPlugin = webmanager::JournalPlugin::GetSingleton();
    plugins.push_back(journalPlugin);
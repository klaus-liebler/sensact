#pragma once
#include "../board_init_interface.hh"
#include "../hal_sensactUpControl.hh"
#include <r503pro_manager.hh>
#include <timeseries.hh>
#include <scheduler.hh>
#include <apps/fingerprint.hh>
#include <cApplications.hh>

// Echter globaler (nicht namespace-gebundener) Scope noetig: fingerprint_webmanager_sensact_
// adapter.hh referenziert diese Konstante unqualifiziert, genau wie main.ccs eigenes
// NVS_PARTITION_NAME -- muss deshalb VOR diesem Include in Sichtweite sein.
constexpr const char* NVS_FINGER_NAME_2_FINGER_INDEX_NAMESPACE{"finger"};
constexpr const char* NVS_FINGER_INDEX_2_ACTION_INDEX_NAMESPACE{"finger_act"};
constexpr const char* NVS_FINGER_INDEX_2_SCHEDULER_NAME_NAMESPACE{"finger_sched"};
constexpr const char* NVS_SCHEDULER_NAMESPACE{"scheduler"};

#include <fingerprint/fingerprint_webmanager_sensact_adapter.hh>
#include <journal_plugin.hh>

namespace sensact {

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactUpControl::cHAL(ctx.TempHandle);
        this->hal = h;
        outBusmasters.push_back(new DirectGPIOBusmaster("GPIO", h));

        auto* fingerprintApp = (sensact::apps::cFingerprint*)sensact::apps::cApplications::Glo2locCmd[(int)sensact::eApplicationID::FINGR_L1_UTIL_1];

        nvs_handle_t nvsFingerName2FingerIndex;
        nvs_handle_t nvsFingerIndex2SchedulerName;
        nvs_handle_t nvsFingerIndex2ActionIndex;
        nvs_handle_t nvsSchedulerName2SchedulerObjHandle;
        ESP_ERROR_CHECK(nvs_open_from_partition(ctx.NvsPartitionName, NVS_FINGER_NAME_2_FINGER_INDEX_NAMESPACE, NVS_READWRITE, &nvsFingerName2FingerIndex));
        ESP_ERROR_CHECK(nvs_open_from_partition(ctx.NvsPartitionName, NVS_FINGER_INDEX_2_SCHEDULER_NAME_NAMESPACE, NVS_READWRITE, &nvsFingerIndex2SchedulerName));
        ESP_ERROR_CHECK(nvs_open_from_partition(ctx.NvsPartitionName, NVS_FINGER_INDEX_2_ACTION_INDEX_NAMESPACE, NVS_READWRITE, &nvsFingerIndex2ActionIndex));
        ESP_ERROR_CHECK(nvs_open_from_partition(ctx.NvsPartitionName, NVS_SCHEDULER_NAMESPACE, NVS_READWRITE, &nvsSchedulerName2SchedulerObjHandle));

        auto* sched = new scheduler::Scheduler(nvsSchedulerName2SchedulerObjHandle);
        sched->Begin();

        auto* fingerprintAdapter = new fingerprint::FingerprintWebmanagerSensactAdapter(h, nvsFingerName2FingerIndex, nvsFingerIndex2SchedulerName, nvsFingerIndex2ActionIndex);
        auto* fpm = new fingerprint::R503ProManager(UART_NUM_1, sensact::hal::SensactUpControl::P::FINGER_IRQ, fingerprintAdapter, sched, nvsFingerName2FingerIndex, nvsFingerIndex2SchedulerName, nvsFingerIndex2ActionIndex);
        fpm->Begin(sensact::hal::SensactUpControl::P::FINGER_TX_HOST, sensact::hal::SensactUpControl::P::FINGER_RX_HOST);
        fingerprintAdapter->Begin(fingerprintApp, fpm, sched);
        ctx.Plugins->push_back(fingerprintAdapter);

        ctx.Plugins->push_back(webmanager::JournalPlugin::GetSingleton());

        return h;
    }

    void CreateHosts(IInitializationContext& ctx, sensact::iHostContext* hostCtx, std::vector<sensact::iHost*>& outHosts) override
    {
        auto* applicationHost = new cApplicationHost(this->hal, hostCtx, ctx.CanMBP);
        outHosts.push_back(applicationHost);
        ctx.Plugins->push_back(applicationHost);
    }
};

}

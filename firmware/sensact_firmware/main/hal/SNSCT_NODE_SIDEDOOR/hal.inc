

constexpr const char* NVS_FINGER_NAME_2_FINGER_INDEX_NAMESPACE{"finger"};
constexpr const char* NVS_FINGER_INDEX_2_ACTION_INDEX_NAMESPACE{"finger_act"};
constexpr const char* NVS_FINGER_INDEX_2_SCHEDULER_NAME_NAMESPACE{"finger_sched"};
constexpr const char* NVS_SCHEDULER_NAMESPACE{"scheduler"};

#include "../hal_sensactUpControl.hh"
#include <r503pro_manager.hh>
#include <timeseries.hh>
#include <scheduler.hh>
#include <apps/fingerprint.hh>
#include "./fingerprint/fingerprint_webmanager_sensact_adapter.hh"
#include "./journal_plugin.hh"

fingerprint::R503ProManager *fpm{nullptr};
scheduler::Scheduler *sched{nullptr};
fingerprint::FingerprintWebmanagerSensactAdapter* fingerprintAdapter{nullptr};
webmanager::JournalPlugin* journalPlugin{nullptr};



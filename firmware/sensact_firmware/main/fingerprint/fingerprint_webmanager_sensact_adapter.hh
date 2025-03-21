#pragma once
#include <r503pro_manager.hh>
#include <apps/fingerprint.hh>
#include <hal.hh>
#include "flatbuffers_cpp/ns07fingerprint_generated.h"
#define TAG "FGRADPTR"
namespace fingerprint
{
    class FingerprintWebmanagerSensactAdapter : public fingerprint::iFingerprintActionHandler, public webmanager::iWebmanagerPlugin
    {
    private:
        webmanager::iWebmanagerCallback *callback{nullptr};
        sensact::hal::iHAL *hal{nullptr};
        nvs_handle_t nvsFingerName2FingerIndex;
        nvs_handle_t nvsFingerIndex2SchedulerName;
        nvs_handle_t nvsFingerIndex2ActionIndex;
        sensact::apps::cFingerprint *myFingerprintSensactApp{nullptr};
        fingerprint::R503ProManager *fpm{nullptr};
        scheduler::Scheduler* sched{nullptr};
    public:
        FingerprintWebmanagerSensactAdapter(
            sensact::hal::iHAL *hal,
            nvs_handle_t nvsFingerName2FingerIndex,
            nvs_handle_t nvsFingerIndex2SchedulerName,
            nvs_handle_t nvsFingerIndex2ActionIndex) : hal(hal),
                                                       nvsFingerName2FingerIndex(nvsFingerName2FingerIndex),
                                                       nvsFingerIndex2SchedulerName(nvsFingerIndex2SchedulerName),
                                                       nvsFingerIndex2ActionIndex(nvsFingerIndex2ActionIndex) {}

        void Begin(sensact::apps::cFingerprint *myFingerprintSensactApp, fingerprint::R503ProManager *fingerprintManager, scheduler::Scheduler* sched)
        {
            this->myFingerprintSensactApp = myFingerprintSensactApp;
            this->fpm=fingerprintManager;
            this->sched=sched;
            // xTaskCreate([](void *p){((Webmanager2Fingerprint2Hardware *)p)->task(); }, "wm2fp2hw", 4096, this, 10, nullptr);
        }

        void OnBegin(webmanager::iWebmanagerCallback *callback) override //from iWebmanagerPlugin
        {
            this->callback = callback;
        }
        void OnWifiConnect(webmanager::iWebmanagerCallback *callback) override //from iWebmanagerPlugin
        {
            this->callback = callback;
        }
        void OnWifiDisconnect(webmanager::iWebmanagerCallback *callback) override
        {
            this->callback = callback;
        }
        void OnTimeUpdate(webmanager::iWebmanagerCallback *callback) override
        {
            this->callback = callback;
        }
        webmanager::eMessageReceiverResult ProvideWebsocketMessage(webmanager::iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint32_t ns, uint8_t *buf) override
        {
            this->callback = callback;
            if (ns != fingerprint::Namespace::Namespace_Value)
                return webmanager::eMessageReceiverResult::NOT_FOR_ME;
            auto rw = flatbuffers::GetRoot<fingerprint::RequestWrapper>(buf);
            flatbuffers::FlatBufferBuilder b(1024);
            switch (rw->request_type())
            {
            case fingerprint::Requests::Requests_RequestEnrollNewFinger:
            {
                const char *name = rw->request_as_RequestEnrollNewFinger()->name()->c_str();
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseEnrollNewFinger,
                        fingerprint::CreateResponseEnrollNewFinger(b, (uint16_t)fpm->TryEnrollAndStore(name)).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestDeleteAllFingers:
            {
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseDeleteAllFingers,
                        fingerprint::CreateResponseDeleteAllFingers(b, (uint16_t)fpm->TryDeleteAll()).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestDeleteFinger:
            {
                const char *name = rw->request_as_RequestDeleteFinger()->name()->c_str();
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseDeleteFinger,
                        fingerprint::CreateResponseDeleteFingerDirect(b, (uint16_t)fpm->TryDelete(name), name).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestCancelInstruction:
            {
                 b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseCancelInstruction,
                        fingerprint::CreateResponseCancelInstruction(b, (uint16_t)fpm->CancelInstruction()).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestRenameFinger:
            {
                const char *oldName = rw->request_as_RequestRenameFinger()->old_name()->c_str();
                const char *newName = rw->request_as_RequestRenameFinger()->new_name()->c_str();
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseRenameFinger,
                        fingerprint::CreateResponseRenameFinger(b, (uint16_t)fpm->TryRename(oldName, newName)).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests_RequestFingerprintSensorInfo:
            {
                auto p = fpm->GetAllParams();

                fingerprint::Uint8x32 usedIndices(p->libraryIndicesUsed);
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseFingerprintSensorInfo,
                        fingerprint::CreateResponseFingerprintSensorInfoDirect(b,
                                                                                                                 p->status,
                                                                                                                 p->librarySizeMax,
                                                                                                                 p->librarySizeUsed,
                                                                                                                 &usedIndices,
                                                                                                                 p->securityLevel,
                                                                                                                 p->deviceAddress,
                                                                                                                 p->dataPacketSizeCode,
                                                                                                                 p->baudRateTimes9600,
                                                                                                                 p->algVer, p->fwVer)
                                                               .Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestStoreFingerAction:
            {
                auto s =rw->request_as_RequestStoreFingerAction();
                fpm->TryStoreFingerAction(s->finger_index(), s->action_index());
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseStoreFingerAction,
                        fingerprint::CreateResponseStoreFingerAction(b).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }

            case fingerprint::Requests::Requests_RequestStoreFingerSchedule:
            {
                auto s =rw->request_as_RequestStoreFingerSchedule();
                fpm->TryStoreFingerScheduler(s->finger_index(), s->schedule_name()->c_str());
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseStoreFingerSchedule,
                        fingerprint::CreateResponseStoreFingerSchedule(b).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }
            case fingerprint::Requests::Requests_RequestFingers:
            {
                std::vector<flatbuffers::Offset<flatbuffers::String>> scheduleNames;
                sched->FillFlatbufferWithAvailableNames(b, scheduleNames);

                std::vector<flatbuffers::Offset<fingerprint::Finger>> fingers_vector;

                nvs_iterator_t it{nullptr};
                esp_err_t res = nvs_entry_find(NVS_PARTITION_NAME, NVS_FINGER_NAME_2_FINGER_INDEX_NAMESPACE, NVS_TYPE_U16, &it);
                while (res == ESP_OK)
                {
                    nvs_entry_info_t info;
                    nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL
                    uint16_t fingerIndex;
                    ESP_ERROR_CHECK(nvs_get_u16(nvsFingerName2FingerIndex, info.key, &fingerIndex));
                    char fingerIndexAsString[6];
                    snprintf(fingerIndexAsString, 6, "%d", fingerIndex);

                    uint16_t actionIndex = 0;
                    if (nvs_get_u16(nvsFingerIndex2ActionIndex, fingerIndexAsString, &actionIndex) != ESP_OK)
                    {
                        ESP_LOGW(TAG, "Problem while fetching actionIndex for fingerIndex %s (%s). Assuming action 0", fingerIndexAsString, info.key);
                        actionIndex = 0;
                    }

                    size_t scheduleNameLen{0};
                    nvs_get_str(nvsFingerIndex2SchedulerName, fingerIndexAsString, nullptr, &scheduleNameLen);
                    char scheduleName[scheduleNameLen]; // scheduleNameLen+1 is NOT necessary!
                    auto err = nvs_get_str(nvsFingerIndex2SchedulerName, fingerIndexAsString, scheduleName, &scheduleNameLen);
                    if (err != ESP_OK)
                    {
                        ESP_LOGW(TAG, "Problem while fetching scheduleName for fingerIndex %s (%s). Error=%s. Assuming empty string.", fingerIndexAsString, info.key, esp_err_to_name(err));
                        scheduleName[0] = 0;
                    }
                    fingers_vector.push_back(fingerprint::CreateFingerDirect(b, info.key, fingerIndex, scheduleName, actionIndex));
                    res = nvs_entry_next(&it);
                }
                nvs_release_iterator(it);
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseFingers,
                        fingerprint::CreateResponseFingersDirect(b, &scheduleNames, &fingers_vector).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }

            case fingerprint::Requests::Requests_RequestFingerActionManually:
            {
                auto fingerIndex = rw->request_as_RequestFingerActionManually()->finger_index();
                auto actionIndex = rw->request_as_RequestFingerActionManually()->action_index();
                ESP_LOGI(TAG, "Manually do FingerprintAction");
                this->HandleFingerprintAction(fingerIndex, actionIndex);
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_ResponseFingerActionManually,
                        fingerprint::CreateResponseFingerActionManually(b).Union()
                    )
                );
                return callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b) == ESP_OK
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }

            default:
                break;
            }
            return webmanager::eMessageReceiverResult::NOT_FOR_ME;
        }

        void HandleFingerprintAction(uint16_t fingerIndex, int action) override
        {
            ESP_LOGI(TAG, "Fingerprint action %d", action);
            this->myFingerprintSensactApp->NotifyFingerDetected(action);
        }

        void HandleFingerprintDetected(uint16_t errorCode, uint16_t finger, uint16_t score) override
        {
            if (callback)
            {
                flatbuffers::FlatBufferBuilder b(256);
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_NotifyFingerDetected,
                        fingerprint::CreateNotifyFingerDetected(b, errorCode, finger, score).Union()
                    )
                );
                callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b);
                
            }
            if (errorCode == (uint8_t)grow_fingerprint::RET::OK)
            {

                ESP_LOGI(TAG, "Fingerprint detected successfully: fingerIndex=%d", finger);
                // buzzer->PlaySong(BUZZER::RINGTONE_SONG::POSITIVE);
            }
            else if (errorCode == (uint8_t)grow_fingerprint::RET::FINGER_NOT_FOUND)
            {
                ESP_LOGW(TAG, "Unknown finger!");
                // buzzer->PlaySong(BUZZER::RINGTONE_SONG::NEGATIV);
            }
        }

        void HandleEnrollmentUpdate(uint16_t errorCode, uint8_t step, uint16_t fingerIndex, const char *name) override
        {
            if (callback)
            {
                flatbuffers::FlatBufferBuilder b(256);
                b.Finish(
                    fingerprint::CreateResponseWrapper(
                        b,
                        fingerprint::Responses::Responses_NotifyEnrollNewFinger,
                        fingerprint::CreateNotifyEnrollNewFingerDirect(b, name, fingerIndex, step, errorCode).Union()
                    )
                );
                callback->WrapAndSendAsync(fingerprint::Namespace::Namespace_Value, b);
            }
        }
    };
}
#undef TAG
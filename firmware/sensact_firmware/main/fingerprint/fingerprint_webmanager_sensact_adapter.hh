#pragma once
#include <r503pro_manager.hh>
#include <apps/fingerprint.hh>
#include <hal.hh>
#include "wsprotocol_cpp/ws_protocol.hh"
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
        uint16_t lastRequestId{0};
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

        template <typename TPayload>
        webmanager::eMessageReceiverResult sendResponse(size_t (*encode)(const TPayload&, uint8_t*, size_t), const TPayload &payload)
        {
            uint8_t buf[1024];
            size_t len = encode(payload, buf, sizeof(buf));
            return (len > 0 && callback->SendRawAsync(buf, len) == ESP_OK)
                ? webmanager::eMessageReceiverResult::OK
                : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
        }

        webmanager::eMessageReceiverResult ProvideWebsocketMessage(webmanager::iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint16_t namespaceId, uint16_t messageTypeId, const uint8_t *frame, size_t frameLen) override
        {
            this->callback = callback;
            if (namespaceId != WsProtocol::fingerprint::NAMESPACE_ID)
                return webmanager::eMessageReceiverResult::NOT_FOR_ME;
            switch (messageTypeId)
            {
            case WsProtocol::fingerprint::RequestEnrollNewFinger::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestEnrollNewFinger::Payload r{};
                if (!WsProtocol::fingerprint::RequestEnrollNewFinger::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                WsProtocol::fingerprint::ResponseEnrollNewFinger::Payload resp{};
                resp.requestId = r.requestId;
                resp.errorcode = (uint16_t)fpm->TryEnrollAndStore(r.name);
                return sendResponse(WsProtocol::fingerprint::ResponseEnrollNewFinger::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestDeleteAllFingers::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestDeleteAllFingers::Payload r{};
                if (!WsProtocol::fingerprint::RequestDeleteAllFingers::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                WsProtocol::fingerprint::ResponseDeleteAllFingers::Payload resp{};
                resp.requestId = r.requestId;
                resp.errorcode = (uint16_t)fpm->TryDeleteAll();
                return sendResponse(WsProtocol::fingerprint::ResponseDeleteAllFingers::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestDeleteFinger::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestDeleteFinger::Payload r{};
                if (!WsProtocol::fingerprint::RequestDeleteFinger::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                WsProtocol::fingerprint::ResponseDeleteFinger::Payload resp{};
                resp.requestId = r.requestId;
                resp.errorcode = (uint16_t)fpm->TryDelete(r.name);
                resp.name = r.name;
                return sendResponse(WsProtocol::fingerprint::ResponseDeleteFinger::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestCancelInstruction::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestCancelInstruction::Payload r{};
                if (!WsProtocol::fingerprint::RequestCancelInstruction::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                WsProtocol::fingerprint::ResponseCancelInstruction::Payload resp{};
                resp.requestId = r.requestId;
                resp.errorcode = (uint16_t)fpm->CancelInstruction();
                return sendResponse(WsProtocol::fingerprint::ResponseCancelInstruction::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestRenameFinger::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestRenameFinger::Payload r{};
                if (!WsProtocol::fingerprint::RequestRenameFinger::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                WsProtocol::fingerprint::ResponseRenameFinger::Payload resp{};
                resp.requestId = r.requestId;
                resp.errorcode = (uint16_t)fpm->TryRename(r.oldName, r.newName);
                return sendResponse(WsProtocol::fingerprint::ResponseRenameFinger::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestFingerprintSensorInfo::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestFingerprintSensorInfo::Payload r{};
                if (!WsProtocol::fingerprint::RequestFingerprintSensorInfo::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                auto p = fpm->GetAllParams();
                WsProtocol::fingerprint::ResponseFingerprintSensorInfo::Payload resp{};
                resp.requestId = r.requestId;
                resp.status = p->status;
                resp.librarySizeMax = p->librarySizeMax;
                resp.librarySizeUsed = p->librarySizeUsed;
                static_assert(sizeof(resp.libraryUsedIndices.v) == sizeof(p->libraryIndicesUsed), "size mismatch");
                memcpy(resp.libraryUsedIndices.v, p->libraryIndicesUsed, sizeof(resp.libraryUsedIndices.v));
                resp.securityLevel = p->securityLevel;
                resp.deviceAddress = p->deviceAddress;
                resp.dataPacketSizeCode = p->dataPacketSizeCode;
                resp.baudRateTimes9600 = p->baudRateTimes9600;
                resp.algVer = p->algVer;
                resp.fwVer = p->fwVer;
                return sendResponse(WsProtocol::fingerprint::ResponseFingerprintSensorInfo::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestStoreFingerAction::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestStoreFingerAction::Payload r{};
                if (!WsProtocol::fingerprint::RequestStoreFingerAction::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                fpm->TryStoreFingerAction(r.fingerIndex, r.actionIndex);
                WsProtocol::fingerprint::ResponseStoreFingerAction::Payload resp{};
                resp.requestId = r.requestId;
                return sendResponse(WsProtocol::fingerprint::ResponseStoreFingerAction::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestStoreFingerSchedule::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestStoreFingerSchedule::Payload r{};
                if (!WsProtocol::fingerprint::RequestStoreFingerSchedule::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                fpm->TryStoreFingerScheduler(r.fingerIndex, r.scheduleName);
                WsProtocol::fingerprint::ResponseStoreFingerSchedule::Payload resp{};
                resp.requestId = r.requestId;
                return sendResponse(WsProtocol::fingerprint::ResponseStoreFingerSchedule::Encode, resp);
            }
            case WsProtocol::fingerprint::RequestFingers::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestFingers::Payload r{};
                if (!WsProtocol::fingerprint::RequestFingers::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;

                std::vector<std::string> scheduleNames;
                sched->FillAvailableScheduleNames(scheduleNames);
                // Sequenz null-terminierter Strings, keine Praefixierung je Element (UniformVariableArrayField).
                static uint8_t scheduleNames_scratch[64 * 33];
                size_t scheduleNames_pos = 0;
                for (auto &n : scheduleNames)
                {
                    size_t len = n.size();
                    if (scheduleNames_pos + len + 1 > sizeof(scheduleNames_scratch)) break;
                    memcpy(scheduleNames_scratch + scheduleNames_pos, n.c_str(), len);
                    scheduleNames_pos += len;
                    scheduleNames_scratch[scheduleNames_pos++] = 0;
                }

                static uint8_t fingers_scratch[64 * 96];
                size_t fingers_pos = 0;
                size_t fingers_count = 0;

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
                    WsProtocol::fingerprint::Finger::Payload item{};
                    item.name = info.key;
                    item.index = fingerIndex;
                    item.scheduleName = scheduleName;
                    item.actionIndex = actionIndex;
                    size_t newPos = WsProtocol::fingerprint::AppendResponseFingersFingersFingerElement(item, fingers_scratch, fingers_pos, sizeof(fingers_scratch));
                    if (newPos > 0)
                    {
                        fingers_pos = newPos;
                        fingers_count++;
                    }
                    res = nvs_entry_next(&it);
                }
                nvs_release_iterator(it);

                WsProtocol::fingerprint::ResponseFingers::Payload resp{};
                resp.requestId = r.requestId;
                resp.scheduleNamesData = scheduleNames_scratch;
                resp.scheduleNamesCount = scheduleNames.size();
                resp.scheduleNamesDataSize = scheduleNames_pos;
                resp.fingersData = fingers_scratch;
                resp.fingersCount = fingers_count;
                resp.fingersDataSize = fingers_pos;

                static uint8_t buf[8192];
                size_t len = WsProtocol::fingerprint::ResponseFingers::Encode(resp, buf, sizeof(buf));
                return (len > 0 && callback->SendRawAsync(buf, len) == ESP_OK)
                    ? webmanager::eMessageReceiverResult::OK
                    : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            }

            case WsProtocol::fingerprint::RequestFingerActionManually::TYPE_ID:
            {
                WsProtocol::fingerprint::RequestFingerActionManually::Payload r{};
                if (!WsProtocol::fingerprint::RequestFingerActionManually::Decode(frame, frameLen, r)) return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
                ESP_LOGI(TAG, "Manually do FingerprintAction");
                this->HandleFingerprintAction(r.fingerIndex, r.actionIndex);
                WsProtocol::fingerprint::ResponseFingerActionManually::Payload resp{};
                resp.requestId = r.requestId;
                return sendResponse(WsProtocol::fingerprint::ResponseFingerActionManually::Encode, resp);
            }

            default:
                break;
            }
            return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
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
                WsProtocol::fingerprint::NotifyFingerDetected::Payload notify{};
                notify.requestId = 0; // Server-Push, nicht durch einen Client-Request angestossen
                notify.errorcode = errorCode;
                notify.index = finger;
                notify.score = (uint8_t)score;
                uint8_t buf[64];
                size_t len = WsProtocol::fingerprint::NotifyFingerDetected::Encode(notify, buf, sizeof(buf));
                if (len > 0) (void)callback->SendRawAsync(buf, len);
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
                WsProtocol::fingerprint::NotifyEnrollNewFinger::Payload notify{};
                notify.requestId = 0; // Server-Push, nicht durch einen Client-Request angestossen
                notify.name = name;
                notify.index = fingerIndex;
                notify.step = step;
                notify.errorcode = errorCode;
                uint8_t buf[128];
                size_t len = WsProtocol::fingerprint::NotifyEnrollNewFinger::Encode(notify, buf, sizeof(buf));
                if (len > 0) (void)callback->SendRawAsync(buf, len);
            }
        }
    };
}
#undef TAG

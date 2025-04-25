#pragma once
#include <esp_log.h>
#include <array>
#include "webmanager_interfaces.hh"
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers_cpp/ns10journal_generated.h"
#include "messagecodes.hh"
#define TAG "JRNL"
namespace webmanager
{
    constexpr size_t STORAGE_LENGTH{16};
    struct MessageLogEntry
    {
        uint32_t messageCode;
        uint32_t lastMessageData;
        uint32_t messageCount;
        time_t lastMessageTimestamp;

        MessageLogEntry(uint32_t messageCode, uint32_t lastMessageData, uint32_t messageCount, time_t lastMessageTimestamp) : messageCode(messageCode),
                                                                                                                              lastMessageData(lastMessageData),
                                                                                                                              messageCount(messageCount),
                                                                                                                              lastMessageTimestamp(lastMessageTimestamp)
        {
        }
        MessageLogEntry() : messageCode(0),
                            lastMessageData(0),
                            messageCount(0),
                            lastMessageTimestamp(0)
        {
        }

        bool operator<(const MessageLogEntry &str) const
        {
            return (lastMessageTimestamp < str.lastMessageTimestamp);
        }
    };

    class JournalPlugin : public webmanager::iWebmanagerPlugin
    {
    private:
        static JournalPlugin *singleton;
        static __NOINIT_ATTR std::array<MessageLogEntry, STORAGE_LENGTH> messageLog;
        SemaphoreHandle_t webmanager_semaphore{nullptr};
        JournalPlugin(){
            webmanager_semaphore = xSemaphoreCreateBinary();
            xSemaphoreGive(webmanager_semaphore);
        }

    public:
        static JournalPlugin *GetSingleton()
        {
            if (!singleton)
            {
                singleton = new JournalPlugin();
            }
            return singleton;
        }
        
        void ResetJournal(){
            messageLog.fill({0, 0, 0, 0});
        }

        void OnBegin(iWebmanagerCallback *callback) override{
            
        }
        
        void OnWifiConnect(iWebmanagerCallback *callback) override{

        }
        
        void OnWifiDisconnect(iWebmanagerCallback *callback) override{

        }
        
        void OnTimeUpdate(iWebmanagerCallback *callback) override{

        }
        eMessageReceiverResult ProvideWebsocketMessage(iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint32_t ns, uint8_t* buf)override{
            if (ns != journal::Namespace::Namespace_Value)
                return webmanager::eMessageReceiverResult::NOT_FOR_ME;
            
            auto rw = flatbuffers::GetRoot<journal::RequestWrapper>(buf);
            if(rw->request_type()!=journal::Requests::Requests_RequestJournal)
                return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;

            ESP_LOGI(TAG, "Prepare to send ResponseJournal");
            flatbuffers::FlatBufferBuilder b(1024);

            std::vector<flatbuffers::Offset<journal::JournalItem>> item_vector;
            xSemaphoreTake(webmanager_semaphore, portMAX_DELAY);
            // std::sort(BUFFER.rbegin(), BUFFER.rend());
            for (int i = 0; i < messageLog.size(); i++)
            {
                if (messageLog[i].messageCode == 0)
                    continue;
                auto itemOffset = journal::CreateJournalItemDirect(b, messageLog[i].lastMessageTimestamp, messageLog[i].messageCode, messagecodes::N[messageLog[i].messageCode], messageLog[i].lastMessageData, messageLog[i].messageCount);
                item_vector.push_back(itemOffset);
            }
            xSemaphoreGive(webmanager_semaphore);
            b.Finish(
                journal::CreateResponseWrapper(
                    b,
                    journal::Responses::Responses_ResponseJournal,
                    journal::CreateResponseJournalDirect(b, &item_vector).Union()
                )
            );
            return callback->WrapAndSendAsync(journal::Namespace::Namespace_Value, b)==ESP_OK?webmanager::eMessageReceiverResult::OK:webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
        }

        void LogJournal(messagecodes::C messageCode, uint32_t messageData)
        {
            bool entryFound{false};
            struct timeval tv_now;
            gettimeofday(&tv_now, nullptr);
            xSemaphoreTake(webmanager_semaphore, portMAX_DELAY);
            time_t oldestTimestamp = INT64_MAX;
            size_t oldestIndex{0};
            for (int i = 0; i < messageLog.size(); i++)
            {
                if (messageLog[i].messageCode == 0)
                {
                    ESP_LOGD(TAG, "Found an empty logging slot on pos %d for messageCode %lu", i, (uint32_t)messageCode);
                    messageLog[i].messageCode = (uint32_t)messageCode;
                    messageLog[i].lastMessageData = messageData;
                    messageLog[i].lastMessageTimestamp = tv_now.tv_sec;
                    messageLog[i].messageCount = 1;
                    entryFound = true;
                    break;
                }
                else if (messageLog[i].messageCode == (uint32_t)messageCode)
                {
                    ESP_LOGD(TAG, "Found an updateable logging slot on pos %d for messageCode %lu", i, (uint32_t)messageCode);
                    messageLog[i].lastMessageData = messageData;
                    messageLog[i].lastMessageTimestamp = tv_now.tv_sec;
                    messageLog[i].messageCount++;
                    entryFound = true;
                    break;
                }
                if (messageLog[i].lastMessageTimestamp < oldestTimestamp)
                {
                    oldestTimestamp = messageLog[i].lastMessageTimestamp;
                    oldestIndex = i;
                }
            }
            if (!entryFound)
            {
                ESP_LOGD(TAG, "Found the oldest logging slot on pos %d for messageCode %lu", oldestIndex, (uint32_t)messageCode);
                messageLog[oldestIndex].messageCode = (uint32_t)messageCode;
                messageLog[oldestIndex].lastMessageData = messageData;
                messageLog[oldestIndex].lastMessageTimestamp = tv_now.tv_sec;
                messageLog[oldestIndex].messageCount = 1;
            }
            xSemaphoreGive(webmanager_semaphore);
            return;
        }
    };
}
#undef TAG
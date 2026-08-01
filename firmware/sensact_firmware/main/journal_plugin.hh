#pragma once
#include <esp_log.h>
#include <sys/time.h>
#include <array>
#include "webmanager_interfaces.hh"
#include "wsprotocol_cpp/ws_protocol.hh"
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
        eMessageReceiverResult ProvideWebsocketMessage(iWebmanagerCallback *callback, httpd_req_t *req, httpd_ws_frame_t *ws_pkt, uint16_t namespaceId, uint16_t messageTypeId, const uint8_t *frame, size_t frameLen)override{
            if (namespaceId != WsProtocol::journal::NAMESPACE_ID)
                return webmanager::eMessageReceiverResult::NOT_FOR_ME;
            if (messageTypeId != WsProtocol::journal::RequestJournal::TYPE_ID)
                return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
            WsProtocol::journal::RequestJournal::Payload reqPayload{};
            if (!WsProtocol::journal::RequestJournal::Decode(frame, frameLen, reqPayload))
                return webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;

            ESP_LOGI(TAG, "Prepare to send ResponseJournal");

            // Grosszuegig bemessen: bis zu STORAGE_LENGTH Eintraege, je [classId:u16][timestamp:8]
            // [messageCode:4][messageString<=64+null][messageData:4][messageCount:4].
            static uint8_t items_scratch[STORAGE_LENGTH * 128];
            size_t items_pos = 0;
            size_t items_count = 0;

            xSemaphoreTake(webmanager_semaphore, portMAX_DELAY);
            // std::sort(BUFFER.rbegin(), BUFFER.rend());
            for (int i = 0; i < messageLog.size(); i++)
            {
                if (messageLog[i].messageCode == 0)
                    continue;
                WsProtocol::journal::JournalItem::Payload item{};
                item.lastMessageTimestamp = (uint64_t)messageLog[i].lastMessageTimestamp;
                item.messageCode = messageLog[i].messageCode;
                item.messageString = messagecodes::N[messageLog[i].messageCode];
                item.messageData = messageLog[i].lastMessageData;
                item.messageCount = messageLog[i].messageCount;
                size_t newPos = WsProtocol::journal::AppendResponseJournalJournalItemsJournalItemElement(item, items_scratch, items_pos, sizeof(items_scratch));
                if (newPos > 0)
                {
                    items_pos = newPos;
                    items_count++;
                }
            }
            xSemaphoreGive(webmanager_semaphore);

            WsProtocol::journal::ResponseJournal::Payload resp{};
            resp.requestId = reqPayload.requestId;
            resp.journalItemsData = items_scratch;
            resp.journalItemsCount = items_count;
            resp.journalItemsDataSize = items_pos;

            static uint8_t buf[2560];
            size_t len = WsProtocol::journal::ResponseJournal::Encode(resp, buf, sizeof(buf));
            return (len > 0 && callback->SendRawAsync(buf, len)==ESP_OK) ? webmanager::eMessageReceiverResult::OK : webmanager::eMessageReceiverResult::FOR_ME_BUT_FAILED;
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
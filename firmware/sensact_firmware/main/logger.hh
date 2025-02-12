#pragma once
#include <messagecodes.hh>
#include <journal_plugin.hh>

#define MESSAGELOG_ON_ERROR(x, ml)                                                          \
    do                                                                                      \
    {                                                                                       \
        esp_err_t err_rc_ = (x);                                                            \
        if (unlikely(err_rc_ != ESP_OK))                                                    \
        {                                                                                   \
            LOGGER::Journal(ml, (uint32_t)err_rc_);                      \
            ESP_LOGE(TAG, "%s(%d) %s %lu", __FUNCTION__, __LINE__, #ml, (uint32_t)err_rc_); \
        }                                                                                   \
    } while (0)
#define MESSAGELOG_ON_ERRORCODE(x, mc)                                                      \
    do                                                                                      \
    {                                                                                       \
        ErrorCode err_rc_ = (x);                                                            \
        if (err_rc_ != ErrorCode::OK)                                                       \
        {                                                                                   \
            LOGGER::Journal(mc, (uint32_t)err_rc_);                      \
            ESP_LOGE(TAG, "%s(%d) %s %lu", __FUNCTION__, __LINE__, #mc, (uint32_t)err_rc_); \
        }                                                                                   \
    } while (0)

namespace LOGGER{
 void Journal(messagecodes::C messageCode, uint32_t messageData){
    webmanager::JournalPlugin::GetSingleton()->LogJournal(messageCode, messageData);
 }
}
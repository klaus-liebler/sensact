#pragma once
// Host-Ersatz fuer ESP-IDFs esp_log.h. Standardmaessig still; mit Umgebungsvariable
// SENSACT_TEST_LOG=1 werden alle Log-Ausgaben der Apps auf stdout ausgegeben (hilfreich beim
// Debuggen eines fehlschlagenden Tests).
#include <cstdarg>

void sensact_host_log(char level, const char *tag, const char *format, ...);

#define ESP_LOGE(tag, format, ...) sensact_host_log('E', tag, format, ##__VA_ARGS__)
#define ESP_LOGW(tag, format, ...) sensact_host_log('W', tag, format, ##__VA_ARGS__)
#define ESP_LOGI(tag, format, ...) sensact_host_log('I', tag, format, ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) sensact_host_log('D', tag, format, ##__VA_ARGS__)
#define ESP_LOGV(tag, format, ...) sensact_host_log('V', tag, format, ##__VA_ARGS__)

#pragma once
// Ersatz fuer ESP-IDFs esp_log.h: die gemeinsamen Sensact-Apps loggen ueber ESP_LOGx. Auf dem F103
// gehen die Meldungen auf USART1 (hal/log.cc). Debug/Verbose sind in Release-Builds wegfallend
// (SENSACT_LOG_LEVEL), damit ihre Format-Strings nicht im 128-KB-Flash landen.
#ifdef __cplusplus
extern "C" {
#endif
void sensact_log(char level, const char *tag, const char *format, ...) __attribute__((format(printf, 3, 4)));
#ifdef __cplusplus
}
#endif

#ifndef SENSACT_LOG_LEVEL
#define SENSACT_LOG_LEVEL 3 // 1 = E, 2 = W, 3 = I, 4 = D, 5 = V
#endif

#define SENSACT_LOG_IF(lvl, ch, tag, format, ...)                     \
    do                                                                 \
    {                                                                  \
        if constexpr (SENSACT_LOG_LEVEL >= (lvl))                      \
            sensact_log(ch, tag, format __VA_OPT__(, ) __VA_ARGS__);   \
    } while (0)

#define ESP_LOGE(tag, format, ...) SENSACT_LOG_IF(1, 'E', tag, format __VA_OPT__(, ) __VA_ARGS__)
#define ESP_LOGW(tag, format, ...) SENSACT_LOG_IF(2, 'W', tag, format __VA_OPT__(, ) __VA_ARGS__)
#define ESP_LOGI(tag, format, ...) SENSACT_LOG_IF(3, 'I', tag, format __VA_OPT__(, ) __VA_ARGS__)
#define ESP_LOGD(tag, format, ...) SENSACT_LOG_IF(4, 'D', tag, format __VA_OPT__(, ) __VA_ARGS__)
#define ESP_LOGV(tag, format, ...) SENSACT_LOG_IF(5, 'V', tag, format __VA_OPT__(, ) __VA_ARGS__)

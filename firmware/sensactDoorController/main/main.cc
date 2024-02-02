#include <stdio.h>
#include "fingerprint.hh"
#include <esp_log.h>
#define TAG "MAIN"

extern "C" void app_main(void)
{
    fingerprint::M* fpm = new fingerprint::M(UART_NUM_1, GPIO_NUM_35, nullptr);
    fpm->begin(GPIO_NUM_32, GPIO_NUM_39);
    while (true)
    {
        ESP_LOGI(TAG, "Free Heap: %lu", esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
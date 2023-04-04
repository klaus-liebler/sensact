#include <stdio.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "tusb_console.h"
#include "sdkconfig.h"

#include "driver/twai.h"

#include <sys/socket.h>
#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"
#include "esp_err.h"

#include <vector>
#include <array>
#include <cstring>
#include "can_message_builder_parser.hh"




constexpr twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
constexpr twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_125KBITS();

constexpr twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_7, GPIO_NUM_15, TWAI_MODE_NORMAL);

sensact::aCANMessageBuilderParser* canMBP = new sensact::cCANMessageBuilderParserOld();
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

#define TAG "MON"
constexpr auto RX_TASK_PRIO{11};



static void twai_receive_task(void *arg)
{

	// set up address to sendto
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(CONFIG_UDP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); // send message to 255.255.255.255
	int fd;
	fd = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
	LWIP_ASSERT("fd >= 0", fd >= 0);
    
    ESP_LOGI(TAG, "Start Receive and Broadcast via UDP");
    while (true) {
        twai_message_t rx_msg;
        ESP_ERROR_CHECK(twai_receive(&rx_msg, portMAX_DELAY));
        sensact::CANMessage message;
        message.Id=rx_msg.identifier;
        memcpy(message.Data, rx_msg.data, 8);
        message.DataLen=rx_msg.data_length_code;
        canMBP->TraceCommandMessage(message);
		/*int ret = */lwip_sendto(fd, (void*)&rx_msg, sizeof(rx_msg), 0, (struct sockaddr *)&addr, sizeof(addr));
       
    }
    vTaskDelete(NULL);
}


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

constexpr int MAXIMUM_RETRY{10};


static int s_retry_num = 0;


extern "C" void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "The show begins - we connect...");
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            ESP_ERROR_CHECK(esp_wifi_connect());
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            ESP_LOGE(TAG, "xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else{
        ESP_LOGI(TAG, "unknown wifi event %d", (int)event_id);   
    }
}

extern "C" void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){     
    if(event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }else{
        ESP_LOGI(TAG, "unknown ip event %d", (int)event_id);  
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        nullptr,
                                                        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ip_event_handler,
                                                        nullptr,
                                                        nullptr));

    wifi_config_t wifi_config_sta={};
    strcpy((char *)wifi_config_sta.sta.ssid, CONFIG_WIFI_SSID);
    strcpy((char *)wifi_config_sta.sta.password, CONFIG_WIFI_PASSWORD);
    wifi_config_sta.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config_sta.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}


extern "C" void app_main(void)
{
        /* Setting TinyUSB up */
    ESP_LOGI(TAG, "USB initialization");

    tinyusb_config_t tusb_cfg = {};
    tusb_cfg.device_descriptor = NULL;
    tusb_cfg.string_descriptor = NULL;
    tusb_cfg.external_phy = false; // In the most cases you need to use a `false` value
    tusb_cfg.configuration_descriptor = NULL;
    

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {}; // the configuration uses default values
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    ESP_LOGI(TAG, "log -> USB");
    esp_tusb_init_console(TINYUSB_CDC_ACM_0); // log to usb
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    
    //Install and start TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAG, "Driver installed and started");

    xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, nullptr, RX_TASK_PRIO, nullptr, tskNO_AFFINITY);

    while(true){
        
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Running...");

    }


}

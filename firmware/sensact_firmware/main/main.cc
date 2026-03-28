//#define HTTP
#define HTTPS

//c++ lib incudes
#include <cstdio>
#include <cstring>
#include <vector>

//FreeRTOS & Lwip includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//esp idf includes
#include <esp_system.h>
#include <esp_log.h>
#include <esp_littlefs.h>
#include <spi_flash_mmap.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "tinyusb.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_console.h"
#include "tinyusb_default_config.h"

#ifdef HTTPS
#include <esp_https_server.h>
#ifndef CONFIG_ESP_HTTPS_SERVER_ENABLE
#error "Enable HTTPS_SERVER in menuconfig!"
#endif
#endif
#ifdef HTTP
#include <esp_http_server.h>
#endif

//klaus-liebler component components
#include <common-esp32.hh>
#include <adc_buttons.hh>
#include <webmanager.hh>

//sensact in
#include <sensact_projectconfig.hh>
#include <can_message_builder_parser.hh>
#include <nodemaster.hh>
#include <busmaster.hh>
#include <model_node.hh>
#include <runtimeconfig_cpp/runtimeconfig.hh>
#include <webmanager_plugins/systeminfo_plugin.hh>


constexpr const char* NVS_PARTITION_NAME{NVS_DEFAULT_PART_NAME};

//board specific includes
#include <hal.inc>
#include <cApplications.hh>

#define TAG "main"
using namespace sensact;


iHAL* halobj{nullptr};

httpd_handle_t http_server{nullptr};

FLASH_FILE(esp32_pem_crt);
FLASH_FILE(esp32_pem_key);

// ---- USB descriptors --------------------------------------------------------
// Espressif VID 0x303A, PID 0x4001 = CDC-only (bit 0 set)
static const tusb_desc_device_t s_usb_device_desc = {
    sizeof(tusb_desc_device_t), // bLength
    TUSB_DESC_DEVICE,           // bDescriptorType
    0x0200,                     // bcdUSB  USB 2.0
    TUSB_CLASS_MISC,            // bDeviceClass    (required for IAD/CDC)
    MISC_SUBCLASS_COMMON,       // bDeviceSubClass
    MISC_PROTOCOL_IAD,          // bDeviceProtocol
    CFG_TUD_ENDPOINT0_SIZE,     // bMaxPacketSize0
    0x303Au,                    // idVendor  (Espressif)
    0x4001u,                    // idProduct (CDC-only auto-PID)
    0x0100u,                    // bcdDevice
    0x01,                       // iManufacturer
    0x02,                       // iProduct
    0x03,                       // iSerialNumber
    0x01,                       // bNumConfigurations
};

static const char *s_usb_string_desc[] = {
    "\x09\x04",           // 0: Language ID – English (0x0409)
    "Klaus Liebler",       // 1: Manufacturer
    cfg::NODE_ID,          // 2: Product
    cfg::NODE_ID,          // 3: Serial number (unique per node)
    "Sensact CDC Device",  // 4: CDC interface
    nullptr,
};

// CDC-ACM configuration descriptor (matches component default EP layout:
// EP1 IN = CDC notification, EP2 OUT/IN = CDC data)
#define SENSACT_USB_EPNUM_CDC_NOTIF  0x81
#define SENSACT_USB_EPNUM_CDC_OUT    0x02
#define SENSACT_USB_EPNUM_CDC_IN     0x82
#define SENSACT_USB_CFG_DESC_LEN     (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
static const uint8_t s_usb_cfg_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, SENSACT_USB_CFG_DESC_LEN, 0, 100),
    TUD_CDC_DESCRIPTOR(0, 4, SENSACT_USB_EPNUM_CDC_NOTIF, 8,
                       SENSACT_USB_EPNUM_CDC_OUT, SENSACT_USB_EPNUM_CDC_IN, 64),
};
// -----------------------------------------------------------------------------

extern "C" void app_main(void)
{
    //ESP_LOGI(TAG, "\n%s", cfg::BANNER);
    ESP_LOGI(TAG, "%s is booting up. Firmware build at %s on Git %s", cfg::NODE_ID, cfg::CREATION_DT_STR, cfg::GIT_SHORT_HASH);
    
    // Configure NVS and SPIFFS
    size_t total = 0, used = 0;
    esp_vfs_littlefs_conf_t conf = {"/spiffs", "storage", nullptr, 0,0,0,0};
    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&conf));
    ESP_ERROR_CHECK(esp_littlefs_info(conf.partition_label, &total, &used));
    ESP_LOGI(TAG, "LittleFS Partition successfully mounted: total: %dbyte, used: %dbyte", total, used);
    nvs_stats_t nvsStats;
    ESP_ERROR_CHECK(nvs_flash_init_and_erase_lazily(NVS_PARTITION_NAME, &nvsStats));
    ESP_LOGI(TAG, "NVS flash initialized. Namespace count: %d, total entries: %d, used entries: %d, free entries: %d, available entries: %d", nvsStats.namespace_count, nvsStats.total_entries, nvsStats.used_entries, nvsStats.free_entries, nvsStats.available_entries);
    
    //Configure USB
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.descriptor.device = &s_usb_device_desc;
    tusb_cfg.descriptor.string = s_usb_string_desc;
    tusb_cfg.descriptor.string_count = 5; // indices 0-4
    tusb_cfg.descriptor.full_speed_config = s_usb_cfg_desc;
    tusb_cfg.phy.self_powered = true;
    tusb_cfg.phy.vbus_monitor_io = sensact::hal::usb::VBUS_SENSE;
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    tinyusb_config_cdcacm_t acm_cfg = {}; // the configuration uses default values
    ESP_ERROR_CHECK(tinyusb_cdcacm_init(&acm_cfg));
    ESP_LOGI(TAG, "USB initialization completed. Device IDs are VID=0x%04X PID=0x%04X", s_usb_device_desc.idVendor, s_usb_device_desc.idProduct);

    //Install Temperature sensor. Temperature Sensor is used in generic hal for generic use _AND_ is used in the SystemPlugin
    //this is why the handly is created here and passed to both the hal and the plugin
    temperature_sensor_handle_t tempHandle;
    temperature_sensor_config_t temp_sensor_config = {-10, 80, TEMPERATURE_SENSOR_CLK_SRC_DEFAULT, {0}};
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &tempHandle));
    ESP_ERROR_CHECK(temperature_sensor_enable(tempHandle));
    ESP_LOGI(TAG, "Temperature sensor initialized and enabled");
    
    std::vector<webmanager::iWebmanagerPlugin*> plugins;
    plugins.push_back(new SystemInfoPlugin(tempHandle));

    aCANMessageBuilderParser* canMBP;
    if(sensact::config::USE_NEW_CAN_ID){
        canMBP = new cCANMessageBuilderParserNew();
    } else{
        canMBP = new cCANMessageBuilderParserOld();
    }
    
    #include <station_config_hardware.inc>
    assert(halobj);

    cNodemaster* nodemaster = new cNodemaster(halobj, &busmasters, canMBP);
    plugins.push_back(nodemaster);
    std::vector<sensact::iHost *> hosts;
    
    sensact::cApplicationHost* applicationHost{nullptr};

    #include <station_config_hosts.inc>
    if(applicationHost!=nullptr){
        plugins.push_back(applicationHost);
    }
    nodemaster->Setup(hosts);
    
    //Configure Network
    webmanager::M* wm = webmanager::M::GetSingleton();
    ESP_ERROR_CHECK(wm->Begin(cfg::NODE_ID, "sensact1", cfg::NODE_ID, false, &plugins, true, false));
    
    const char *hostname = wm->GetHostname();
#if defined(HTTPS)
    httpd_ssl_config_t httpd_conf = HTTPD_SSL_CONFIG_DEFAULT();
    httpd_conf.servercert = esp32_pem_crt_start;
    httpd_conf.servercert_len = esp32_pem_crt_end-esp32_pem_crt_start;
    httpd_conf.prvtkey_pem = esp32_pem_key_start;
    httpd_conf.prvtkey_len = esp32_pem_key_end-esp32_pem_key_start;
    httpd_conf.httpd.uri_match_fn = httpd_uri_match_wildcard;
    httpd_conf.httpd.max_uri_handlers = 15;
    ESP_ERROR_CHECK(httpd_ssl_start(&http_server, &httpd_conf));
    ESP_LOGI(TAG, "Secure HTTPS Server listening on https://%s:%d", hostname, httpd_conf.port_secure);
#elif defined(HTTP)
    httpd_config_t httpd_conf = HTTPD_DEFAULT_CONFIG();
    httpd_conf.uri_match_fn = httpd_uri_match_wildcard;
    httpd_conf.max_uri_handlers = 15;
    ESP_ERROR_CHECK(httpd_start(&http_server, &httpd_conf));
    ESP_LOGI(TAG, "Unsecure HTTP Server (not secure!) listening on http://%s:%d", hostname, httpd_conf.server_port);
#else
    #error "Either define HTTP or HTTPS"
#endif
    
    wm->RegisterHTTPDHandlers(http_server);
    wm->CallMeAfterInitializationToMarkCurrentPartitionAsValid();
    
    nodemaster->RunEternalLoopInTask();
    
    int counter{0};
    while (true)
    {
        wm->Supervise();
        if (++counter % 10 == 0)
        {
            ESP_LOGI(TAG, "Heap %6lu nodemaster tells:\n %s", esp_get_free_heap_size(), nodemaster->GetStatusMessage());
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}

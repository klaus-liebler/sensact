#pragma once
#include "esp_vfs.h"
#include "esp_spiffs.h"
namespace Paths{
    constexpr size_t FILE_PATH_MAX =ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN;
    constexpr const char* CFGSTORE_BASE = "/spiffs/cfg/";
    constexpr const char* CFGSTORE_BASE_DIRECTORY = "/spiffs/cfg";
    constexpr const char *DEFAULTCFG_PATH = "/spiffs/defaultiocfg.bin";
}
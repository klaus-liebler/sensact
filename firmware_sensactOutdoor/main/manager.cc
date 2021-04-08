#include "esp_log.h"
#include "common.hh"
#include <vector>
#include "manager.hh"
#include "aBlind.hh"
#include "aRgbPwm.hh"
#include "aWwcwPwm.hh"
#include "aSinglePwm.hh"
#include "aStandbycontroller.hh"
#include "paths_and_files.hh"
#include "cJSON.h"

#define TAG "manager"
/*
static cStandbyController standby1(eApplicationID::STANDBY1, RELAY5, 10000);
static cStandbyController standby2(eApplicationID::STANDBY2, RELAY6, 10000);
static cRgbPWM rgb(eApplicationID::RGB, MOSFET1, MOSFET2, MOSFET3, &standby1);
static cWwcwPWM wwcw(eApplicationID::WWCW, MOSFET4, MOSFET5, &standby1);
static cSinglePWM spots(eApplicationID::SPOTS, LED_CC, &standby2);
static cBlind awning(eApplicationID::AWNING, RELAY1, RELAY2);
static cBlind rollo(eApplicationID::ROLLO, RELAY3, RELAY4);

//std::vector<cApplication *> apps;
//{&rgb, &wwcw, &spots, &awning, &rollo, &standby1, &standby2};
*/
Manager::Manager(HAL *hal) : hal(hal)
{
    ctx.node = hal;
    ctx.postOffice=this;
    ctx.now = hal->GetMillis();
}

ErrorCode Manager::HandleWebUICommand(cJSON* json)
{
    int applicationId = cJSON_GetObjectItem(json, "applicationid")->valueint;
    cApplication* app = apps.at(applicationId-1);
    if(!app){
        return ErrorCode::INVALID_APPLICATION_ID;
    }
    return app->ProcessJsonCommand(json);
}

ErrorCode Manager::WebUIData(BinaryWriter *sw)
{
    for (const auto &app : apps)
    {
        app->FillStatus(sw);
    }
    return ErrorCode::OK;
}

ErrorCode Manager::Init()
{
    ctx.now = this->hal->GetMillis();
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "Trying to open %s", Paths::DEFAULTCFG_JSON_PATH);
    if (stat(Paths::DEFAULTCFG_JSON_PATH, &file_stat) == -1) {
        ESP_LOGI(TAG, "Default PLC file %s does not exist. Using factory default instead", Paths::DEFAULTCFG_JSON_PATH);
        return ErrorCode::OK;
    }
    fd = fopen(Paths::DEFAULTCFG_JSON_PATH, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", Paths::DEFAULTCFG_JSON_PATH);
        return ErrorCode::FILE_SYSTEM_ERROR;
    }
    char buf[file_stat.st_size+1];
	buf[file_stat.st_size] = 0;
    size_t size_read = fread(buf, 1, file_stat.st_size, fd);
    if(size_read!=file_stat.st_size){
        ESP_LOGE(TAG, "Unable to read file completely : %s", Paths::DEFAULTCFG_JSON_PATH);
        return ErrorCode::FILE_SYSTEM_ERROR;
    }
    ESP_LOGI(TAG, "Successfully read %s", Paths::DEFAULTCFG_JSON_PATH);
    

    return this->ParseConfigAndInitApps(buf, size_read);
    for (const auto &app : apps)
    {
        app->Setup(&this->ctx);
    }
    return ErrorCode::OK;
}



ErrorCode Manager::ParseConfigAndInitApps(const char *buf, size_t len){
    
	cJSON *root = cJSON_Parse(buf);
    int root_array_size = cJSON_GetArraySize(root);
    this->apps.resize(root_array_size);
    ESP_LOGI(TAG, "root_array_size=%d", root_array_size);
	for (uint32_t i=0;i<root_array_size;i++) {
        int id=i+1;
		cJSON *arrayItem = cJSON_GetArrayItem(root,i);
        char *type = cJSON_GetObjectItem(arrayItem,"type")->valuestring;
        switch (hashStr(type))
        {
        case hashStr("BLIND"):
            apps[i]=cBlind::BuildFromJSON(id, arrayItem);
            break;
        case hashStr("SINGLEPWM"):
            apps[i]=cSinglePWM::BuildFromJSON(id, arrayItem);
            break;
        default:
            break;
        }
		
	}
	cJSON_Delete(root);
    return ErrorCode::OK;
}

ErrorCode Manager::Loop()
{
    ctx.now = this->hal->GetMillis();
    for (const auto &app : apps)
    {
        app->Loop(&this->ctx);
    }
    return ErrorCode::OK;
}

ErrorCode Manager::PostCommand(uint32_t target, cJSON *json){
    if(target==0){
        return ErrorCode::OK;
    }
    cApplication *app=this->apps.at(target-1);
    if(!app){
        return ErrorCode::NONE_AVAILABLE;
    }
    return app->ProcessJsonCommand(json);
}


#undef TAG
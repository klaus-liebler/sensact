#include "esp_log.h"
#include "common.hh"
#include <vector>
#include "manager.hh"
#include "aBlind.hh"
#include "aSinglePwm.hh"
#include "aOnOff.hh"
#include "paths_and_files.hh"
#include "cJSON.h"
#include "../generated/webui_core_comm_generated.h"

using namespace sensact::comm;

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

ErrorCode Manager::HandleCommandFromWebUI(const sensact::comm::tCommand *cmd)
{
    cApplication* app = apps.at(cmd->applicationId()-1);
    if(!app){
        return ErrorCode::INVALID_APPLICATION_ID;
    }
    return app->ProcessCommand(cmd);
}

ErrorCode Manager::FillBuilderWithStateForWebUI(flatbuffers::FlatBufferBuilder *builder){
    
    std::vector<flatbuffers::Offset<tStateWrapper>> status_vector;
    for (const auto &app : apps)
    {
        app->FillStatus(builder, &status_vector);
    }
    auto states = builder->CreateVector(status_vector);
    auto state_obj = CreatetState(*builder, hal->GetMillis(), states);
    builder->Finish(state_obj);
    return ErrorCode::OK;
}

ErrorCode Manager::Init()
{
    ctx.now = this->hal->GetMillis();
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "Trying to open %s", Paths::DEFAULTCFG_PATH);
    if (stat(Paths::DEFAULTCFG_PATH, &file_stat) == -1) {
        ESP_LOGI(TAG, "Default PLC file %s does not exist. Exiting..", Paths::DEFAULTCFG_PATH);
        return ErrorCode::NO_CONFIGURATION_FOUND;
    }
    fd = fopen(Paths::DEFAULTCFG_PATH, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", Paths::DEFAULTCFG_PATH);
        return ErrorCode::FILE_SYSTEM_ERROR;
    }
    uint8_t buf[file_stat.st_size];
    size_t size_read = fread(buf, 1, file_stat.st_size, fd);
    if(size_read!=file_stat.st_size){
        ESP_LOGE(TAG, "Unable to read file completely : %s", Paths::DEFAULTCFG_PATH);
        return ErrorCode::FILE_SYSTEM_ERROR;
    }
    ESP_LOGI(TAG, "Successfully read %s", Paths::DEFAULTCFG_PATH);
    auto cfg = flatbuffers::GetRoot<tIoConfig>(buf);
    auto cfg_vect = cfg->configs();
    auto cfg_vect_size= cfg_vect->size();
    this->apps.resize(cfg_vect_size);
    for(int i=0;i<cfg_vect_size;i++){
        int id=i+1;
        auto x = cfg_vect->Get(i);
        switch(x->config_type()){
        case uConfig_tBlindConfig:
            apps[i]=cBlind::Build(id, x);
            break;
        case uConfig_tOnOffConfig:
            apps[i]=cOnOff::Build(id, x);
            break;
        case uConfig_tSinglePwmConfig:
            apps[i]=cSinglePWM::Build(id, x);
        default:
            continue;
        }
    }
    for (const auto &app : apps)
    {
        app->Setup(&this->ctx);
    }
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

ErrorCode Manager::PostCommand(const tCommand *cmd){
    if(cmd->applicationId()==0){
        return ErrorCode::OK;
    }
    cApplication *app=this->apps.at(cmd->applicationId()-1);
    if(!app){
        return ErrorCode::NONE_AVAILABLE;
    }
    return app->ProcessCommand(cmd);
}


#undef TAG
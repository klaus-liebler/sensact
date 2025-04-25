#include <algorithm> // std::max
#include "milightcontroller.hh"
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "cApplications.hh"
#define TAG "MILIGHT"
#include <sensact_logger.hh>

namespace sensact::apps
{
	
	cMilightController::cMilightController(eApplicationID id) : cApplication(id)
	{
		milightQueue = xQueueCreate(10, sizeof(std::pair<uint8_t, uint8_t>));

		this->keyCode2command[milight::keycodesFUT89::KON]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendONCommand(eApplicationID::PWM___LX_BACK_C1, sensact::time::H12_ms);
		};
		this->keyCode2command[milight::keycodesFUT89::KOFF]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendOFFCommand(eApplicationID::PWM___LX_BACK_C1, 0);
		};
		this->keyCode2command[milight::keycodesFUT89::K1_0]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendUPCommand(eApplicationID::BLIND_LX_BACK_J2, false);
		};
		this->keyCode2command[milight::keycodesFUT89::K1_1]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendDOWNCommand(eApplicationID::BLIND_LX_BACK_J2, false);
		};
		this->keyCode2command[milight::keycodesFUT89::K2_0]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendUPCommand(eApplicationID::BLIND_LX_BACK_J3, false);
		};
		this->keyCode2command[milight::keycodesFUT89::K2_1]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendDOWNCommand(sensact::eApplicationID::BLIND_LX_BACK_J3, false);
		};
	}

	eAppType cMilightController::GetAppType()
	{
		return eAppType::REMOT;
	}

	eAppCallResult cMilightController::Setup(iSensactContext *ctx)
	{
		return eAppCallResult::OK;
	}

	eAppCallResult cMilightController::Loop(iSensactContext *ctx)
	{
		std::pair<uint8_t, uint8_t> message;

		while (xQueueReceive(milightQueue, &message, 0) == pdTRUE)
		{
			uint8_t cmd = message.first;
			uint8_t arg = message.second;
			switch (cmd)
            {
            case milight::commandsFUT89::KEYS:{
                if(arg>=milight::keycodesFUT89::MAX) continue;;
                if(keyCode2command[arg]==nullptr) continue;;
                (keyCode2command[arg])(ctx);
                continue;
            }
            case milight::commandsFUT89::DIMMER:{
                //arg von 0...100%
                //ab 95% -->voll
                uint16_t val{0};
                if(arg>98){
                    val=UINT16_MAX;
                }else{
                    val=arg*(UINT16_MAX/100.0);
                }
				ESP_LOGD(TAG, "ctx->SendSET_VERTICAL_TARGETCommand(eApplicationID::PWM___LX_BACK_C1, val)");
				ctx->SendSET_VERTICAL_TARGETCommand(eApplicationID::PWM___LX_BACK_C1, val);
                continue;
            }

            default:
                ESP_LOGD(TAG, "Command %d has no associated handler", cmd);
                continue;;
            }
		}
		return eAppCallResult::OK;
	}

	eFillStatusResult cMilightController::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		buf[0]=buf[1]=buf[2]=buf[3]=0;
		return eFillStatusResult::NO_STATUS;
	}

	void cMilightController::ReceivedFromMilight(uint8_t cmd, uint8_t arg){
		auto now= esp_timer_get_time();
		if(arg == previousArg && cmd == previousCmd && (now - lastForwarded_us) < 500'000) {
			ESP_LOGD(TAG, "CMD %3d ARG %3d (blocked)", cmd, arg);
			return;
		}
		ESP_LOGI(TAG, "CMD %3d ARG %3d (forwarded)", cmd, arg);
		previousArg=arg;
		previousCmd=cmd;
		lastForwarded_us=now;

		std::pair<uint8_t, uint8_t> message = {cmd, arg};
		
		if (xQueueSend(milightQueue, &message, pdMS_TO_TICKS(10)) != pdPASS)
		{
			ESP_LOGW(TAG, "Queue ist voll, Nachricht verworfen!");
		}
	}
}


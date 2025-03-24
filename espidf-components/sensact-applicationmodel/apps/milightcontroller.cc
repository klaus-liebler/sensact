#include <algorithm> // std::max
#include "milightcontroller.hh"
#include "cApplications.hh"
#define TAG "MILIGHT"
#include <sensact_logger.hh>

namespace sensact::apps
{
	
	cMilightController::cMilightController(eApplicationID id) : cApplication(id)
	{
		this->keyCode2command[milight::keycodesFUT89::KON]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendONCommand(eApplicationID::PWM___X1_XX1_42, sensact::time::H12_ms);
		};
		this->keyCode2command[milight::keycodesFUT89::KOFF]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendOFFCommand(eApplicationID::PWM___X1_XX1_42, 0);
		};
		this->keyCode2command[milight::keycodesFUT89::K1_0]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendUPCommand(eApplicationID::BLIND_X1_XX1_42, true);
		};
		this->keyCode2command[milight::keycodesFUT89::K1_1]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendDOWNCommand(eApplicationID::BLIND_X1_XX1_42, true);
		};
		this->keyCode2command[milight::keycodesFUT89::K2_0]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendUPCommand(eApplicationID::BLIND_X1_XX1_43, true);
		};
		this->keyCode2command[milight::keycodesFUT89::K2_1]= [](sensact::apps::iSensactContext *ctx){
			ctx->SendDOWNCommand(sensact::eApplicationID::BLIND_X1_XX1_43, true);
		};
	}

	eAppType cMilightController::GetAppType()
	{
		return eAppType::REMOT;
	}

	eAppCallResult cMilightController::Setup(iSensactContext *ctx)
	{
		milightQueue = xQueueCreate(10, sizeof(std::pair<uint8_t, uint8_t>));

		if (milightQueue == nullptr)
		{
			ESP_LOGE(TAG, "Fehler beim Erstellen der Queue");
			return eAppCallResult::ERROR_GENERIC;
		}
		return eAppCallResult::OK;
	}

	eAppCallResult cMilightController::Loop(iSensactContext *ctx)
	{
		if (milightQueue == nullptr)
		{
			ESP_LOGE(TAG, "Queue nicht initialisiert!");
			return eAppCallResult::ERROR_GENERIC;
		}

		std::pair<uint8_t, uint8_t> message;

		// Prüfen, ob eine Nachricht in der Queue ist (nicht blockierend)
		while (xQueueReceive(milightQueue, &message, 0) == pdTRUE)
		{
			uint8_t cmd =message.first;
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
                if(arg>95){
                    val=UINT16_MAX;
                }else{
                    val=arg*(UINT16_MAX/100.0);
                }
                //float x = arg * (0.95 / 100.0) + 0.05;
				ctx->SendSET_VERTICAL_TARGETCommand(eApplicationID::PWM___X1_XX1_42, val);
                continue;
            }

            default:
                ESP_LOGW(TAG, "Command %d has no associated handler", cmd);
                continue;;
            }
		}
		return eAppCallResult::OK;
	}

	eAppCallResult cMilightController::FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf){
		return eAppCallResult::OK;
	}

	void cMilightController::ReceivedFromMilight(uint8_t cmd, uint8_t arg){
		if (!milightQueue)
		{
			ESP_LOGE(TAG, "Queue nicht initialisiert!");
			return;
		}

		std::pair<uint8_t, uint8_t> message = {cmd, arg};
		
		if (xQueueSend(milightQueue, &message, pdMS_TO_TICKS(10)) != pdPASS)
		{
			ESP_LOGW(TAG, "Queue ist voll, Nachricht verworfen!");
		}
	}
}


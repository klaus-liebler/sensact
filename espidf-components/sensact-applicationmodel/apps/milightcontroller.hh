#pragma once
#include <array>
#include <functional>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "cApplication.hh"
#include "milight.hh"

namespace sensact::apps
{
	namespace milight{
		namespace keycodesFUT89
		{
			constexpr uint8_t K1_1 = 1;
			constexpr uint8_t K1_0 = 10;
			constexpr uint8_t K2_1 = 2;
			constexpr uint8_t K2_0 = 11;
			constexpr uint8_t K3_1 = 3;
			constexpr uint8_t K3_0 = 12;
			constexpr uint8_t K4_1 = 4;
			constexpr uint8_t K4_0 = 13;
			constexpr uint8_t K5_1 = 5;
			constexpr uint8_t K5_0 = 14;
			constexpr uint8_t K6_1 = 6;
			constexpr uint8_t K6_0 = 15;
			constexpr uint8_t K7_1 = 7;
			constexpr uint8_t K7_0 = 16;
			constexpr uint8_t K8_1 = 8;
			constexpr uint8_t K8_0 = 17;
			constexpr uint8_t KON = 0;
			constexpr uint8_t KW = 20;
			constexpr uint8_t KOFF = 9;
			constexpr uint8_t KSMINUS = 19;
			constexpr uint8_t KSPLUS = 18;
			constexpr uint8_t MAX=21;
		}
	
		namespace commandsFUT89
		{
			constexpr uint8_t KEYS = 1;
			constexpr uint8_t WHEEL = 2;
			constexpr uint8_t DIMMER = 5;
			constexpr uint8_t MODE = 6;
			constexpr uint8_t SATURATION = 7;
		}
	}
	
	class cMilightController : public cApplication, public ::milight::iMilightCallback
	{
	private:
		std::array<std::function<void(sensact::apps::iSensactContext*)>, milight::keycodesFUT89::MAX> keyCode2command = {};
		 QueueHandle_t milightQueue{nullptr};
	public:
		void ReceivedFromMilight(uint8_t cmd, uint8_t arg) override;
		eAppType GetAppType() override;
		eAppCallResult Setup(iSensactContext *ctx) override;
		eAppCallResult Loop(iSensactContext *ctx) override;
		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override;
		cMilightController(eApplicationID id);
	};
}

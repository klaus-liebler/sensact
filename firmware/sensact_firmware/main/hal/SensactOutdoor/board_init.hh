#pragma once
// Board-Initialisierung fuer den Board-Typ SensactOutdoor (model/catalog.xml): vier Relais,
// LED-Leistungsausgang, Milight-Funkempfaenger. Der Empfaenger wird mit der ersten
// Milight-App der Stationskonfiguration verbunden.
#include "../board_init_interface.hh"
#include "../hal_sensactOutdoor.hh"
#include <apps/milightcontroller.hh>
#include <milight.hh>
#include <nrf24.hh>
#include <esp_timer.h>
#include <cApplications.hh>

namespace sensact {

// Verbindet den Milight-Funkempfaenger (ESP-IDF-Komponente milight) mit der App -- die App selbst
// kennt kein ESP-IDF (host-testbar, s. sensact-applicationmodel/host_test).
class MilightToAppAdapter : public milight::iMilightCallback {
    sensact::apps::cMilightController* app;
public:
    explicit MilightToAppAdapter(sensact::apps::cMilightController* app) : app(app) {}
    void ReceivedFromMilight(uint8_t cmd, uint8_t arg) override { app->ReceivedFromMilight(cmd, arg, esp_timer_get_time()); }
};

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    gpio_num_t VbusSensePin() override
    {
        return sensact::hal::SensactOutdoor::P::VBUS_SENSE;
    }

    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactOutdoor::cHAL(ctx.TempHandle);
        this->hal = h;
        CreateBusmastersFromStationConfig(h, 0, {GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC}, outBusmasters, false);
        return h;
    }

    void CreateHosts(IInitializationContext& ctx, sensact::iHostContext* hostCtx, std::vector<sensact::iHost*>& outHosts) override
    {
        auto* applicationHost = new cApplicationHost(this->hal, hostCtx, ctx.CanMBP, ctx.Usersettings);
        outHosts.push_back(applicationHost);
        ctx.Plugins->push_back(applicationHost);

        using namespace sensact::hal::SensactOutdoor;
        // static_cast genuegt: nur cMilightController meldet GetAppType() == REMOT
        auto* milightControllerApp = static_cast<sensact::apps::cMilightController*>(sensact::apps::cApplications::FindFirst(sensact::eAppType::REMOT));
        if (milightControllerApp == nullptr)
        {
            ESP_LOGI("BoardInit", "No milight app in the station configuration -- milight receiver not started");
            return;
        }

        const uint8_t CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT{0b00110000}; // mask TX_DS, mask MAX_RT, disable CRC, CRC data len=1
        auto* nrf24 = new Nrf24Receiver(CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT, P::NRF_CE, P::NRF_IRQ);
        nrf24->SetupSpi(SPI2_HOST, P::NRF_MISO, P::NRF_MOSI, P::NRF_SCLK, P::NRF_CS);
        auto* milight = new milight::Milight9BytePayload(nrf24, new MilightToAppAdapter(milightControllerApp));
        ESP_LOGI("BoardInit", "Init milight");
        ESP_ERROR_CHECK(milight->SetupAndRun());
    }
};

}

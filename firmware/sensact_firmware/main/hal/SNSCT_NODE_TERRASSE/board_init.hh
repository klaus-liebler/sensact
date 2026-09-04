#pragma once
#include "../board_init_interface.hh"
#include "../hal_sensactOutdoor.hh"
#include <apps/milightcontroller.hh>
#include <nrf24.hh>
#include <cApplications.hh>

namespace sensact {

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
        outBusmasters.push_back(new DirectGPIOBusmaster("GPIO", h));
        return h;
    }

    void CreateHosts(IInitializationContext& ctx, sensact::iHostContext* hostCtx, std::vector<sensact::iHost*>& outHosts) override
    {
        auto* applicationHost = new cApplicationHost(this->hal, hostCtx, ctx.CanMBP);
        outHosts.push_back(applicationHost);
        ctx.Plugins->push_back(applicationHost);

        using namespace sensact::hal::SensactOutdoor;
        auto* milightControllerApp = static_cast<sensact::apps::cMilightController*>(sensact::apps::cApplications::Glo2locCmd[(int)sensact::eApplicationID::REMOT_LX_BACK_1]);

        const uint8_t CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT{0b00110000}; // mask TX_DS, mask MAX_RT, disable CRC, CRC data len=1
        auto* nrf24 = new Nrf24Receiver(CONFIG_REGISTER_DEFAULT_VALUE_FOR_MILIGHT, P::NRF_CE, P::NRF_IRQ);
        nrf24->SetupSpi(SPI2_HOST, P::NRF_MISO, P::NRF_MOSI, P::NRF_SCLK, P::NRF_CS);
        auto* milight = new milight::Milight9BytePayload(nrf24, milightControllerApp);
        ESP_LOGI("BoardInit", "Init milight");
        ESP_ERROR_CHECK(milight->SetupAndRun());
    }
};

}

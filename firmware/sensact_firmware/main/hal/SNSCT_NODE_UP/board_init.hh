#pragma once
#include "../board_init_interface.hh"
#include "../hal_sensactUp3.hh"
#include <timeseries.hh>
#include <scheduler.hh>
#include <fingerprint.hh>

namespace sensact {

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactUp3::cHAL(ctx.TempHandle);
        this->hal = h;

        auto* pca9555_0 = new PCA9555::M(PCA9555::Device::Dev0, 0x0000, 0xFFFF, 0xFFFF);
        std::vector<PCA9555::M*> pca9555_vec{pca9555_0};
        std::vector<PCA9685::M*> pca9685_vec{};

        outBusmasters.push_back(new DirectGPIOBusmaster("GPIO", h));
        outBusmasters.push_back(new I2CBusmaster("SensactBus", (uint8_t)sensact::hal::SensactUp3::I2C_INTERNAL, sensact::hal::SensactUp3::INTERRUPT_LINES, pca9555_vec, pca9685_vec));

        return h;
    }

    void CreateHosts(IInitializationContext& ctx, sensact::iHostContext* hostCtx, std::vector<sensact::iHost*>& outHosts) override
    {
        auto* applicationHost = new cApplicationHost(this->hal, hostCtx, ctx.CanMBP);
        outHosts.push_back(applicationHost);
        ctx.Plugins->push_back(applicationHost);
    }
};

}

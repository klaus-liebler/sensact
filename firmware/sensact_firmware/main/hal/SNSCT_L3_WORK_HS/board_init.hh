#pragma once
#include "../board_init_interface.hh"
#include "../hal_sensactUpHs.hh"

namespace sensact {

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactUpHs::cHAL(ctx.TempHandle);
        this->hal = h;

        // A0 is connected to VCC
        auto* pca9685_0 = new PCA9685::M(PCA9685::Device::Dev01, PCA9685::InvOutputs::NotInvOutputs, PCA9685::OutputDriver::TotemPole, PCA9685::OutputNotEn::OutputNotEn_0, PCA9685::Frequency::Frequency_400Hz);
        std::vector<PCA9555::M*> pca9555_vec{};
        std::vector<PCA9685::M*> pca9685_vec{pca9685_0};

        outBusmasters.push_back(new DirectGPIOBusmaster("GPIO", h));
        outBusmasters.push_back(new I2CBusmaster("SensactBus", (size_t)sensact::hal::SensactUpHs::R::I2C_INT_PORT, sensact::hal::SensactUpHs::INTERRUPT_LINES, pca9555_vec, pca9685_vec));

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

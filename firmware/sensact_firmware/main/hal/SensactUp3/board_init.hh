#pragma once
// Board-Initialisierung fuer den Board-Typ SensactUp3 (model/catalog.xml); Erweiterungs-Chips und
// Apps aus der Stationskonfiguration.
#include "../board_init_interface.hh"
#include "../hal_sensactUp3.hh"

namespace sensact {

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactUp3::cHAL(ctx.TempHandle);
        this->hal = h;
        CreateBusmastersFromStationConfig(h, (uint8_t)sensact::hal::SensactUp3::I2C_INTERNAL, sensact::hal::SensactUp3::INTERRUPT_LINES, outBusmasters);
        return h;
    }

    void CreateHosts(IInitializationContext& ctx, sensact::iHostContext* hostCtx, std::vector<sensact::iHost*>& outHosts) override
    {
        auto* applicationHost = new cApplicationHost(this->hal, hostCtx, ctx.CanMBP, ctx.Usersettings);
        outHosts.push_back(applicationHost);
        ctx.Plugins->push_back(applicationHost);
    }
};

}

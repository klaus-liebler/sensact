#pragma once
// Board-Initialisierung fuer den Board-Typ SensactHsNano3 (model/catalog.xml). Gilt fuer jede
// Station mit diesem Board; was stationsspezifisch ist (Erweiterungs-Chips, Apps), kommt aus der
// Stationskonfiguration (s. CreateBusmastersFromStationConfig).
#include "../board_init_interface.hh"
#include "../hal_sensactHsNano3.hh"

namespace sensact {

class BoardInit : public IBoardInit {
private:
    sensact::hal::iHAL* hal{nullptr};

public:
    sensact::hal::iHAL* CreateHalAndBusmasters(IInitializationContext& ctx, std::vector<AbstractBusmaster*>& outBusmasters) override
    {
        auto* h = new sensact::hal::SensactHsNano3::cHAL(ctx.Hostname, ctx.TempHandle);
        this->hal = h;
        CreateBusmastersFromStationConfig(h, (uint8_t)sensact::hal::SensactHsNano3::I2C_EXTERNAL, sensact::hal::SensactHsNano3::INTERRUPT_LINES, outBusmasters);
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

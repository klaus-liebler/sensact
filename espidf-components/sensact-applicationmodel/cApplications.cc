#include "cApplications.hh"

namespace sensact
{
    const char *AppName(uint16_t id) { return apps::cApplications::Name(id); }
}

namespace sensact::apps
{
    eApplicationID cApplications::NodeMasterApplication{eApplicationID::NO_APPLICATION};
    std::vector<cApplication *> cApplications::byId;
    std::vector<cApplication *> cApplications::local;
    std::vector<const char *> cApplications::names;

    void cApplications::Reset(uint16_t appIdCount)
    {
        for (auto *a : local) delete a;
        local.clear();
        byId.assign(appIdCount, nullptr);
        names.assign(appIdCount, nullptr);
        NodeMasterApplication = eApplicationID::NO_APPLICATION;
    }

    bool cApplications::Register(cApplication *app)
    {
        uint16_t id = (uint16_t)app->id;
        if (id >= byId.size() || byId[id] != nullptr) return false;
        byId[id] = app;
        local.push_back(app);
        return true;
    }

    void cApplications::SetName(uint16_t id, const char *name)
    {
        if (id >= names.size()) names.resize(id + 1, nullptr);
        names[id] = name;
    }

    const char *cApplications::Name(uint16_t id)
    {
        if (id == (uint16_t)eApplicationID::NO_APPLICATION) return "NO_APPLICATION";
        if (id < names.size() && names[id] != nullptr) return names[id];
        return "?";
    }
}

#include <array>
// Es sind (noch) keine ueber UsersettingsPlugins eigenes WS-Protokoll
// (RequestGetUserSettings/RequestSetUserSettings, s. usersettings_controller.ts) verwaltete
// Settings-Gruppen registriert. Die einzige aktuelle Nutzung -- cApplicationHost::
// LoadPersistedParams/SavePersistedParams (fuer cSound Tag/Nacht-Einstellungen) -- geht direkt
// per Get/SetIntegerSetting an NVS, ohne ueber GetGroup()/dieses Array zu laufen.
constexpr std::array<const GroupCfg *, 0> groups{};

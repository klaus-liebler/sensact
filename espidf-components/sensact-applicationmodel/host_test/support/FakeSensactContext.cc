#include "FakeSensactContext.hh"
#include <common.hh>

namespace sensact::test
{
    using namespace sensact;

    bool FakeSensactContext::Dispatch(cApplication &appRef, eCommandType command, const uint8_t *payload, uint8_t payloadLength)
    {
        (void)payloadLength;
        cApplication *app = &appRef;
        iSensactContext *ctx = this;
        switch (command)
        {
#include <common/parseCommand.inc>
        default:
            return false;
        }
        return true;
    }

    void FakeSensactContext::SendApplicationCommandToMessageBus(eApplicationID destinationApp, eCommandType command, uint8_t *payload, uint8_t payloadLength)
    {
        sentCommands.push_back({destinationApp, command, std::vector<uint8_t>(payload, payload + payloadLength)});
    }

    void FakeSensactContext::PublishApplicationEventToMessageBus(eApplicationID sourceApp, eEventType event, const uint8_t *const payload, uint8_t payloadLength)
    {
        publishedEvents.push_back({sourceApp, event, std::vector<uint8_t>(payload, payload + payloadLength)});
    }

// Die generierten Implementierungen sind fuer die Klasse cApplicationHost geschrieben -- per
// Makro auf die Fake-Klasse umgebogen, damit der Fake exakt denselben Kodier-Code benutzt.
#define cApplicationHost FakeSensactContext
#include <common/sendCommandImplementation.inc>
#include <common/publishEventImplementation.inc>
#undef cApplicationHost
}

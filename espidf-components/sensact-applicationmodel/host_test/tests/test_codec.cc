// Prueft den generierten Kodier-/Dekodierweg der Kommandos (sendCommandImplementation.inc <->
// parseCommand.inc), auf den sich alle anderen Tests (und die echte Firmware) verlassen.
#include "TestHelpers.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    class RecordingApp : public sensact::apps::cApplication
    {
    public:
        RecordingApp() : cApplication(ID::POWIT_X1_XX3_9) {}
        eAppType GetAppType() override { return eAppType::POWIT; }
        eAppCallResult Setup(apps::iSensactContext *) override { return eAppCallResult::OK; }
        eAppCallResult Loop(apps::iSensactContext *) override { return eAppCallResult::OK; }
        eFillStatusResult FillStatus(apps::iSensactContext &, std::array<uint16_t, 4> &) override { return eFillStatusResult::NO_STATUS; }

        std::vector<std::string> calls;
        void OnONCommand(uint32_t ms, apps::iSensactContext *) override { calls.push_back("ON " + std::to_string(ms)); }
        void OnUPCommand(uint8_t forced, apps::iSensactContext *) override { calls.push_back("UP " + std::to_string(forced)); }
        void OnSTEP_VERTICALCommand(int16_t step, apps::iSensactContext *) override { calls.push_back("STEP " + std::to_string(step)); }
        void OnSET_VERTICAL_TARGETCommand(uint16_t t, apps::iSensactContext *) override { calls.push_back("TARGET " + std::to_string(t)); }
        void OnSET_RGBWCommand(uint8_t r, uint8_t g, uint8_t b, uint8_t w, apps::iSensactContext *) override
        {
            calls.push_back("RGBW " + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(w));
        }
        void OnSET_PARAMCommand(uint16_t idx, int32_t v, apps::iSensactContext *) override
        {
            calls.push_back("PARAM " + std::to_string(idx) + "=" + std::to_string(v));
        }
        void OnHEARTBEATCommand(uint32_t sender, apps::iSensactContext *) override { calls.push_back("HB " + std::to_string(sender)); }
    };
}

TEST(Codec, OnCommandEncodesU32LittleEndian)
{
    FakeSensactContext ctx;
    ctx.SendONCommand(ID::POWIT_X1_XX3_9, 0x01020304);
    ASSERT_EQ(ctx.sentCommands.size(), 1u);
    auto &c = ctx.sentCommands[0];
    EXPECT_EQ(c.target, ID::POWIT_X1_XX3_9);
    EXPECT_EQ(c.command, CMD::ON);
    EXPECT_EQ(c.payload, (std::vector<uint8_t>{0x04, 0x03, 0x02, 0x01}));
}

TEST(Codec, RoundTripThroughGeneratedParser)
{
    FakeSensactContext ctx;
    RecordingApp app;
    ctx.SendONCommand(app.id, 123456);
    ctx.SendUPCommand(app.id, 1);
    ctx.SendSTEP_VERTICALCommand(app.id, -4096);
    ctx.SendSET_VERTICAL_TARGETCommand(app.id, 65535);
    ctx.SendSET_RGBWCommand(app.id, 1, 2, 3, 4);
    ctx.SendSET_PARAMCommand(app.id, 3, -70000);
    ctx.SendHEARTBEATCommand(app.id, 42);
    for (auto &c : ctx.sentCommands)
        EXPECT_TRUE(ctx.Dispatch(app, c));
    EXPECT_EQ(app.calls, (std::vector<std::string>{"ON 123456", "UP 1", "STEP -4096", "TARGET 65535", "RGBW 1,2,3,4", "PARAM 3=-70000", "HB 42"}));
}

TEST(Codec, CommandsNotHandledByAnAppAreSilentlyIgnored)
{
    // Default-Handler aus commandHandlerDeclarationsVirtual.inc tun nichts -- ein Kommando, das
    // eine App nicht kennt, darf sie nicht in einen undefinierten Zustand bringen.
    FakeSensactContext ctx;
    RecordingApp app;
    ctx.SendTOGGLECommand(app.id);
    EXPECT_TRUE(ctx.Dispatch(app, ctx.sentCommands[0]));
    EXPECT_TRUE(app.calls.empty());
}

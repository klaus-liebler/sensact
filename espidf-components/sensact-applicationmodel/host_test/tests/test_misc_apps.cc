// Kleinere Apps: Lichtschranke, Node, Fingerprint.
#include "TestHelpers.hh"
#include "lightbarrier.hh"
#include "node.hh"
#include "fingerprint.hh"

using namespace sensact;
using namespace sensact::test;

TEST(Lightbarrier, RisingEdgeSendsOnToTarget)
{
    FakeSensactContext ctx;
    apps::cLightbarrier app(ID::FINGR_X1_XX1_42, 5, true, ID::NO_APPLICATION, ID::POWIT_X1_XX1_42);
    ctx.now = 1000;
    app.Setup(&ctx);
    RunFor(app, ctx, 500);
    EXPECT_TRUE(ctx.sentCommands.empty());
    ctx.SetInput(5, 1);
    RunFor(app, ctx, 2000);
    ASSERT_EQ(ctx.sentCommands.size(), 1u);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::ON);
    EXPECT_EQ(PayloadU32(ctx.sentCommands[0]), 0u);
    EXPECT_EQ(Status(app, ctx)[1], 1);
}

TEST(Lightbarrier, ActiveLowInput)
{
    FakeSensactContext ctx;
    apps::cLightbarrier app(ID::FINGR_X1_XX1_42, 5, false, ID::NO_APPLICATION, ID::POWIT_X1_XX1_42);
    ctx.SetInput(5, 1);
    ctx.now = 1000;
    app.Setup(&ctx);
    RunFor(app, ctx, 500);
    EXPECT_TRUE(ctx.sentCommands.empty());
    ctx.SetInput(5, 0);
    RunFor(app, ctx, 500);
    EXPECT_EQ(ctx.sentCommands.size(), 1u);
}

TEST(Lightbarrier, NoTargetSendsNothing)
{
    FakeSensactContext ctx;
    apps::cLightbarrier app(ID::FINGR_X1_XX1_42, 5, true, ID::NO_APPLICATION, ID::NO_APPLICATION);
    app.Setup(&ctx);
    ctx.SetInput(5, 1);
    RunFor(app, ctx, 500);
    EXPECT_TRUE(ctx.sentCommands.empty());
}

TEST(Node, PingIsAnsweredWithPongCarryingThePayload)
{
    FakeSensactContext ctx;
    apps::cSensactNode app(ID::SNSCT_NODE_HS);
    ctx.SendPINGCommand(app.id, 0xCAFEBABE);
    ctx.Dispatch(app, ctx.sentCommands[0]);
    ASSERT_EQ(ctx.publishedEvents.size(), 1u);
    EXPECT_EQ(ctx.publishedEvents[0].event, eEventType::PONG);
    EXPECT_EQ(ctx.publishedEvents[0].source, ID::SNSCT_NODE_HS);
    EXPECT_EQ(ctx.publishedEvents[0].payload, (std::vector<uint8_t>{0xBE, 0xBA, 0xFE, 0xCA}));
}

TEST(Fingerprint, DetectedFingerSwitchesMappedTargetOnForOneSecond)
{
    FakeSensactContext ctx;
    apps::cFingerprint app(ID::FINGR_X1_XX1_42, ID::POWIT_X1_XX1_42, ID::POWIT_X1_XX3_9, ID::POWIT_X2_XX4_10);
    app.Setup(&ctx);
    app.NotifyFingerDetected(1);
    RunFor(app, ctx, 200);
    ASSERT_EQ(ctx.sentCommands.size(), 1u);
    EXPECT_EQ(ctx.sentCommands[0].target, ID::POWIT_X1_XX3_9);
    EXPECT_EQ(ctx.sentCommands[0].command, CMD::ON);
    EXPECT_EQ(PayloadU32(ctx.sentCommands[0]), 1000u);
    RunFor(app, ctx, 1000);
    EXPECT_EQ(ctx.sentCommands.size(), 1u);
}

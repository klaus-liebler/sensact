// App-Fabrik der datengetriebenen Basis (station_config.cc): Die Stationskonfigurationen des
// Testmodells (vom Builder per BestBinaryBuffers.Runtime.BinaryEncoder geschrieben) muessen genau die
// Apps ergeben, die der bisherige Code-Generator fuer dieselbe Station als C++-Objekte angelegt haette
// (applicationInitializers.inc) -- gleiche Nummern, Namen und App-Typen, und sie muessen sich so
// verhalten wie direkt konstruierte Apps.
#include <fstream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include "TestHelpers.hh"
#include "cApplications.hh"
#include "station_config.hh"

using namespace sensact;
using namespace sensact::test;

namespace
{
    std::vector<uint8_t> ReadFile(const std::string &path)
    {
        std::ifstream f(path, std::ios::binary);
        return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    }

    // Name -> C++-Klasse aus dem generierten applicationInitializers.inc einer Station
    std::map<std::string, std::string> GeneratedApps(const std::string &node)
    {
        std::ifstream f(std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/" + node + "/applicationInitializers.inc");
        std::stringstream ss;
        ss << f.rdbuf();
        std::string text = ss.str();
        std::map<std::string, std::string> result;
        std::regex re(R"(sensact::apps::(c[A-Za-z0-9]+) ([A-Za-z0-9_]+)\()");
        for (auto it = std::sregex_iterator(text.begin(), text.end(), re); it != std::sregex_iterator(); ++it)
            result[(*it)[2]] = (*it)[1];
        return result;
    }

    eAppType TypeOfClass(const std::string &cls)
    {
        static const std::map<std::string, eAppType> m{
            {"cBlind", eAppType::BLIND}, {"cOnOff", eAppType::POWIT}, {"cSinglePWM", eAppType::PWM},
            {"cPushbuttonSingle2Toggle", eAppType::PSHBT}, {"cPushbuttonSingle2PwmSingle", eAppType::PSHBT},
            {"cPushbuttonDual2PWM", eAppType::PSHBT}, {"cPushbuttonDual2Blind", eAppType::PSHBT},
            {"cRotaryEncoder2PWM", eAppType::ROTAR}, {"cRgbw", eAppType::RGBW}, {"cSound", eAppType::SOUND},
            {"cSensactNode", eAppType::SNSCT}, {"cPump", eAppType::PUMP}, {"cBlindsTimer", eAppType::BLITIM},
            {"cFingerprint", eAppType::FINGR}, {"cMilightController", eAppType::REMOT}, {"cLightbarrier", eAppType::LIBAR},
        };
        auto it = m.find(cls);
        return it == m.end() ? eAppType::UNDEFINED : it->second;
    }

    struct StationConfigTest : ::testing::TestWithParam<std::string>
    {
        std::vector<uint8_t> data;
        void SetUp() override { data = ReadFile(std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/station_config/" + GetParam() + ".bin"); }
        void TearDown() override { apps::cApplications::Reset(0); }
    };
}

TEST_P(StationConfigTest, CreatesExactlyTheAppsOfTheGeneratedCode)
{
    ASSERT_FALSE(data.empty());
    std::string error;
    ASSERT_TRUE(config::LoadStationConfig(data.data(), data.size(), error)) << error;
    EXPECT_TRUE(config::Station.loaded);
    EXPECT_EQ(config::Station.nodeName, GetParam());
    EXPECT_EQ(config::Station.houseName, "TwoNodesDemo");

    auto expected = GeneratedApps(GetParam());
    ASSERT_FALSE(expected.empty());
    std::set<std::string> created;
    for (auto *app : apps::cApplications::Local())
    {
        std::string name = AppName(app->id);
        created.insert(name);
        ASSERT_TRUE(expected.count(name)) << name << " wurde angelegt, steht aber nicht im generierten Code";
        EXPECT_EQ(app->GetAppType(), TypeOfClass(expected[name])) << name;
        EXPECT_EQ(apps::cApplications::Get(app->id), app);
        // Name zur Nummer wie im generierten Enum des Testmodells
        EXPECT_STREQ(AppName(app->id), TestmodelAppNames[(uint16_t)app->id]);
    }
    for (auto &[name, cls] : expected)
        EXPECT_TRUE(created.count(name)) << name << " (" << cls << ") fehlt";
    EXPECT_EQ(apps::cApplications::AppIdCount(), (uint16_t)eApplicationID::CNT);
    EXPECT_EQ(apps::cApplications::NodeMasterApplication, config::Station.nodeAppId);
}

INSTANTIATE_TEST_SUITE_P(Testmodel, StationConfigTest,
                         ::testing::Values("SNSCT_NODE_HS", "SNSCT_NODE_UP", "SNSCT_NODE_SIDEDOOR", "SNSCT_NODE_TERRASSE"));

// Dasselbe fuer die ESP32-Stationen des echten Hausmodells (Sattlerstrasse16): deckt die App-Typen ab,
// die das Testmodel nicht hat (dimButton, lightbarrier, pump, rgbw).
struct HouseStationConfigTest : ::testing::TestWithParam<std::string>
{
    void TearDown() override { apps::cApplications::Reset(0); }
};

TEST_P(HouseStationConfigTest, CreatesExactlyTheAppsOfTheGeneratedCode)
{
    const std::string dir = std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/Sattlerstrasse16";
    auto data = ReadFile(dir + "/station_config/" + GetParam() + ".bin");
    ASSERT_FALSE(data.empty());
    std::string error;
    ASSERT_TRUE(config::LoadStationConfig(data.data(), data.size(), error)) << error;

    std::ifstream f(dir + "/" + GetParam() + "/applicationInitializers.inc");
    std::stringstream ss;
    ss << f.rdbuf();
    std::string text = ss.str();
    std::map<std::string, std::string> expected;
    std::regex re(R"(sensact::apps::(c[A-Za-z0-9]+) ([A-Za-z0-9_]+)\()");
    for (auto it = std::sregex_iterator(text.begin(), text.end(), re); it != std::sregex_iterator(); ++it)
        expected[(*it)[2]] = (*it)[1];
    ASSERT_FALSE(expected.empty());

    std::set<std::string> created;
    for (auto *app : apps::cApplications::Local())
    {
        std::string name = AppName(app->id);
        created.insert(name);
        ASSERT_TRUE(expected.count(name)) << name << " wurde angelegt, steht aber nicht im generierten Code";
        EXPECT_EQ(app->GetAppType(), TypeOfClass(expected[name])) << name;
    }
    for (auto &[name, cls] : expected)
        EXPECT_TRUE(created.count(name)) << name << " (" << cls << ") fehlt";
}

INSTANTIATE_TEST_SUITE_P(Sattlerstrasse16, HouseStationConfigTest,
                         ::testing::Values("SNSCT_L0_TECH_HS_1", "SNSCT_NODE_SIDEDOOR", "SNSCT_NODE_TERRASSE", "SNSCT_L3_WORK_HS"));

TEST(StationConfig, BusesComeFromTheHouseModel)
{
    auto data = ReadFile(std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/station_config/SNSCT_NODE_HS.bin");
    std::string error;
    ASSERT_TRUE(config::LoadStationConfig(data.data(), data.size(), error)) << error;
    // Testmodel.xml: SensactHsNano3 mit GPIO (Bus 0) und SensactBus (Bus 1, PCA9555 0x20, PCA9685 0x40)
    ASSERT_EQ(config::Station.buses.size(), 2u);
    EXPECT_FALSE(config::Station.buses[0].isI2c);
    EXPECT_TRUE(config::Station.buses[1].isI2c);
    EXPECT_EQ(config::Station.buses[1].index, 1);
    EXPECT_EQ(config::Station.buses[1].pca9555, std::vector<uint8_t>{0x20});
    EXPECT_EQ(config::Station.buses[1].pca9685, std::vector<uint8_t>{0x40});
    EXPECT_EQ(config::Station.nodeId, (eNodeID)0); // erste Station im Hausmodell
    apps::cApplications::Reset(0);
}

TEST(StationConfig, AppsFromConfigBehaveLikeDirectlyConstructedApps)
{
    auto data = ReadFile(std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/station_config/SNSCT_NODE_HS.bin");
    std::string error;
    ASSERT_TRUE(config::LoadStationConfig(data.data(), data.size(), error)) << error;
    // Testmodel.xml: <toggleButton id="PUSHB_X3_XX3_5" input="IN0.2" targets="POWIT_X1_XX3_9 POWIT_X2_XX4_10" />
    // IN0.2 = Bus 1, Chip 0, Kanal 2 = 0x4002
    auto *button = apps::cApplications::Get(eApplicationID::PUSHB_X3_XX3_5);
    ASSERT_NE(button, nullptr);
    FakeSensactContext ctx;
    ctx.now = 10000;
    button->Setup(&ctx);
    RunFor(*button, ctx, 1000);
    ctx.ClearRecordings();
    ctx.SetInput(0x4002, 1);
    RunFor(*button, ctx, 100);
    EXPECT_EQ(CommandsTo(ctx, eApplicationID::POWIT_X1_XX3_9), std::vector<eCommandType>{eCommandType::TOGGLE});
    EXPECT_EQ(CommandsTo(ctx, eApplicationID::POWIT_X2_XX4_10), std::vector<eCommandType>{eCommandType::TOGGLE});
    apps::cApplications::Reset(0);
}

TEST(StationConfig, RejectsDamagedAndForeignData)
{
    auto data = ReadFile(std::string(SENSACT_HOST_TEST_MODEL_DIR) + "/station_config/SNSCT_NODE_HS.bin");
    std::string error;
    // abgeschnitten
    EXPECT_FALSE(config::LoadStationConfig(data.data(), data.size() - 5, error));
    EXPECT_FALSE(error.empty());
    EXPECT_TRUE(apps::cApplications::Local().empty());
    EXPECT_FALSE(config::Station.loaded);
    // falsche Formatversion (erstes Feld nach dem 8-Byte-Kopf)
    auto wrong = data;
    wrong[8] = 99;
    EXPECT_FALSE(config::LoadStationConfig(wrong.data(), wrong.size(), error));
    EXPECT_NE(error.find("format version"), std::string::npos) << error;
    EXPECT_TRUE(apps::cApplications::Local().empty());
}

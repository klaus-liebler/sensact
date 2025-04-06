using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model;
using System.Collections.Generic;
using Klli.Sensact.Common;

namespace Klli.Sensact.Config
{
    public class TestModelBuilder:ModelBuilder<ApplicationId>
    {
        const ushort INTI = 0x0000;
        const ushort BUS0 = 0x4000;
        const ushort BUS1 = 0x8000;
        const ushort BUS2 = 0xC000;
        const ushort BUS3 = 0xF000;
        const ushort I2C = 0x0000;
        const ushort OW0 = 0x0400;
        const ushort OW1 = 0x0800;
        const ushort OW2 = 0x0C00;
        const ushort OW3 = 0x1000;

        public TestModelBuilder(): base("TwoNodesDemo")
        {
        }
        
        protected override void BuildModel()
        {
            Node NODE_HS = new SensactHsNano3((ushort)Model.ApplicationId.SNSCT_NODE_HS, Model.ApplicationId.SNSCT_NODE_HS.ToString());
            model.Nodes.Add(NODE_HS);
            Node NODE_UP = new SensactUp03((ushort)Model.ApplicationId.SNSCT_NODE_UP, Model.ApplicationId.SNSCT_NODE_UP.ToString());
            model.Nodes.Add(NODE_UP);
            Node NODE_SIDEDOOR = new SensactUpControl((ushort)Model.ApplicationId.SNSCT_NODE_SIDEDOOR, Model.ApplicationId.SNSCT_NODE_SIDEDOOR.ToString());
            model.Nodes.Add(NODE_SIDEDOOR);
            Node NODE_TERRASSE = new SensactOutdoor((ushort)Model.ApplicationId.SNSCT_NODE_TERRASSE, Model.ApplicationId.SNSCT_NODE_TERRASSE.ToString());
            model.Nodes.Add(NODE_TERRASSE);
            AddToggleButton(NODE_HS, Model.ApplicationId.PUSHB_X1_XX1_3, BUS0+I2C+0, Model.ApplicationId.POWIT_X1_XX3_9);//macht erstes Licht an
            AddToggleButton(NODE_HS, Model.ApplicationId.PUSHB_X2_XX2_4, BUS0+I2C+1, Model.ApplicationId.SOUND_X2_XX2_0);//Klingel
            AddToggleButton(NODE_HS, Model.ApplicationId.PUSHB_X3_XX3_5, BUS0+I2C+2, new HashSet<Model.ApplicationId>{Model.ApplicationId.POWIT_X1_XX3_9,Model.ApplicationId.POWIT_X2_XX4_10});//erstes und zweites Licht
            AddToggleButton(NODE_HS, Model.ApplicationId.PUSHB_X1_XX4__6, BUS0+I2C+3, new HashSet<Model.ApplicationId>{Model.ApplicationId.POWIT_X3_XX1_11,Model.ApplicationId.POWIT_X1_XX2_12});//drittes und viertes Licht
            AddBlindButtons(NODE_HS, Model.ApplicationId.BLINDB_X2_XX1_7, BUS0+I2C+4, BUS0+I2C+5, Model.ApplicationId.BLIND_X2_XX3_13);
            AddTwoDimButtons(NODE_HS, Model.ApplicationId.DIM2B_X3_XX2_8, BUS0+I2C+6, BUS0+I2C+7, Model.ApplicationId.PWM___X3_XX4_14);

            AddPowIt(NODE_HS, Model.ApplicationId.POWIT_X1_XX3_9, "erstes Licht", BUS0+I2C+0, 5000);//erstes Licht
            AddPowIt(NODE_HS, Model.ApplicationId.POWIT_X2_XX4_10, "zweites Licht", BUS0+I2C+1, 5000);//zweites Licht
            AddPowIt(NODE_HS, Model.ApplicationId.POWIT_X3_XX1_11, "drittes Licht", BUS0+I2C+2, 5000);//drittes Licht
            AddPowIt(NODE_HS, Model.ApplicationId.POWIT_X1_XX2_12, "viertes Licht", BUS0+I2C+3, 5000);//viertes Licht
            AddBlindApplication(NODE_HS, Model.ApplicationId.BLIND_X2_XX3_13, "Test-Rollade", BUS0+I2C+4, BUS0+I2C+5, RelayInterlockMode.R1_DOWN__R2_POWER, 8, 5);
            AddPWMApplication(NODE_HS, Model.ApplicationId.PWM___X3_XX4_14, "Dimmbares Licht", Model.ApplicationId.STDBY_X1_XX1_15, new HashSet<ushort>{BUS0+I2C+6}, 5000, 20, 255);
            AddPowIt(NODE_HS, Model.ApplicationId.STDBY_X1_XX1_15, "Standby für die PWMApplication", BUS0+I2C+7, 10000);//ist der Standny für die PWMApplication
            AddSound(NODE_HS, Model.ApplicationId.SOUND_X2_XX2_0, "Soundgenerator", 20, 1);

            AddRotaryEncoder(NODE_UP,Model.ApplicationId.ROTAR_X3_XX3_0, RotaryEncoder.ROTARYENCODER_1, Model.ApplicationId.PWM___X3_XX4_14);
            
            AddFingerprint(NODE_SIDEDOOR, Model.ApplicationId.FINGR_X1_XX1_42, "Fingerprint Seiteneingang", Model.ApplicationId.POWIT_X1_XX1_42, Model.ApplicationId.POWIT_X1_XX1_42, Model.ApplicationId.POWIT_X1_XX1_42);
            AddPowIt(NODE_SIDEDOOR, Model.ApplicationId.POWIT_X1_XX1_42, "Fingerprint Seiteneingang", INTI+0, 1000);

            AddMilightControllerApplication(NODE_TERRASSE, Model.ApplicationId.REMOT_X1_XX1_42, "Milight Controller");
            AddPWMApplication(NODE_TERRASSE, Model.ApplicationId.PWM___X1_XX1_42, "Spots Terrassendach", Model.ApplicationId.NO_APPLICATION, new HashSet<ushort>{INTI+4, INTI+5}, 10000, 512, 65535);
            AddBlindApplication(NODE_TERRASSE, Model.ApplicationId.BLIND_X1_XX1_42, "Markise Horizontal", INTI+0, INTI+1, RelayInterlockMode.R1_DOWN__R2_UP, 20, 15);
            AddBlindApplication(NODE_TERRASSE, Model.ApplicationId.BLIND_X1_XX1_43, "Markise Vertikal", INTI+2, INTI+3,  RelayInterlockMode.R1_DOWN__R2_UP, 20, 15);
        }
    }
}

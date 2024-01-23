using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{

    public static class TestModelBuilder
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
        
        public static Klli.Sensact.Model.Common.Model TwoNodesDemo()
        {

            Klli.Sensact.Model.Common.Model model = new Klli.Sensact.Model.Common.Model("TwoNodesDemo");
            Node NODE_HS = new SensactHsNano3((ushort)Model.ApplicationId.SNSCT_NODE_HS, Model.ApplicationId.SNSCT_NODE_HS.ToString());
            model.Nodes.Add(NODE_HS);
            Node NODE_UP = new SensactUp03((ushort)Model.ApplicationId.SNSCT_NODE_UP, Model.ApplicationId.SNSCT_NODE_UP.ToString());
            model.Nodes.Add(NODE_UP);
            NODE_HS.Applications.AddToggleButton(Model.ApplicationId.PUSHB_XX_XXX_3, BUS0+I2C+0, Model.ApplicationId.POWIT_9);//macht erstes Licht an
            NODE_HS.Applications.AddToggleButton(Model.ApplicationId.PUSHB_XX_XXX_4, BUS0+I2C+1, Model.ApplicationId.SOUND_XX_XXX_0);//Klingel
            NODE_HS.Applications.AddToggleButton(Model.ApplicationId.PUSHB_XX_XXX_5, BUS0+I2C+2, new HashSet<Model.ApplicationId>{Model.ApplicationId.POWIT_9,Model.ApplicationId.POWIT_10});//erstes und zweites Licht
            NODE_HS.Applications.AddToggleButton(Model.ApplicationId.PUSHB_XX_XXX__6, BUS0+I2C+3, new HashSet<Model.ApplicationId>{Model.ApplicationId.POWIT_11,Model.ApplicationId.POWIT_12});//drittes und viertes Licht
            NODE_HS.Applications.AddBlindButtons(Model.ApplicationId.BLINDB_7, BUS0+I2C+4, BUS0+I2C+5, Model.ApplicationId.BLIND_XX_XXX_13);
            NODE_HS.Applications.AddTwoDimButtons(Model.ApplicationId.DIM2B_8, BUS0+I2C+6, BUS0+I2C+7, Model.ApplicationId.PWM___XX_XXX_14);

            NODE_HS.Applications.AddPowIt(Model.ApplicationId.POWIT_9, "erstes Licht", BUS0+I2C+0, 5000);//erstes Licht
            NODE_HS.Applications.AddPowIt(Model.ApplicationId.POWIT_10, "zweites Licht", BUS0+I2C+1, 5000);//zweites Licht
            NODE_HS.Applications.AddPowIt(Model.ApplicationId.POWIT_11, "drittes Licht", BUS0+I2C+2, 5000);//drittes Licht
            NODE_HS.Applications.AddPowIt(Model.ApplicationId.POWIT_12, "viertes Licht", BUS0+I2C+3, 5000);//viertes Licht
            NODE_HS.Applications.AddBlindApplication(Model.ApplicationId.BLIND_XX_XXX_13, "Test-Rollade", BUS0+I2C+4, BUS0+I2C+5, RelayInterlockMode.R1_DOWN__R2_POWER, 8, 5);
            NODE_HS.Applications.AddPWMApplication(Model.ApplicationId.PWM___XX_XXX_14, "Dimmbares Licht", Model.ApplicationId.STDBY_XX_XXX_15, new HashSet<ushort>{BUS0+I2C+6}, 5000, 20, 255);
            NODE_HS.Applications.AddPowIt(Model.ApplicationId.STDBY_XX_XXX_15, "Standby für die PWMApplication", BUS0+I2C+7, 10000);//ist der Standny für die PWMApplication
            NODE_HS.Applications.AddSound(Model.ApplicationId.SOUND_XX_XXX_0, "Soundgenerator", 20, 1);

            NODE_UP.Applications.AddRotaryEncoder(Model.ApplicationId.ROTAR_XX_XXX_0, RotaryEncoder.ROTARYENCODER_1, Model.ApplicationId.PWM___XX_XXX_14);
            
            return model;
        }
    }
}

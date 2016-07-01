using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Klli.Sensact.Config
{
    public static class Sattlerstrasse16
    {
        private static string _(this ID id){return id.ToString();}

        public static Model Build()//Demo for Scharmann
        {
            Model model = new Model();
            Node SNSCT_L0_TECH_HS07 = new SensactHs07()
            {
                Id = "SNSCT_L0_TECH_HS07",
                Applications = new List<SensactApplication>()
                
            };
            model.Nodes.Add(SNSCT_L0_TECH_HS07);

            //Keller
            SNSCT_L0_TECH_HS07.Applications.AddToggleButton(ID.PUSHB_L0_CORR_B11, InputPin.I01, ID.POWIT_L0_CORR_C1);
            SNSCT_L0_TECH_HS07.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_CORR_C1), OutputRessource = OutputPin.O09 });
            SNSCT_L0_TECH_HS07.Applications.AddTwoDimButtons(ID.PUSHB_L0_PRTY_B11, ID.PUSHB_L0_PRTY_B12, InputPin.I02, InputPin.I03, ID.PWM___L0_PRTY_S);
            SNSCT_L0_TECH_HS07.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_PRTY_S), StandbyController=_(ID.STDBY_L0_48V), OutputRessources = new List<PwmPin> { PwmPin.P01, PwmPin.P02, PwmPin.P03, PwmPin.P04 } });
            SNSCT_L0_TECH_HS07.Applications.AddToggleButton(ID.PUSHB_L0_STO1_B11, InputPin.I04, ID.POWIT_L0_STO1_C1);
            SNSCT_L0_TECH_HS07.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_STO1_C1), OutputRessource = OutputPin.O10 });
            SNSCT_L0_TECH_HS07.Applications.AddToggleButton(ID.PUSHB_L0_TECH_B11, InputPin.I05, ID.POWIT_L0_TECH_C1);
            SNSCT_L0_TECH_HS07.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_TECH_C1), OutputRessource = OutputPin.O11 });
            SNSCT_L0_TECH_HS07.Applications.AddToggleButton(ID.PUSHB_L0_WELL_B11, InputPin.I06, ID.POWIT_L0_WELL_C1);
            SNSCT_L0_TECH_HS07.Applications.AddOnePushbuttonDimmer(ID.PUSHB_L0_WELL_B21, InputPin.I07, ID.PWM___L0_WELL_S1);//Für Dusche
            SNSCT_L0_TECH_HS07.Applications.AddOnePushbuttonDimmer(ID.PUSHB_L0_WELL_B22, InputPin.I08, ID.PWM___L0_WELL_S2); //Für WC
            SNSCT_L0_TECH_HS07.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WELL_C1), OutputRessource = OutputPin.O12 });
            SNSCT_L0_TECH_HS07.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S1), StandbyController = _(ID.STDBY_L0_24V), OutputRessources = new List<PwmPin> { PwmPin.P05 } }); //Dusche
            SNSCT_L0_TECH_HS07.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S2), StandbyController = _(ID.STDBY_L0_24V), OutputRessources = new List<PwmPin> { PwmPin.P06 } }); //WC
            SNSCT_L0_TECH_HS07.Applications.AddToggleButton(ID.PUSHB_L0_WORK_B11, InputPin.I09, ID.POWIT_L0_WORK_C1);
            SNSCT_L0_TECH_HS07.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WORK_C1), OutputRessource = OutputPin.O13 });

            SNSCT_L0_TECH_HS07.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L0_24V), OutputRessource = OutputPin.O14, WaittimeInMsec = 10000 });
            SNSCT_L0_TECH_HS07.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L0_48V), OutputRessource = OutputPin.O15, WaittimeInMsec = 10000 });


            //L1.BATH
            SNSCT_L0_TECH_HS07.Applications.Add(
                new PushButtonXApplication()
                {
                    ApplicationId = _(ID.PUSHB_L1_BATH_B11),
                    InputRessource = InputPin.I10,
                    CommandsOnReleased = new List<Command>()
                    {
                        new Command()
                        {
                            CommandType = CommandType.TOGGLE,
                            TargetAppId =_(ID.RGBW__L1_BATH_W1),
                        },
                    },
                    CommandsOnReleasedShort = new List<Command>()
                    {
                        new Command()
                        {
                            CommandType=CommandType.TOGGLE,
                            TargetAppId=_(ID.PWM___L1_BATH_S),
                        },
                    },

                    CommandsOnPressedShortAndHold = new List<Command>()
                    {
                        new Command()
                        {
                            CommandType=CommandType.START,
                            TargetAppId=_(ID.PWM___L1_BATH_S),
                        },
                    },

                    CommandsOnReleasedLong = new List<Command>()
                    {
                        new Command()
                        {
                            CommandType=CommandType.STOP,
                            TargetAppId=_(ID.PWM___L1_BATH_S),
                        },
                    },

                    }
                );
            SNSCT_L0_TECH_HS07.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_BATH_S), StandbyController=_(ID.STDBY_L0_48V), OutputRessources = new List<PwmPin> { PwmPin.P07 } });
            SNSCT_L0_TECH_HS07.Applications.AddBlindButtons(ID.PUSHB_L1_BATH_B12, ID.PUSHB_L1_BATH_B13, InputPin.I11, InputPin.I12, ID.BLIND_L1_BATH_J1);
            SNSCT_L0_TECH_HS07.Applications.Add(new BlindApplication {ApplicationId=_(ID.BLIND_L1_BATH_J1), OpenCloseTimeInSeconds=40, OutputRessourceUpOrPower=OutputPin.O16, OutputRessourceDown=OutputPin.O16, RelayMode=RelayMode.TWO_PHASES });
            SNSCT_L0_TECH_HS07.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L1_BATH_W1), StandbyController=_(ID.NO_APPLICATION), LowMeansLampOn = true, OutputR = PwmPin.OP01, OutputB = PwmPin.OP02, OutputG = PwmPin.OP03, OutputW = PwmPin.NONE });
            //TODO: Dekolicht soll mit angehen, wenn der Taster gedrückt wird, dazu TOGGLE-Kommando implementieren und Standby implementieren


            
            return model;
        }
    }
}

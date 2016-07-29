using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class Sattlerstrasse16
    {
        private static string _(this ID id){return id.ToString();}


        //Zählreihenfolge beim EIB-Kabel: rot, schwarz, weiß, gelb
        public static Model Build()
        {
            #region Floor sensors
            /*
            L1.BATH 10
            L1.KTCH 1
            L1.UTIL 7
            L1.LVNG 4
            L1.CORR 8
            L1.WORK 9
            L2.CORR 20
            L2.KID1 6
            L2.KID2 11
            L2.WORK 16
            L2.BEDR 19
            L2.BATH 17
            */

            #endregion
            #region Nodes
            //Spezialitäten oben
            //5V-Versorgung der Lichtschranken
            //Input der Lichtschranken (FT-Eingänge)
            //12V-Versorgung der RGBW-Spots im L2.BATH
            //ggf LEDs für "diverses" im Tastermodul an den Betten, ein beleuchteter Rahmen könnte hier auch als Nachtlicht verwendet werden?

            //Spezialitäten unten
            //Ansteuerung des MP3-Moduls (seriell)
            //12V-Versorgung der RGBW-Spots im L1.BATH
            //LED für Außensteckdosen im Tastermodul am Terassenausgang
            Model model = new Model();
            Node SNSAC_L0_TECH_HS07_1 = new SensactHs07()
            {
                Id = "SNSAC_L0_TECH_HS07_1",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L0_TECH_HS07_1);

            Node SNSAC_L0_TECH_HS07_2 = new SensactHs07()
            {
                Id = "SNSAC_L0_TECH_HS07_2",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L0_TECH_HS07_2);

            Node SNSAC_L3_TECH_HS07_1 = new SensactHs07()
            {
                Id = "SNSAC_L3_TECH_HS07_1",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L3_TECH_HS07_1);

            Node SNSAC_L3_TECH_HS07_2 = new SensactHs07()
            {
                Id = "SNSAC_L3_TECH_HS07_2",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L3_TECH_HS07_2);

            Node SNSAC_L1_KTCH_UP02_1 = new SensactUp02()
            {
                Id = "SNSAC_L1_KTCH_UP02_1",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L1_KTCH_UP02_1);

            Node SNSAC_L1_LVNG_UP02_1 = new SensactUp02()
            {
                Id = "SNSAC_L1_LVNG_UP02_1",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L1_LVNG_UP02_1);

            Node SNSAC_L2_BATH_UP02_1 = new SensactUp02()
            {
                Id = "SNSAC_L2_BATH_UP02_1",
                Applications = new List<SensactApplication>(),
            };
            model.Nodes.Add(SNSAC_L2_BATH_UP02_1);
            #endregion


            #region L0
            //L0
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B11),
                InputRessource = InputPin.I01,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_L0_CORR_C1),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_W2),
                    },
                }

            });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B21),
                InputRessource = InputPin.I02,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_L0_CORR_C1),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_W2),
                    },
                }

            });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B31),
                InputRessource = InputPin.I03,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_L0_CORR_C1),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_W2),
                    },
                }

            });

            //PRTY
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_CORR_C1), OutputRessource = OutputPin.O09 });
            SNSAC_L0_TECH_HS07_1.Applications.AddTwoDimButtons(ID.PUSHB_L0_PRTY_B11, ID.PUSHB_L0_PRTY_B12, InputPin.I04, InputPin.I05, ID.PWM___L0_PRTY_S);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_PRTY_S), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P01, PwmPin.P02, PwmPin.P03, PwmPin.P04 } });
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L0_STO1_B11, InputPin.I06, ID.POWIT_L0_STO1_C1);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_STO1_C1), OutputRessource = OutputPin.O10 });

            //TECH
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L0_TECH_B11, InputPin.I07, ID.POWIT_L0_TECH_C1);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_TECH_C1), OutputRessource = OutputPin.O11 });
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.DEVCE_L0_TECH_PUMP, InputPin.I08, ID.NO_APPLICATION);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new DoorbellApplication
            {
                ApplicationId = _(ID.DEVCE_L0_TECH_AUDIO),
            });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L0_TECH_48V), OutputRessource = OutputPin.O22, WaittimeInMsec = 10000 });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L2_CORR_24V), OutputRessource = OutputPin.O23, WaittimeInMsec = 10000 });
            //MISSING:Heizungsanlage

            //WELL
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L0_WELL_B11, InputPin.I09, ID.POWIT_L0_WELL_C1);
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B21, InputPin.I10, ID.PWM___L0_WELL_S1);//Für Dusche
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B22, InputPin.I11, ID.PWM___L0_WELL_S2); //Für WC
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WELL_C1), OutputRessource = OutputPin.O12 });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S1), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P05 } }); //Dusche
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S2), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P06 } }); //WC
            //WORK
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L0_WORK_B11, InputPin.I12, ID.POWIT_L0_WORK_C1);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WORK_C1), OutputRessource = OutputPin.O13 });
            #endregion
            #region LX
            //Back
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_BACK_P1, OutputPin.O03);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W1, OutputPin.O04);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W2, OutputPin.O05);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W3, OutputPin.O06);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_LX_BACK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O07, OutputRessourceDown = OutputPin.O08, RelayMode = RelayMode.INTERLOCKED, });
            //Front (Klingelknopf, Licht, Bewegungsmelder)
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_LX_FRON_B1),
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.START,
                        TargetAppId=_(ID.DEVCE_L0_TECH_AUDIO),
                    }
                },
                InputRessource = InputPin.I05,
            });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.PUSHB_LX_FRON_B2),
                InputRessource = InputPin.I01,
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.POWIT_LX_FRON_W1),
            });
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_FRON_W1, OutputPin.O07);

            //GARA
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_LX_GARA_B11, InputPin.I14, ID.POWIT_LX_GARA_C1);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_GARA_C1, OutputPin.O10);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_GARA_P1, OutputPin.O11);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.BLIND_LX_GARA_J1, OutputPin.O12, 500); //Beim Tastendruck wird das Relais (vorerst mal) für 500msek angezogen

            //LEFT
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_LEFT_W1, OutputPin.O08);

            //RGHT
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_LX_RGHT_W1, OutputPin.O09);

            #endregion
            #region L1

            //BATH
            SNSAC_L0_TECH_HS07_1.Applications.Add(
                new PushButtonXApplication()
                {
                    ApplicationId = _(ID.PUSHB_L1_BATH_B11),
                    InputRessource = InputPin.I13,
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
            SNSAC_L0_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L1_BATH_B12, ID.PUSHB_L1_BATH_B13, InputPin.I11, InputPin.I12, ID.BLIND_L1_BATH_J1);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_BATH_S), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P07 } });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_BATH_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O14, OutputRessourceDown = OutputPin.O15, RelayMode = RelayMode.INTERLOCKED });
            //Dekoleuchten werden aus dem 24V-Sensact-Systemnetzteil versorgt
            SNSAC_L0_TECH_HS07_1.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L1_BATH_W1), StandbyController = _(ID.NO_APPLICATION), LowMeansLampOn = true, OutputR = PwmPin.OP01, OutputB = PwmPin.OP02, OutputG = PwmPin.OP03, OutputW = PwmPin.NONE });


            //CORR
            //Hinweis: LeavingHome/ComingHome ist noch nicht beschaltet
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B11, InputPin.I12, ID.PWM___L1_CORR_S);
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B21, InputPin.I13, ID.PWM___L1_CORR_S);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B22, InputPin.I14, ID.POWIT_L1_CORR_W1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B23, InputPin.I15, ID.PWM___L1_CORR_S);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B24, InputPin.I16, ID.POWIT_L1_CORR_W1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B25, InputPin.I17, ID.POWIT_LX_FRON_W1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B26, InputPin.I18, ID.POWIT_LX_LEFT_W1);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_CORR_S), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P08, PwmPin.P09, PwmPin.P10 } });
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_CORR_W1, OutputPin.O16);

            //L1.KTCH
            SNSAC_L1_KTCH_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L1_KTCH_B21, RotaryEncoder.ROTARYENCODER_1, InputPin.ROTAR_PUSH_1, ID.PWM___L1_KTCH_S1);
            SNSAC_L1_KTCH_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L1_KTCH_B22, RotaryEncoder.ROTARYENCODER_2, InputPin.ROTAR_PUSH_2, ID.PWM___L1_KTCH_S2);
            SNSAC_L1_KTCH_UP02_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B23, InputPin.I01, ID.POWIT_L1_KTCH_C1);
            SNSAC_L1_KTCH_UP02_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B24, InputPin.I02, ID.POWIT_L1_KTCH_W1);
            SNSAC_L1_KTCH_UP02_1.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B25, ID.PUSHB_L1_KTCH_B26, InputPin.I03, InputPin.I04, ID.BLIND_L1_KTCH_J1);
            SNSAC_L1_KTCH_UP02_1.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B27, ID.PUSHB_L1_KTCH_B28, InputPin.I05, InputPin.I06, ID.BLIND_L1_KTCH_J2);
            //Vier Spots
            SNSAC_L1_KTCH_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_KTCH_S1), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P16, PwmPin.P17, PwmPin.P18, PwmPin.P19 } });
            //Einzelspot
            SNSAC_L1_KTCH_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_KTCH_S2), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P20 } });


            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B11, InputPin.I19, ID.POWIT_L1_KTCH_C1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B12, InputPin.I20, ID.POWIT_LX_BACK_W1);
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B31, InputPin.I21, ID.PWM___L1_KTCH_S2);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_KTCH_C1, OutputPin.O17);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_KTCH_W1, OutputPin.O18);
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_KTCH_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O19, OutputRessourceDown = OutputPin.O20, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_KTCH_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O21, OutputRessourceDown = OutputPin.O22, RelayMode = RelayMode.INTERLOCKED });

            //L1.LVNG

            SNSAC_L1_LVNG_UP02_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B31, ID.PUSHB_L1_LVNG_B32, InputPin.I01, InputPin.I02, ID.BLIND_L1_LVNG_J1);
            SNSAC_L1_LVNG_UP02_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B33, ID.PUSHB_L1_LVNG_B34, InputPin.I03, InputPin.I04, ID.BLIND_L1_LVNG_J2);
            SNSAC_L1_LVNG_UP02_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B35, ID.PUSHB_L1_LVNG_B36, InputPin.I05, InputPin.I06, ID.BLIND_L1_LVNG_J3);
            SNSAC_L1_LVNG_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L1_LVNG_B41, RotaryEncoder.ROTARYENCODER_2, InputPin.ROTAR_PUSH_2, ID.PWM___L1_LVNG_S);
            SNSAC_L1_LVNG_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L1_LVNG_B42, RotaryEncoder.ROTARYENCODER_1, InputPin.ROTAR_PUSH_1, ID.PWM___L1_LVNG_S);
            SNSAC_L1_LVNG_UP02_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B43, InputPin.I07, ID.POWIT_L1_LVNG_W1);
            SNSAC_L1_LVNG_UP02_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B44, InputPin.I08, ID.POWIT_L1_LVNG_C1);
            SNSAC_L1_LVNG_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_LVNG_S), StandbyController = _(ID.STDBY_L2_CORR_24V), OutputRessources = new List<PwmPin> { PwmPin.P16, PwmPin.P17, PwmPin.P18, PwmPin.P19, PwmPin.P20, PwmPin.P21, PwmPin.P22, PwmPin.P23, PwmPin.P24, PwmPin.P25, PwmPin.P26, PwmPin.P27, PwmPin.P28 } });

            SNSAC_L0_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B11, ID.PUSHB_L1_LVNG_B12, InputPin.I01, InputPin.I02, ID.BLIND_LX_BACK_J1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B13, InputPin.I24, ID.POWIT_LX_BACK_W2);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B14, InputPin.I24, ID.POWIT_LX_BACK_W3);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B15, InputPin.I24, ID.POWIT_LX_BACK_P1);//P1 sind die Außensteckdosen
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B16, InputPin.I24, ID.PWM___L1_LVNG_S);

            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B21, InputPin.I22, ID.POWIT_L1_LVNG_W4);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B22, InputPin.I23, ID.POWIT_L1_LVNG_W4);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B23, InputPin.I24, ID.POWIT_LX_RGHT_W1);

            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B51, InputPin.I24, ID.PWM___L1_LVNG_S);
            SNSAC_L0_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B52, InputPin.I24, ID.PWM___L1_KTCH_S1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B53, InputPin.I24, ID.POWIT_L1_KTCH_C1);

            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O23, OutputRessourceDown = OutputPin.O24, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O25, OutputRessourceDown = OutputPin.O26, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L0_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J3), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O27, OutputRessourceDown = OutputPin.O28, RelayMode = RelayMode.INTERLOCKED });
           
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_C1, OutputPin.O29);
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_W1, OutputPin.O30);
            //W2 ist parallel zu W1 geschaltet
            //W3 ist noch nicht existente Effektbeleuchtung
            SNSAC_L0_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_W4, OutputPin.O31);

            //UTIL
            SNSAC_L0_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L1_UTIL_B11, ID.PUSHB_L1_UTIL_B12, InputPin.I02, InputPin.I03, ID.BLIND_L1_UTIL_J1);
            SNSAC_L0_TECH_HS07_2.Applications.AddOneDimButton(ID.PUSHB_L1_UTIL_B13, InputPin.I01, ID.PWM___L1_KTCH_S1); //unterhalb des Jalousietasters, weil für anderen Raum

            SNSAC_L0_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B21, InputPin.I04, ID.POWIT_L1_UTIL_C1);
            SNSAC_L0_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B22, InputPin.I05, ID.POWIT_LX_LEFT_W1);
            SNSAC_L0_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B23, InputPin.I06, ID.BLIND_LX_GARA_J1);
            SNSAC_L0_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B24, InputPin.I07, ID.BLIND_LX_GARA_J1);
            SNSAC_L0_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_L1_UTIL_C1, OutputPin.O12);
            SNSAC_L0_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_UTIL_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O13, OutputRessourceDown = OutputPin.O14, RelayMode = RelayMode.INTERLOCKED });


            //WORK
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B11, InputPin.I08, ID.POWIT_L1_WORK_C1);
            SNSAC_L0_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B12, InputPin.I09, ID.POWIT_L1_WORK_C2);

            SNSAC_L0_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B13, ID.PUSHB_L1_WORK_B14, InputPin.I10, InputPin.I11, ID.BLIND_L1_WORK_J1);
            SNSAC_L0_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B15, ID.PUSHB_L1_WORK_B16, InputPin.I12, InputPin.I13, ID.BLIND_L1_WORK_J2);
            SNSAC_L0_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B17, InputPin.I14, ID.POWIT_LX_RGHT_W1);
            SNSAC_L0_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_L1_WORK_C1, OutputPin.O15);
            SNSAC_L0_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_L1_WORK_C2, OutputPin.O16);
            SNSAC_L0_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O17, OutputRessourceDown = OutputPin.O18, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L0_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O19, OutputRessourceDown = OutputPin.O20, RelayMode = RelayMode.INTERLOCKED });
            #endregion
            #region L2
            //BATH
            SNSAC_L2_BATH_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B11, RotaryEncoder.ROTARYENCODER_1, InputPin.ROTAR_PUSH_1, ID.PWM___L2_BATH_S);
            SNSAC_L2_BATH_UP02_1.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B12, RotaryEncoder.ROTARYENCODER_2, InputPin.ROTAR_PUSH_2, ID.RGBW__L2_BATH_W);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_BATH_B13, ID.PUSHB_L2_BATH_B14, InputPin.I01, InputPin.I02, ID.BLIND_L2_BATH_J1);
            SNSAC_L2_BATH_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L2_BATH_S), StandbyController = _(ID.STDBY_L3_ROOF_48V), OutputRessources = new List<PwmPin> { PwmPin.P16, PwmPin.P17 } });
            SNSAC_L2_BATH_UP02_1.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L2_BATH_W), StandbyController = _(ID.STDBY_L3_ROOF_48V), LowMeansLampOn = true, OutputR = PwmPin.P18, OutputB = PwmPin.P19, OutputG = PwmPin.P20, OutputW = PwmPin.P21 });
            SNSAC_L3_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_BATH_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O01, OutputRessourceDown = OutputPin.O02, RelayMode = RelayMode.INTERLOCKED });


            //L2.BEDR
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B11, InputPin.I03, ID.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            SNSAC_L3_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L2_BEDR_B12, InputPin.I04, ID.PWM___L2_BEDR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B13, InputPin.I05, ID.POWIT_L2_BEDR_P1);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B14, ID.PUSHB_L2_BEDR_B15, InputPin.I06, InputPin.I07, ID.BLIND_L2_BEDR_J1);

            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B21, InputPin.I08, ID.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B22, InputPin.I10, ID.POWIT_L2_BEDR_P2);
            SNSAC_L3_TECH_HS07_1.Applications.AddOneDimButton(ID.PUSHB_L2_BEDR_B23, InputPin.I09, ID.PWM___L2_BEDR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B24, ID.PUSHB_L2_BEDR_B25, InputPin.I11, InputPin.I12, ID.BLIND_L2_BEDR_J1);

            SNSAC_L3_TECH_HS07_1.Applications.AddTwoDimButtons(ID.PUSHB_L2_BEDR_B31, ID.PUSHB_L2_BEDR_B32, InputPin.I13, InputPin.I14, ID.PWM___L2_BEDR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B33, InputPin.I15, ID.POWIT_L2_BEDR_P1);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B34, InputPin.I16, ID.POWIT_L2_BEDR_P2);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B35, ID.PUSHB_L2_BEDR_B36, InputPin.I17, InputPin.I18, ID.BLIND_L2_BEDR_J1);

            SNSAC_L3_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_BEDR_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O03, OutputRessourceDown = OutputPin.O04, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L3_TECH_HS07_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L2_BEDR_S), StandbyController = _(ID.STDBY_L3_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P01, PwmPin.P02 } });
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P1, OutputPin.O05);
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P2, OutputPin.O06);
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P3, OutputPin.O07);

            //CORR
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_CORR_B11, InputPin.I19, ID.PWM___L2_CORR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_CORR_B21, InputPin.I20, ID.PWM___L2_CORR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_CORR_B31, InputPin.I21, ID.PWM___L2_CORR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_CORR_B41, InputPin.I20, ID.PWM___L2_CORR_S);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_CORR_B42, ID.PUSHB_L2_CORR_B43, InputPin.I21, InputPin.I22, ID.BLIND_L2_CORR_J1);
            SNSAC_L3_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_CORR_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O08, OutputRessourceDown = OutputPin.O09, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L2_BATH_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L2_CORR_S), StandbyController = _(ID.STDBY_L3_ROOF_48V), OutputRessources = new List<PwmPin> { PwmPin.P22, PwmPin.P23 } });

            //KID1
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_KID1_B11, InputPin.I23, ID.POWIT_L2_KID1_C1);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_KID1_B12, ID.PUSHB_L2_KID1_B13, InputPin.I24, InputPin.I25, ID.BLIND_L2_KID1_J1);
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_KID1_B21, InputPin.I26, ID.POWIT_L2_KID1_C2);
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_KID1_C1, OutputPin.O10);
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_KID1_C2, OutputPin.O11, 7200);
            SNSAC_L3_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_KID1_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O12, OutputRessourceDown = OutputPin.O13, RelayMode = RelayMode.INTERLOCKED });

            //KID2
            SNSAC_L3_TECH_HS07_1.Applications.AddToggleButton(ID.PUSHB_L2_KID2_B11, InputPin.I27, ID.POWIT_L2_KID2_C1);
            SNSAC_L3_TECH_HS07_1.Applications.AddBlindButtons(ID.PUSHB_L2_KID2_B12, ID.PUSHB_L2_KID2_B13, InputPin.I28, InputPin.I29, ID.BLIND_L2_KID2_J1);
            SNSAC_L3_TECH_HS07_1.Applications.AddPowIt(ID.POWIT_L2_KID2_C1, OutputPin.O14);
            SNSAC_L3_TECH_HS07_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_KID2_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O15, OutputRessourceDown = OutputPin.O16, RelayMode = RelayMode.INTERLOCKED });

            //WORK
            SNSAC_L3_TECH_HS07_2.Applications.AddOneDimButton(ID.PUSHB_L2_WORK_B11, InputPin.I01, ID.PWM___L2_WORK_S);
            SNSAC_L3_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L2_WORK_B12, ID.PUSHB_L2_WORK_B13, InputPin.I02, InputPin.I03, ID.BLIND_L2_WORK_J1);
            SNSAC_L3_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L2_WORK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O01, OutputRessourceDown = OutputPin.O02, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L2_WORK_S), StandbyController = _(ID.STDBY_L3_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P01, PwmPin.P02 } });
            #endregion
            #region L3
            //BEDR
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L3_BEDR_B11, InputPin.I27, ID.POWIT_L3_BEDR_C1);
            SNSAC_L3_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L3_BEDR_B12, ID.PUSHB_L3_BEDR_B13, InputPin.I04, InputPin.I05, ID.BLIND_L3_BEDR_J1);
            SNSAC_L3_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_L3_BEDR_C1, OutputPin.O03);
            SNSAC_L3_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L3_BEDR_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O04, OutputRessourceDown = OutputPin.O05, RelayMode = RelayMode.INTERLOCKED });

            //TECH
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_L3_TECH_B11, InputPin.I06, ID.POWIT_L3_TECH_C1);
            SNSAC_L3_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_L3_TECH_C1, OutputPin.O06);
            SNSAC_L3_TECH_HS07_2.Applications.AddPlaceholder(ID.SBRGH_LX_ROOF);
            SNSAC_L3_TECH_HS07_2.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L3_TECH_48V), OutputRessource = OutputPin.O21, WaittimeInMsec = 10000 });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new StandbyController { ApplicationId = _(ID.STDBY_L3_ROOF_48V), OutputRessource = OutputPin.O21, WaittimeInMsec = 10000 });


            //WORK
            SNSAC_L3_TECH_HS07_2.Applications.AddRotaryEncoder(ID.ROTAR_L3_WORK_B11, RotaryEncoder.ROTARYENCODER_1, InputPin.ROTAR_PUSH_1, ID.PWM___L3_WORK_S);
            SNSAC_L3_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L3_WORK_B12, ID.PUSHB_L3_WORK_B13, InputPin.I07, InputPin.I08, ID.BLIND_L3_WORK_J1);
            SNSAC_L3_TECH_HS07_2.Applications.AddBlindButtons(ID.PUSHB_L3_WORK_B14, ID.PUSHB_L3_WORK_B15, InputPin.I09, InputPin.I10, ID.BLIND_L3_WORK_J2);
            SNSAC_L3_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L3_WORK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O07, OutputRessourceDown = OutputPin.O08, RelayMode = RelayMode.INTERLOCKED });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L3_WORK_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = OutputPin.O09, OutputRessourceDown = OutputPin.O10, RelayMode = RelayMode.INTERLOCKED });

            SNSAC_L3_TECH_HS07_2.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L3_WORK_S), StandbyController = _(ID.STDBY_L3_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P03, PwmPin.P04, PwmPin.P05 } });
            #endregion
            #region LS

            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId= _(ID.LIBAR_LS_STRS_B11),
                ActiveSignalLevel=true,
                BrightnessSensor=_(ID.SBRGH_LX_ROOF),
                FinalTarget=_(ID.PWM___LS_STRS_W2),
                InputRessource= InputPin.II01, 
             });
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B21, InputPin.I16, ID.POWIT_LS_STRS_W3);
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B22, InputPin.I17, ID.PWM___L1_CORR_S);
            SNSAC_L3_TECH_HS07_2.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_LS_STRS_B23),
                InputRessource = InputPin.I18,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_L0_CORR_C1),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_LS_STRS_W1),
                    },
                }

            });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B31),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W2),
                InputRessource = InputPin.II02,
            });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B32),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W4),
                InputRessource = InputPin.II03,
            });

            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B41, InputPin.I19, ID.POWIT_LS_STRS_W5);
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B42, InputPin.I20, ID.PWM___L2_CORR_S);
            SNSAC_L3_TECH_HS07_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B43, InputPin.I21, ID.POWIT_LS_STRS_W3);

            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B51),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W4),
                InputRessource = InputPin.II04,
            });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B52),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W6),
                InputRessource = InputPin.II05,
            });


            SNSAC_L3_TECH_HS07_2.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_LS_STRS_B61),
                InputRessource = InputPin.I22,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_S),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_LS_STRS_W5),
                    },
                }

            });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_LS_STRS_B62),
                InputRessource = InputPin.I23,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_S),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_LS_STRS_W5),
                    },
                }

            });

            SNSAC_L3_TECH_HS07_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B71),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W6),
                InputRessource = InputPin.II06,
            });


            SNSAC_L1_LVNG_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___LS_STRS_W2), StandbyController = _(ID.STDBY_L2_CORR_24V), OutputRessources = new List<PwmPin> { PwmPin.P29 } });
            SNSAC_L1_LVNG_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___LS_STRS_W4), StandbyController = _(ID.STDBY_L2_CORR_24V), OutputRessources = new List<PwmPin> { PwmPin.P30 } });
            SNSAC_L1_LVNG_UP02_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___LS_STRS_W6), StandbyController = _(ID.STDBY_L2_CORR_24V), OutputRessources = new List<PwmPin> { PwmPin.P31 } });
            SNSAC_L3_TECH_HS07_2.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___LS_STRS_S), StandbyController = _(ID.STDBY_L3_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P06 } });
            SNSAC_L3_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_LS_STRS_W1, OutputPin.O11);
            SNSAC_L3_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_LS_STRS_W3, OutputPin.O12);
            SNSAC_L3_TECH_HS07_2.Applications.AddPowIt(ID.POWIT_LS_STRS_W5, OutputPin.O13);
            #endregion
            return model;
        }
    }
}

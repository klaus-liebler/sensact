using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class Sattlerstrasse16Classic
    {
        private static string _(this ID id){return id.ToString();}
        private const int DEFAULT_MIN_DIM_LEVEL = 10;
        private const int DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS = 20000;
        const ushort INTI = 0x0000;
        const ushort BUS0 = 0x4000;
        const ushort BB = BUS0;
        const ushort BUS1 = 0x8000;
        const ushort RB = BUS1;
        const ushort BUS2 = 0xC000;
        const ushort BUS3 = 0xF000;
        const ushort I2C = 0x0000;
        const ushort OW0 = 0x0400;
        const ushort OW1 = 0x0800;
        const ushort OW2 = 0x0C00;
        const ushort OW3 = 0x1000;
        //Zählreihenfolge beim EIB-Kabel: rot, schwarz, weiß, gelb

        enum InputPin
        {
            I00= BB+I2C+0, I01= BB + I2C+1, I02= BB + I2C+2, I03=BB+I2C+3, I04= BB + I2C +4, I05= BB + I2C +5, I06= BB + I2C +6, I07= BB + I2C +7, I08= BB + I2C +8, I09= BB + I2C +9, I10= BB + I2C +10, I11= BB + I2C +11, I12= BB + I2C +12, I13= BB + I2C +13, I14= BB + I2C +14, I15= BB + I2C +15, I16=BB+I2C+16, I17=BB+I2C+17, I18= BB + I2C +18, I19= BB + I2C +19, I20= BB + I2C +20, I21= BB + I2C +21, I22= BB + I2C +22, I23= BB + I2C +23, I24= BB + I2C +24, I25= BB + I2C +25, I26= BB + I2C +26, I27= BB + I2C +27, I28= BB + I2C +28, I29= BB + I2C +29, I30= BB + I2C +30, I31= BB + I2C +31,
        }
        enum OutputPin
        {
            O00 = 0, O01 = 1, O02 = 2, O03, O04, O05, O06, O07, O08, O09, O10, O11, O12, O13, O14, O15, O16, O17, O18, O19, O20, O21, O22, O23, O24, O25, O26,O27, O28, O29, O30, O31,
        }

        //Infos: RL 32 ist für LX.Back.P3 auf Klemme X1.40.
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
            //5V-Versorgung des OneWire
            //ggf LEDs für "diverses" im Tastermodul an den Betten, ein beleuchteter Rahmen könnte hier auch als Nachtlicht verwendet werden?

            //Spezialitäten unten
            //Ansteuerung des MP3-Moduls + Verstärker (seriell)
            //12V-Versorgung der RGBW-Spots im L1.BATH
            //5V-Versorgung des OneWire
            //LED für Außensteckdosen im Tastermodul am Terassenausgang
            Model model = new Model("Sattlerstraße 16");
            Node SNSCT_L0_TECH_HS_1 = new SensactHs07(ID.SNSCT_L0_TECH_HS_1);
            model.Nodes.Add(SNSCT_L0_TECH_HS_1);

            Node SNSCT_L0_TECH_HS_2 = new SensactHs07(ID.SNSCT_L0_TECH_HS_2);
            model.Nodes.Add(SNSCT_L0_TECH_HS_2);

            Node SNSCT_L3_TECH_HS_1 = new SensactHs07(ID.SNSCT_L3_TECH_HS_1);
            model.Nodes.Add(SNSCT_L3_TECH_HS_1);

            Node SNSCT_L1_KTCH_UP = new SensactUp02(ID.SNSCT_L1_KTCH_UP);
            model.Nodes.Add(SNSCT_L1_KTCH_UP);

            Node SNSCT_L1_LVNG_UP = new SensactUp02(ID.SNSCT_L1_LVNG_UP);
            model.Nodes.Add(SNSCT_L1_LVNG_UP);

            Node SNSCT_L2_BATH_UP = new SensactUp02(ID.SNSCT_L2_BATH_UP);
            model.Nodes.Add(SNSCT_L2_BATH_UP);
            #endregion


            #region L0
            //CORR
            SNSCT_L0_TECH_HS_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B11),
                InputRessource = (ushort)InputPin.I13, //36
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
            SNSCT_L0_TECH_HS_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B21),
                InputRessource = (ushort)InputPin.I12, //34
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
            SNSCT_L0_TECH_HS_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L0_CORR_B31),
                InputRessource = (ushort)InputPin.I11,//33
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
                                                                                                                                    
            SNSCT_L0_TECH_HS_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_CORR_C1), OutputRessource = (ushort)OutputPin.O25 }); //RL7

            //PRTY
            SNSCT_L0_TECH_HS_1.Applications.AddTwoDimButtons(ID.PUSHB_L0_PRTY_B11, ID.PUSHB_L0_PRTY_B12, (ushort)InputPin.I10, (ushort)InputPin.I09, ID.PWM___L0_PRTY_S);//32,30
            SNSCT_L0_TECH_HS_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_PRTY_S), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<ushort> { PwmPin.P11, PwmPin.P10, PwmPin.P09, PwmPin.P08 } });//144,143,142,141

            //STO1
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_STO1_B11, (ushort)InputPin.I08, ID.POWIT_L0_STO1_C1);//28
            SNSCT_L0_TECH_HS_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_STO1_C1), OutputRessource = (ushort)OutputPin.O24 });//RL6

            //TECH
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_TECH_B11, (ushort)InputPin.I07, ID.POWIT_L0_TECH_C1);//24
            SNSCT_L0_TECH_HS_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_TECH_C1), OutputRessource = (ushort)OutputPin.O23 });//RL5
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.DEVCE_L0_TECH_PUMP, InputPin.I14, ID.NO_APPLICATION);//56
            SNSCT_L0_TECH_HS_2.Applications.Add(new SoundApplication
            {
                ApplicationId = _(ID.DEVCE_L0_TECH_AUDIO),
                StandbyOutput = (ushort)(INTI + 64u + 2u),//e2,
                NameOfVoulumeScheduleOrNull = null,
            });
            SNSCT_L0_TECH_HS_2.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L0_TECH_48V), OutputRessource = (ushort)OutputPin.O04, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });//RL15
            SNSCT_L0_TECH_HS_2.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L2_CORR_24V), OutputRessource = (ushort)OutputPin.O06, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });//RL17, nicht 16!!!
            //MISSING:Heizungsanlage
            //MISSING: Zirkulationspumpe RL14, O21 HS_1!!!

            //WELL
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_WELL_B11, (ushort)InputPin.I06, ID.POWIT_L0_WELL_C1);//20
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B12, (ushort)InputPin.I05, ID.PWM___L0_WELL_S1);//18Für Dusche
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B21, (ushort)InputPin.I04, ID.PWM___L0_WELL_S2); //16 Für WC
            SNSCT_L0_TECH_HS_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WELL_C1), OutputRessource = (ushort)OutputPin.O22 });//RL4
            SNSCT_L0_TECH_HS_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S1), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<ushort> { (ushort)PwmPin.P07 } }); //140 Dusche
            SNSCT_L0_TECH_HS_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L0_WELL_S2), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<ushort> { (ushort)PwmPin.P06 } }); //138 WC

            //WORK
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_WORK_B11, (ushort)InputPin.I03, ID.POWIT_L0_WORK_C1); //12
            SNSCT_L0_TECH_HS_1.Applications.Add(new PoweritemApplication { ApplicationId = _(ID.POWIT_L0_WORK_C1), OutputRessource = (ushort)OutputPin.O21 }); //RL2
            #endregion
            #region LX
            //Back
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_BACK_P1, (ushort)OutputPin.O21); //RL18
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_BACK_W1, (ushort)OutputPin.O12); //RL27
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_BACK_W2, (ushort)OutputPin.O11); //RL26
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_BACK_W3, (ushort)OutputPin.O10); //RL25
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_LX_BACK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O10, OutputRessourceDownOrDirection = (ushort)OutputPin.O20, RelayMode = RelayMode.INTERLOCKED_ACTIVE_DOWN, });//K19 K20

            //Front (Klingelknopf, Licht, Bewegungsmelder)
            SNSCT_L0_TECH_HS_2.Applications.Add(new PushButtonXApplication
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
                InputRessource = (ushort)InputPin.I22,
            });
            SNSCT_L0_TECH_HS_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LX_FRON_B2),
                InputRessource = (ushort)InputPin.I23, //Relais-Kabel
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.POWIT_LX_FRON_W1),
            });
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_FRON_W1, (ushort)OutputPin.O16);//RL31

            //GARA
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_LX_GARA_B11, (ushort)InputPin.I02, ID.POWIT_LX_GARA_C1);//4
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_LX_GARA_B21, (ushort)InputPin.I01, ID.BLIND_LX_GARA_J1); //2
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_GARA_C1, (ushort)OutputPin.O15); //RL30
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.BLIND_LX_GARA_J1, (ushort)OutputPin.O18, 500); //unbeschriftet

            //LEFT
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_LEFT_W1, (ushort)OutputPin.O14); //RL29

            //RGHT
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_LX_RGHT_W1, (ushort)OutputPin.O13);//RL28

            #endregion
            #region L1

            //BATH
            SNSCT_L0_TECH_HS_2.Applications.Add(
                new PushButtonXApplication()
                {
                    ApplicationId = _(ID.PUSHB_L1_BATH_B11),
                    InputRessource = (ushort)InputPin.I04, //96
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
            SNSCT_L0_TECH_HS_2.Applications.AddBlindButtons(ID.PUSHB_L1_BATH_B12, ID.PUSHB_L1_BATH_B13, (ushort)InputPin.I03, (ushort)InputPin.I02, ID.BLIND_L1_BATH_J1); //94+93
            SNSCT_L0_TECH_HS_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_BATH_S), StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<ushort> { (ushort)PwmPin.P04 } }); //128
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_BATH_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O01, OutputRessourceDownOrDirection = (ushort)OutputPin.O11, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K1 K2
            //Dekoleuchten werden aus dem 48V-Netzteil + Spannungswandler versorgt
            SNSCT_L0_TECH_HS_2.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L1_BATH_W1), StandbyController = _(ID.STDBY_L0_TECH_48V), LowMeansLampOn = true, OutputRessourceR = PwmPin.OP01, OutputRessourceB = (ushort)PwmPin.OP02, OutputRessourceG = (ushort)PwmPin.OP03, OutputRessourceW = (ushort)PwmPin.NONE });


            //CORR
            //Hinweis: LeavingHome/ComingHome ist noch nicht beschaltet
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B11, (ushort)InputPin.I14, ID.PWM___L1_CORR_S);//52
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B21, (ushort)InputPin.I32, ID.PWM___L1_CORR_S);//88
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B22, (ushort)InputPin.I31, ID.POWIT_L1_CORR_W1);//86
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B23, (ushort)InputPin.I30, ID.PWM___L1_CORR_S);//85
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B24, (ushort)InputPin.I29, ID.POWIT_L1_CORR_W1);//84
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B25, (ushort)InputPin.I28, ID.POWIT_LX_FRON_W1);//82
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B26, (ushort)InputPin.I27, ID.POWIT_LX_LEFT_W1);//81
            SNSCT_L0_TECH_HS_1.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_CORR_S), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<ushort> { (ushort)PwmPin.P03, (ushort)PwmPin.P02, (ushort)PwmPin.P01 } }); //126, 125, 124
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_L1_CORR_W1, (ushort)OutputPin.O09);//RL24

            //L1.KTCH
            SNSCT_L1_KTCH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_KTCH_B21, RotaryEncoder.ROTARYENCODER_1, (ushort)InputPin.ROTAR_PUSH_1, ID.PWM___L1_KTCH_S1);
            SNSCT_L1_KTCH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_KTCH_B22, RotaryEncoder.ROTARYENCODER_2, (ushort)InputPin.ROTAR_PUSH_2, ID.PWM___L1_KTCH_S2);
            SNSCT_L1_KTCH_UP.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B23, (ushort)InputPin.I01, ID.POWIT_L1_KTCH_C1);
            SNSCT_L1_KTCH_UP.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B24, (ushort)InputPin.I02, ID.POWIT_L1_KTCH_W1);
            SNSCT_L1_KTCH_UP.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B25, ID.PUSHB_L1_KTCH_B26, (ushort)InputPin.I03, (ushort)InputPin.I04, ID.BLIND_L1_KTCH_J1);
            SNSCT_L1_KTCH_UP.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B27, ID.PUSHB_L1_KTCH_B28, (ushort)InputPin.I05, (ushort)InputPin.I06, ID.BLIND_L1_KTCH_J2);
            //Vier Spots
            SNSCT_L1_KTCH_UP.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_KTCH_S1), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P16, PwmPin.P17, PwmPin.P18, PwmPin.P19 } });
            //Einzelspot
            SNSCT_L1_KTCH_UP.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_KTCH_S2), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P20 } });


            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B11, (ushort)InputPin.I26, ID.POWIT_L1_KTCH_C1);//80
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B12, (ushort)InputPin.I25, ID.POWIT_LX_BACK_W1);//78
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B31, (ushort)InputPin.I24, ID.PWM___L1_KTCH_S2);//76
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_L1_KTCH_C1, (ushort)OutputPin.O08);//RL23
            SNSCT_L0_TECH_HS_2.Applications.AddPowIt(ID.POWIT_L1_KTCH_W1, (ushort)OutputPin.O07);//RL22
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_KTCH_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O03, OutputRessourceDownOrDirection = (ushort)OutputPin.O13, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K5 K6
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_KTCH_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O02, OutputRessourceDownOrDirection = (ushort)OutputPin.O12, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K3 K4
            
            //L1.LVNG

            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B31, ID.PUSHB_L1_LVNG_B32, (ushort)InputPin.I01, (ushort)InputPin.I02, ID.BLIND_L1_LVNG_J1);
            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B33, ID.PUSHB_L1_LVNG_B34, (ushort)InputPin.I03, (ushort)InputPin.I04, ID.BLIND_L1_LVNG_J2);
            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B35, ID.PUSHB_L1_LVNG_B36, (ushort)InputPin.I05, (ushort)InputPin.I06, ID.BLIND_L1_LVNG_J3);
            SNSCT_L1_LVNG_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_LVNG_B41, RotaryEncoder.ROTARYENCODER_2, (ushort)InputPin.ROTAR_PUSH_2, ID.PWM___L1_LVNG_S);
            SNSCT_L1_LVNG_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_LVNG_B42, RotaryEncoder.ROTARYENCODER_1, (ushort)InputPin.ROTAR_PUSH_1, ID.PWM___L1_LVNG_S);
            SNSCT_L1_LVNG_UP.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B43, (ushort)InputPin.I07, ID.POWIT_L1_LVNG_W1);
            SNSCT_L1_LVNG_UP.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B44, (ushort)InputPin.I08, ID.POWIT_L1_LVNG_C1);
            SNSCT_L1_LVNG_UP.Applications.Add(new PWMApplication { ApplicationId = _(ID.PWM___L1_LVNG_S), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L2_CORR_24V), OutputRessources = new List<PwmPin> { PwmPin.P16, PwmPin.P17, PwmPin.P18, PwmPin.P19, PwmPin.P20, PwmPin.P21, PwmPin.P22, PwmPin.P23, PwmPin.P24, PwmPin.P25, PwmPin.P26, PwmPin.P27 } });
            //B11 I23
            //B12 I22
            //B13 I21
            //14 I17
            //
            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B11, ID.PUSHB_L1_LVNG_B12, (ushort)InputPin.I23, (ushort)InputPin.I22, ID.BLIND_LX_BACK_J1);
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B13, (ushort)InputPin.I21, ID.POWIT_LX_BACK_W2);
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B14, (ushort)InputPin.I17, ID.POWIT_LX_BACK_W3);
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B15, (ushort)InputPin.I16, ID.POWIT_LX_BACK_P1);//P1 sind die Außensteckdosen
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B16, (ushort)InputPin.I15, ID.PWM___L1_LVNG_S);

            SNSCT_L0_TECH_HS_1.Applications.AddTwoDimButtons(ID.PUSHB_L1_LVNG_B21, ID.PUSHB_L1_LVNG_B22, (ushort)InputPin.I20, (ushort)InputPin.I19, ID.PWM___L1_LVNG_W4);
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B23, (ushort)InputPin.I18, ID.POWIT_LX_RGHT_W1);

            SNSCT_L0_TECH_HS_2.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B51, (ushort)InputPin.I21, ID.PWM___L1_LVNG_S);//120
            SNSCT_L0_TECH_HS_2.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B52, (ushort)InputPin.I20, ID.PWM___L1_KTCH_S1);//118
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B53, (ushort)InputPin.I19, ID.POWIT_L1_KTCH_C1);//117

            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O04, OutputRessourceDownOrDirection = (ushort)OutputPin.O14, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });///K7 K8
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O05, OutputRessourceDownOrDirection = (ushort)OutputPin.O15, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K9 K10
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_LVNG_J3), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O06, OutputRessourceDownOrDirection = (ushort)OutputPin.O16, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K11 K12

            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_C1, (ushort)OutputPin.O29);
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_W1, (ushort)OutputPin.O31);
            //W2 ist parallel zu W1 geschaltet
            //W3 ist noch nicht existente Effektbeleuchtung
            //Begin beCareful: Wird vom zweiten device gesteuert, weil dessen Pfostenstecker noch frei ist
            SNSCT_L0_TECH_HS_2.Applications.Add(new PWMApplication() {ApplicationId= _(ID.PWM___L1_LVNG_W4), MinimalOnLevel = DEFAULT_MIN_DIM_LEVEL, StandbyController = _(ID.STDBY_L0_TECH_48V), OutputRessources = new List<PwmPin> { PwmPin.P01 } }); //OutputPin.O30 wird frei!
            //End BeCareful
            //UTIL
            SNSCT_L0_TECH_HS_2.Applications.AddBlindButtons(ID.PUSHB_L1_UTIL_B11, ID.PUSHB_L1_UTIL_B12, (ushort)InputPin.I18, (ushort)InputPin.I17, ID.BLIND_L1_UTIL_J1); //116 + 114
            SNSCT_L0_TECH_HS_2.Applications.AddOneDimButton(ID.PUSHB_L1_UTIL_B13, (ushort)InputPin.I16, ID.PWM___L1_KTCH_S1); //113,unterhalb des Jalousietasters, weil für anderen Raum
            
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B21, (ushort)InputPin.I15, ID.POWIT_L1_UTIL_C1);//112
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B22, (ushort)InputPin.I14, ID.POWIT_LX_LEFT_W1);//110
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B23, (ushort)InputPin.I13, ID.BLIND_LX_GARA_J1); //109
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B24, (ushort)InputPin.I12, ID.BLIND_LX_GARA_J1); //108
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_UTIL_C1, (ushort)OutputPin.O26);//rl8
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_UTIL_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O07, OutputRessourceDownOrDirection = (ushort)OutputPin.O17, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });


            //WORK
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B11, (ushort)InputPin.I07, ID.POWIT_L1_WORK_C1);//100
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B12, (ushort)InputPin.I06, ID.POWIT_L1_WORK_C2);//98

            SNSCT_L0_TECH_HS_2.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B13, ID.PUSHB_L1_WORK_B14, (ushort)InputPin.I05, (ushort)InputPin.I11, ID.BLIND_L1_WORK_J1);//97+104
            SNSCT_L0_TECH_HS_2.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B15, ID.PUSHB_L1_WORK_B16, (ushort)InputPin.I10, (ushort)InputPin.I09, ID.BLIND_L1_WORK_J2);//103+102
            SNSCT_L0_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B17, (ushort)InputPin.I08, ID.POWIT_LX_RGHT_W1);//101
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_WORK_C1, (ushort)OutputPin.O28); //rl10
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_WORK_C2, (ushort)OutputPin.O27);//rl9
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J1), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O08, OutputRessourceDownOrDirection = (ushort)OutputPin.O18, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP }); //K13 14
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = (ushort)OutputPin.O09, OutputRessourceDownOrDirection = (ushort)OutputPin.O19, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K15 16
            #endregion
            #region L2
            //BATH
            SNSCT_L2_BATH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B11, RotaryEncoder.ROTARYENCODER_1, SensactUp02.Pin_ROTD, ID.PWM___L2_BATH_S);
            SNSCT_L2_BATH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B12, RotaryEncoder.ROTARYENCODER_2, SensactUp02.Pin_xROTD, ID.RGBW__L2_BATH_W);
            //Start "be careful". These buttons are connected to central sensact
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_BATH_B13, ID.PUSHB_L2_BATH_B14, BB + I2C + 1, BB + I2C + 0, ID.BLIND_L2_BATH_J1);//4 2
            //End "be careful"
            SNSCT_L2_BATH_UP.Applications.AddPWMApplication(ID.PWM___L2_BATH_S, ID.STDBY_L3_ROOF_48V, new List<ushort> { BUS0 + I2C + 0, BUS0 + I2C + 1, BUS0 + I2C + 2 });
            SNSCT_L2_BATH_UP.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L2_BATH_W), StandbyController = _(ID.STDBY_L3_ROOF_48V), LowMeansLampOn = true, OutputRessourceR = /*Nach L2.Corr!*/BUS0 + I2C + 6, OutputRessourceG = BUS0 + I2C + 7, OutputRessourceB = BUS0 + I2C + 8, OutputRessourceW = BUS0 + I2C + 9 });
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_BATH_J1, 40, RB + I2C + 16, RB + I2C + 25, RelayMode.INTERLOCKED_ACTIVE_UP);


            //L2.BEDR
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B11, BB + I2C + 3, ID.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B12, BB + I2C + 2, ID.POWIT_L2_BEDR_P1);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_BEDR_B13, BB + I2C + 6, ID.PWM___L2_BEDR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B14, ID.PUSHB_L2_BEDR_B15, BB + I2C + 5, BB + I2C + 4, ID.BLIND_L2_BEDR_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B25, BB + I2C + 10, ID.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B24, BB + I2C + 11, ID.POWIT_L2_BEDR_P2);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_BEDR_B23, BB + I2C + 9, ID.PWM___L2_BEDR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B21, ID.PUSHB_L2_BEDR_B22, BB + I2C + 8, BB + I2C + 7, ID.BLIND_L2_BEDR_J1);

            SNSCT_L3_TECH_HS_1.Applications.AddTwoDimButtons(ID.PUSHB_L2_BEDR_B31, ID.PUSHB_L2_BEDR_B32, BB + I2C + 15, BB + I2C + 13, ID.PWM___L2_BEDR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B33, BB + I2C + 12, ID.POWIT_L2_BEDR_P1);
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_BEDR_B34, BB + I2C + 17, ID.POWIT_L2_BEDR_P2);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_BEDR_B35, ID.PUSHB_L2_BEDR_B36, BB + I2C + 14, BB + I2C + 16, ID.BLIND_L2_BEDR_J1);

            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_BEDR_J1, 40, RB + I2C + 17, RB + I2C + 26, RelayMode.INTERLOCKED_ACTIVE_UP);
            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L2_BEDR_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 1, RB + I2C + 2 });
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P1, RB + I2C + 44);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P2, RB + I2C + 45);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P3, RB + I2C + 46);


            //L2.CORR
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B11, BB + I2C + 20, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B21, BB + I2C + 19, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B31, BB + I2C + 18, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B41, BB + I2C + 23, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_CORR_B42, ID.PUSHB_L2_CORR_B43, BB + I2C + 22, BB + I2C + 21, ID.BLIND_L2_CORR_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_CORR_J1, 40, RB + I2C + 18, RB + I2C + 27, RelayMode.INTERLOCKED_ACTIVE_UP);
            //Start be careful: The LED Spots are controlled by the bath-sensactUP
            SNSCT_L2_BATH_UP.Applications.AddPWMApplication(ID.PWM___L2_CORR_S, ID.STDBY_L3_ROOF_48V, new List<ushort> { BUS0 + I2C + 3, BUS0 + I2C + 4, BUS0 + I2C + 5 }, 1000 * 60 * 240);
            //End be careful

            //KID1
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_KID1_B11, BB + I2C + 26, ID.POWIT_L2_KID1_C1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_KID1_B12, ID.PUSHB_L2_KID1_B13, BB + I2C + 25, BB + I2C + 24, ID.BLIND_L2_KID1_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_KID1_B21, BB + I2C + 28, ID.POWIT_L2_KID1_C1);
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_KID1_B22, BB + I2C + 27, ID.POWIT_L2_KID1_C2);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_KID1_C1, RB + I2C + 39);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_KID1_C2, RB + I2C + 40, 360000);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_KID1_J1, 40, RB + I2C + 19, RB + I2C + 28, RelayMode.INTERLOCKED_ACTIVE_UP);

            //KID2
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L2_KID2_B11, BB + I2C + 31, ID.POWIT_L2_KID2_C1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_KID2_B12, ID.PUSHB_L2_KID2_B13, BB + I2C + 30, BB + I2C + 29, ID.BLIND_L2_KID2_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_KID2_C1, RB + I2C + 41);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_KID2_J1, 40, RB + I2C + 20, RB + I2C + 29, RelayMode.INTERLOCKED_ACTIVE_UP);


            //WORK
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_WORK_B11, BB + I2C + 34, ID.PWM___L2_WORK_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_WORK_B12, ID.PUSHB_L2_WORK_B13, BB + I2C + 33, BB + I2C + 32, ID.BLIND_L2_WORK_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_WORK_J1, 40, RB + I2C + 21, RB + I2C + 30, RelayMode.INTERLOCKED_ACTIVE_UP);
            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L2_WORK_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 3, RB + I2C + 4 });
            #endregion
            #region L3
            //BEDR
            SNSCT_L3_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L3_BEDR_B11, BB + I2C + 38, ID.POWIT_L3_BEDR_C1);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L3_BEDR_B12, BB + I2C + 37, ID.PWM___LS_STRS_W6);
            /*
            SNSCT_L3_TECH_HS_1.Applications.Add(new PushButtonXApplication
             {
                 ApplicationId = _(ID.PUSHB_L3_BEDR_B12),
                 InputRessource = BB + I2C + 37,
                 CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___L3_CORR_S),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_W6),
                    },
                }

             });
             */
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L3_BEDR_B13, ID.PUSHB_L3_BEDR_B14, BB + I2C + 36, BB + I2C + 35, ID.BLIND_L3_BEDR_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L3_BEDR_C1, RB + I2C + 42, 1000 * 60 * 240);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L3_BEDR_J1, 40, RB + I2C + 23, RB + I2C + 32, RelayMode.INTERLOCKED_ACTIVE_DOWN);
            //J2-Relais sind frei, ggf J1 und J2 vertauscht, falls Rolladen nicht geht
            //CORR


            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L3_CORR_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 5 }, 1000 * 60 * 10);


            //TECH
            //SNSCT_L3_TECH_HS_1.Applications.AddPlaceholder(ID.SBRGH_LX_ROOF);
            SNSCT_L3_TECH_HS_1.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L3_TECH_48V), OutputRessource = RB + I2C + 35, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });
            SNSCT_L3_TECH_HS_1.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L3_ROOF_48V), OutputRessource = RB + I2C + 34, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });


            //WORK
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L3_WORK_B11, BB + I2C + 41, ID.PWM___L3_WORK_C1);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L3_WORK_B12, BB + I2C + 40, ID.PWM___L3_WORK_S);

            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L3_WORK_B13, ID.PUSHB_L3_WORK_B14, BB + I2C + 39, BB + I2C + 44, ID.BLIND_L3_WORK_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L3_WORK_B15, BB + I2C + 43, ID.PWM___LS_STRS_W6);

            /*
            SNSCT_L3_TECH_HS_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_L3_WORK_B15),
                InputRessource = BB + I2C + 43,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___L3_CORR_S),
                    },
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.PWM___LS_STRS_W6),
                    },
                }

            });
            */
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L3_WORK_J1, 40, RB + I2C + 22, RB + I2C + 31, RelayMode.INTERLOCKED_ACTIVE_DOWN);
            //SNSCT_L3_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L3_WORK_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = BUS0 + I2C + 8, OutputRessourceDownOrDirection = BUS0 + I2C + 17, RelayMode = RelayMode.INTERLOCKED_ACTIVE_DOWN });

            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L3_WORK_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 6, RB + I2C + 7, RB + I2C + 8 });
            SNSCT_L3_TECH_HS_1.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L3_WORK_C1), OutputRessource = RB + I2C + 43, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });//RL10

            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L3_WORK_C1, ID.STDBY_L3_WORK_C1, new List<ushort> { RB + I2C + 0 });
            #endregion
            #region LS
            /*
            SNSCT_L3_TECH_HS_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId= _(ID.LIBAR_LS_STRS_B11),
                ActiveSignalLevel=true,
                BrightnessSensor= _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                FinalTarget =_(ID.PWM___LS_STRS_W2),
                InputRessource= BUS0 + I2C + 45, 
             });
             */
            //SNSCT_L3_TECH_HS_2.Applications.AddToggleButton(ID.PUSHB_LS_STRS_B21, InputPin.I21, ID.POWIT_LS_STRS_W3);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B21, BB + I2C + 48, ID.PWM___LS_STRS_W4);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B22, BB + I2C + 47, ID.PWM___L1_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.Add(new PushButtonXApplication
            {
                ApplicationId = _(ID.PUSHB_LS_STRS_B23),
                InputRessource = BB + I2C + 46,
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
                    /*
                    new Command
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=_(ID.POWIT_LS_STRS_W1),
                    },
                    */
                }

            });
            /*
            SNSCT_L3_TECH_HS_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B31),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),// _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W2),
                InputRessource = BUS0 + I2C + 49,
            });
            
            SNSCT_L3_TECH_HS_2.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B32),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W4),
                InputRessource = BUS0 + I2C + 50,
            });
            */

            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B41, BB + I2C + 55, ID.PWM___LS_STRS_W6);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B42, BB + I2C + 54, ID.PWM___L2_CORR_S);
            //SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B43, InputPin.I22, ID.POWIT_LS_STRS_W3);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B43, BB + I2C + 53, ID.PWM___LS_STRS_W4);
            /*
            SNSCT_L3_TECH_HS_1.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B51),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W4),
                InputRessource = BUS0 + I2C + 51,
            });
            SNSCT_L3_TECH_HS_1.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B52),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W6),
                InputRessource = BUS0 + I2C + 52,
            });



            SNSCT_L3_TECH_HS_1.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LS_STRS_B71),
                ActiveSignalLevel = true,
                BrightnessSensor = _(ID.NO_APPLICATION),// _(ID.SBRGH_LX_ROOF),
                FinalTarget = _(ID.PWM___LS_STRS_W6),
                InputRessource = BUS0 + I2C + 42,
            });
            */
            //!!!SNSAC_L1_LVNG_UP

            //Treppenstufen, vom Keller zum Dach
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W2, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 16 }, 1000 * 60 * 60 * 6 /*Verzögerung siehe Wand*/);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W4, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 17 }, 1000 * 120);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W6, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 18 }, 1000 * 120);

            //Wandlampen, vom Keller zum Dach
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W1, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 19 }, 1000 * 60 * 60 * 6 /*Verzögerung siehe Treppe*/);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W3, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 20 }, 1000 * 120);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W5, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 21 }, 1000 * 120);

            #endregion

            return model;
        }
    }
}
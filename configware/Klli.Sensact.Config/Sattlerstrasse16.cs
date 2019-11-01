using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class Sattlerstrasse16
    {
        private static string _(this ID id){return id.ToString();}
        
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
            Node SNSCT_L0_TECH_HS_1 = new SensactHs08(ID.SNSCT_L0_TECH_HS_1);
            model.Nodes.Add(SNSCT_L0_TECH_HS_1);

            Node SNSCT_L3_TECH_HS_1 = new SensactHs08(ID.SNSCT_L3_TECH_HS_1);
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
                InputRessource = BB + I2C + 13, //36
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
                InputRessource = BB + I2C + 12, //34
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
                InputRessource = BB + I2C + 11,//33
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
                                                                                                                                    
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_CORR_C1, BB + I2C + 40, 1000 * 60 * 15 ); //RL7

            //PRTY
            SNSCT_L0_TECH_HS_1.Applications.AddTwoDimButtons(ID.PUSHB_L0_PRTY_B11, ID.PUSHB_L0_PRTY_B12, BB + I2C + 10, BB + I2C + 9, ID.PWM___L0_PRTY_S);//32,30
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L0_PRTY_S,ID.STDBY_L0_TECH_48V, new List<ushort> { BB + I2C + 13, BB + I2C + 12, BB + I2C + 11, BB + I2C + 10 });//144,143,142,141

            //STO1
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_STO1_B11, BB + I2C + 8, ID.POWIT_L0_STO1_C1);//28
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_STO1_C1, BB + I2C + 39,1000 * 60 * 15 );//RL6

            //TECH
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_TECH_B11, BB + I2C + 7, ID.POWIT_L0_TECH_C1);//24
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_TECH_C1, BB + I2C + 38, 1000 * 60 * 15 );//RL5
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.DEVCE_L0_TECH_PUMP, InputPin.I14, ID.NO_APPLICATION);//56
            SNSCT_L0_TECH_HS_1.Applications.Add(new SoundApplication
            {
                ApplicationId = _(ID.DEVCE_L0_TECH_AUDIO),
                StandbyOutput = (ushort)(INTI + 64u + 2u),//e2,
                NameOfVoulumeScheduleOrNull=null,
                
                
            });
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_TECH_PUMP, BUS0 + I2C + 45, 0, 5000); //sollte 5sek nach Start an gehen und dann auch an bleiben RL14
            SNSCT_L0_TECH_HS_1.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L0_TECH_48V), OutputRessource = BB + I2C + 46, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });//RL15 K115
            SNSCT_L0_TECH_HS_1.Applications.Add(new StandbyControllerApplication { ApplicationId = _(ID.STDBY_L2_CORR_24V), OutputRessource = BB + I2C + 47, WaittimeInMsec = DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS });//RL17 K117
            
            //TODO:Heizungsanlage

            //WELL
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_WELL_B11, BB + I2C + 6, ID.POWIT_L0_WELL_C1);//20
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B12, BB + I2C + 5, ID.PWM___L0_WELL_S1);//18Für Dusche
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L0_WELL_B21, BB + I2C + 4, ID.PWM___L0_WELL_S2); //16 Für WC
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_WELL_C1, BB + I2C + 37, 1000 * 60 * 240 );//RL4
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L0_WELL_S1, ID.STDBY_L0_TECH_48V, new List<ushort> { BB + I2C + 9 }, 1000 * 60 * 30); //140 Dusche
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L0_WELL_S2,ID.STDBY_L0_TECH_48V, new List<ushort> { BB + I2C + 8 }, 1000 * 60 * 30); //138 WC

            //WORK
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L0_WORK_B11, BB + I2C + 3, ID.POWIT_L0_WORK_C1); //12
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L0_WORK_C1, BB + I2C + 36, 1000 * 60 * 240 ); //RL2
            #endregion
            #region LX
            //Back
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_BACK_P1, BB + I2C + 60, 1000 * 60 * 360); //RL18 K118
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_BACK_P3, BB + I2C + 56, 1000 * 60 * 360); //RL32 K132//Nur für Lampe!!!
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W1, BB + I2C + 51, 1000 * 60 * 360); //RL27 K127
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W2, BB + I2C + 50, 1000 * 60 * 360); //RL26 K126
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_BACK_W3, BB + I2C + 49, 1000 * 60 * 360); //RL25 K125
			
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_LX_BACK_J1, 40,BB + I2C + 25,BB + I2C + 35, RelayMode.INTERLOCKED_ACTIVE_DOWN);//K19 K20

            //Front (Klingelknopf, Licht, Bewegungsmelder)
            SNSCT_L0_TECH_HS_1.Applications.Add(new PushButtonXApplication
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
                InputRessource = BB + I2C + 62,
            });
            SNSCT_L0_TECH_HS_1.Applications.Add(new LightbarrierApplication
            {
                ApplicationId = _(ID.LIBAR_LX_FRON_B2),
                InputRessource = BB + I2C + 63, //Relais-Kabel
                ActiveSignalLevel = false,
                BrightnessSensor = _(ID.NO_APPLICATION),//_(ID.SBRGH_LX_ROOF),
                // FinalTarget = _(ID.POWIT_LX_FRON_W1),
            });
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_FRON_W1, BB + I2C + 55, 1000 * 60 * 2);//RL31 K131

            //GARA
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_LX_GARA_B11, BB + I2C + 2, ID.POWIT_LX_GARA_C1);//4
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_LX_GARA_B21, BUS0 + I2C + 0, ID.BLIND_LX_GARA_J1); //2
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_GARA_C1, BB + I2C + 54, 1000 * 60 * 360); //RL30 K130
            //SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.BLIND_LX_GARA_J1, OutputPin.O18, 500); //unbeschriftet

            //LEFT
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_LEFT_W1, BB + I2C + 53, 1000 * 60 * 360); //RL29 K129

            //RGHT
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_LX_RGHT_W1, BB + I2C + 52, 1000 * 60 * 360);//RL28 K128

            #endregion
            #region L1

            //BATH
            SNSCT_L0_TECH_HS_1.Applications.Add(
                new PushButtonXApplication()
                {
                    ApplicationId = _(ID.PUSHB_L1_BATH_B11),
                    InputRessource = BB + I2C + 38, //96
                    CommandsOnReleased = new List<Command>()
                    {
                        new Command()
                        {
                            CommandType = CommandType.TOGGLE,
                            TargetAppId =_(ID.RGBW__L1_BATH_W1),
                        },
                        new Command()
                        {
                            CommandType=CommandType.TOGGLE,
                            TargetAppId=_(ID.PWM___L1_BATH_S),
                        },
                    },
                }
                );
            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_BATH_B12, ID.PUSHB_L1_BATH_B13, BUS0 + I2C + 37, BUS0 + I2C + 36, ID.BLIND_L1_BATH_J1); //94+93
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication (ID.PWM___L1_BATH_S, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 3 }, 1000 * 60 * 60); //128
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_BATH_J1, 40, BB + I2C + 16, BUS0 + I2C + 26, RelayMode.INTERLOCKED_ACTIVE_UP);//K1 K2
            //Dekoleuchten werden aus dem 48V-Netzteil + Spannungswandler versorgt
            SNSCT_L0_TECH_HS_1.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L1_BATH_W1), StandbyController = _(ID.STDBY_L0_TECH_48V), LowMeansLampOn = true, OutputRessourceR = BUS0 + I2C + 7, OutputRessourceB = BB + I2C + 5, OutputRessourceG = BB + I2C + 6, OutputRessourceW = ushort.MaxValue, AutoOffIntervalMsecs= 1000 * 60 * 60 });


            //CORR
            //Hinweis: LeavingHome/ComingHome ist noch nicht beschaltet
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B11, BUS0 + I2C + 16, ID.PWM___L1_CORR_S);//52

            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B21, BUS0 + I2C + 35, ID.PWM___L1_CORR_S);//88
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B22, BUS0 + I2C + 34, ID.PWM___L1_CORR_W1);//86
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B23, BUS0 + I2C + 33, ID.PWM___L1_CORR_S);//85
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_CORR_B24, BUS0 + I2C + 32, ID.PWM___L1_CORR_W1);//84
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B25, BUS0 + I2C + 31, ID.POWIT_LX_FRON_W1);//82
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_CORR_B26, BUS0 + I2C + 30, ID.POWIT_LX_LEFT_W1);//81
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L1_CORR_S, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 2, BUS0 + I2C + 1, BUS0 + I2C + 0 }, 1000*60*10 ); //126, 125, 124
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L1_CORR_W1, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 4 }, 1000 * 60 * 240);

            //L1.KTCH
            SNSCT_L1_KTCH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_KTCH_B21, RotaryEncoder.ROTARYENCODER_1, SensactUp02.Pin_ROTD, ID.PWM___L1_KTCH_S1);
            SNSCT_L1_KTCH_UP.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B22, SensactUp02.Pin_xROTD, ID.POWIT_L1_KTCH_C1);
            SNSCT_L1_KTCH_UP.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B23, SensactUp02.IO1, ID.PWM___L1_KTCH_W1);
            SNSCT_L1_KTCH_UP.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B24, SensactUp02.IO2, ID.PWM___L1_KTCH_S2);
            SNSCT_L1_KTCH_UP.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B25, ID.PUSHB_L1_KTCH_B26, SensactUp02.IO3, SensactUp02.IO4, ID.BLIND_L1_KTCH_J1);
            SNSCT_L1_KTCH_UP.Applications.AddBlindButtons(ID.PUSHB_L1_KTCH_B27, ID.PUSHB_L1_KTCH_B28, SensactUp02.IO5, SensactUp02.IO6, ID.BLIND_L1_KTCH_J2);
            //Vier Spots
            SNSCT_L1_KTCH_UP.Applications.AddPWMApplication(ID.PWM___L1_KTCH_S1, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C+0, BUS0 + I2C + 1, BUS0 + I2C + 2, BUS0 + I2C + 3 } );
            //Einzelspot
            SNSCT_L1_KTCH_UP.Applications.AddPWMApplication(ID.PWM___L1_KTCH_S2, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 4 } );
            //Wandlampen
            SNSCT_L1_KTCH_UP.Applications.AddPWMApplication(ID.PWM___L1_KTCH_W1, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 5 }, 1000*60*360, 150 ); //hohes minimales DIM-Level, um "Verschmierungen" durch lange Leitungen zu kompensieren

            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B11, BUS0 + I2C + 29, ID.POWIT_L1_KTCH_C1);//80
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_KTCH_B12, BUS0 + I2C + 28, ID.POWIT_LX_BACK_W1);//78
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B31, BUS0 + I2C + 27, ID.PWM___L1_KTCH_S2);//76
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_KTCH_B32, BUS0 + I2C + 26, ID.PWM___L1_KTCH_S1);//74
			//TODO obsolet SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_CORR_W1);//RL24
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_KTCH_C1, BUS0 + I2C + 48);//RL23 K123
            //TODO obsolet SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_KTCH_W1, BB + I2C + 49);//RL22
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_KTCH_J1, 40, BUS0 + I2C + 18, BUS0 + I2C + 28, RelayMode.INTERLOCKED_ACTIVE_UP );//K5 K6
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_KTCH_J2, 40, BUS0 + I2C + 17, BUS0 + I2C + 27, RelayMode.INTERLOCKED_ACTIVE_UP );//K3 K4
            
            //L1.LVNG

            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B31, ID.PUSHB_L1_LVNG_B32, SensactUp02.IO1, SensactUp02.IO2, ID.BLIND_L1_LVNG_J1);
            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B33, ID.PUSHB_L1_LVNG_B34, SensactUp02.IO3, SensactUp02.IO4, ID.BLIND_L1_LVNG_J2);
            SNSCT_L1_LVNG_UP.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B35, ID.PUSHB_L1_LVNG_B36, SensactUp02.IO5, SensactUp02.IO6, ID.BLIND_L1_LVNG_J3);
            SNSCT_L1_LVNG_UP.Applications.AddRotaryEncoder(ID.ROTAR_L1_LVNG_B41, RotaryEncoder.ROTARYENCODER_1, SensactUp02.Pin_ROTD, ID.PWM___L1_LVNG_S);
            SNSCT_L1_LVNG_UP.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B42,SensactUp02.Pin_xROTD, ID.POWIT_L1_LVNG_C1);
            SNSCT_L1_LVNG_UP.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B43, SensactUp02.IO7, ID.PWM___L1_LVNG_W1);
            SNSCT_L1_LVNG_UP.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B44, SensactUp02.IO8, ID.PWM___L1_LVNG_C2);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___L1_LVNG_S, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 0, BUS0 + I2C + 1, BUS0 + I2C + 2, BUS0 + I2C + 3, BUS0 + I2C + 4, BUS0 + I2C + 5, BUS0 + I2C + 6, BUS0 + I2C + 7, BUS0 + I2C + 8, BUS0 + I2C + 9, BUS0 + I2C + 10, BUS0 + I2C + 11 } );
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___L1_LVNG_W1, ID.STDBY_L0_TECH_48V, new List<ushort> { BUS0 + I2C + 12 }, 1000 * 60 * 360, 150);//hohes minimales DIM-Level, um "Verschmierungen" durch lange Leitungen zu kompensieren
            //TODO: C2 soll mal eine RGBW-Applikation werden und es braucht dann 3 oder 4 Outputs
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___L1_LVNG_C2, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 13 /*14+15?*/ } );
            //Erinnerung: Treppenstufenbeleuchtung und Wandlampen im Treppenhaus werden auch von diesem SensactUP gesteuert

            //B11 I23
            //B12 I22
            //B13 I21
            //14 I17
            //
            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_LVNG_B11, ID.PUSHB_L1_LVNG_B12, BUS0 + I2C + 25, BUS0 + I2C + 24, ID.BLIND_LX_BACK_J1);//72+70
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B13, BUS0 + I2C + 23, ID.POWIT_LX_BACK_W2);//69
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B14, BUS0 + I2C + 19, ID.POWIT_LX_BACK_W3);//60
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B15, BUS0 + I2C + 18, ID.POWIT_LX_BACK_P1);//58, P1 sind die Außensteckdosen
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B16, BUS0 + I2C + 17, ID.PWM___L1_LVNG_S);//57

            SNSCT_L0_TECH_HS_1.Applications.AddTwoDimButtons(ID.PUSHB_L1_LVNG_B21, ID.PUSHB_L1_LVNG_B22, BUS0 + I2C + 22, BUS0 + I2C + 21, ID.PWM___L1_LVNG_W4);//68+66
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B23, BUS0 + I2C + 20, ID.POWIT_LX_RGHT_W1);//65

            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B51, BUS0 + I2C + 61, ID.PWM___L1_LVNG_S);//120
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_LVNG_B52, BUS0 + I2C + 60, ID.PWM___L1_KTCH_S1);//118
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_LVNG_B53, BUS0 + I2C + 59, ID.POWIT_L1_KTCH_C1);//117

            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_LVNG_J1, 40, BUS0 + I2C + 19, BUS0 + I2C + 29, RelayMode.INTERLOCKED_ACTIVE_UP );///K7 K8
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_LVNG_J2, 40, BUS0 + I2C + 20, BUS0 + I2C + 30,RelayMode.INTERLOCKED_ACTIVE_UP );//K9 K10
            SNSCT_L0_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L1_LVNG_J3,40, BUS0 + I2C + 21, BUS0 + I2C + 31, RelayMode.INTERLOCKED_ACTIVE_UP);//K11 K12

            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_C1, BUS0 + I2C + 44);//RL11
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_P4, BUS0 + I2C + 61);//RL19 K119
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_LVNG_P1, BUS0 + I2C + 62);//RL20
            //W2 ist parallel zu W1 geschaltet
            //W3 ist noch nicht existente Effektbeleuchtung, sowohl für die Kaminregale als auch für die Wohnwand
            SNSCT_L0_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L1_LVNG_W4,ID.STDBY_L0_TECH_48V,new List<ushort> { BB + I2C + 14 } ); //Grünes Spezialkabel, das im Schaltschrank oben raus geht

            //UTIL
            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_UTIL_B11, ID.PUSHB_L1_UTIL_B12, BB + I2C + 58, BB + I2C + 57, ID.BLIND_L1_UTIL_J1); //116 + 114
            SNSCT_L0_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L1_UTIL_B13, BB + I2C + 56, ID.PWM___L1_KTCH_S1); //113,unterhalb des Jalousietasters, weil für anderen Raum
            
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B21, BB + I2C + 55, ID.POWIT_L1_UTIL_C1);//112
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B22, BB + I2C + 54, ID.POWIT_LX_LEFT_W1);//110
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B23, BUS0 + I2C + 53, ID.BLIND_LX_GARA_J1); //109
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_UTIL_B24, BUS0 + I2C + 52, ID.BLIND_LX_GARA_J1); //108
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_UTIL_C1, BB + I2C + 41);//rl8
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_UTIL_J1), OpenCloseTimeInSeconds = 40, OutputResourceUpOrPower = BB + I2C + 22, OutputResourceDownOrDirection = BB + I2C + 32, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP }); //K13 K14


            //WORK
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B11, BB + I2C + 41, ID.POWIT_L1_WORK_C1);//100
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B12, BB + I2C + 40, ID.POWIT_L1_WORK_C2);//98

            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B13, ID.PUSHB_L1_WORK_B14, BB + I2C + 39, BB + I2C + 51, ID.BLIND_L1_WORK_J1);//97+104
            SNSCT_L0_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L1_WORK_B15, ID.PUSHB_L1_WORK_B16, BB + I2C + 50, BB + I2C + 49, ID.BLIND_L1_WORK_J2);//103+102
            SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.PUSHB_L1_WORK_B17, BB + I2C + 48, ID.POWIT_LX_RGHT_W1);//101
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_WORK_C1, BB + I2C + 43); //rl10
            SNSCT_L0_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L1_WORK_C2, BB + I2C + 42);//rl9
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J1), OpenCloseTimeInSeconds = 40, OutputResourceUpOrPower = BB + I2C + 23, OutputResourceDownOrDirection = BB + I2C + 33, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP }); //K15 16
            SNSCT_L0_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L1_WORK_J2), OpenCloseTimeInSeconds = 40, OutputResourceUpOrPower = BB + I2C + 24, OutputResourceDownOrDirection = BB + I2C + 34, RelayMode = RelayMode.INTERLOCKED_ACTIVE_UP });//K17 18
            #endregion
            #region L2
            //BATH
            SNSCT_L2_BATH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B11, RotaryEncoder.ROTARYENCODER_1, SensactUp02.Pin_ROTD, ID.PWM___L2_BATH_S);
            SNSCT_L2_BATH_UP.Applications.AddRotaryEncoder(ID.ROTAR_L2_BATH_B12, RotaryEncoder.ROTARYENCODER_2, SensactUp02.Pin_xROTD, ID.RGBW__L2_BATH_W);
            //Start "be careful". These buttons are connected to central sensact
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_BATH_B13, ID.PUSHB_L2_BATH_B14, BB + I2C + 1,  BB + I2C + 0, ID.BLIND_L2_BATH_J1);//4 2
            //End "be careful"
            SNSCT_L2_BATH_UP.Applications.AddPWMApplication(ID.PWM___L2_BATH_S, ID.STDBY_L3_ROOF_48V, new List<ushort> { BUS0 + I2C + 0, BUS0 + I2C + 1, BUS0 + I2C + 2 });
            SNSCT_L2_BATH_UP.Applications.Add(new RgbwApplication { ApplicationId = _(ID.RGBW__L2_BATH_W), StandbyController = _(ID.STDBY_L3_ROOF_48V), LowMeansLampOn = true, OutputRessourceR = /*Nach L2.Corr!*/BUS0 + I2C + 6, OutputRessourceG = BUS0 + I2C + 7, OutputRessourceB = BUS0 + I2C + 8 , OutputRessourceW = BUS0 + I2C + 9 });
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
            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L2_BEDR_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 1, RB + I2C + 2 } );
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P1, RB + I2C + 44);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P2, RB + I2C + 45);
            SNSCT_L3_TECH_HS_1.Applications.AddPowIt(ID.POWIT_L2_BEDR_P3, RB + I2C + 46);
            
            
            //L2.CORR
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B11, BB + I2C + 20, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B21, BB + I2C + 19, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B31, BB + I2C + 18, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_CORR_B41, BB + I2C + 23, ID.PWM___L2_CORR_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_CORR_B42, ID.PUSHB_L2_CORR_B43, BB + I2C + 22, BB + I2C + 21, ID.BLIND_L2_CORR_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_CORR_J1, 40,RB + I2C + 18, RB + I2C + 27, RelayMode.INTERLOCKED_ACTIVE_UP);
            //Start be careful: The LED Spots are controlled by the bath-sensactUP
            SNSCT_L2_BATH_UP.Applications.AddPWMApplication(ID.PWM___L2_CORR_S, ID.STDBY_L3_ROOF_48V,new List<ushort> { BUS0 + I2C + 3, BUS0 + I2C + 4, BUS0 + I2C + 5 }, 1000 * 60 * 240);
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
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_KID2_J1, 40,RB + I2C + 20, RB + I2C + 29, RelayMode.INTERLOCKED_ACTIVE_UP );


            //WORK
            SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_L2_WORK_B11, BB + I2C + 34, ID.PWM___L2_WORK_S);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindButtons(ID.PUSHB_L2_WORK_B12, ID.PUSHB_L2_WORK_B13, BB + I2C + 33, BB + I2C + 32, ID.BLIND_L2_WORK_J1);
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L2_WORK_J1, 40,RB + I2C + 21, RB + I2C + 30, RelayMode.INTERLOCKED_ACTIVE_UP );
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
            SNSCT_L3_TECH_HS_1.Applications.AddBlindApplication(ID.BLIND_L3_BEDR_J1, 40, RB + I2C + 23, RB + I2C + 32, RelayMode.INTERLOCKED_ACTIVE_DOWN );
            //J2-Relais sind frei, ggf J1 und J2 vertauscht, falls Rolladen nicht geht
            //CORR


            SNSCT_L3_TECH_HS_1.Applications.AddPWMApplication(ID.PWM___L3_CORR_S, ID.STDBY_L3_TECH_48V, new List<ushort> { RB + I2C + 5 }, 1000*60*10 );


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
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W2, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 16 }, 1000*60*60* 6 /*Verzögerung siehe Wand*/);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W4, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 17 }, 1000*120 );
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W6, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 18 }, 1000 * 120);

            //Wandlampen, vom Keller zum Dach
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W1, ID.STDBY_L2_CORR_24V, new List<ushort> { BUS0 + I2C + 19 }, 1000 * 60 * 60 * 6 /*Verzögerung siehe Treppe*/);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W3, ID.STDBY_L2_CORR_24V,new List<ushort> { BUS0 + I2C + 20 }, 1000 * 120);
            SNSCT_L1_LVNG_UP.Applications.AddPWMApplication(ID.PWM___LS_STRS_W5, ID.STDBY_L2_CORR_24V,new List<ushort> { BUS0 + I2C + 21 }, 1000 * 120);

            #endregion

            return model;


        }
    }
}

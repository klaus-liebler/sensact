using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using Klli.Sensact.Model.Common;
using Klli.Sensact.Model;
using System.Collections.Generic;
using Klli.Sensact.Common;

namespace Klli.Sensact.Config
{
    public class Sattlerstrasse16Builder:ModelBuilder<ApplicationId>
    {
        public Sattlerstrasse16Builder(): base("Sattlerstraße 16")
        {
        }

        
        //private static string _(this ApplicationId id){return id.ToString();}
        
        private const int DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS = 20000;
        private const int DEFAULT_BLIND_UP_sec = 40;
        private const int DEFAULT_BLIND_DOWN_sec = 40;

        private const int DEFAULT_LONG_BLIND_UP_sec = 40;
        private const int DEFAULT_LONG_BLIND_DOWN_sec = 40;
        
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

        const uint T_5m__ms = 5*60*1000;
        const uint T_30m__ms = 30*60*1000;
        const uint T_2h__ms = 120*60*1000;
        const uint T_12h__ms = 720*60*1000;

        const ushort INPUT1 = 0x3FFF;      // steht für einen Input mit dauerhaft 1 (letzte Bits sind 1 (wie I=input)-1)
        const ushort INPUT0 = 0x3FFE;      // steht für einen Input mit dauerhaft 0 (letzte Bits sind 1 (wie I=input)-0)
        const ushort OUTPUT_NULL = 0x3FFC; // steht für einen unbeschalteten Output
        

        //Zählreihenfolge beim EIB-Kabel: rot, schwarz, weiß, gelb

        protected override void BuildModel()
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
            //10V-PWM für L3.Office

            //Spezialitäten unten
            //Ansteuerung des MP3-Moduls + Verstärker (seriell)
            //12V-Versorgung der RGBW-Spots im L1.BATH
            //5V-Versorgung des OneWire
            //LED für Außensteckdosen im Tastermodul am Terassenausgang
            
            Node SNSCT_L0_TECH_HS_1 = new SensactHs08((ushort)ApplicationId.SNSCT_L0_TECH_HS_1, ApplicationId.SNSCT_L0_TECH_HS_1.ToString());
            model.Nodes.Add(SNSCT_L0_TECH_HS_1);

            Node SNSCT_L3_TECH_HS_1 = new SensactHs08((ushort)ApplicationId.SNSCT_L3_TECH_HS_1, ApplicationId.SNSCT_L3_TECH_HS_1.ToString());
            model.Nodes.Add(SNSCT_L3_TECH_HS_1);

            Node SNSCT_L1_KTCH_UP = new SensactUp02((ushort)ApplicationId.SNSCT_L1_KTCH_UP, ApplicationId.SNSCT_L1_KTCH_UP.ToString());
            model.Nodes.Add(SNSCT_L1_KTCH_UP);

            Node SNSCT_L1_LVNG_UP = new SensactUp02((ushort)ApplicationId.SNSCT_L1_LVNG_UP, ApplicationId.SNSCT_L1_LVNG_UP.ToString());
            model.Nodes.Add(SNSCT_L1_LVNG_UP);

            Node SNSCT_L2_BATH_UP = new SensactUp02((ushort)ApplicationId.SNSCT_L2_BATH_UP, ApplicationId.SNSCT_L2_BATH_UP.ToString());
            model.Nodes.Add(SNSCT_L2_BATH_UP);

            Node NODE_SIDEDOOR = new SensactUpControl((ushort)Model.ApplicationId.SNSCT_NODE_SIDEDOOR, Model.ApplicationId.SNSCT_NODE_SIDEDOOR.ToString());
            model.Nodes.Add(NODE_SIDEDOOR);
            Node NODE_TERRASSE = new SensactOutdoor((ushort)Model.ApplicationId.SNSCT_NODE_TERRASSE, Model.ApplicationId.SNSCT_NODE_TERRASSE.ToString());
            model.Nodes.Add(NODE_TERRASSE);
            #endregion


            #region L0
            //CORR
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_CORR_B11, BB + I2C + 13, //36,
                 new HashSet<ApplicationId>{ApplicationId.POWIT_L0_CORR_C1, ApplicationId.PWM___LS_STRS_W2});

                
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_CORR_B21, BB + I2C + 12, //34
                new HashSet<ApplicationId>{ApplicationId.POWIT_L0_CORR_C1, ApplicationId.PWM___LS_STRS_W2});

            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_CORR_B31, BB + I2C + 11,//33
                new HashSet<ApplicationId>{ApplicationId.POWIT_L0_CORR_C1, ApplicationId.PWM___LS_STRS_W2});
                                                                                                                                  
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L0_CORR_C1, "Deckenlicht", BB + I2C + 40, T_30m__ms); //RL7

            //PRTY
            AddTwoDimButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_PRTY_B11, BB + I2C + 10, BB + I2C + 9, ApplicationId.PWM___L0_PRTY_S);//32, 30
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L0_PRTY_S, "Spots", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BB + I2C + 13, BB + I2C + 12, BB + I2C + 11, BB + I2C + 10 }, T_12h__ms);//144,143,142,141

            //STO1
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_STO1_B11, BB + I2C + 8, ApplicationId.POWIT_L0_STO1_C1);//28
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L0_STO1_C1, "Deckenlicht", BB + I2C + 39, T_30m__ms);//RL6

            //TECH
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_TECH_B11, BB + I2C + 7, ApplicationId.POWIT_L0_TECH_C1);//24
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L0_TECH_C1, "Deckenlicht", BB + I2C + 38, T_30m__ms);//RL5
            //SNSCT_L0_TECH_HS_1.Applications.AddToggleButton(ID.DEVCE_L0_TECH_PUMP, InputPin.I14, ID.NO_APPLICATION);//56
            SNSCT_L0_TECH_HS_1.Applications.Add(new SoundApplication((ushort)ApplicationId.SOUND_L0_TECH_AUDIO, ApplicationId.SOUND_L0_TECH_AUDIO.ToString(), "Türklingel", 17));
            SNSCT_L0_TECH_HS_1.Applications.Add(new PumpApplication((ushort)ApplicationId.PUMP__L0_TECH_PUMP, ApplicationId.PUMP__L0_TECH_PUMP.ToString(), "Hebepumpe", BUS0 + I2C + 45, T_5m__ms, T_2h__ms)); // RL14
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.STDBY_L0_TECH_48V, "Netzteil 48V", BB + I2C + 48, DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS);//RL15 K115
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.STDBY_L2_CORR_24V, "Netzteil 24V", BB + I2C + 49, DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS );//RL17 K117
            
            //TODO:Heizungsanlage

            //WELL
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_WELL_B11, BB + I2C + 6, ApplicationId.POWIT_L0_WELL_C1);//20
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_WELL_B12, BB + I2C + 5, ApplicationId.PWM___L0_WELL_S1);//18Für Dusche
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_WELL_B21, BB + I2C + 4, ApplicationId.PWM___L0_WELL_S2); //16 Für WC
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L0_WELL_C1, "Deckenlicht", BB + I2C + 37, 1000 * 60 * 240 );//RL4
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L0_WELL_S1, "Spot Dusche", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BB + I2C + 9 }, 1000 * 60 * 30); //140 Dusche
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L0_WELL_S2,"Spot WC", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BB + I2C + 8 }, 1000 * 60 * 30); //138 WC

            //WORK
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L0_WORK_B11, BB + I2C + 3, ApplicationId.POWIT_L0_WORK_C1); //12
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L0_WORK_C1, "Deckenlicht", BB + I2C + 36, 1000 * 60 * 240 ); //RL2
            #endregion
            #region LX
            //Back
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_BACK_P1, "Phase1", BB + I2C + 59, 1000 * 60 * 360); //RL18 K118
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_BACK_P3, "Phase3", BB + I2C + 58, 1000 * 60 * 360); //RL32 K132//Nur für Lampe!!!
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_BACK_W1, "Wandlicht Küche", BB + I2C + 53, 1000 * 60 * 360); //RL27 K127
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_BACK_W2, "Wandlicht mitte", BB + I2C + 52, 1000 * 60 * 360); //RL26 K126
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_BACK_W3, "Wandlicht Wohnen", BB + I2C + 51, 1000 * 60 * 360); //RL25 K125
			
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_LX_BACK_J1, "Markise nicht genutzt", BB + I2C + 25,BB + I2C + 35, RelayInterlockMode.R1_POWER__R2_DOWN, 60, 60);//K19 K20

            //Front (Klingelknopf, Licht, Bewegungsmelder)
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_LX_FRON_B1, BB + I2C + 62, ApplicationId.SOUND_L0_TECH_AUDIO);//Klingelknopf
            SNSCT_L0_TECH_HS_1.Applications.Add(new LightbarrierApplication(
                (ushort)ApplicationId.LIBAR_LX_FRON_B2,
                ApplicationId.LIBAR_LX_FRON_B2.ToString(),
                "Bewegungsmelder",
                BB + I2C + 63, //Relais-Kabel
                (ushort)ApplicationId.NO_APPLICATION, 
                (ushort)ApplicationId.NO_APPLICATION,//_(ID.SBRGH_LX_ROOF),
                false));
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_FRON_W1, "Außenleuchte", BB + I2C + 57, 1000 * 60 * 2);//RL31 K131

            //GARA
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_LX_GARA_B11, BB + I2C + 2, ApplicationId.POWIT_LX_GARA_C1);//4
            //AddToggleButton(SNSCT_L0_TECH_HS_1, ID.PUSHB_LX_GARA_B21, BUS0 + I2C + 0, ID.BLIND_LX_GARA_J1); //2
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_GARA_C1, "Deckenlicht", BB + I2C + 56, 1000 * 60 * 360); //RL30 K130
            //AddPowIt(SNSCT_L0_TECH_HS_1, ID.BLIND_LX_GARA_J1, OutputPin.O18, 500); //unbeschriftet

            //LEFT
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_LEFT_W1, "Außenlicht links", BB + I2C + 55, 1000 * 60 * 360); //RL29 K129

            //RGHT
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_LX_RGHT_W1, "Außenlicht rechts", BB + I2C + 54, 1000 * 60 * 360);//RL28 K128

            #endregion
            #region L1

            //BATH
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_BATH_B11, BB + I2C + 38, //96
                new HashSet<ApplicationId>{ApplicationId.RGBW__L1_BATH_W1, ApplicationId.PWM___L1_BATH_S});

            AddBlindButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_BATH_B12, BUS0 + I2C + 37, BUS0 + I2C + 36, ApplicationId.BLIND_L1_BATH_J1); //94+93
            AddPWMApplication (SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L1_BATH_S, "Spots", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 3 }, 1000 * 60 * 60); //128
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_BATH_J1, "Rolladen", BB + I2C + 16, BUS0 + I2C + 26, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);//K1 K2
            //Dekoleuchten werden aus dem 48V-Netzteil + Spannungswandler versorgt
            SNSCT_L0_TECH_HS_1.Applications.Add(new RgbwApplication((ushort)ApplicationId.RGBW__L1_BATH_W1, ApplicationId.RGBW__L1_BATH_W1.ToString(), "Dekolicht", BUS0 + I2C + 7, BB + I2C + 6, BB + I2C + 5, OUTPUT_NULL, T_2h__ms, (ushort)ApplicationId.STDBY_L0_TECH_48V));


            //CORR
            //Hinweis: LeavingHome/ComingHome ist noch nicht beschaltet
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B11, BUS0 + I2C + 16, ApplicationId.PWM___L1_CORR_S);//52

            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B21, BUS0 + I2C + 35, ApplicationId.PWM___L1_CORR_S);//88
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B22, BUS0 + I2C + 34, ApplicationId.PWM___L1_CORR_W1);//86
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B23, BUS0 + I2C + 33, ApplicationId.PWM___L1_CORR_S);//85
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B24, BUS0 + I2C + 32, ApplicationId.PWM___L1_CORR_W1);//84
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B25, BUS0 + I2C + 31, ApplicationId.POWIT_LX_FRON_W1);//82
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_CORR_B26, BUS0 + I2C + 30, ApplicationId.POWIT_LX_LEFT_W1);//81
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L1_CORR_S, "Spots", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 2, BUS0 + I2C + 1, BUS0 + I2C + 0 }, T_2h__ms); //126, 125, 124
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L1_CORR_W1, "Wandlicht", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 4 }, T_2h__ms);

            //L1.KTCH
            AddRotaryEncoder(SNSCT_L1_KTCH_UP, ApplicationId.ROTAR_L1_KTCH_B21, RotaryEncoder.ROTARYENCODER_1, ApplicationId.PWM___L1_KTCH_S1);
            AddToggleButton(SNSCT_L1_KTCH_UP, ApplicationId.PUSHB_L1_KTCH_B22, SensactUp02.Pin_xROTD, ApplicationId.POWIT_L1_KTCH_C1);
            AddOneDimButton(SNSCT_L1_KTCH_UP, ApplicationId.PUSHB_L1_KTCH_B23, SensactUp02.IO1, ApplicationId.PWM___L1_KTCH_W1);
            AddOneDimButton(SNSCT_L1_KTCH_UP, ApplicationId.PUSHB_L1_KTCH_B24, SensactUp02.IO2, ApplicationId.PWM___L1_KTCH_S2);
            AddBlindButtons(SNSCT_L1_KTCH_UP, ApplicationId.PUSHB_L1_KTCH_B25, SensactUp02.IO3, SensactUp02.IO4, ApplicationId.BLIND_L1_KTCH_J1);
            AddBlindButtons(SNSCT_L1_KTCH_UP, ApplicationId.PUSHB_L1_KTCH_B27, SensactUp02.IO5, SensactUp02.IO6, ApplicationId.BLIND_L1_KTCH_J2);
            //Vier Spots
            AddPWMApplication(SNSCT_L1_KTCH_UP, ApplicationId.PWM___L1_KTCH_S1, "Spots", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C+0, BUS0 + I2C + 1, BUS0 + I2C + 2, BUS0 + I2C + 3 } );
            //Einzelspot
            AddPWMApplication(SNSCT_L1_KTCH_UP, ApplicationId.PWM___L1_KTCH_S2, "Einzelspot Spüle", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 4 } );
            //Wandlampen
            AddPWMApplication(SNSCT_L1_KTCH_UP, ApplicationId.PWM___L1_KTCH_W1, "Wandlampen", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 5 }, 1000*60*360, 150 ); //hohes minimales DIM-Level, um "Verschmierungen" durch lange Leitungen zu kompensieren

            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_KTCH_B11, BUS0 + I2C + 29, ApplicationId.POWIT_L1_KTCH_C1);//80
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_KTCH_B12, BUS0 + I2C + 28, ApplicationId.POWIT_LX_BACK_W1);//78
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_KTCH_B31, BUS0 + I2C + 27, ApplicationId.PWM___L1_KTCH_S2);//76
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_KTCH_B32, BUS0 + I2C + 26, ApplicationId.PWM___L1_KTCH_S1);//74
			//TODO obsolet AddPowIt(SNSCT_L0_TECH_HS_1, ID.POWIT_L1_CORR_W1);//RL24
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_KTCH_C1, "Deckenleuchte", BUS0 + I2C + 50);//RL23 K123
            //TODO obsolet AddPowIt(SNSCT_L0_TECH_HS_1, ID.POWIT_L1_KTCH_W1, BB + I2C + 49);//RL22
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_KTCH_J1, "Rollo Garten", BUS0 + I2C + 18, BUS0 + I2C + 28, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_LONG_BLIND_UP_sec, DEFAULT_LONG_BLIND_DOWN_sec);//K5 K6
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_KTCH_J2, "Rollo Garage", BUS0 + I2C + 17, BUS0 + I2C + 27, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);//K3 K4
            
            //L1.LVNG
            AddBlindButtons(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B31, SensactUp02.IO1, SensactUp02.IO2, ApplicationId.BLIND_L1_LVNG_J1);
            AddBlindButtons(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B33, SensactUp02.IO3, SensactUp02.IO4, ApplicationId.BLIND_L1_LVNG_J2);
            AddBlindButtons(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B35, SensactUp02.IO5, SensactUp02.IO6, ApplicationId.BLIND_L1_LVNG_J3);
            AddRotaryEncoder(SNSCT_L1_LVNG_UP, ApplicationId.ROTAR_L1_LVNG_B41, RotaryEncoder.ROTARYENCODER_1, ApplicationId.PWM___L1_LVNG_S);
            AddToggleButton(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B42,SensactUp02.Pin_xROTD, ApplicationId.POWIT_L1_LVNG_C1);
            AddOneDimButton(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B43, SensactUp02.IO7, ApplicationId.PWM___L1_LVNG_W1);
            AddOneDimButton(SNSCT_L1_LVNG_UP, ApplicationId.PUSHB_L1_LVNG_B44, SensactUp02.IO8, ApplicationId.PWM___L1_LVNG_C2);
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___L1_LVNG_S, "Spots", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 0, BUS0 + I2C + 1, BUS0 + I2C + 2, BUS0 + I2C + 3, BUS0 + I2C + 4, BUS0 + I2C + 5, BUS0 + I2C + 6, BUS0 + I2C + 7, BUS0 + I2C + 8, BUS0 + I2C + 9, BUS0 + I2C + 10, BUS0 + I2C + 11 } );
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___L1_LVNG_W1, "Wandleuchten", ApplicationId.STDBY_L0_TECH_48V, new HashSet<ushort> { BUS0 + I2C + 12 }, 1000 * 60 * 360, 150);//hohes minimales DIM-Level, um "Verschmierungen" durch lange Leitungen zu kompensieren
            //TODO: C2 soll mal eine RGBW-Applikation werden und es braucht dann 3 oder 4 Outputs
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___L1_LVNG_C2, "Lampe Deko?", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 13 /*14+15?*/ } );
            //Erinnerung: Treppenstufenbeleuchtung und Wandlampen im Treppenhaus werden auch von diesem SensactUP gesteuert

            //B11 I23
            //B12 I22
            //B13 I21
            //14 I17
            //
            AddBlindButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B11, BUS0 + I2C + 25, BUS0 + I2C + 24, ApplicationId.BLIND_LX_BACK_J1);//72+70
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B13, BUS0 + I2C + 23, ApplicationId.POWIT_LX_BACK_W2);//69
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B14, BUS0 + I2C + 19, ApplicationId.POWIT_LX_BACK_W3);//60
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B15, BUS0 + I2C + 18, ApplicationId.POWIT_LX_BACK_P1);//58, P1 sind die Außensteckdosen
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B16, BUS0 + I2C + 17, ApplicationId.PWM___L1_LVNG_S);//57

            AddTwoDimButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B21, BUS0 + I2C + 22, BUS0 + I2C + 21, ApplicationId.PWM___L1_LVNG_W4);//68+66
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B23, BUS0 + I2C + 20, ApplicationId.POWIT_LX_RGHT_W1);//65

            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B51, BUS0 + I2C + 61, ApplicationId.PWM___L1_LVNG_S);//120
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B52, BUS0 + I2C + 60, ApplicationId.PWM___L1_KTCH_S1);//118
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_LVNG_B53, BUS0 + I2C + 59, ApplicationId.POWIT_L1_KTCH_C1);//117

            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_LVNG_J1, "Rollo Kaninchen", BUS0 + I2C + 19, BUS0 + I2C + 29, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_LONG_BLIND_UP_sec, DEFAULT_LONG_BLIND_DOWN_sec);///K7 K8
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_LVNG_J2, "Rollo Terrasse",BUS0 + I2C + 20, BUS0 + I2C + 30,RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_LONG_BLIND_UP_sec, DEFAULT_LONG_BLIND_DOWN_sec);//K9 K10
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_LVNG_J3, "Rollo Esszimmer", BUS0 + I2C + 21, BUS0 + I2C + 31, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_LONG_BLIND_UP_sec, DEFAULT_LONG_BLIND_DOWN_sec);//K11 K12

            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_LVNG_C1, "Wandlampen", BUS0 + I2C + 44);//RL11
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_LVNG_P4, "Phase 4", BUS0 + I2C + 60);//RL19 K119
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_LVNG_P1, "Phase 1" ,BUS0 + I2C + 61);//RL20
            //W2 ist parallel zu W1 geschaltet
            //W3 ist noch nicht existente Effektbeleuchtung, sowohl für die Kaminregale als auch für die Wohnwand
            AddPWMApplication(SNSCT_L0_TECH_HS_1, ApplicationId.PWM___L1_LVNG_W4, "??LVNG_W4??,", ApplicationId.STDBY_L0_TECH_48V,new HashSet<ushort> { BB + I2C + 14 } ); //Grünes Spezialkabel, das im Schaltschrank oben raus geht

            //UTIL
            AddBlindButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_UTIL_B11, BB + I2C + 58, BB + I2C + 57, ApplicationId.BLIND_L1_UTIL_J1); //116 + 114
            AddOneDimButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_UTIL_B13, BB + I2C + 56, ApplicationId.PWM___L1_KTCH_S1); //113,unterhalb des Jalousietasters, weil für anderen Raum
            
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_UTIL_B21, BB + I2C + 55, ApplicationId.POWIT_L1_UTIL_C1);//112
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_UTIL_B22, BB + I2C + 54, ApplicationId.POWIT_LX_LEFT_W1);//110
            //AddToggleButton(SNSCT_L0_TECH_HS_1, ID.PUSHB_L1_UTIL_B23, BUS0 + I2C + 53, ID.BLIND_LX_GARA_J1); //109
            //AddToggleButton(SNSCT_L0_TECH_HS_1, ID.PUSHB_L1_UTIL_B24, BUS0 + I2C + 52, ID.BLIND_LX_GARA_J1); //108
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_UTIL_C1, "Deckenleuchte", BB + I2C + 41);//rl8
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_UTIL_J1, "Rollo", BB + I2C + 22, BB + I2C + 32, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec); //K13 K14


            //WORK
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_WORK_B11, BB + I2C + 41, ApplicationId.POWIT_L1_WORK_C1);//100
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_WORK_B12, BB + I2C + 40, ApplicationId.POWIT_L1_WORK_C2);//98

            AddBlindButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_WORK_B13, BB + I2C + 39, BB + I2C + 51, ApplicationId.BLIND_L1_WORK_J1);//97+104
            AddBlindButtons(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_WORK_B15, BB + I2C + 50, BB + I2C + 49, ApplicationId.BLIND_L1_WORK_J2);//103+102
            AddToggleButton(SNSCT_L0_TECH_HS_1, ApplicationId.PUSHB_L1_WORK_B17, BB + I2C + 48, ApplicationId.POWIT_LX_RGHT_W1);//101
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_WORK_C1, "Deckenleuchte 1", BB + I2C + 43); //rl10
            AddPowIt(SNSCT_L0_TECH_HS_1, ApplicationId.POWIT_L1_WORK_C2, "Deckenleuchte 2 (na)", BB + I2C + 42);//rl9
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_WORK_J1, "Rollo Stellplatz", BB + I2C + 23, BB + I2C + 33, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec); //K15 16
            AddBlindApplication(SNSCT_L0_TECH_HS_1, ApplicationId.BLIND_L1_WORK_J2, "Rollo Straße", BB + I2C + 24, BB + I2C + 34, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_LONG_BLIND_UP_sec, DEFAULT_LONG_BLIND_DOWN_sec);//K17 18
            #endregion
            #region L2
            //BATH
            AddRotaryEncoder(SNSCT_L2_BATH_UP, ApplicationId.ROTAR_L2_BATH_B11, RotaryEncoder.ROTARYENCODER_1, ApplicationId.PWM___L2_BATH_S);
            AddRotaryEncoder(SNSCT_L2_BATH_UP, ApplicationId.ROTAR_L2_BATH_B12, RotaryEncoder.ROTARYENCODER_2, ApplicationId.RGBW__L2_BATH_W);
            //Start "be careful". These buttons are connected to central sensact
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BATH_B13, BB + I2C + 1,  BB + I2C + 0, ApplicationId.BLIND_L2_BATH_J1);//4 2
            //End "be careful"
            AddPWMApplication(SNSCT_L2_BATH_UP, ApplicationId.PWM___L2_BATH_S, "Spots",ApplicationId.STDBY_L3_ROOF_48V, new HashSet<ushort> { BUS0 + I2C + 0, BUS0 + I2C + 1, BUS0 + I2C + 2 });
            SNSCT_L2_BATH_UP.Applications.Add(new RgbwApplication((ushort)ApplicationId.RGBW__L2_BATH_W, ApplicationId.RGBW__L2_BATH_W.ToString(), "Dekolicht", /*Nach L2.Corr!*/BUS0 + I2C + 6, BUS0 + I2C + 7, BUS0 + I2C + 8 , BUS0 + I2C + 9, T_2h__ms, (ushort)ApplicationId.STDBY_L3_ROOF_48V));
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_BATH_J1, "Rollo", RB + I2C + 16, RB + I2C + 25, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);


            //L2.BEDR
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B11, BB + I2C + 3, ApplicationId.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B12, BB + I2C + 2, ApplicationId.POWIT_L2_BEDR_P1);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B13, BB + I2C + 6, ApplicationId.PWM___L2_BEDR_S);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B14, BB + I2C + 5, BB + I2C + 4, ApplicationId.BLIND_L2_BEDR_J1);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B25, BB + I2C + 10, ApplicationId.POWIT_L2_BEDR_P3); //Erst mal nur Netzfreischaltung
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B24, BB + I2C + 11, ApplicationId.POWIT_L2_BEDR_P2);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B23, BB + I2C + 9, ApplicationId.PWM___L2_BEDR_S);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B21, BB + I2C + 8, BB + I2C + 7, ApplicationId.BLIND_L2_BEDR_J1);

            AddTwoDimButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B31, BB + I2C + 15, BB + I2C + 13, ApplicationId.PWM___L2_BEDR_S);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B33, BB + I2C + 12, ApplicationId.POWIT_L2_BEDR_P1);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B34, BB + I2C + 17, ApplicationId.POWIT_L2_BEDR_P2);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_BEDR_B35, BB + I2C + 14, BB + I2C + 16, ApplicationId.BLIND_L2_BEDR_J1);

            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_BEDR_J1, "Rollo", RB + I2C + 17, RB + I2C + 26, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
            AddPWMApplication(SNSCT_L3_TECH_HS_1, ApplicationId.PWM___L2_BEDR_S, "Spots", ApplicationId.STDBY_L3_TECH_48V, new HashSet<ushort> { RB + I2C + 1, RB + I2C + 2 } );
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_BEDR_P1, "Phase 1", RB + I2C + 44);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_BEDR_P2, "Phase 2", RB + I2C + 45);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_BEDR_P3, "Phase 3", RB + I2C + 46);
            
            
            //L2.CORR
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_CORR_B11, BB + I2C + 20, ApplicationId.PWM___L2_CORR_S);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_CORR_B21, BB + I2C + 19, ApplicationId.PWM___L2_CORR_S);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_CORR_B31, BB + I2C + 18, ApplicationId.PWM___L2_CORR_S);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_CORR_B41, BB + I2C + 23, ApplicationId.PWM___L2_CORR_S);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_CORR_B42, BB + I2C + 22, BB + I2C + 21, ApplicationId.BLIND_L2_CORR_J1);
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_CORR_J1, "Rollo", RB + I2C + 18, RB + I2C + 27, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
            //Start be careful: The LED Spots are controlled by the bath-sensactUP
            AddPWMApplication(SNSCT_L2_BATH_UP, ApplicationId.PWM___L2_CORR_S, "Spots", ApplicationId.STDBY_L3_ROOF_48V,new HashSet<ushort> { BUS0 + I2C + 3, BUS0 + I2C + 4, BUS0 + I2C + 5 }, T_2h__ms);
            //End be careful

            //L2.KID1 (TODO Klara oder Simon???)
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID1_B11, BB + I2C + 26, ApplicationId.POWIT_L2_KID1_C1);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID1_B12, BB + I2C + 25, BB + I2C + 24, ApplicationId.BLIND_L2_KID1_J1);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID1_B21, BB + I2C + 28, ApplicationId.POWIT_L2_KID1_C1);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID1_B22, BB + I2C + 27, ApplicationId.POWIT_L2_KID1_C2);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_KID1_C1, "Deckenleuchte KLA?", RB + I2C + 39, T_12h__ms);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_KID1_C2, "Deckenleuchte KLA na?", RB + I2C + 40, T_12h__ms);
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_KID1_J1, "Rollo KLA?", RB + I2C + 19, RB + I2C + 28, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
  
            //L2.KID2 (TODO Klara oder Simon???)
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID2_B11, BB + I2C + 31, ApplicationId.POWIT_L2_KID2_C1);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_KID2_B12, BB + I2C + 30, BB + I2C + 29, ApplicationId.BLIND_L2_KID2_J1);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L2_KID2_C1, "Deckenleuchte SIM?", RB + I2C + 41, T_12h__ms);
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_KID2_J1, "Rollo SIM?", RB + I2C + 20, RB + I2C + 29, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);


            //WORK ("Halbes" Zimmer, das mit Klaras Zimmer verbunden ist)
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_WORK_B11, BB + I2C + 34, ApplicationId.PWM___L2_WORK_S);
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L2_WORK_B12, BB + I2C + 33, BB + I2C + 32, ApplicationId.BLIND_L2_WORK_J1);
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L2_WORK_J1, "Rollo Garten", RB + I2C + 21, RB + I2C + 30, RelayInterlockMode.R1_POWER__R2_UP, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
            AddPWMApplication(SNSCT_L3_TECH_HS_1, ApplicationId.PWM___L2_WORK_S, "Spots Klavier", ApplicationId.STDBY_L3_TECH_48V, new HashSet<ushort> { RB + I2C + 3, RB + I2C + 4 }, T_12h__ms);
            #endregion
            #region L3
            //BEDR
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_BEDR_B11, BB + I2C + 38, ApplicationId.POWIT_L3_BEDR_C1);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_BEDR_B12, BB + I2C + 37, ApplicationId.PWM___LS_STRS_W6);
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
            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_BEDR_B13, BB + I2C + 36, BB + I2C + 35, ApplicationId.BLIND_L3_BEDR_J1);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.POWIT_L3_BEDR_C1, "Deckenleuchte", RB + I2C + 42, T_2h__ms);
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L3_BEDR_J1, "Rollo", RB + I2C + 23, RB + I2C + 32, RelayInterlockMode.R1_POWER__R2_DOWN, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
            //J2-Relais sind frei, ggf J1 und J2 vertauscht, falls Rolladen nicht geht
            //CORR


            AddPWMApplication(SNSCT_L3_TECH_HS_1, ApplicationId.PWM___L3_CORR_S, "Spots", ApplicationId.STDBY_L3_TECH_48V, new HashSet<ushort> { RB + I2C + 5 }, 1000*60*10 );


            //TECH
            //SNSCT_L3_TECH_HS_1.Applications.AddPlaceholder(ID.SBRGH_LX_ROOF);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.STDBY_L3_TECH_48V, "Standby Kabuff 48V",RB + I2C + 35, DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS);
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.STDBY_L3_ROOF_48V, "Standby Dach 48V", RB + I2C + 34, DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS);


            //WORK
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_WORK_B11, BB + I2C + 41, ApplicationId.PWM___L3_WORK_C1);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_WORK_B12, BB + I2C + 40, ApplicationId.PWM___L3_WORK_S);

            AddBlindButtons(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_WORK_B13, BB + I2C + 39, BB + I2C + 44, ApplicationId.BLIND_L3_WORK_J1);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_L3_WORK_B15, BB + I2C + 43, ApplicationId.PWM___LS_STRS_W6);

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
            AddBlindApplication(SNSCT_L3_TECH_HS_1, ApplicationId.BLIND_L3_WORK_J1, "Rollo", RB + I2C + 22, RB + I2C + 31, RelayInterlockMode.R1_UP__R2_DOWN, DEFAULT_BLIND_UP_sec, DEFAULT_BLIND_DOWN_sec);
            //SNSCT_L3_TECH_HS_1.Applications.Add(new BlindApplication { ApplicationId = _(ID.BLIND_L3_WORK_J2), OpenCloseTimeInSeconds = 40, OutputRessourceUpOrPower = BUS0 + I2C + 8, OutputRessourceDownOrDirection = BUS0 + I2C + 17, RelayMode = RelayMode.INTERLOCKED_ACTIVE_DOWN });

            AddPWMApplication(SNSCT_L3_TECH_HS_1, ApplicationId.PWM___L3_WORK_S, "Spots", ApplicationId.STDBY_L3_TECH_48V, new HashSet<ushort> { RB + I2C + 6, RB + I2C + 7, RB + I2C + 8 });
            AddPowIt(SNSCT_L3_TECH_HS_1, ApplicationId.STDBY_L3_WORK_C1, "Deckenleuchte Netzteil", RB + I2C + 43, DEFAULT_STANDBYCONTROLLER_WAITTIME_MSECS);//RL10

            AddPWMApplication(SNSCT_L3_TECH_HS_1, ApplicationId.PWM___L3_WORK_C1, "Deckenleuchte", ApplicationId.STDBY_L3_WORK_C1, new HashSet<ushort> { RB + I2C + 0 });
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
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B21, BB + I2C + 48, ApplicationId.PWM___LS_STRS_W4);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B22, BB + I2C + 47, ApplicationId.PWM___L1_CORR_S);
            AddToggleButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B23, BB + I2C + 46, new HashSet<ApplicationId>{ApplicationId.POWIT_L0_CORR_C1, ApplicationId.PWM___LS_STRS_W2, /*ID.POWIT_LS_STRS_W1*/});
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

            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B41, BB + I2C + 55, ApplicationId.PWM___LS_STRS_W6);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B42, BB + I2C + 54, ApplicationId.PWM___L2_CORR_S);
            //SNSCT_L3_TECH_HS_1.Applications.AddOneDimButton(ID.PUSHB_LS_STRS_B43, InputPin.I22, ID.POWIT_LS_STRS_W3);
            AddOneDimButton(SNSCT_L3_TECH_HS_1, ApplicationId.PUSHB_LS_STRS_B43, BB + I2C + 53, ApplicationId.PWM___LS_STRS_W4);
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
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W2, "Treppe L0-L1", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 16 }, 1000*60*60* 6 /*Verzögerung siehe Wand*/);
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W4, "Treppe L1-L2", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 17 }, 1000*120 );
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W6, "Treppe L2-L3", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 18 }, 1000 * 120);

            //Wandlampen, vom Keller zum Dach
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W1, "Wand L0-L1", ApplicationId.STDBY_L2_CORR_24V, new HashSet<ushort> { BUS0 + I2C + 19 }, 1000 * 60 * 60 * 6 /*Verzögerung siehe Treppe*/);
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W3, "Wand L1-L2", ApplicationId.STDBY_L2_CORR_24V,new HashSet<ushort> { BUS0 + I2C + 20 }, 1000 * 120);
            AddPWMApplication(SNSCT_L1_LVNG_UP, ApplicationId.PWM___LS_STRS_W5, "Wand L2-L3", ApplicationId.STDBY_L2_CORR_24V,new HashSet<ushort> { BUS0 + I2C + 21 }, 1000 * 120);

            #endregion
            #region Fingerprint und Terrasse
            AddFingerprint(NODE_SIDEDOOR, ApplicationId.FINGR_L1_UTIL, "Fingerprint Seiteneingang", ApplicationId.POWIT_X1_UTIL_DOOR, ApplicationId.POWIT_X1_UTIL_DOOR, ApplicationId.POWIT_X1_UTIL_DOOR);
            AddPowIt(NODE_SIDEDOOR, ApplicationId.POWIT_X1_UTIL_DOOR, "Seiteneingang Öffner", INTI+0, 1000);

            AddMilightControllerApplication(NODE_TERRASSE, ApplicationId.REMOT_LX_BACK, "Milight Controller");
            AddPWMApplication(NODE_TERRASSE, ApplicationId.PWM___LX_BACK_C1, "Spots Terrassendach", ApplicationId.NO_APPLICATION, new HashSet<ushort>{INTI+4, INTI+5}, T_12h__ms, 512, 65535);
            AddBlindApplication(NODE_TERRASSE, ApplicationId.BLIND_LX_BACK_J2, "Markise Horizontal", INTI+0, INTI+1, RelayInterlockMode.R1_DOWN__R2_UP, 60, 60);
            AddBlindApplication(NODE_TERRASSE, ApplicationId.BLIND_LX_BACK_J3, "Markise Vertikal", INTI+2, INTI+3,  RelayInterlockMode.R1_DOWN__R2_UP, 60, 60);

            #endregion


        }
    }
}

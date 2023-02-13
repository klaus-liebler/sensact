using Klli.Sensact.Model.Common;
namespace Klli.Sensact.Model
{

    public enum ApplicationId
    {
        MASTER,
        //Sensact-Geräte
        SNSCT_L0_TECH_HS_1,
        SNSCT_L0_TECH_HS_2,
        SNSCT_L3_TECH_HS_1,
        SNSCT_L3_TECH_HS_2,

        SNSCT_L1_LVNG_UP,
        SNSCT_L1_KTCH_UP,
        SNSCT_L2_BATH_UP,

        //Keller
        PUSHB_L0_CORR_B11, //STRS.W1. Das ist immer der rechte Taster
        PUSHB_L0_CORR_B21, // das ist der linke Taster vor dem Technikraum
        PUSHB_L0_CORR_B31, //das ist der linke Taster vor dem Saunaraum
        //Der linke Taster vor dem Partyraum wird mit dem rechten parallel geschaltet
        POWIT_L0_CORR_C1,
        SCLIM_L0_CORR_AIR,
        PUSHB_L0_PRTY_B11, //up
        PUSHB_L0_PRTY_B12, //down
        POWIT_L0_PRTY_P1,
        POWIT_L0_PRTY_P2,
        PWM___L0_PRTY_S, //S1 Gartenseite, S4 Straßenseite
        STEMP_L0_PRTY_AIR,
        VALVE_L0_PRTY_V1,
        VALVE_L0_PRTY_V2,
        PUSHB_L0_STO1_B11,
        POWIT_L0_STO1_C1,
        SCLIM_L0_STO1_AIR,
        PUSHB_L0_TECH_B11,
        POWIT_L0_TECH_C1,
        DEVCE_L0_TECH_HEAT,
        DEVCE_L0_TECH_PUMP, //Störsignal Hebepumpe
        PUMP__L0_TECH_PUMP, //Power für Zirkulationspumpe
        POWIT_L0_TECH_P1,
        POWIT_L0_TECH_P2,
        SCLIM_L0_TECH_AIR,
        VALVE_L0_TECH,
        PUSHB_L0_WELL_B11, //für Deckenleuchte
        PUSHB_L0_WELL_B12, //Reserve
        PUSHB_L0_WELL_B21, //Für Dusche
        PUSHB_L0_WELL_B22, //Für WC
        POWIT_L0_WELL_C1,
        POWIT_L0_WELL_P1,
        POWIT_L0_WELL_P2,
        PWM___L0_WELL_S1, //Dusche
        PWM___L0_WELL_S2, //WC
        SCLIM_L0_WELL_AIR,
        VALVE_L0_WELL,
        PUSHB_L0_WORK_B11,
        POWIT_L0_WORK_C1,
        SCLIM_L0_WORK_AIR,
        VALVE_L0_WORK,

        //Erdgeschoss
        PUSHB_L1_BATH_B11, //Licht
        PUSHB_L1_BATH_B12, //hoch
        PUSHB_L1_BATH_B13, //runter
        BLIND_L1_BATH_J1,
        PWM___L1_BATH_S,
        RGBW__L1_BATH_W1, //Dekolicht
        SCLIM_L1_BATH_AIR,
        STEMP_L1_BATH_FLOOR,
        SWIND_L1_BATH_W1,
        VALVE_L1_BATH,

        SOUND_L0_TECH_AUDIO, //auch gleichzeitig DEVCE_L1_CORR_Audio
        PUSHB_L1_CORR_B11, //für Spots
        PUSHB_L1_CORR_B21, //S
        PUSHB_L1_CORR_B22, //W1
        PUSHB_L1_CORR_B23, //Coming Home
        PUSHB_L1_CORR_B24, //leaving home
        PUSHB_L1_CORR_B25, //FRON.W1
        PUSHB_L1_CORR_B26, //LEFT.W1
        POWIT_L1_CORR_P1,
        POWIT_L1_CORR_P2,
        PWM___L1_CORR_S,
        POWIT_L1_CORR_W1, //TODO: Obsolet, löschen, Es wird nur noch PWM benötigt
        PWM___L1_CORR_W1,
        SCLIM_L1_CORR_AIR,
        STEMP_L1_CORR_FLOOR,
        SDOOR_L1_CORR, //door sensor (was auch immer der kann)
        VALVE_L1_CORR,


        PUSHB_L1_KTCH_B11, //C1
        PUSHB_L1_KTCH_B12, //Außenbeleuchtung W1
        ROTAR_L1_KTCH_B21, //Spots 1...4 dimmen
        PUSHB_L1_KTCH_B22, //Spot 5 dimmen
        PUSHB_L1_KTCH_B23, //Deckenleuchte
        PUSHB_L1_KTCH_B24, //Wandleuchten
        PUSHB_L1_KTCH_B25, //Rollo Garten hoch
        PUSHB_L1_KTCH_B26, // ...runter
        PUSHB_L1_KTCH_B27, //Rollo Garage hoch
        PUSHB_L1_KTCH_B28, // ...runter
        PUSHB_L1_KTCH_B31, //Spot S5
        PUSHB_L1_KTCH_B32, //für zukünftige Erweiterung, falls hier ein Doppeltaster hinkommt
        POWIT_L1_KTCH_C1,
        PWM___L1_KTCH_C1,
        BLIND_L1_KTCH_J1,
        BLIND_L1_KTCH_J2,
        BLIND_L1_KTCH_P1,
        BLIND_L1_KTCH_P2,
        PWM___L1_KTCH_S1, //die ersten vier Spots
        PWM___L1_KTCH_S2, //über Spüle
        POWIT_L1_KTCH_W1_Dummy, //TODO obsolet!!
        PWM___L1_KTCH_W1,
        SCLIM_L1_KTCH_AIR,
        STEMP_L1_KTCH_FLOOR,
        SWIND_L1_KTCH_W1,
        SWIND_L1_KTCH_W2,
        VALVE_L1_KTCH,

        PUSHB_L1_LVNG_B11, //LX.BACK.J1 (Markise) hoch
        PUSHB_L1_LVNG_B12, //runter
        PUSHB_L1_LVNG_B13, //LX.BACK.W2
        PUSHB_L1_LVNG_B14, //LX.BACK.W3
        PUSHB_L1_LVNG_B15, //LX.BACK.P3
        PUSHB_L1_LVNG_B16, //L1.LVNG.S* one button dimmer

        PUSHB_L1_LVNG_B21, //L1.LVNG.W4 Wohnwahdleuchten Dim hoch
        PUSHB_L1_LVNG_B22, //L1.LVNG.W4 Wohnwahdleuchten Dim runter
        PUSHB_L1_LVNG_B23, //LX.RGHT.W1 Außenlampe

        PUSHB_L1_LVNG_B31, //J1
        PUSHB_L1_LVNG_B32,
        PUSHB_L1_LVNG_B33, //J2
        PUSHB_L1_LVNG_B34,
        PUSHB_L1_LVNG_B35, //J3
        PUSHB_L1_LVNG_B36,

        ROTAR_L1_LVNG_B41, //Spots
        PUSHB_L1_LVNG_B42, //C1 Esstisch-Deckenlampe
        PUSHB_L1_LVNG_B43, //C2 Kaminregal one Button Dimmer
        PUSHB_L1_LVNG_B44, //W1 Deko-Wandlampen

        PUSHB_L1_LVNG_B51, //L1.LVNG.S* one button dimmer
        PUSHB_L1_LVNG_B52, //L1.KTCH.S*
        PUSHB_L1_LVNG_B53, //L1.KTCH.C1
        

        POWIT_L1_LVNG_C1, //Lampe über Essbereich
        PWM___L1_LVNG_C2, //Lampe im Kaminregal
        BLIND_L1_LVNG_J1,
        BLIND_L1_LVNG_J2,
        BLIND_L1_LVNG_J3,
        POWIT_L1_LVNG_P1,
        POWIT_L1_LVNG_P2,
        POWIT_L1_LVNG_P4,
        POWIT_L1_LVNG_P5,
        PWM___L1_LVNG_S,
        POWIT_L1_LVNG_W1, //TODO Wandleuchten geschaltet TODO obsolet, entfernen!!!
        PWM___L1_LVNG_W1, //Wandleuchten
        RGBW__L1_LVNG_W3, //Effektleuchte (Reserve)
        PWM___L1_LVNG_W4, //Wohnwandleuchte
        SCLIM_L1_LVNG_AIR,
        SCLIM_L1_LVNG_AIR2, //zweiter Messpunkt im sensactUP
        STEMP_L1_LVNG_FLOOR,
        SWIND_L1_LVNG_W1,
        SWIND_L1_LVNG_W2,
        SWIND_L1_LVNG_W3,
        VALVE_L1_LVNG,


        PUSHB_L1_UTIL_B11, //L1.KTCH.S*
        PUSHB_L1_UTIL_B12, //Hoch
        PUSHB_L1_UTIL_B13, //runter
        PUSHB_L1_UTIL_B21, //L1.UTIL.C1
        PUSHB_L1_UTIL_B22, //LX.LEFT.W1
        PUSHB_L1_UTIL_B23, //Garagentor hochrunter
        PUSHB_L1_UTIL_B24, //Garagentor hochrunter
        POWIT_L1_UTIL_C1,
        BLIND_L1_UTIL_J1,
        SCLIM_L1_UTIL_AIR,
        STEMP_L1_UTIL_FLOOR,
        SWIND_L1_UTIL_W1,
        SDOOR_L1_UTIL_D1,
        VALVE_L1_UTIL,

        PUSHB_L1_WORK_B11, //C1
        PUSHB_L1_WORK_B12, //C2
        PUSHB_L1_WORK_B13, //J1
        PUSHB_L1_WORK_B14,
        PUSHB_L1_WORK_B15,  //J2
        PUSHB_L1_WORK_B16,
        PUSHB_L1_WORK_B17, //LX.RGHT.W1, später Carport?
        POWIT_L1_WORK_C1,
        POWIT_L1_WORK_C2,
        BLIND_L1_WORK_J1,
        BLIND_L1_WORK_J2,
        POWIT_L1_WORK_P1,
        POWIT_L1_WORK_P2,
        SCLIM_L1_WORK_AIR,
        STEMP_L1_WORK_FLOOR,
        SWIND_L1_WORK_W1,
        SWIND_L1_WORK_W2,
        VALVE_L1_WORK,

        BLIND_LX_BACK_J1,
        POWIT_LX_BACK_P1, //Außensteckdosen
        POWIT_LX_BACK_P2, //drei Phasen des Gartenkabels
        POWIT_LX_BACK_P3,
        POWIT_LX_BACK_P4,
        POWIT_LX_BACK_W1, //Garagenwand
        POWIT_LX_BACK_W2, //Flachdachausbau
        POWIT_LX_BACK_W3, //Wonhzimmerterrasse
        POWIT_LX_CARP_P1, //Carport
        POWIT_LX_CARP_P2,
        PUSHB_LX_FRON_B1, //Klingelknopf
        LIBAR_LX_FRON_B2, //Bewegungsmelder
        POWIT_LX_FRON_W1, //Licht für Haupteingang
        PUSHB_LX_GARA_B11, //ein Taster am TÜReingang
        PUSHB_LX_GARA_B21, //ein Taster am TOReingang, gemeinsames Kabel mit B11
        POWIT_LX_GARA_C1,
        POWIT_LX_GARA_P1,
        POWIT_LX_GARA_P2,
        BLIND_LX_GARA_J1, //Garagentor
        POWIT_LX_LEFT_W1,
        POWIT_LX_RGHT_W1,

        ROTAR_L2_BATH_B11,
        ROTAR_L2_BATH_B12,
        PUSHB_L2_BATH_B13, //hoch
        PUSHB_L2_BATH_B14,//runter
        BLIND_L2_BATH_J1,
        PWM___L2_BATH_S,
        POWIT_L2_BATH_P1,
        POWIT_L2_BATH_P2,
        RGBW__L2_BATH_W, //Umfasst die komplette Bad-Dekobeleuchtung, eigene Applikation schreiben
        //Bodenbeleuchtung, Seifenkastenbeleuchtung, T-Deckenbeleuchtung, Sternenhimmel
        SCLIM_L2_BATH_AIR,
        STEMP_L2_BATH_FLOOR,
        SWIND_L2_BATH,
        VALVE_L2_BATH,

        PUSHB_L2_BEDR_B11, //Tag-Nacht, zunächst toggle P3
        PUSHB_L2_BEDR_B12, //S*
        PUSHB_L2_BEDR_B13, //P1
        PUSHB_L2_BEDR_B14, //J1
        PUSHB_L2_BEDR_B15, 
        PUSHB_L2_BEDR_B21, //Tag-Nacht
        PUSHB_L2_BEDR_B22, //S*
        PUSHB_L2_BEDR_B23, //P2
        PUSHB_L2_BEDR_B24, //J1
        PUSHB_L2_BEDR_B25,
        PUSHB_L2_BEDR_B31, //S* hoch
        PUSHB_L2_BEDR_B32, //S* runter
        PUSHB_L2_BEDR_B33, //P1
        PUSHB_L2_BEDR_B34, //P2
        PUSHB_L2_BEDR_B35, //J1 hoch
        PUSHB_L2_BEDR_B36, //J1 runter
        BLIND_L2_BEDR_J1,
        POWIT_L2_BEDR_P1, //Nachttischlampe linkes Bett
        POWIT_L2_BEDR_P2, //Nachttischlampe rechtes Bett
        POWIT_L2_BEDR_P3, //Netzfreischaltung
        PWM___L2_BEDR_S,
        //PWM___L2_BEDR_S1, //näher an Dachschräge
        //PWM___L2_BEDR_S2, //näher an Schrank
        SCLIM_L2_BEDR_AIR,
        STEMP_L2_BEDR_FLOOR,
        SWIND_L2_BEDR,
        VALVE_L2_BEDR,


        PUSHB_L2_CORR_B11, //L2_CORR_S*, vermutlich mit 21 und 31 parallel geschaltet 
        PUSHB_L2_CORR_B21,
        PUSHB_L2_CORR_B31,
        PUSHB_L2_CORR_B41, //L2_CORR_S*
        PUSHB_L2_CORR_B42, //hoch
        PUSHB_L2_CORR_B43, //runter
        BLIND_L2_CORR_J1,
        POWIT_L2_CORR_P1,
        POWIT_L2_CORR_P2,
        PWM___L2_CORR_S,
        SCLIM_L2_CORR_AIR,
        STEMP_L2_CORR_FLOOR,
        SWIND_L2_CORR,
        VALVE_L2_CORR,

        PUSHB_L2_KID1_B11, //C1
        PUSHB_L2_KID1_B12, //J1
        PUSHB_L2_KID1_B13,
        PUSHB_L2_KID1_B21, //C1
        PUSHB_L2_KID1_B22, //C2

        PWM___L2_KID1_C1,
        POWIT_L2_KID1_C1,
        POWIT_L2_KID1_C2, //Balkon
        BLIND_L2_KID1_J1,
        POWIT_L2_KID1_P1,
        POWIT_L2_KID1_P2,
        SCLIM_L2_KID1_AIR,
        STEMP_L2_KID1_FLOOR,
        SWIND_L2_KID1,
        VALVE_L2_KID1,

        PUSHB_L2_KID2_B11, //C1
        PUSHB_L2_KID2_B12, //J1
        PUSHB_L2_KID2_B13,
        PWM___L2_KID2_C1,
        POWIT_L2_KID2_C1,
        BLIND_L2_KID2_J1,
        POWIT_L2_KID2_P1,
        POWIT_L2_KID2_P2,
        SCLIM_L2_KID2_AIR,
        STEMP_L2_KID2_FLOOR,
        SWIND_L2_KID2,
        VALVE_L2_KID2,

        PUSHB_L2_WORK_B11, //C1
        PUSHB_L2_WORK_B12, //J1
        PUSHB_L2_WORK_B13,
        BLIND_L2_WORK_J1,
        PWM___L2_WORK_S,
        SCLIM_L2_WORK_AIR,
        STEMP_L2_WORK_FLOOR,
        SWIND_L2_WORK,
        VALVE_L2_WORK,


        PUSHB_L3_BEDR_B11, //C1
        PUSHB_L3_BEDR_B12, //CORR_S + STRS
        PUSHB_L3_BEDR_B13, //J1
        PUSHB_L3_BEDR_B14,
        POWIT_L3_BEDR_C1,
        BLIND_L3_BEDR_J1,
        SCLIM_L3_BEDR_AIR,
        POWIT_L3_BEDR_P1,
        POWIT_L3_BEDR_P2,
        SWIND_L3_BEDR,
        VALVE_L3_BEDR,

        PUSHB_L3_CORR_Dummy1, //TODO Deleteme
        PUSHB_L3_CORR_Dummy2, //TODO Deleteme
        PUSHB_L3_CORR_Dummy3, //TODO Deleteme

        PWM___L3_CORR_S, //es ist kein Schalter im Korridor oben. Die Lichter werden von den Zimmern aus geschaltet

        DEVCE_L3_TECH_VENT,
        DEVCE_L3_TECH_SATI, //Satellitenreceiver
        DEVCE_L3_TECH_DUMMY, //TODO Deleteme

        PUSHB_L3_WORK_B11, //S
        PUSHB_L3_WORK_B12, //C1
        PUSHB_L3_WORK_B13,//J1
        PUSHB_L3_WORK_B14,//
        PUSHB_L3_WORK_B15,//CORR_S + STRS
        PWM___L3_WORK_C1,
        BLIND_L3_WORK_J1,
        BLIND_L3_WORK_J2,
        POWIT_L3_WORK_P1,
        POWIT_L3_WORK_P2,
        PWM___L3_WORK_S,
        SCLIM_L3_WORK_AIR, //Klimaanlage; TODO: Auch für Kinderzimmer
        SWIND_L3_WORK_W1,
        SWIND_L3_WORK_W2,
        VALVE_L3_WORK,

        //Treppenhaus (von unten nach oben)
        //Lichtschranke im Keller
        LIBAR_LS_STRS_B11, //W2

        //Schaltergruppe im L1
        PUSHB_LS_STRS_B21, //W3
        PUSHB_LS_STRS_B22, //S*
        PUSHB_LS_STRS_B23, //W1

        LIBAR_LS_STRS_B31, //W2 (Lichtschranke in Richtung L0)
        LIBAR_LS_STRS_B32, //W4 (Lichtschranke in Richtung L2)

        //Schaltergruppe im L2
        PUSHB_LS_STRS_B41, //W5
        PUSHB_LS_STRS_B42, //S*
        PUSHB_LS_STRS_B43, //W3

        LIBAR_LS_STRS_B51, //W4 (Lichtschranke in Richtung L2)
        LIBAR_LS_STRS_B52, //W6 (Lichtschranke in Richtung L3)

        LIBAR_LS_STRS_B71, //W6 (Lichtschranke in Richtung L2)

        PWM___LS_STRS_W1, //L0-L1-Treppe Wand
        PWM___LS_STRS_W2, //L0-L1-Treppe Trittstufen
        PWM___LS_STRS_W3, //L1-L2-Treppe Wand
        PWM___LS_STRS_W4, //L1-L2-Trepp Trittstufen
        PWM___LS_STRS_W5, //L2-L3-Treppe Wand
        PWM___LS_STRS_W6, //L2-L3-Treppe Trittstufen




        STEMP_LX_ROOF, //Außentemperatur (ggf. Sensor der Heizung mitnutzen?)
        SBRGH_LX_ROOF, //Helligkeitssensor
        SPRES_LX_ROOF, //Luftdrucksensor
        SWIND_LX_ROOF, //Windrichtung und -Stärke
        SRAIN_LX_ROOF, //Regenmesser
        SMOKE_L0_CORR,
        SMOKE_L0_LVNG,
        SMOKE_L1_CORR,
        SMOKE_L1_BEDR,
        SMOKE_L1_KID1,
        SMOKE_L1_KID2,
        SMOKE_L3_BEDR,
        SMOKE_L3_CORR,

        STDBY_L0_TECH_48V,
        STDBY_L2_CORR_24V,
        STDBY_L3_TECH_48V,
        STDBY_L3_ROOF_48V, //Sitzt im Drempel über dem Bad
        STDBY_L3_WORK_C1, //ist die Stromversorgung fur die Deckenlampe im Arbeitszimmer
        NO_APPLICATION=Sensact.Model.Common.Model.NO_APPLICATION_ID
    }
}

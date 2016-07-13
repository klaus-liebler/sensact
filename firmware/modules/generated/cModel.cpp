/*
@file		hc_model.cpp
@author		autogen
@version	1.0
@date		07/08/2016 23:20:54 (Rendered)
@brief		The Home Model
@details	All params and details about the home model
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */


#include "common.h"
#include "cModel.h"
#include "cBlind.h"
#include "cRotar.h"
#include "cPoweritem.h"
#include "cPushbutton.h"
#include "cPwm.h"
#include "cStandbycontroller.h"
#include "cBrightnessSensor.h"
#include "cBell.h"
#include "cRgbw.h"
#include <chrono>
#include "date.h"
#include "bh1750.h"
#include "cRCEvent.h"

using namespace std;

namespace sensact {

const char * const MODEL::ApplicationNames[]={
		    "MASTER",
    "PUSHB_L0_CORR_B11",
    "POWIT_L0_CORR_C1",
    "PUSHB_L0_PRTY_B11",
    "PUSHB_L0_PRTY_B12",
    "PWM___L0_PRTY_S",
    "PUSHB_L0_STO1_B11",
    "POWIT_L0_STO1_C1",
    "PUSHB_L0_TECH_B11",
    "POWIT_L0_TECH_C1",
    "PUSHB_L0_WELL_B11",
    "PUSHB_L0_WELL_B21",
    "PUSHB_L0_WELL_B22",
    "POWIT_L0_WELL_C1",
    "PWM___L0_WELL_S1",
    "PWM___L0_WELL_S2",
    "PUSHB_L0_WORK_B11",
    "POWIT_L0_WORK_C1",
    "PUSHB_L1_BATH_B11",
    "PWM___L1_BATH_S",
    "PUSHB_L1_BATH_B12",
    "PUSHB_L1_BATH_B13",
    "BLIND_L1_BATH_J1",
    "RGBW__L1_BATH_W1",
    "DEVCE_L1_CORR_Audio",
    "PUSHB_L1_CORR_B11",
    "PUSHB_L1_CORR_B21",
    "PUSHB_L1_CORR_B22",
    "PUSHB_L1_CORR_B23",
    "PUSHB_L1_CORR_B24",
    "PUSHB_L1_CORR_B25",
    "PUSHB_L1_CORR_B26",
    "PWM___L1_CORR_S",
    "POWIT_L1_CORR_W1",
    "PUSHB_L1_KTCH_B11",
    "PUSHB_L1_KTCH_B12",
    "PUSHB_L1_KTCH_B31",
    "POWIT_L1_KTCH_C1",
    "POWIT_L1_KTCH_W1",
    "BLIND_L1_KTCH_J1",
    "BLIND_L1_KTCH_J2",
    "PUSHB_L1_LVNG_B21",
    "PUSHB_L1_LVNG_B22",
    "PUSHB_L1_LVNG_B23",
    "BLIND_L1_LVNG_J1",
    "BLIND_L1_LVNG_J2",
    "BLIND_L1_LVNG_J3",
    "POWIT_L1_LVNG_C1",
    "POWIT_L1_LVNG_W1",
    "POWIT_L1_LVNG_W4",
    "PUSHB_L1_WORK_B11",
    "PUSHB_L1_WORK_B12",
    "PUSHB_L1_UTIL_B11",
    "PUSHB_L1_UTIL_B12",
    "PUSHB_L1_UTIL_B13",
    "PUSHB_L1_UTIL_B21",
    "PUSHB_L1_UTIL_B22",
    "PUSHB_L1_UTIL_B23",
    "PUSHB_L1_UTIL_B24",
    "POWIT_L1_UTIL_C1",
    "BLIND_L1_UTIL_J1",
    "PUSHB_L1_WORK_B13",
    "PUSHB_L1_WORK_B14",
    "PUSHB_L1_WORK_B15",
    "PUSHB_L1_WORK_B16",
    "PUSHB_L1_WORK_B17",
    "POWIT_L1_WORK_C1",
    "POWIT_L1_WORK_C2",
    "BLIND_L1_WORK_J1",
    "BLIND_L1_WORK_J2",
    "POWIT_L1_CORR_P1",
    "POWIT_L1_LVNG_P4",
    "POWIT_LX_BACK_P1",
    "POWIT_LX_BACK_W1",
    "POWIT_LX_BACK_W2",
    "POWIT_LX_BACK_W3",
    "POWIT_LX_FRON_W1",
    "POWIT_LX_LEFT_W1",
    "POWIT_LX_RGHT_W1",
    "POWIT_LX_GARA_C1",
    "POWIT_LX_GARA_P1",
    "STDBY_L0_TECH_24V",
    "STDBY_L0_TECH_48V",
    "STDBY_L2_CORR_24V",
    "PUSHB_L2_BATH_B13",
    "PUSHB_L2_BATH_B14",
    "BLIND_L2_BATH_J1",
    "PUSHB_L2_BEDR_B11",
    "PUSHB_L2_BEDR_B12",
    "PUSHB_L2_BEDR_B13",
    "PUSHB_L2_BEDR_B14",
    "PUSHB_L2_BEDR_B15",
    "PUSHB_L2_BEDR_B21",
    "PUSHB_L2_BEDR_B22",
    "PUSHB_L2_BEDR_B23",
    "PUSHB_L2_BEDR_B24",
    "PUSHB_L2_BEDR_B25",
    "PUSHB_L2_BEDR_B31",
    "PUSHB_L2_BEDR_B32",
    "PUSHB_L2_BEDR_B33",
    "PUSHB_L2_BEDR_B34",
    "PUSHB_L2_BEDR_B35",
    "PUSHB_L2_BEDR_B36",
    "BLIND_L2_BEDR_J1",
    "PWM___L2_BEDR_S",
    "POWIT_L2_BEDR_P1",
    "POWIT_L2_BEDR_P2",
    "POWIT_L2_BEDR_P3",
    "PUSHB_L2_CORR_B11",
    "PUSHB_L2_CORR_B41",
    "PUSHB_L2_CORR_B42",
    "PUSHB_L2_CORR_B43",
    "BLIND_L2_CORR_J1",
    "PUSHB_L2_KID1_B11",
    "PUSHB_L2_KID1_B12",
    "PUSHB_L2_KID1_B13",
    "PUSHB_L2_KID1_B21",
    "POWIT_L2_KID1_C1",
    "POWIT_L2_KID1_C2",
    "BLIND_L2_KID1_J1",
    "PUSHB_L2_KID2_B11",
    "PUSHB_L2_KID2_B12",
    "PUSHB_L2_KID2_B13",
    "POWIT_L2_KID2_C1",
    "BLIND_L2_KID2_J1",
    "PUSHB_L2_WORK_B11",
    "PUSHB_L2_WORK_B12",
    "PUSHB_L2_WORK_B13",
    "BLIND_L2_WORK_J1",
    "PWM___L2_WORK_S",
    "PUSHB_L3_BEDR_B11",
    "PUSHB_L3_BEDR_B12",
    "PUSHB_L3_BEDR_B13",
    "POWIT_L3_BEDR_C1",
    "BLIND_L3_BEDR_J1",
    "PUSHB_L3_TECH_B11",
    "POWIT_L3_TECH_C1",
    "ROTAR_L3_WORK_B11",
    "PUSHB_L3_WORK_B12",
    "PUSHB_L3_WORK_B13",
    "PUSHB_L3_WORK_B14",
    "PUSHB_L3_WORK_B15",
    "BLIND_L3_WORK_J1",
    "BLIND_L3_WORK_J2",
    "PWM___L3_WORK_S",
    "PUSHB_LS_STRS_B11",
    "PUSHB_LS_STRS_B21",
    "PUSHB_LS_STRS_B22",
    "PUSHB_LS_STRS_B23",
    "PUSHB_LS_STRS_B31",
    "PUSHB_LS_STRS_B32",
    "PUSHB_LS_STRS_B41",
    "PUSHB_LS_STRS_B42",
    "PUSHB_LS_STRS_B43",
    "PUSHB_LS_STRS_B51",
    "PUSHB_LS_STRS_B52",
    "PUSHB_LS_STRS_B61",
    "PUSHB_LS_STRS_B62",
    "PUSHB_LS_STRS_B71",
    "PWM___LS_STRS_S",
    "POWIT_LS_STRS_W1",
    "POWIT_LS_STRS_W3",
    "POWIT_LS_STRS_W5",
    "ROTAR_L1_KTCH_B21",
    "ROTAR_L1_KTCH_B22",
    "PUSHB_L1_KTCH_B23",
    "PUSHB_L1_KTCH_B24",
    "PUSHB_L1_KTCH_B25",
    "PUSHB_L1_KTCH_B26",
    "PUSHB_L1_KTCH_B27",
    "PUSHB_L1_KTCH_B28",
    "PWM___L1_KTCH_S1",
    "PWM___L1_KTCH_S2",
    "PUSHB_L1_LVNG_B31",
    "PUSHB_L1_LVNG_B32",
    "PUSHB_L1_LVNG_B33",
    "PUSHB_L1_LVNG_B34",
    "PUSHB_L1_LVNG_B35",
    "PUSHB_L1_LVNG_B36",
    "ROTAR_L1_LVNG_B41",
    "ROTAR_L1_LVNG_B42",
    "PUSHB_L1_LVNG_B43",
    "PUSHB_L1_LVNG_B44",
    "PWM___L1_LVNG_S",
    "PWM___LS_STRS_W2",
    "PWM___LS_STRS_W4",
    "PWM___LS_STRS_W6",
    "ROTAR_L2_BATH_B11",
    "ROTAR_L2_BATH_B12",
    "PWM___L2_BATH_S",
    "RGBW__L2_BATH_W",
    "PWM___L2_CORR_S",

	};


#ifdef NODE_SNSAC_L0_TECH_HS07_1

eNodeID NODE = eNodeID::SNSAC_L0_TECH_HS07_1;

const char MODEL::ModelString[] ="NodeId SNSAC_L0_TECH_HS07_1 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// PushButtonX PUSHB_L0_CORR_B11
const Command PUSHB_L0_CORR_B11_OnPressed[2]={{eApplicationID::POWIT_L0_CORR_C1, eCommandType::TOGGLE},{eApplicationID::PWM___LS_STRS_W2, eCommandType::TOGGLE},};
const Command *const PUSHB_L0_CORR_B11_OnReleased=0;
const Command *const PUSHB_L0_CORR_B11_OnReleasedShort=0;
const Command *const PUSHB_L0_CORR_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L0_CORR_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_CORR_B11_LocalEvents=0;
eEventType *PUSHB_L0_CORR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_CORR_B11("PUSHB_L0_CORR_B11", eApplicationID::PUSHB_L0_CORR_B11, eInput::I01, PUSHB_L0_CORR_B11_LocalEvents, 0, PUSHB_L0_CORR_B11_BusEvents, 0, PUSHB_L0_CORR_B11_OnPressed, 2, PUSHB_L0_CORR_B11_OnReleased, 0, PUSHB_L0_CORR_B11_OnReleasedShort, 0, PUSHB_L0_CORR_B11_OnPressedShortAndHold, 0, PUSHB_L0_CORR_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_CORR_C1
sensact::cPoweritem POWIT_L0_CORR_C1("POWIT_L0_CORR_C1", eApplicationID::POWIT_L0_CORR_C1, ePoweredOutput::O09, 0);

// PushButtonX PUSHB_L0_PRTY_B11
const Command *const PUSHB_L0_PRTY_B11_OnPressed=0;
const Command *const PUSHB_L0_PRTY_B11_OnReleased=0;
const Command PUSHB_L0_PRTY_B11_OnReleasedShort[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::TOGGLE},};
const Command PUSHB_L0_PRTY_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::UP},};
const Command PUSHB_L0_PRTY_B11_OnReleasedLong[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::STOP},};
eEventType *PUSHB_L0_PRTY_B11_LocalEvents=0;
eEventType *PUSHB_L0_PRTY_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_PRTY_B11("PUSHB_L0_PRTY_B11", eApplicationID::PUSHB_L0_PRTY_B11, eInput::I02, PUSHB_L0_PRTY_B11_LocalEvents, 0, PUSHB_L0_PRTY_B11_BusEvents, 0, PUSHB_L0_PRTY_B11_OnPressed, 0, PUSHB_L0_PRTY_B11_OnReleased, 0, PUSHB_L0_PRTY_B11_OnReleasedShort, 1, PUSHB_L0_PRTY_B11_OnPressedShortAndHold, 1, PUSHB_L0_PRTY_B11_OnReleasedLong, 1);

// PushButtonX PUSHB_L0_PRTY_B12
const Command *const PUSHB_L0_PRTY_B12_OnPressed=0;
const Command *const PUSHB_L0_PRTY_B12_OnReleased=0;
const Command PUSHB_L0_PRTY_B12_OnReleasedShort[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::TOGGLE},};
const Command PUSHB_L0_PRTY_B12_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::DOWN},};
const Command PUSHB_L0_PRTY_B12_OnReleasedLong[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::STOP},};
eEventType *PUSHB_L0_PRTY_B12_LocalEvents=0;
eEventType *PUSHB_L0_PRTY_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_PRTY_B12("PUSHB_L0_PRTY_B12", eApplicationID::PUSHB_L0_PRTY_B12, eInput::I03, PUSHB_L0_PRTY_B12_LocalEvents, 0, PUSHB_L0_PRTY_B12_BusEvents, 0, PUSHB_L0_PRTY_B12_OnPressed, 0, PUSHB_L0_PRTY_B12_OnReleased, 0, PUSHB_L0_PRTY_B12_OnReleasedShort, 1, PUSHB_L0_PRTY_B12_OnPressedShortAndHold, 1, PUSHB_L0_PRTY_B12_OnReleasedLong, 1);

// PWM PWM___L0_PRTY_S (Dimmer )
ePWMOutput PWM___L0_PRTY_S_output[4]={ePWMOutput::P01,ePWMOutput::P02,ePWMOutput::P03,ePWMOutput::P04,};
sensact::cPWM PWM___L0_PRTY_S("PWM___L0_PRTY_S", eApplicationID::PWM___L0_PRTY_S, PWM___L0_PRTY_S_output, 4, 1, 255, false, eApplicationID::STDBY_L0_TECH_48V);

// PushButtonX PUSHB_L0_STO1_B11
const Command PUSHB_L0_STO1_B11_OnPressed[1]={{eApplicationID::POWIT_L0_STO1_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L0_STO1_B11_OnReleased=0;
const Command *const PUSHB_L0_STO1_B11_OnReleasedShort=0;
const Command *const PUSHB_L0_STO1_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L0_STO1_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_STO1_B11_LocalEvents=0;
eEventType *PUSHB_L0_STO1_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_STO1_B11("PUSHB_L0_STO1_B11", eApplicationID::PUSHB_L0_STO1_B11, eInput::I04, PUSHB_L0_STO1_B11_LocalEvents, 0, PUSHB_L0_STO1_B11_BusEvents, 0, PUSHB_L0_STO1_B11_OnPressed, 1, PUSHB_L0_STO1_B11_OnReleased, 0, PUSHB_L0_STO1_B11_OnReleasedShort, 0, PUSHB_L0_STO1_B11_OnPressedShortAndHold, 0, PUSHB_L0_STO1_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_STO1_C1
sensact::cPoweritem POWIT_L0_STO1_C1("POWIT_L0_STO1_C1", eApplicationID::POWIT_L0_STO1_C1, ePoweredOutput::O10, 0);

// PushButtonX PUSHB_L0_TECH_B11
const Command PUSHB_L0_TECH_B11_OnPressed[1]={{eApplicationID::POWIT_L0_TECH_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L0_TECH_B11_OnReleased=0;
const Command *const PUSHB_L0_TECH_B11_OnReleasedShort=0;
const Command *const PUSHB_L0_TECH_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L0_TECH_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_TECH_B11_LocalEvents=0;
eEventType *PUSHB_L0_TECH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_TECH_B11("PUSHB_L0_TECH_B11", eApplicationID::PUSHB_L0_TECH_B11, eInput::I05, PUSHB_L0_TECH_B11_LocalEvents, 0, PUSHB_L0_TECH_B11_BusEvents, 0, PUSHB_L0_TECH_B11_OnPressed, 1, PUSHB_L0_TECH_B11_OnReleased, 0, PUSHB_L0_TECH_B11_OnReleasedShort, 0, PUSHB_L0_TECH_B11_OnPressedShortAndHold, 0, PUSHB_L0_TECH_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_TECH_C1
sensact::cPoweritem POWIT_L0_TECH_C1("POWIT_L0_TECH_C1", eApplicationID::POWIT_L0_TECH_C1, ePoweredOutput::O11, 0);

// PushButtonX PUSHB_L0_WELL_B11
const Command PUSHB_L0_WELL_B11_OnPressed[1]={{eApplicationID::POWIT_L0_WELL_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L0_WELL_B11_OnReleased=0;
const Command *const PUSHB_L0_WELL_B11_OnReleasedShort=0;
const Command *const PUSHB_L0_WELL_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L0_WELL_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_WELL_B11_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B11("PUSHB_L0_WELL_B11", eApplicationID::PUSHB_L0_WELL_B11, eInput::I06, PUSHB_L0_WELL_B11_LocalEvents, 0, PUSHB_L0_WELL_B11_BusEvents, 0, PUSHB_L0_WELL_B11_OnPressed, 1, PUSHB_L0_WELL_B11_OnReleased, 0, PUSHB_L0_WELL_B11_OnReleasedShort, 0, PUSHB_L0_WELL_B11_OnPressedShortAndHold, 0, PUSHB_L0_WELL_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L0_WELL_B21
const Command *const PUSHB_L0_WELL_B21_OnPressed=0;
const Command *const PUSHB_L0_WELL_B21_OnReleased=0;
const Command PUSHB_L0_WELL_B21_OnReleasedShort[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::TOGGLE},};
const Command PUSHB_L0_WELL_B21_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::START},};
const Command PUSHB_L0_WELL_B21_OnReleasedLong[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::STOP},};
eEventType *PUSHB_L0_WELL_B21_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B21("PUSHB_L0_WELL_B21", eApplicationID::PUSHB_L0_WELL_B21, eInput::I07, PUSHB_L0_WELL_B21_LocalEvents, 0, PUSHB_L0_WELL_B21_BusEvents, 0, PUSHB_L0_WELL_B21_OnPressed, 0, PUSHB_L0_WELL_B21_OnReleased, 0, PUSHB_L0_WELL_B21_OnReleasedShort, 1, PUSHB_L0_WELL_B21_OnPressedShortAndHold, 1, PUSHB_L0_WELL_B21_OnReleasedLong, 1);

// PushButtonX PUSHB_L0_WELL_B22
const Command *const PUSHB_L0_WELL_B22_OnPressed=0;
const Command *const PUSHB_L0_WELL_B22_OnReleased=0;
const Command PUSHB_L0_WELL_B22_OnReleasedShort[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::TOGGLE},};
const Command PUSHB_L0_WELL_B22_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::START},};
const Command PUSHB_L0_WELL_B22_OnReleasedLong[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::STOP},};
eEventType *PUSHB_L0_WELL_B22_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B22("PUSHB_L0_WELL_B22", eApplicationID::PUSHB_L0_WELL_B22, eInput::I08, PUSHB_L0_WELL_B22_LocalEvents, 0, PUSHB_L0_WELL_B22_BusEvents, 0, PUSHB_L0_WELL_B22_OnPressed, 0, PUSHB_L0_WELL_B22_OnReleased, 0, PUSHB_L0_WELL_B22_OnReleasedShort, 1, PUSHB_L0_WELL_B22_OnPressedShortAndHold, 1, PUSHB_L0_WELL_B22_OnReleasedLong, 1);

// POWIT POWIT_L0_WELL_C1
sensact::cPoweritem POWIT_L0_WELL_C1("POWIT_L0_WELL_C1", eApplicationID::POWIT_L0_WELL_C1, ePoweredOutput::O12, 0);

// PWM PWM___L0_WELL_S1 (Dimmer )
ePWMOutput PWM___L0_WELL_S1_output[1]={ePWMOutput::P05,};
sensact::cPWM PWM___L0_WELL_S1("PWM___L0_WELL_S1", eApplicationID::PWM___L0_WELL_S1, PWM___L0_WELL_S1_output, 1, 1, 255, false, eApplicationID::STDBY_L0_TECH_24V);

// PWM PWM___L0_WELL_S2 (Dimmer )
ePWMOutput PWM___L0_WELL_S2_output[1]={ePWMOutput::P06,};
sensact::cPWM PWM___L0_WELL_S2("PWM___L0_WELL_S2", eApplicationID::PWM___L0_WELL_S2, PWM___L0_WELL_S2_output, 1, 1, 255, false, eApplicationID::STDBY_L0_TECH_24V);

// PushButtonX PUSHB_L0_WORK_B11
const Command PUSHB_L0_WORK_B11_OnPressed[1]={{eApplicationID::POWIT_L0_WORK_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L0_WORK_B11_OnReleased=0;
const Command *const PUSHB_L0_WORK_B11_OnReleasedShort=0;
const Command *const PUSHB_L0_WORK_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L0_WORK_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_WORK_B11_LocalEvents=0;
eEventType *PUSHB_L0_WORK_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WORK_B11("PUSHB_L0_WORK_B11", eApplicationID::PUSHB_L0_WORK_B11, eInput::I09, PUSHB_L0_WORK_B11_LocalEvents, 0, PUSHB_L0_WORK_B11_BusEvents, 0, PUSHB_L0_WORK_B11_OnPressed, 1, PUSHB_L0_WORK_B11_OnReleased, 0, PUSHB_L0_WORK_B11_OnReleasedShort, 0, PUSHB_L0_WORK_B11_OnPressedShortAndHold, 0, PUSHB_L0_WORK_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_WORK_C1
sensact::cPoweritem POWIT_L0_WORK_C1("POWIT_L0_WORK_C1", eApplicationID::POWIT_L0_WORK_C1, ePoweredOutput::O13, 0);

// PushButtonX PUSHB_L1_BATH_B11
const Command *const PUSHB_L1_BATH_B11_OnPressed=0;
const Command PUSHB_L1_BATH_B11_OnReleased[1]={{eApplicationID::RGBW__L1_BATH_W1, eCommandType::TOGGLE},};
const Command PUSHB_L1_BATH_B11_OnReleasedShort[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::TOGGLE},};
const Command PUSHB_L1_BATH_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::START},};
const Command PUSHB_L1_BATH_B11_OnReleasedLong[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B11_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B11("PUSHB_L1_BATH_B11", eApplicationID::PUSHB_L1_BATH_B11, eInput::I10, PUSHB_L1_BATH_B11_LocalEvents, 0, PUSHB_L1_BATH_B11_BusEvents, 0, PUSHB_L1_BATH_B11_OnPressed, 0, PUSHB_L1_BATH_B11_OnReleased, 1, PUSHB_L1_BATH_B11_OnReleasedShort, 1, PUSHB_L1_BATH_B11_OnPressedShortAndHold, 1, PUSHB_L1_BATH_B11_OnReleasedLong, 1);

// PWM PWM___L1_BATH_S (Dimmer )
ePWMOutput PWM___L1_BATH_S_output[1]={ePWMOutput::P07,};
sensact::cPWM PWM___L1_BATH_S("PWM___L1_BATH_S", eApplicationID::PWM___L1_BATH_S, PWM___L1_BATH_S_output, 1, 1, 255, false, eApplicationID::STDBY_L0_TECH_48V);

// PushButtonX PUSHB_L1_BATH_B12
const Command PUSHB_L1_BATH_B12_OnPressed[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::UP},};
const Command *const PUSHB_L1_BATH_B12_OnReleased=0;
const Command *const PUSHB_L1_BATH_B12_OnReleasedShort=0;
const Command *const PUSHB_L1_BATH_B12_OnPressedShortAndHold=0;
const Command PUSHB_L1_BATH_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B12_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B12("PUSHB_L1_BATH_B12", eApplicationID::PUSHB_L1_BATH_B12, eInput::I11, PUSHB_L1_BATH_B12_LocalEvents, 0, PUSHB_L1_BATH_B12_BusEvents, 0, PUSHB_L1_BATH_B12_OnPressed, 1, PUSHB_L1_BATH_B12_OnReleased, 0, PUSHB_L1_BATH_B12_OnReleasedShort, 0, PUSHB_L1_BATH_B12_OnPressedShortAndHold, 0, PUSHB_L1_BATH_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_BATH_B13
const Command PUSHB_L1_BATH_B13_OnPressed[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::DOWN},};
const Command *const PUSHB_L1_BATH_B13_OnReleased=0;
const Command *const PUSHB_L1_BATH_B13_OnReleasedShort=0;
const Command *const PUSHB_L1_BATH_B13_OnPressedShortAndHold=0;
const Command PUSHB_L1_BATH_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B13_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B13("PUSHB_L1_BATH_B13", eApplicationID::PUSHB_L1_BATH_B13, eInput::I12, PUSHB_L1_BATH_B13_LocalEvents, 0, PUSHB_L1_BATH_B13_BusEvents, 0, PUSHB_L1_BATH_B13_OnPressed, 1, PUSHB_L1_BATH_B13_OnReleased, 0, PUSHB_L1_BATH_B13_OnReleasedShort, 0, PUSHB_L1_BATH_B13_OnPressedShortAndHold, 0, PUSHB_L1_BATH_B13_OnReleasedLong, 1);

// Blind BLIND_L1_BATH_J1
cBlind BLIND_L1_BATH_J1("BLIND_L1_BATH_J1", eApplicationID::BLIND_L1_BATH_J1, ePoweredOutput::O14, ePoweredOutput::O15, eRelayMode::INTERLOCKED, 40);

// RGBW RGBW__L1_BATH_W1 (Full Color Light)
sensact::cRgbw RGBW__L1_BATH_W1("RGBW__L1_BATH_W1", eApplicationID::RGBW__L1_BATH_W1, ePWMOutput::OP01, ePWMOutput::OP03, ePWMOutput::OP02, ePWMOutput::NONE, true, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::STDBY_L0_TECH_24V);

// DOORBELL DEVCE_L1_CORR_Audio
sensact::cBell DEVCE_L1_CORR_Audio("DEVCE_L1_CORR_Audio", eApplicationID::DEVCE_L1_CORR_Audio, &MODEL::volumeSchedule);

// PushButtonX PUSHB_L1_CORR_B11
const Command *const PUSHB_L1_CORR_B11_OnPressed=0;
const Command *const PUSHB_L1_CORR_B11_OnReleased=0;
const Command PUSHB_L1_CORR_B11_OnReleasedShort[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::TOGGLE},};
const Command PUSHB_L1_CORR_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::START},};
const Command PUSHB_L1_CORR_B11_OnReleasedLong[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::STOP},};
eEventType *PUSHB_L1_CORR_B11_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B11("PUSHB_L1_CORR_B11", eApplicationID::PUSHB_L1_CORR_B11, eInput::I12, PUSHB_L1_CORR_B11_LocalEvents, 0, PUSHB_L1_CORR_B11_BusEvents, 0, PUSHB_L1_CORR_B11_OnPressed, 0, PUSHB_L1_CORR_B11_OnReleased, 0, PUSHB_L1_CORR_B11_OnReleasedShort, 1, PUSHB_L1_CORR_B11_OnPressedShortAndHold, 1, PUSHB_L1_CORR_B11_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_CORR_B21
const Command *const PUSHB_L1_CORR_B21_OnPressed=0;
const Command *const PUSHB_L1_CORR_B21_OnReleased=0;
const Command PUSHB_L1_CORR_B21_OnReleasedShort[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::TOGGLE},};
const Command PUSHB_L1_CORR_B21_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::START},};
const Command PUSHB_L1_CORR_B21_OnReleasedLong[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::STOP},};
eEventType *PUSHB_L1_CORR_B21_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B21("PUSHB_L1_CORR_B21", eApplicationID::PUSHB_L1_CORR_B21, eInput::I13, PUSHB_L1_CORR_B21_LocalEvents, 0, PUSHB_L1_CORR_B21_BusEvents, 0, PUSHB_L1_CORR_B21_OnPressed, 0, PUSHB_L1_CORR_B21_OnReleased, 0, PUSHB_L1_CORR_B21_OnReleasedShort, 1, PUSHB_L1_CORR_B21_OnPressedShortAndHold, 1, PUSHB_L1_CORR_B21_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_CORR_B22
const Command PUSHB_L1_CORR_B22_OnPressed[1]={{eApplicationID::POWIT_L1_CORR_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_CORR_B22_OnReleased=0;
const Command *const PUSHB_L1_CORR_B22_OnReleasedShort=0;
const Command *const PUSHB_L1_CORR_B22_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_CORR_B22_OnReleasedLong=0;
eEventType *PUSHB_L1_CORR_B22_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B22("PUSHB_L1_CORR_B22", eApplicationID::PUSHB_L1_CORR_B22, eInput::I14, PUSHB_L1_CORR_B22_LocalEvents, 0, PUSHB_L1_CORR_B22_BusEvents, 0, PUSHB_L1_CORR_B22_OnPressed, 1, PUSHB_L1_CORR_B22_OnReleased, 0, PUSHB_L1_CORR_B22_OnReleasedShort, 0, PUSHB_L1_CORR_B22_OnPressedShortAndHold, 0, PUSHB_L1_CORR_B22_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_CORR_B23
const Command PUSHB_L1_CORR_B23_OnPressed[1]={{eApplicationID::POWIT_LX_FRON_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_CORR_B23_OnReleased=0;
const Command *const PUSHB_L1_CORR_B23_OnReleasedShort=0;
const Command *const PUSHB_L1_CORR_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_CORR_B23_OnReleasedLong=0;
eEventType *PUSHB_L1_CORR_B23_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B23("PUSHB_L1_CORR_B23", eApplicationID::PUSHB_L1_CORR_B23, eInput::I15, PUSHB_L1_CORR_B23_LocalEvents, 0, PUSHB_L1_CORR_B23_BusEvents, 0, PUSHB_L1_CORR_B23_OnPressed, 1, PUSHB_L1_CORR_B23_OnReleased, 0, PUSHB_L1_CORR_B23_OnReleasedShort, 0, PUSHB_L1_CORR_B23_OnPressedShortAndHold, 0, PUSHB_L1_CORR_B23_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_CORR_B24
const Command PUSHB_L1_CORR_B24_OnPressed[1]={{eApplicationID::POWIT_LX_LEFT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_CORR_B24_OnReleased=0;
const Command *const PUSHB_L1_CORR_B24_OnReleasedShort=0;
const Command *const PUSHB_L1_CORR_B24_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_CORR_B24_OnReleasedLong=0;
eEventType *PUSHB_L1_CORR_B24_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B24_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B24("PUSHB_L1_CORR_B24", eApplicationID::PUSHB_L1_CORR_B24, eInput::I16, PUSHB_L1_CORR_B24_LocalEvents, 0, PUSHB_L1_CORR_B24_BusEvents, 0, PUSHB_L1_CORR_B24_OnPressed, 1, PUSHB_L1_CORR_B24_OnReleased, 0, PUSHB_L1_CORR_B24_OnReleasedShort, 0, PUSHB_L1_CORR_B24_OnPressedShortAndHold, 0, PUSHB_L1_CORR_B24_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_CORR_B25
const Command PUSHB_L1_CORR_B25_OnPressed[1]={{eApplicationID::POWIT_LX_FRON_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_CORR_B25_OnReleased=0;
const Command *const PUSHB_L1_CORR_B25_OnReleasedShort=0;
const Command *const PUSHB_L1_CORR_B25_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_CORR_B25_OnReleasedLong=0;
eEventType *PUSHB_L1_CORR_B25_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B25_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B25("PUSHB_L1_CORR_B25", eApplicationID::PUSHB_L1_CORR_B25, eInput::I17, PUSHB_L1_CORR_B25_LocalEvents, 0, PUSHB_L1_CORR_B25_BusEvents, 0, PUSHB_L1_CORR_B25_OnPressed, 1, PUSHB_L1_CORR_B25_OnReleased, 0, PUSHB_L1_CORR_B25_OnReleasedShort, 0, PUSHB_L1_CORR_B25_OnPressedShortAndHold, 0, PUSHB_L1_CORR_B25_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_CORR_B26
const Command PUSHB_L1_CORR_B26_OnPressed[1]={{eApplicationID::POWIT_LX_LEFT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_CORR_B26_OnReleased=0;
const Command *const PUSHB_L1_CORR_B26_OnReleasedShort=0;
const Command *const PUSHB_L1_CORR_B26_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_CORR_B26_OnReleasedLong=0;
eEventType *PUSHB_L1_CORR_B26_LocalEvents=0;
eEventType *PUSHB_L1_CORR_B26_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_CORR_B26("PUSHB_L1_CORR_B26", eApplicationID::PUSHB_L1_CORR_B26, eInput::I18, PUSHB_L1_CORR_B26_LocalEvents, 0, PUSHB_L1_CORR_B26_BusEvents, 0, PUSHB_L1_CORR_B26_OnPressed, 1, PUSHB_L1_CORR_B26_OnReleased, 0, PUSHB_L1_CORR_B26_OnReleasedShort, 0, PUSHB_L1_CORR_B26_OnPressedShortAndHold, 0, PUSHB_L1_CORR_B26_OnReleasedLong, 0);

// PWM PWM___L1_CORR_S (Dimmer )
ePWMOutput PWM___L1_CORR_S_output[3]={ePWMOutput::P08,ePWMOutput::P09,ePWMOutput::P10,};
sensact::cPWM PWM___L1_CORR_S("PWM___L1_CORR_S", eApplicationID::PWM___L1_CORR_S, PWM___L1_CORR_S_output, 3, 1, 255, false, eApplicationID::STDBY_L0_TECH_48V);

// POWIT POWIT_L1_CORR_W1
sensact::cPoweritem POWIT_L1_CORR_W1("POWIT_L1_CORR_W1", eApplicationID::POWIT_L1_CORR_W1, ePoweredOutput::O16, 0);

// PushButtonX PUSHB_L1_KTCH_B11
const Command PUSHB_L1_KTCH_B11_OnPressed[1]={{eApplicationID::POWIT_L1_KTCH_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_KTCH_B11_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B11_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_KTCH_B11_OnReleasedLong=0;
eEventType *PUSHB_L1_KTCH_B11_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B11("PUSHB_L1_KTCH_B11", eApplicationID::PUSHB_L1_KTCH_B11, eInput::I19, PUSHB_L1_KTCH_B11_LocalEvents, 0, PUSHB_L1_KTCH_B11_BusEvents, 0, PUSHB_L1_KTCH_B11_OnPressed, 1, PUSHB_L1_KTCH_B11_OnReleased, 0, PUSHB_L1_KTCH_B11_OnReleasedShort, 0, PUSHB_L1_KTCH_B11_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_KTCH_B12
const Command PUSHB_L1_KTCH_B12_OnPressed[1]={{eApplicationID::POWIT_LX_BACK_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_KTCH_B12_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B12_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B12_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_KTCH_B12_OnReleasedLong=0;
eEventType *PUSHB_L1_KTCH_B12_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B12("PUSHB_L1_KTCH_B12", eApplicationID::PUSHB_L1_KTCH_B12, eInput::I20, PUSHB_L1_KTCH_B12_LocalEvents, 0, PUSHB_L1_KTCH_B12_BusEvents, 0, PUSHB_L1_KTCH_B12_OnPressed, 1, PUSHB_L1_KTCH_B12_OnReleased, 0, PUSHB_L1_KTCH_B12_OnReleasedShort, 0, PUSHB_L1_KTCH_B12_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B12_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_KTCH_B31
const Command *const PUSHB_L1_KTCH_B31_OnPressed=0;
const Command *const PUSHB_L1_KTCH_B31_OnReleased=0;
const Command PUSHB_L1_KTCH_B31_OnReleasedShort[1]={{eApplicationID::PWM___L1_KTCH_S2, eCommandType::TOGGLE},};
const Command PUSHB_L1_KTCH_B31_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_KTCH_S2, eCommandType::START},};
const Command PUSHB_L1_KTCH_B31_OnReleasedLong[1]={{eApplicationID::PWM___L1_KTCH_S2, eCommandType::STOP},};
eEventType *PUSHB_L1_KTCH_B31_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B31_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B31("PUSHB_L1_KTCH_B31", eApplicationID::PUSHB_L1_KTCH_B31, eInput::I21, PUSHB_L1_KTCH_B31_LocalEvents, 0, PUSHB_L1_KTCH_B31_BusEvents, 0, PUSHB_L1_KTCH_B31_OnPressed, 0, PUSHB_L1_KTCH_B31_OnReleased, 0, PUSHB_L1_KTCH_B31_OnReleasedShort, 1, PUSHB_L1_KTCH_B31_OnPressedShortAndHold, 1, PUSHB_L1_KTCH_B31_OnReleasedLong, 1);

// POWIT POWIT_L1_KTCH_C1
sensact::cPoweritem POWIT_L1_KTCH_C1("POWIT_L1_KTCH_C1", eApplicationID::POWIT_L1_KTCH_C1, ePoweredOutput::O17, 0);

// POWIT POWIT_L1_KTCH_W1
sensact::cPoweritem POWIT_L1_KTCH_W1("POWIT_L1_KTCH_W1", eApplicationID::POWIT_L1_KTCH_W1, ePoweredOutput::O18, 0);

// Blind BLIND_L1_KTCH_J1
cBlind BLIND_L1_KTCH_J1("BLIND_L1_KTCH_J1", eApplicationID::BLIND_L1_KTCH_J1, ePoweredOutput::O19, ePoweredOutput::O20, eRelayMode::INTERLOCKED, 40);

// Blind BLIND_L1_KTCH_J2
cBlind BLIND_L1_KTCH_J2("BLIND_L1_KTCH_J2", eApplicationID::BLIND_L1_KTCH_J2, ePoweredOutput::O21, ePoweredOutput::O22, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L1_LVNG_B21
const Command PUSHB_L1_LVNG_B21_OnPressed[1]={{eApplicationID::POWIT_L1_LVNG_W4, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_LVNG_B21_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B21_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B21_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_LVNG_B21_OnReleasedLong=0;
eEventType *PUSHB_L1_LVNG_B21_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B21("PUSHB_L1_LVNG_B21", eApplicationID::PUSHB_L1_LVNG_B21, eInput::I22, PUSHB_L1_LVNG_B21_LocalEvents, 0, PUSHB_L1_LVNG_B21_BusEvents, 0, PUSHB_L1_LVNG_B21_OnPressed, 1, PUSHB_L1_LVNG_B21_OnReleased, 0, PUSHB_L1_LVNG_B21_OnReleasedShort, 0, PUSHB_L1_LVNG_B21_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B21_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_LVNG_B22
const Command PUSHB_L1_LVNG_B22_OnPressed[1]={{eApplicationID::POWIT_L1_LVNG_W4, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_LVNG_B22_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B22_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B22_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_LVNG_B22_OnReleasedLong=0;
eEventType *PUSHB_L1_LVNG_B22_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B22("PUSHB_L1_LVNG_B22", eApplicationID::PUSHB_L1_LVNG_B22, eInput::I23, PUSHB_L1_LVNG_B22_LocalEvents, 0, PUSHB_L1_LVNG_B22_BusEvents, 0, PUSHB_L1_LVNG_B22_OnPressed, 1, PUSHB_L1_LVNG_B22_OnReleased, 0, PUSHB_L1_LVNG_B22_OnReleasedShort, 0, PUSHB_L1_LVNG_B22_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B22_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_LVNG_B23
const Command PUSHB_L1_LVNG_B23_OnPressed[1]={{eApplicationID::POWIT_LX_RGHT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_LVNG_B23_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B23_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_LVNG_B23_OnReleasedLong=0;
eEventType *PUSHB_L1_LVNG_B23_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B23("PUSHB_L1_LVNG_B23", eApplicationID::PUSHB_L1_LVNG_B23, eInput::I24, PUSHB_L1_LVNG_B23_LocalEvents, 0, PUSHB_L1_LVNG_B23_BusEvents, 0, PUSHB_L1_LVNG_B23_OnPressed, 1, PUSHB_L1_LVNG_B23_OnReleased, 0, PUSHB_L1_LVNG_B23_OnReleasedShort, 0, PUSHB_L1_LVNG_B23_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B23_OnReleasedLong, 0);

// Blind BLIND_L1_LVNG_J1
cBlind BLIND_L1_LVNG_J1("BLIND_L1_LVNG_J1", eApplicationID::BLIND_L1_LVNG_J1, ePoweredOutput::O23, ePoweredOutput::O24, eRelayMode::INTERLOCKED, 40);

// Blind BLIND_L1_LVNG_J2
cBlind BLIND_L1_LVNG_J2("BLIND_L1_LVNG_J2", eApplicationID::BLIND_L1_LVNG_J2, ePoweredOutput::O25, ePoweredOutput::O26, eRelayMode::INTERLOCKED, 40);

// Blind BLIND_L1_LVNG_J3
cBlind BLIND_L1_LVNG_J3("BLIND_L1_LVNG_J3", eApplicationID::BLIND_L1_LVNG_J3, ePoweredOutput::O27, ePoweredOutput::O28, eRelayMode::INTERLOCKED, 40);

// POWIT POWIT_L1_LVNG_C1
sensact::cPoweritem POWIT_L1_LVNG_C1("POWIT_L1_LVNG_C1", eApplicationID::POWIT_L1_LVNG_C1, ePoweredOutput::O29, 0);

// POWIT POWIT_L1_LVNG_W1
sensact::cPoweritem POWIT_L1_LVNG_W1("POWIT_L1_LVNG_W1", eApplicationID::POWIT_L1_LVNG_W1, ePoweredOutput::O30, 0);

// POWIT POWIT_L1_LVNG_W4
sensact::cPoweritem POWIT_L1_LVNG_W4("POWIT_L1_LVNG_W4", eApplicationID::POWIT_L1_LVNG_W4, ePoweredOutput::O31, 0);

// PushButtonX PUSHB_L1_WORK_B11
const Command PUSHB_L1_WORK_B11_OnPressed[1]={{eApplicationID::POWIT_L1_WORK_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_WORK_B11_OnReleased=0;
const Command *const PUSHB_L1_WORK_B11_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_WORK_B11_OnReleasedLong=0;
eEventType *PUSHB_L1_WORK_B11_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B11("PUSHB_L1_WORK_B11", eApplicationID::PUSHB_L1_WORK_B11, eInput::I08, PUSHB_L1_WORK_B11_LocalEvents, 0, PUSHB_L1_WORK_B11_BusEvents, 0, PUSHB_L1_WORK_B11_OnPressed, 1, PUSHB_L1_WORK_B11_OnReleased, 0, PUSHB_L1_WORK_B11_OnReleasedShort, 0, PUSHB_L1_WORK_B11_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_WORK_B12
const Command PUSHB_L1_WORK_B12_OnPressed[1]={{eApplicationID::POWIT_L1_WORK_C2, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_WORK_B12_OnReleased=0;
const Command *const PUSHB_L1_WORK_B12_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B12_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_WORK_B12_OnReleasedLong=0;
eEventType *PUSHB_L1_WORK_B12_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B12("PUSHB_L1_WORK_B12", eApplicationID::PUSHB_L1_WORK_B12, eInput::I09, PUSHB_L1_WORK_B12_LocalEvents, 0, PUSHB_L1_WORK_B12_BusEvents, 0, PUSHB_L1_WORK_B12_OnPressed, 1, PUSHB_L1_WORK_B12_OnReleased, 0, PUSHB_L1_WORK_B12_OnReleasedShort, 0, PUSHB_L1_WORK_B12_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B12_OnReleasedLong, 0);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    &PUSHB_L0_CORR_B11,
    &POWIT_L0_CORR_C1,
    &PUSHB_L0_PRTY_B11,
    &PUSHB_L0_PRTY_B12,
    &PWM___L0_PRTY_S,
    &PUSHB_L0_STO1_B11,
    &POWIT_L0_STO1_C1,
    &PUSHB_L0_TECH_B11,
    &POWIT_L0_TECH_C1,
    &PUSHB_L0_WELL_B11,
    &PUSHB_L0_WELL_B21,
    &PUSHB_L0_WELL_B22,
    &POWIT_L0_WELL_C1,
    &PWM___L0_WELL_S1,
    &PWM___L0_WELL_S2,
    &PUSHB_L0_WORK_B11,
    &POWIT_L0_WORK_C1,
    &PUSHB_L1_BATH_B11,
    &PWM___L1_BATH_S,
    &PUSHB_L1_BATH_B12,
    &PUSHB_L1_BATH_B13,
    &BLIND_L1_BATH_J1,
    &RGBW__L1_BATH_W1,
    &DEVCE_L1_CORR_Audio,
    &PUSHB_L1_CORR_B11,
    &PUSHB_L1_CORR_B21,
    &PUSHB_L1_CORR_B22,
    &PUSHB_L1_CORR_B23,
    &PUSHB_L1_CORR_B24,
    &PUSHB_L1_CORR_B25,
    &PUSHB_L1_CORR_B26,
    &PWM___L1_CORR_S,
    &POWIT_L1_CORR_W1,
    &PUSHB_L1_KTCH_B11,
    &PUSHB_L1_KTCH_B12,
    &PUSHB_L1_KTCH_B31,
    &POWIT_L1_KTCH_C1,
    &POWIT_L1_KTCH_W1,
    &BLIND_L1_KTCH_J1,
    &BLIND_L1_KTCH_J2,
    &PUSHB_L1_LVNG_B21,
    &PUSHB_L1_LVNG_B22,
    &PUSHB_L1_LVNG_B23,
    &BLIND_L1_LVNG_J1,
    &BLIND_L1_LVNG_J2,
    &BLIND_L1_LVNG_J3,
    &POWIT_L1_LVNG_C1,
    &POWIT_L1_LVNG_W1,
    &POWIT_L1_LVNG_W4,
    &PUSHB_L1_WORK_B11,
    &PUSHB_L1_WORK_B12,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L0_TECH_HS07_2

eNodeID NODE = eNodeID::SNSAC_L0_TECH_HS07_2;

const char MODEL::ModelString[] ="NodeId SNSAC_L0_TECH_HS07_2 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// PushButtonX PUSHB_L1_UTIL_B11
const Command *const PUSHB_L1_UTIL_B11_OnPressed=0;
const Command *const PUSHB_L1_UTIL_B11_OnReleased=0;
const Command PUSHB_L1_UTIL_B11_OnReleasedShort[1]={{eApplicationID::PWM___L1_KTCH_S1, eCommandType::TOGGLE},};
const Command PUSHB_L1_UTIL_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_KTCH_S1, eCommandType::START},};
const Command PUSHB_L1_UTIL_B11_OnReleasedLong[1]={{eApplicationID::PWM___L1_KTCH_S1, eCommandType::STOP},};
eEventType *PUSHB_L1_UTIL_B11_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B11("PUSHB_L1_UTIL_B11", eApplicationID::PUSHB_L1_UTIL_B11, eInput::I01, PUSHB_L1_UTIL_B11_LocalEvents, 0, PUSHB_L1_UTIL_B11_BusEvents, 0, PUSHB_L1_UTIL_B11_OnPressed, 0, PUSHB_L1_UTIL_B11_OnReleased, 0, PUSHB_L1_UTIL_B11_OnReleasedShort, 1, PUSHB_L1_UTIL_B11_OnPressedShortAndHold, 1, PUSHB_L1_UTIL_B11_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_UTIL_B12
const Command PUSHB_L1_UTIL_B12_OnPressed[1]={{eApplicationID::BLIND_L1_UTIL_J1, eCommandType::UP},};
const Command *const PUSHB_L1_UTIL_B12_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B12_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B12_OnPressedShortAndHold=0;
const Command PUSHB_L1_UTIL_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L1_UTIL_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_UTIL_B12_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B12("PUSHB_L1_UTIL_B12", eApplicationID::PUSHB_L1_UTIL_B12, eInput::I02, PUSHB_L1_UTIL_B12_LocalEvents, 0, PUSHB_L1_UTIL_B12_BusEvents, 0, PUSHB_L1_UTIL_B12_OnPressed, 1, PUSHB_L1_UTIL_B12_OnReleased, 0, PUSHB_L1_UTIL_B12_OnReleasedShort, 0, PUSHB_L1_UTIL_B12_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_UTIL_B13
const Command PUSHB_L1_UTIL_B13_OnPressed[1]={{eApplicationID::BLIND_L1_UTIL_J1, eCommandType::DOWN},};
const Command *const PUSHB_L1_UTIL_B13_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B13_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B13_OnPressedShortAndHold=0;
const Command PUSHB_L1_UTIL_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L1_UTIL_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_UTIL_B13_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B13("PUSHB_L1_UTIL_B13", eApplicationID::PUSHB_L1_UTIL_B13, eInput::I03, PUSHB_L1_UTIL_B13_LocalEvents, 0, PUSHB_L1_UTIL_B13_BusEvents, 0, PUSHB_L1_UTIL_B13_OnPressed, 1, PUSHB_L1_UTIL_B13_OnReleased, 0, PUSHB_L1_UTIL_B13_OnReleasedShort, 0, PUSHB_L1_UTIL_B13_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B13_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_UTIL_B21
const Command PUSHB_L1_UTIL_B21_OnPressed[1]={{eApplicationID::POWIT_L1_UTIL_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_UTIL_B21_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B21_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B21_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_UTIL_B21_OnReleasedLong=0;
eEventType *PUSHB_L1_UTIL_B21_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B21("PUSHB_L1_UTIL_B21", eApplicationID::PUSHB_L1_UTIL_B21, eInput::I04, PUSHB_L1_UTIL_B21_LocalEvents, 0, PUSHB_L1_UTIL_B21_BusEvents, 0, PUSHB_L1_UTIL_B21_OnPressed, 1, PUSHB_L1_UTIL_B21_OnReleased, 0, PUSHB_L1_UTIL_B21_OnReleasedShort, 0, PUSHB_L1_UTIL_B21_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B21_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_UTIL_B22
const Command PUSHB_L1_UTIL_B22_OnPressed[1]={{eApplicationID::POWIT_LX_LEFT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_UTIL_B22_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B22_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B22_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_UTIL_B22_OnReleasedLong=0;
eEventType *PUSHB_L1_UTIL_B22_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B22("PUSHB_L1_UTIL_B22", eApplicationID::PUSHB_L1_UTIL_B22, eInput::I05, PUSHB_L1_UTIL_B22_LocalEvents, 0, PUSHB_L1_UTIL_B22_BusEvents, 0, PUSHB_L1_UTIL_B22_OnPressed, 1, PUSHB_L1_UTIL_B22_OnReleased, 0, PUSHB_L1_UTIL_B22_OnReleasedShort, 0, PUSHB_L1_UTIL_B22_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B22_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_UTIL_B23
const Command PUSHB_L1_UTIL_B23_OnPressed[1]={{eApplicationID::POWIT_LX_LEFT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_UTIL_B23_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B23_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_UTIL_B23_OnReleasedLong=0;
eEventType *PUSHB_L1_UTIL_B23_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B23("PUSHB_L1_UTIL_B23", eApplicationID::PUSHB_L1_UTIL_B23, eInput::I06, PUSHB_L1_UTIL_B23_LocalEvents, 0, PUSHB_L1_UTIL_B23_BusEvents, 0, PUSHB_L1_UTIL_B23_OnPressed, 1, PUSHB_L1_UTIL_B23_OnReleased, 0, PUSHB_L1_UTIL_B23_OnReleasedShort, 0, PUSHB_L1_UTIL_B23_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B23_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_UTIL_B24
const Command PUSHB_L1_UTIL_B24_OnPressed[1]={{eApplicationID::POWIT_LX_LEFT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_UTIL_B24_OnReleased=0;
const Command *const PUSHB_L1_UTIL_B24_OnReleasedShort=0;
const Command *const PUSHB_L1_UTIL_B24_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_UTIL_B24_OnReleasedLong=0;
eEventType *PUSHB_L1_UTIL_B24_LocalEvents=0;
eEventType *PUSHB_L1_UTIL_B24_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_UTIL_B24("PUSHB_L1_UTIL_B24", eApplicationID::PUSHB_L1_UTIL_B24, eInput::I07, PUSHB_L1_UTIL_B24_LocalEvents, 0, PUSHB_L1_UTIL_B24_BusEvents, 0, PUSHB_L1_UTIL_B24_OnPressed, 1, PUSHB_L1_UTIL_B24_OnReleased, 0, PUSHB_L1_UTIL_B24_OnReleasedShort, 0, PUSHB_L1_UTIL_B24_OnPressedShortAndHold, 0, PUSHB_L1_UTIL_B24_OnReleasedLong, 0);

// POWIT POWIT_L1_UTIL_C1
sensact::cPoweritem POWIT_L1_UTIL_C1("POWIT_L1_UTIL_C1", eApplicationID::POWIT_L1_UTIL_C1, ePoweredOutput::O12, 0);

// Blind BLIND_L1_UTIL_J1
cBlind BLIND_L1_UTIL_J1("BLIND_L1_UTIL_J1", eApplicationID::BLIND_L1_UTIL_J1, ePoweredOutput::O13, ePoweredOutput::O14, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L1_WORK_B13
const Command PUSHB_L1_WORK_B13_OnPressed[1]={{eApplicationID::BLIND_L1_WORK_J1, eCommandType::UP},};
const Command *const PUSHB_L1_WORK_B13_OnReleased=0;
const Command *const PUSHB_L1_WORK_B13_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B13_OnPressedShortAndHold=0;
const Command PUSHB_L1_WORK_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L1_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_WORK_B13_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B13("PUSHB_L1_WORK_B13", eApplicationID::PUSHB_L1_WORK_B13, eInput::I10, PUSHB_L1_WORK_B13_LocalEvents, 0, PUSHB_L1_WORK_B13_BusEvents, 0, PUSHB_L1_WORK_B13_OnPressed, 1, PUSHB_L1_WORK_B13_OnReleased, 0, PUSHB_L1_WORK_B13_OnReleasedShort, 0, PUSHB_L1_WORK_B13_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B13_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_WORK_B14
const Command PUSHB_L1_WORK_B14_OnPressed[1]={{eApplicationID::BLIND_L1_WORK_J1, eCommandType::DOWN},};
const Command *const PUSHB_L1_WORK_B14_OnReleased=0;
const Command *const PUSHB_L1_WORK_B14_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B14_OnPressedShortAndHold=0;
const Command PUSHB_L1_WORK_B14_OnReleasedLong[1]={{eApplicationID::BLIND_L1_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_WORK_B14_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B14_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B14("PUSHB_L1_WORK_B14", eApplicationID::PUSHB_L1_WORK_B14, eInput::I11, PUSHB_L1_WORK_B14_LocalEvents, 0, PUSHB_L1_WORK_B14_BusEvents, 0, PUSHB_L1_WORK_B14_OnPressed, 1, PUSHB_L1_WORK_B14_OnReleased, 0, PUSHB_L1_WORK_B14_OnReleasedShort, 0, PUSHB_L1_WORK_B14_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B14_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_WORK_B15
const Command PUSHB_L1_WORK_B15_OnPressed[1]={{eApplicationID::BLIND_L1_WORK_J2, eCommandType::UP},};
const Command *const PUSHB_L1_WORK_B15_OnReleased=0;
const Command *const PUSHB_L1_WORK_B15_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B15_OnPressedShortAndHold=0;
const Command PUSHB_L1_WORK_B15_OnReleasedLong[1]={{eApplicationID::BLIND_L1_WORK_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_WORK_B15_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B15_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B15("PUSHB_L1_WORK_B15", eApplicationID::PUSHB_L1_WORK_B15, eInput::I12, PUSHB_L1_WORK_B15_LocalEvents, 0, PUSHB_L1_WORK_B15_BusEvents, 0, PUSHB_L1_WORK_B15_OnPressed, 1, PUSHB_L1_WORK_B15_OnReleased, 0, PUSHB_L1_WORK_B15_OnReleasedShort, 0, PUSHB_L1_WORK_B15_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B15_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_WORK_B16
const Command PUSHB_L1_WORK_B16_OnPressed[1]={{eApplicationID::BLIND_L1_WORK_J2, eCommandType::DOWN},};
const Command *const PUSHB_L1_WORK_B16_OnReleased=0;
const Command *const PUSHB_L1_WORK_B16_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B16_OnPressedShortAndHold=0;
const Command PUSHB_L1_WORK_B16_OnReleasedLong[1]={{eApplicationID::BLIND_L1_WORK_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_WORK_B16_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B16_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B16("PUSHB_L1_WORK_B16", eApplicationID::PUSHB_L1_WORK_B16, eInput::I13, PUSHB_L1_WORK_B16_LocalEvents, 0, PUSHB_L1_WORK_B16_BusEvents, 0, PUSHB_L1_WORK_B16_OnPressed, 1, PUSHB_L1_WORK_B16_OnReleased, 0, PUSHB_L1_WORK_B16_OnReleasedShort, 0, PUSHB_L1_WORK_B16_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B16_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_WORK_B17
const Command PUSHB_L1_WORK_B17_OnPressed[1]={{eApplicationID::POWIT_LX_RGHT_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_WORK_B17_OnReleased=0;
const Command *const PUSHB_L1_WORK_B17_OnReleasedShort=0;
const Command *const PUSHB_L1_WORK_B17_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_WORK_B17_OnReleasedLong=0;
eEventType *PUSHB_L1_WORK_B17_LocalEvents=0;
eEventType *PUSHB_L1_WORK_B17_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_WORK_B17("PUSHB_L1_WORK_B17", eApplicationID::PUSHB_L1_WORK_B17, eInput::I14, PUSHB_L1_WORK_B17_LocalEvents, 0, PUSHB_L1_WORK_B17_BusEvents, 0, PUSHB_L1_WORK_B17_OnPressed, 1, PUSHB_L1_WORK_B17_OnReleased, 0, PUSHB_L1_WORK_B17_OnReleasedShort, 0, PUSHB_L1_WORK_B17_OnPressedShortAndHold, 0, PUSHB_L1_WORK_B17_OnReleasedLong, 0);

// POWIT POWIT_L1_WORK_C1
sensact::cPoweritem POWIT_L1_WORK_C1("POWIT_L1_WORK_C1", eApplicationID::POWIT_L1_WORK_C1, ePoweredOutput::O15, 0);

// POWIT POWIT_L1_WORK_C2
sensact::cPoweritem POWIT_L1_WORK_C2("POWIT_L1_WORK_C2", eApplicationID::POWIT_L1_WORK_C2, ePoweredOutput::O16, 0);

// Blind BLIND_L1_WORK_J1
cBlind BLIND_L1_WORK_J1("BLIND_L1_WORK_J1", eApplicationID::BLIND_L1_WORK_J1, ePoweredOutput::O17, ePoweredOutput::O18, eRelayMode::INTERLOCKED, 40);

// Blind BLIND_L1_WORK_J2
cBlind BLIND_L1_WORK_J2("BLIND_L1_WORK_J2", eApplicationID::BLIND_L1_WORK_J2, ePoweredOutput::O19, ePoweredOutput::O20, eRelayMode::INTERLOCKED, 40);

// POWIT POWIT_L1_CORR_P1
sensact::cPoweritem POWIT_L1_CORR_P1("POWIT_L1_CORR_P1", eApplicationID::POWIT_L1_CORR_P1, ePoweredOutput::O01, 0);

// POWIT POWIT_L1_LVNG_P4
sensact::cPoweritem POWIT_L1_LVNG_P4("POWIT_L1_LVNG_P4", eApplicationID::POWIT_L1_LVNG_P4, ePoweredOutput::O02, 0);

// POWIT POWIT_LX_BACK_P1
sensact::cPoweritem POWIT_LX_BACK_P1("POWIT_LX_BACK_P1", eApplicationID::POWIT_LX_BACK_P1, ePoweredOutput::O03, 0);

// POWIT POWIT_LX_BACK_W1
sensact::cPoweritem POWIT_LX_BACK_W1("POWIT_LX_BACK_W1", eApplicationID::POWIT_LX_BACK_W1, ePoweredOutput::O04, 0);

// POWIT POWIT_LX_BACK_W2
sensact::cPoweritem POWIT_LX_BACK_W2("POWIT_LX_BACK_W2", eApplicationID::POWIT_LX_BACK_W2, ePoweredOutput::O05, 0);

// POWIT POWIT_LX_BACK_W3
sensact::cPoweritem POWIT_LX_BACK_W3("POWIT_LX_BACK_W3", eApplicationID::POWIT_LX_BACK_W3, ePoweredOutput::O06, 0);

// POWIT POWIT_LX_FRON_W1
sensact::cPoweritem POWIT_LX_FRON_W1("POWIT_LX_FRON_W1", eApplicationID::POWIT_LX_FRON_W1, ePoweredOutput::O07, 0);

// POWIT POWIT_LX_LEFT_W1
sensact::cPoweritem POWIT_LX_LEFT_W1("POWIT_LX_LEFT_W1", eApplicationID::POWIT_LX_LEFT_W1, ePoweredOutput::O08, 0);

// POWIT POWIT_LX_RGHT_W1
sensact::cPoweritem POWIT_LX_RGHT_W1("POWIT_LX_RGHT_W1", eApplicationID::POWIT_LX_RGHT_W1, ePoweredOutput::O09, 0);

// POWIT POWIT_LX_GARA_C1
sensact::cPoweritem POWIT_LX_GARA_C1("POWIT_LX_GARA_C1", eApplicationID::POWIT_LX_GARA_C1, ePoweredOutput::O10, 0);

// POWIT POWIT_LX_GARA_P1
sensact::cPoweritem POWIT_LX_GARA_P1("POWIT_LX_GARA_P1", eApplicationID::POWIT_LX_GARA_P1, ePoweredOutput::O11, 0);

// STDBY STDBY_L0_TECH_24V
sensact::cStandbyController STDBY_L0_TECH_24V("STDBY_L0_TECH_24V", eApplicationID::STDBY_L0_TECH_24V, ePoweredOutput::O21, 10000);

// STDBY STDBY_L0_TECH_48V
sensact::cStandbyController STDBY_L0_TECH_48V("STDBY_L0_TECH_48V", eApplicationID::STDBY_L0_TECH_48V, ePoweredOutput::O22, 10000);

// STDBY STDBY_L2_CORR_24V
sensact::cStandbyController STDBY_L2_CORR_24V("STDBY_L2_CORR_24V", eApplicationID::STDBY_L2_CORR_24V, ePoweredOutput::O23, 10000);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &PUSHB_L1_UTIL_B11,
    &PUSHB_L1_UTIL_B12,
    &PUSHB_L1_UTIL_B13,
    &PUSHB_L1_UTIL_B21,
    &PUSHB_L1_UTIL_B22,
    &PUSHB_L1_UTIL_B23,
    &PUSHB_L1_UTIL_B24,
    &POWIT_L1_UTIL_C1,
    &BLIND_L1_UTIL_J1,
    &PUSHB_L1_WORK_B13,
    &PUSHB_L1_WORK_B14,
    &PUSHB_L1_WORK_B15,
    &PUSHB_L1_WORK_B16,
    &PUSHB_L1_WORK_B17,
    &POWIT_L1_WORK_C1,
    &POWIT_L1_WORK_C2,
    &BLIND_L1_WORK_J1,
    &BLIND_L1_WORK_J2,
    &POWIT_L1_CORR_P1,
    &POWIT_L1_LVNG_P4,
    &POWIT_LX_BACK_P1,
    &POWIT_LX_BACK_W1,
    &POWIT_LX_BACK_W2,
    &POWIT_LX_BACK_W3,
    &POWIT_LX_FRON_W1,
    &POWIT_LX_LEFT_W1,
    &POWIT_LX_RGHT_W1,
    &POWIT_LX_GARA_C1,
    &POWIT_LX_GARA_P1,
    &STDBY_L0_TECH_24V,
    &STDBY_L0_TECH_48V,
    &STDBY_L2_CORR_24V,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L3_TECH_HS07_1

eNodeID NODE = eNodeID::SNSAC_L3_TECH_HS07_1;

const char MODEL::ModelString[] ="NodeId SNSAC_L3_TECH_HS07_1 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// PushButtonX PUSHB_L2_BATH_B13
const Command PUSHB_L2_BATH_B13_OnPressed[1]={{eApplicationID::BLIND_L2_BATH_J1, eCommandType::UP},};
const Command *const PUSHB_L2_BATH_B13_OnReleased=0;
const Command *const PUSHB_L2_BATH_B13_OnReleasedShort=0;
const Command *const PUSHB_L2_BATH_B13_OnPressedShortAndHold=0;
const Command PUSHB_L2_BATH_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BATH_B13_LocalEvents=0;
eEventType *PUSHB_L2_BATH_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BATH_B13("PUSHB_L2_BATH_B13", eApplicationID::PUSHB_L2_BATH_B13, eInput::I01, PUSHB_L2_BATH_B13_LocalEvents, 0, PUSHB_L2_BATH_B13_BusEvents, 0, PUSHB_L2_BATH_B13_OnPressed, 1, PUSHB_L2_BATH_B13_OnReleased, 0, PUSHB_L2_BATH_B13_OnReleasedShort, 0, PUSHB_L2_BATH_B13_OnPressedShortAndHold, 0, PUSHB_L2_BATH_B13_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BATH_B14
const Command PUSHB_L2_BATH_B14_OnPressed[1]={{eApplicationID::BLIND_L2_BATH_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_BATH_B14_OnReleased=0;
const Command *const PUSHB_L2_BATH_B14_OnReleasedShort=0;
const Command *const PUSHB_L2_BATH_B14_OnPressedShortAndHold=0;
const Command PUSHB_L2_BATH_B14_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BATH_B14_LocalEvents=0;
eEventType *PUSHB_L2_BATH_B14_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BATH_B14("PUSHB_L2_BATH_B14", eApplicationID::PUSHB_L2_BATH_B14, eInput::I02, PUSHB_L2_BATH_B14_LocalEvents, 0, PUSHB_L2_BATH_B14_BusEvents, 0, PUSHB_L2_BATH_B14_OnPressed, 1, PUSHB_L2_BATH_B14_OnReleased, 0, PUSHB_L2_BATH_B14_OnReleasedShort, 0, PUSHB_L2_BATH_B14_OnPressedShortAndHold, 0, PUSHB_L2_BATH_B14_OnReleasedLong, 1);

// Blind BLIND_L2_BATH_J1
cBlind BLIND_L2_BATH_J1("BLIND_L2_BATH_J1", eApplicationID::BLIND_L2_BATH_J1, ePoweredOutput::O01, ePoweredOutput::O02, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L2_BEDR_B11
const Command PUSHB_L2_BEDR_B11_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P3, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B11_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B11_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B11_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B11_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B11("PUSHB_L2_BEDR_B11", eApplicationID::PUSHB_L2_BEDR_B11, eInput::I03, PUSHB_L2_BEDR_B11_LocalEvents, 0, PUSHB_L2_BEDR_B11_BusEvents, 0, PUSHB_L2_BEDR_B11_OnPressed, 1, PUSHB_L2_BEDR_B11_OnReleased, 0, PUSHB_L2_BEDR_B11_OnReleasedShort, 0, PUSHB_L2_BEDR_B11_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B12
const Command *const PUSHB_L2_BEDR_B12_OnPressed=0;
const Command *const PUSHB_L2_BEDR_B12_OnReleased=0;
const Command PUSHB_L2_BEDR_B12_OnReleasedShort[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::TOGGLE},};
const Command PUSHB_L2_BEDR_B12_OnPressedShortAndHold[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::START},};
const Command PUSHB_L2_BEDR_B12_OnReleasedLong[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B12_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B12("PUSHB_L2_BEDR_B12", eApplicationID::PUSHB_L2_BEDR_B12, eInput::I04, PUSHB_L2_BEDR_B12_LocalEvents, 0, PUSHB_L2_BEDR_B12_BusEvents, 0, PUSHB_L2_BEDR_B12_OnPressed, 0, PUSHB_L2_BEDR_B12_OnReleased, 0, PUSHB_L2_BEDR_B12_OnReleasedShort, 1, PUSHB_L2_BEDR_B12_OnPressedShortAndHold, 1, PUSHB_L2_BEDR_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B13
const Command PUSHB_L2_BEDR_B13_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P1, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B13_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B13_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B13_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B13_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B13_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B13("PUSHB_L2_BEDR_B13", eApplicationID::PUSHB_L2_BEDR_B13, eInput::I05, PUSHB_L2_BEDR_B13_LocalEvents, 0, PUSHB_L2_BEDR_B13_BusEvents, 0, PUSHB_L2_BEDR_B13_OnPressed, 1, PUSHB_L2_BEDR_B13_OnReleased, 0, PUSHB_L2_BEDR_B13_OnReleasedShort, 0, PUSHB_L2_BEDR_B13_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B13_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B14
const Command PUSHB_L2_BEDR_B14_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::UP},};
const Command *const PUSHB_L2_BEDR_B14_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B14_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B14_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B14_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B14_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B14_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B14("PUSHB_L2_BEDR_B14", eApplicationID::PUSHB_L2_BEDR_B14, eInput::I06, PUSHB_L2_BEDR_B14_LocalEvents, 0, PUSHB_L2_BEDR_B14_BusEvents, 0, PUSHB_L2_BEDR_B14_OnPressed, 1, PUSHB_L2_BEDR_B14_OnReleased, 0, PUSHB_L2_BEDR_B14_OnReleasedShort, 0, PUSHB_L2_BEDR_B14_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B14_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B15
const Command PUSHB_L2_BEDR_B15_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_BEDR_B15_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B15_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B15_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B15_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B15_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B15_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B15("PUSHB_L2_BEDR_B15", eApplicationID::PUSHB_L2_BEDR_B15, eInput::I07, PUSHB_L2_BEDR_B15_LocalEvents, 0, PUSHB_L2_BEDR_B15_BusEvents, 0, PUSHB_L2_BEDR_B15_OnPressed, 1, PUSHB_L2_BEDR_B15_OnReleased, 0, PUSHB_L2_BEDR_B15_OnReleasedShort, 0, PUSHB_L2_BEDR_B15_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B15_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B21
const Command PUSHB_L2_BEDR_B21_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P3, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B21_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B21_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B21_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B21_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B21_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B21("PUSHB_L2_BEDR_B21", eApplicationID::PUSHB_L2_BEDR_B21, eInput::I08, PUSHB_L2_BEDR_B21_LocalEvents, 0, PUSHB_L2_BEDR_B21_BusEvents, 0, PUSHB_L2_BEDR_B21_OnPressed, 1, PUSHB_L2_BEDR_B21_OnReleased, 0, PUSHB_L2_BEDR_B21_OnReleasedShort, 0, PUSHB_L2_BEDR_B21_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B21_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B22
const Command *const PUSHB_L2_BEDR_B22_OnPressed=0;
const Command *const PUSHB_L2_BEDR_B22_OnReleased=0;
const Command PUSHB_L2_BEDR_B22_OnReleasedShort[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::TOGGLE},};
const Command PUSHB_L2_BEDR_B22_OnPressedShortAndHold[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::START},};
const Command PUSHB_L2_BEDR_B22_OnReleasedLong[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B22_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B22("PUSHB_L2_BEDR_B22", eApplicationID::PUSHB_L2_BEDR_B22, eInput::I09, PUSHB_L2_BEDR_B22_LocalEvents, 0, PUSHB_L2_BEDR_B22_BusEvents, 0, PUSHB_L2_BEDR_B22_OnPressed, 0, PUSHB_L2_BEDR_B22_OnReleased, 0, PUSHB_L2_BEDR_B22_OnReleasedShort, 1, PUSHB_L2_BEDR_B22_OnPressedShortAndHold, 1, PUSHB_L2_BEDR_B22_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B23
const Command PUSHB_L2_BEDR_B23_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P2, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B23_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B23_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B23_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B23_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B23("PUSHB_L2_BEDR_B23", eApplicationID::PUSHB_L2_BEDR_B23, eInput::I10, PUSHB_L2_BEDR_B23_LocalEvents, 0, PUSHB_L2_BEDR_B23_BusEvents, 0, PUSHB_L2_BEDR_B23_OnPressed, 1, PUSHB_L2_BEDR_B23_OnReleased, 0, PUSHB_L2_BEDR_B23_OnReleasedShort, 0, PUSHB_L2_BEDR_B23_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B23_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B24
const Command PUSHB_L2_BEDR_B24_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::UP},};
const Command *const PUSHB_L2_BEDR_B24_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B24_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B24_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B24_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B24_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B24_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B24("PUSHB_L2_BEDR_B24", eApplicationID::PUSHB_L2_BEDR_B24, eInput::I11, PUSHB_L2_BEDR_B24_LocalEvents, 0, PUSHB_L2_BEDR_B24_BusEvents, 0, PUSHB_L2_BEDR_B24_OnPressed, 1, PUSHB_L2_BEDR_B24_OnReleased, 0, PUSHB_L2_BEDR_B24_OnReleasedShort, 0, PUSHB_L2_BEDR_B24_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B24_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B25
const Command PUSHB_L2_BEDR_B25_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_BEDR_B25_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B25_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B25_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B25_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B25_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B25_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B25("PUSHB_L2_BEDR_B25", eApplicationID::PUSHB_L2_BEDR_B25, eInput::I12, PUSHB_L2_BEDR_B25_LocalEvents, 0, PUSHB_L2_BEDR_B25_BusEvents, 0, PUSHB_L2_BEDR_B25_OnPressed, 1, PUSHB_L2_BEDR_B25_OnReleased, 0, PUSHB_L2_BEDR_B25_OnReleasedShort, 0, PUSHB_L2_BEDR_B25_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B25_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B31
const Command *const PUSHB_L2_BEDR_B31_OnPressed=0;
const Command *const PUSHB_L2_BEDR_B31_OnReleased=0;
const Command PUSHB_L2_BEDR_B31_OnReleasedShort[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::TOGGLE},};
const Command PUSHB_L2_BEDR_B31_OnPressedShortAndHold[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::UP},};
const Command PUSHB_L2_BEDR_B31_OnReleasedLong[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B31_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B31_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B31("PUSHB_L2_BEDR_B31", eApplicationID::PUSHB_L2_BEDR_B31, eInput::I13, PUSHB_L2_BEDR_B31_LocalEvents, 0, PUSHB_L2_BEDR_B31_BusEvents, 0, PUSHB_L2_BEDR_B31_OnPressed, 0, PUSHB_L2_BEDR_B31_OnReleased, 0, PUSHB_L2_BEDR_B31_OnReleasedShort, 1, PUSHB_L2_BEDR_B31_OnPressedShortAndHold, 1, PUSHB_L2_BEDR_B31_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B32
const Command *const PUSHB_L2_BEDR_B32_OnPressed=0;
const Command *const PUSHB_L2_BEDR_B32_OnReleased=0;
const Command PUSHB_L2_BEDR_B32_OnReleasedShort[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::TOGGLE},};
const Command PUSHB_L2_BEDR_B32_OnPressedShortAndHold[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::DOWN},};
const Command PUSHB_L2_BEDR_B32_OnReleasedLong[1]={{eApplicationID::PWM___L2_BEDR_S, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B32_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B32_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B32("PUSHB_L2_BEDR_B32", eApplicationID::PUSHB_L2_BEDR_B32, eInput::I14, PUSHB_L2_BEDR_B32_LocalEvents, 0, PUSHB_L2_BEDR_B32_BusEvents, 0, PUSHB_L2_BEDR_B32_OnPressed, 0, PUSHB_L2_BEDR_B32_OnReleased, 0, PUSHB_L2_BEDR_B32_OnReleasedShort, 1, PUSHB_L2_BEDR_B32_OnPressedShortAndHold, 1, PUSHB_L2_BEDR_B32_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B33
const Command PUSHB_L2_BEDR_B33_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P1, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B33_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B33_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B33_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B33_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B33_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B33_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B33("PUSHB_L2_BEDR_B33", eApplicationID::PUSHB_L2_BEDR_B33, eInput::I15, PUSHB_L2_BEDR_B33_LocalEvents, 0, PUSHB_L2_BEDR_B33_BusEvents, 0, PUSHB_L2_BEDR_B33_OnPressed, 1, PUSHB_L2_BEDR_B33_OnReleased, 0, PUSHB_L2_BEDR_B33_OnReleasedShort, 0, PUSHB_L2_BEDR_B33_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B33_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B34
const Command PUSHB_L2_BEDR_B34_OnPressed[1]={{eApplicationID::POWIT_L2_BEDR_P2, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_BEDR_B34_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B34_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B34_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_BEDR_B34_OnReleasedLong=0;
eEventType *PUSHB_L2_BEDR_B34_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B34_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B34("PUSHB_L2_BEDR_B34", eApplicationID::PUSHB_L2_BEDR_B34, eInput::I16, PUSHB_L2_BEDR_B34_LocalEvents, 0, PUSHB_L2_BEDR_B34_BusEvents, 0, PUSHB_L2_BEDR_B34_OnPressed, 1, PUSHB_L2_BEDR_B34_OnReleased, 0, PUSHB_L2_BEDR_B34_OnReleasedShort, 0, PUSHB_L2_BEDR_B34_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B34_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_BEDR_B35
const Command PUSHB_L2_BEDR_B35_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::UP},};
const Command *const PUSHB_L2_BEDR_B35_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B35_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B35_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B35_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B35_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B35_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B35("PUSHB_L2_BEDR_B35", eApplicationID::PUSHB_L2_BEDR_B35, eInput::I17, PUSHB_L2_BEDR_B35_LocalEvents, 0, PUSHB_L2_BEDR_B35_BusEvents, 0, PUSHB_L2_BEDR_B35_OnPressed, 1, PUSHB_L2_BEDR_B35_OnReleased, 0, PUSHB_L2_BEDR_B35_OnReleasedShort, 0, PUSHB_L2_BEDR_B35_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B35_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_BEDR_B36
const Command PUSHB_L2_BEDR_B36_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_BEDR_B36_OnReleased=0;
const Command *const PUSHB_L2_BEDR_B36_OnReleasedShort=0;
const Command *const PUSHB_L2_BEDR_B36_OnPressedShortAndHold=0;
const Command PUSHB_L2_BEDR_B36_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_BEDR_B36_LocalEvents=0;
eEventType *PUSHB_L2_BEDR_B36_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_BEDR_B36("PUSHB_L2_BEDR_B36", eApplicationID::PUSHB_L2_BEDR_B36, eInput::I18, PUSHB_L2_BEDR_B36_LocalEvents, 0, PUSHB_L2_BEDR_B36_BusEvents, 0, PUSHB_L2_BEDR_B36_OnPressed, 1, PUSHB_L2_BEDR_B36_OnReleased, 0, PUSHB_L2_BEDR_B36_OnReleasedShort, 0, PUSHB_L2_BEDR_B36_OnPressedShortAndHold, 0, PUSHB_L2_BEDR_B36_OnReleasedLong, 1);

// Blind BLIND_L2_BEDR_J1
cBlind BLIND_L2_BEDR_J1("BLIND_L2_BEDR_J1", eApplicationID::BLIND_L2_BEDR_J1, ePoweredOutput::O03, ePoweredOutput::O04, eRelayMode::INTERLOCKED, 40);

// PWM PWM___L2_BEDR_S (Dimmer )
ePWMOutput PWM___L2_BEDR_S_output[2]={ePWMOutput::P01,ePWMOutput::P02,};
sensact::cPWM PWM___L2_BEDR_S("PWM___L2_BEDR_S", eApplicationID::PWM___L2_BEDR_S, PWM___L2_BEDR_S_output, 2, 1, 255, false, eApplicationID::STDBY_L3_TECH_48V);

// POWIT POWIT_L2_BEDR_P1
sensact::cPoweritem POWIT_L2_BEDR_P1("POWIT_L2_BEDR_P1", eApplicationID::POWIT_L2_BEDR_P1, ePoweredOutput::O05, 0);

// POWIT POWIT_L2_BEDR_P2
sensact::cPoweritem POWIT_L2_BEDR_P2("POWIT_L2_BEDR_P2", eApplicationID::POWIT_L2_BEDR_P2, ePoweredOutput::O06, 0);

// POWIT POWIT_L2_BEDR_P3
sensact::cPoweritem POWIT_L2_BEDR_P3("POWIT_L2_BEDR_P3", eApplicationID::POWIT_L2_BEDR_P3, ePoweredOutput::O07, 0);

// PushButtonX PUSHB_L2_CORR_B11
const Command PUSHB_L2_CORR_B11_OnPressed[1]={{eApplicationID::PWM___L2_CORR_S, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_CORR_B11_OnReleased=0;
const Command *const PUSHB_L2_CORR_B11_OnReleasedShort=0;
const Command *const PUSHB_L2_CORR_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_CORR_B11_OnReleasedLong=0;
eEventType *PUSHB_L2_CORR_B11_LocalEvents=0;
eEventType *PUSHB_L2_CORR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_CORR_B11("PUSHB_L2_CORR_B11", eApplicationID::PUSHB_L2_CORR_B11, eInput::I19, PUSHB_L2_CORR_B11_LocalEvents, 0, PUSHB_L2_CORR_B11_BusEvents, 0, PUSHB_L2_CORR_B11_OnPressed, 1, PUSHB_L2_CORR_B11_OnReleased, 0, PUSHB_L2_CORR_B11_OnReleasedShort, 0, PUSHB_L2_CORR_B11_OnPressedShortAndHold, 0, PUSHB_L2_CORR_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_CORR_B41
const Command PUSHB_L2_CORR_B41_OnPressed[1]={{eApplicationID::PWM___L2_CORR_S, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_CORR_B41_OnReleased=0;
const Command *const PUSHB_L2_CORR_B41_OnReleasedShort=0;
const Command *const PUSHB_L2_CORR_B41_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_CORR_B41_OnReleasedLong=0;
eEventType *PUSHB_L2_CORR_B41_LocalEvents=0;
eEventType *PUSHB_L2_CORR_B41_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_CORR_B41("PUSHB_L2_CORR_B41", eApplicationID::PUSHB_L2_CORR_B41, eInput::I20, PUSHB_L2_CORR_B41_LocalEvents, 0, PUSHB_L2_CORR_B41_BusEvents, 0, PUSHB_L2_CORR_B41_OnPressed, 1, PUSHB_L2_CORR_B41_OnReleased, 0, PUSHB_L2_CORR_B41_OnReleasedShort, 0, PUSHB_L2_CORR_B41_OnPressedShortAndHold, 0, PUSHB_L2_CORR_B41_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_CORR_B42
const Command PUSHB_L2_CORR_B42_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::UP},};
const Command *const PUSHB_L2_CORR_B42_OnReleased=0;
const Command *const PUSHB_L2_CORR_B42_OnReleasedShort=0;
const Command *const PUSHB_L2_CORR_B42_OnPressedShortAndHold=0;
const Command PUSHB_L2_CORR_B42_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_CORR_B42_LocalEvents=0;
eEventType *PUSHB_L2_CORR_B42_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_CORR_B42("PUSHB_L2_CORR_B42", eApplicationID::PUSHB_L2_CORR_B42, eInput::I21, PUSHB_L2_CORR_B42_LocalEvents, 0, PUSHB_L2_CORR_B42_BusEvents, 0, PUSHB_L2_CORR_B42_OnPressed, 1, PUSHB_L2_CORR_B42_OnReleased, 0, PUSHB_L2_CORR_B42_OnReleasedShort, 0, PUSHB_L2_CORR_B42_OnPressedShortAndHold, 0, PUSHB_L2_CORR_B42_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_CORR_B43
const Command PUSHB_L2_CORR_B43_OnPressed[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_CORR_B43_OnReleased=0;
const Command *const PUSHB_L2_CORR_B43_OnReleasedShort=0;
const Command *const PUSHB_L2_CORR_B43_OnPressedShortAndHold=0;
const Command PUSHB_L2_CORR_B43_OnReleasedLong[1]={{eApplicationID::BLIND_L2_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_CORR_B43_LocalEvents=0;
eEventType *PUSHB_L2_CORR_B43_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_CORR_B43("PUSHB_L2_CORR_B43", eApplicationID::PUSHB_L2_CORR_B43, eInput::I22, PUSHB_L2_CORR_B43_LocalEvents, 0, PUSHB_L2_CORR_B43_BusEvents, 0, PUSHB_L2_CORR_B43_OnPressed, 1, PUSHB_L2_CORR_B43_OnReleased, 0, PUSHB_L2_CORR_B43_OnReleasedShort, 0, PUSHB_L2_CORR_B43_OnPressedShortAndHold, 0, PUSHB_L2_CORR_B43_OnReleasedLong, 1);

// Blind BLIND_L2_CORR_J1
cBlind BLIND_L2_CORR_J1("BLIND_L2_CORR_J1", eApplicationID::BLIND_L2_CORR_J1, ePoweredOutput::O08, ePoweredOutput::O09, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L2_KID1_B11
const Command PUSHB_L2_KID1_B11_OnPressed[1]={{eApplicationID::POWIT_L2_KID1_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_KID1_B11_OnReleased=0;
const Command *const PUSHB_L2_KID1_B11_OnReleasedShort=0;
const Command *const PUSHB_L2_KID1_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_KID1_B11_OnReleasedLong=0;
eEventType *PUSHB_L2_KID1_B11_LocalEvents=0;
eEventType *PUSHB_L2_KID1_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID1_B11("PUSHB_L2_KID1_B11", eApplicationID::PUSHB_L2_KID1_B11, eInput::I23, PUSHB_L2_KID1_B11_LocalEvents, 0, PUSHB_L2_KID1_B11_BusEvents, 0, PUSHB_L2_KID1_B11_OnPressed, 1, PUSHB_L2_KID1_B11_OnReleased, 0, PUSHB_L2_KID1_B11_OnReleasedShort, 0, PUSHB_L2_KID1_B11_OnPressedShortAndHold, 0, PUSHB_L2_KID1_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_KID1_B12
const Command PUSHB_L2_KID1_B12_OnPressed[1]={{eApplicationID::BLIND_L2_KID1_J1, eCommandType::UP},};
const Command *const PUSHB_L2_KID1_B12_OnReleased=0;
const Command *const PUSHB_L2_KID1_B12_OnReleasedShort=0;
const Command *const PUSHB_L2_KID1_B12_OnPressedShortAndHold=0;
const Command PUSHB_L2_KID1_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L2_KID1_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_KID1_B12_LocalEvents=0;
eEventType *PUSHB_L2_KID1_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID1_B12("PUSHB_L2_KID1_B12", eApplicationID::PUSHB_L2_KID1_B12, eInput::I24, PUSHB_L2_KID1_B12_LocalEvents, 0, PUSHB_L2_KID1_B12_BusEvents, 0, PUSHB_L2_KID1_B12_OnPressed, 1, PUSHB_L2_KID1_B12_OnReleased, 0, PUSHB_L2_KID1_B12_OnReleasedShort, 0, PUSHB_L2_KID1_B12_OnPressedShortAndHold, 0, PUSHB_L2_KID1_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_KID1_B13
const Command PUSHB_L2_KID1_B13_OnPressed[1]={{eApplicationID::BLIND_L2_KID1_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_KID1_B13_OnReleased=0;
const Command *const PUSHB_L2_KID1_B13_OnReleasedShort=0;
const Command *const PUSHB_L2_KID1_B13_OnPressedShortAndHold=0;
const Command PUSHB_L2_KID1_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L2_KID1_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_KID1_B13_LocalEvents=0;
eEventType *PUSHB_L2_KID1_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID1_B13("PUSHB_L2_KID1_B13", eApplicationID::PUSHB_L2_KID1_B13, eInput::I25, PUSHB_L2_KID1_B13_LocalEvents, 0, PUSHB_L2_KID1_B13_BusEvents, 0, PUSHB_L2_KID1_B13_OnPressed, 1, PUSHB_L2_KID1_B13_OnReleased, 0, PUSHB_L2_KID1_B13_OnReleasedShort, 0, PUSHB_L2_KID1_B13_OnPressedShortAndHold, 0, PUSHB_L2_KID1_B13_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_KID1_B21
const Command PUSHB_L2_KID1_B21_OnPressed[1]={{eApplicationID::POWIT_L2_KID1_C2, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_KID1_B21_OnReleased=0;
const Command *const PUSHB_L2_KID1_B21_OnReleasedShort=0;
const Command *const PUSHB_L2_KID1_B21_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_KID1_B21_OnReleasedLong=0;
eEventType *PUSHB_L2_KID1_B21_LocalEvents=0;
eEventType *PUSHB_L2_KID1_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID1_B21("PUSHB_L2_KID1_B21", eApplicationID::PUSHB_L2_KID1_B21, eInput::I26, PUSHB_L2_KID1_B21_LocalEvents, 0, PUSHB_L2_KID1_B21_BusEvents, 0, PUSHB_L2_KID1_B21_OnPressed, 1, PUSHB_L2_KID1_B21_OnReleased, 0, PUSHB_L2_KID1_B21_OnReleasedShort, 0, PUSHB_L2_KID1_B21_OnPressedShortAndHold, 0, PUSHB_L2_KID1_B21_OnReleasedLong, 0);

// POWIT POWIT_L2_KID1_C1
sensact::cPoweritem POWIT_L2_KID1_C1("POWIT_L2_KID1_C1", eApplicationID::POWIT_L2_KID1_C1, ePoweredOutput::O10, 0);

// POWIT POWIT_L2_KID1_C2
sensact::cPoweritem POWIT_L2_KID1_C2("POWIT_L2_KID1_C2", eApplicationID::POWIT_L2_KID1_C2, ePoweredOutput::O11, 7200);

// Blind BLIND_L2_KID1_J1
cBlind BLIND_L2_KID1_J1("BLIND_L2_KID1_J1", eApplicationID::BLIND_L2_KID1_J1, ePoweredOutput::O12, ePoweredOutput::O13, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L2_KID2_B11
const Command PUSHB_L2_KID2_B11_OnPressed[1]={{eApplicationID::POWIT_L2_KID2_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L2_KID2_B11_OnReleased=0;
const Command *const PUSHB_L2_KID2_B11_OnReleasedShort=0;
const Command *const PUSHB_L2_KID2_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L2_KID2_B11_OnReleasedLong=0;
eEventType *PUSHB_L2_KID2_B11_LocalEvents=0;
eEventType *PUSHB_L2_KID2_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID2_B11("PUSHB_L2_KID2_B11", eApplicationID::PUSHB_L2_KID2_B11, eInput::I27, PUSHB_L2_KID2_B11_LocalEvents, 0, PUSHB_L2_KID2_B11_BusEvents, 0, PUSHB_L2_KID2_B11_OnPressed, 1, PUSHB_L2_KID2_B11_OnReleased, 0, PUSHB_L2_KID2_B11_OnReleasedShort, 0, PUSHB_L2_KID2_B11_OnPressedShortAndHold, 0, PUSHB_L2_KID2_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L2_KID2_B12
const Command PUSHB_L2_KID2_B12_OnPressed[1]={{eApplicationID::BLIND_L2_KID2_J1, eCommandType::UP},};
const Command *const PUSHB_L2_KID2_B12_OnReleased=0;
const Command *const PUSHB_L2_KID2_B12_OnReleasedShort=0;
const Command *const PUSHB_L2_KID2_B12_OnPressedShortAndHold=0;
const Command PUSHB_L2_KID2_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L2_KID2_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_KID2_B12_LocalEvents=0;
eEventType *PUSHB_L2_KID2_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID2_B12("PUSHB_L2_KID2_B12", eApplicationID::PUSHB_L2_KID2_B12, eInput::I28, PUSHB_L2_KID2_B12_LocalEvents, 0, PUSHB_L2_KID2_B12_BusEvents, 0, PUSHB_L2_KID2_B12_OnPressed, 1, PUSHB_L2_KID2_B12_OnReleased, 0, PUSHB_L2_KID2_B12_OnReleasedShort, 0, PUSHB_L2_KID2_B12_OnPressedShortAndHold, 0, PUSHB_L2_KID2_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_KID2_B13
const Command PUSHB_L2_KID2_B13_OnPressed[1]={{eApplicationID::BLIND_L2_KID2_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_KID2_B13_OnReleased=0;
const Command *const PUSHB_L2_KID2_B13_OnReleasedShort=0;
const Command *const PUSHB_L2_KID2_B13_OnPressedShortAndHold=0;
const Command PUSHB_L2_KID2_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L2_KID2_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_KID2_B13_LocalEvents=0;
eEventType *PUSHB_L2_KID2_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_KID2_B13("PUSHB_L2_KID2_B13", eApplicationID::PUSHB_L2_KID2_B13, eInput::I29, PUSHB_L2_KID2_B13_LocalEvents, 0, PUSHB_L2_KID2_B13_BusEvents, 0, PUSHB_L2_KID2_B13_OnPressed, 1, PUSHB_L2_KID2_B13_OnReleased, 0, PUSHB_L2_KID2_B13_OnReleasedShort, 0, PUSHB_L2_KID2_B13_OnPressedShortAndHold, 0, PUSHB_L2_KID2_B13_OnReleasedLong, 1);

// POWIT POWIT_L2_KID2_C1
sensact::cPoweritem POWIT_L2_KID2_C1("POWIT_L2_KID2_C1", eApplicationID::POWIT_L2_KID2_C1, ePoweredOutput::O14, 0);

// Blind BLIND_L2_KID2_J1
cBlind BLIND_L2_KID2_J1("BLIND_L2_KID2_J1", eApplicationID::BLIND_L2_KID2_J1, ePoweredOutput::O15, ePoweredOutput::O16, eRelayMode::INTERLOCKED, 40);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &PUSHB_L2_BATH_B13,
    &PUSHB_L2_BATH_B14,
    &BLIND_L2_BATH_J1,
    &PUSHB_L2_BEDR_B11,
    &PUSHB_L2_BEDR_B12,
    &PUSHB_L2_BEDR_B13,
    &PUSHB_L2_BEDR_B14,
    &PUSHB_L2_BEDR_B15,
    &PUSHB_L2_BEDR_B21,
    &PUSHB_L2_BEDR_B22,
    &PUSHB_L2_BEDR_B23,
    &PUSHB_L2_BEDR_B24,
    &PUSHB_L2_BEDR_B25,
    &PUSHB_L2_BEDR_B31,
    &PUSHB_L2_BEDR_B32,
    &PUSHB_L2_BEDR_B33,
    &PUSHB_L2_BEDR_B34,
    &PUSHB_L2_BEDR_B35,
    &PUSHB_L2_BEDR_B36,
    &BLIND_L2_BEDR_J1,
    &PWM___L2_BEDR_S,
    &POWIT_L2_BEDR_P1,
    &POWIT_L2_BEDR_P2,
    &POWIT_L2_BEDR_P3,
    &PUSHB_L2_CORR_B11,
    &PUSHB_L2_CORR_B41,
    &PUSHB_L2_CORR_B42,
    &PUSHB_L2_CORR_B43,
    &BLIND_L2_CORR_J1,
    &PUSHB_L2_KID1_B11,
    &PUSHB_L2_KID1_B12,
    &PUSHB_L2_KID1_B13,
    &PUSHB_L2_KID1_B21,
    &POWIT_L2_KID1_C1,
    &POWIT_L2_KID1_C2,
    &BLIND_L2_KID1_J1,
    &PUSHB_L2_KID2_B11,
    &PUSHB_L2_KID2_B12,
    &PUSHB_L2_KID2_B13,
    &POWIT_L2_KID2_C1,
    &BLIND_L2_KID2_J1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L3_TECH_HS07_2

eNodeID NODE = eNodeID::SNSAC_L3_TECH_HS07_2;

const char MODEL::ModelString[] ="NodeId SNSAC_L3_TECH_HS07_2 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// PushButtonX PUSHB_L2_WORK_B11
const Command *const PUSHB_L2_WORK_B11_OnPressed=0;
const Command *const PUSHB_L2_WORK_B11_OnReleased=0;
const Command PUSHB_L2_WORK_B11_OnReleasedShort[1]={{eApplicationID::PWM___L2_WORK_S, eCommandType::TOGGLE},};
const Command PUSHB_L2_WORK_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L2_WORK_S, eCommandType::START},};
const Command PUSHB_L2_WORK_B11_OnReleasedLong[1]={{eApplicationID::PWM___L2_WORK_S, eCommandType::STOP},};
eEventType *PUSHB_L2_WORK_B11_LocalEvents=0;
eEventType *PUSHB_L2_WORK_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_WORK_B11("PUSHB_L2_WORK_B11", eApplicationID::PUSHB_L2_WORK_B11, eInput::I01, PUSHB_L2_WORK_B11_LocalEvents, 0, PUSHB_L2_WORK_B11_BusEvents, 0, PUSHB_L2_WORK_B11_OnPressed, 0, PUSHB_L2_WORK_B11_OnReleased, 0, PUSHB_L2_WORK_B11_OnReleasedShort, 1, PUSHB_L2_WORK_B11_OnPressedShortAndHold, 1, PUSHB_L2_WORK_B11_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_WORK_B12
const Command PUSHB_L2_WORK_B12_OnPressed[1]={{eApplicationID::BLIND_L2_WORK_J1, eCommandType::UP},};
const Command *const PUSHB_L2_WORK_B12_OnReleased=0;
const Command *const PUSHB_L2_WORK_B12_OnReleasedShort=0;
const Command *const PUSHB_L2_WORK_B12_OnPressedShortAndHold=0;
const Command PUSHB_L2_WORK_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L2_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_WORK_B12_LocalEvents=0;
eEventType *PUSHB_L2_WORK_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_WORK_B12("PUSHB_L2_WORK_B12", eApplicationID::PUSHB_L2_WORK_B12, eInput::I02, PUSHB_L2_WORK_B12_LocalEvents, 0, PUSHB_L2_WORK_B12_BusEvents, 0, PUSHB_L2_WORK_B12_OnPressed, 1, PUSHB_L2_WORK_B12_OnReleased, 0, PUSHB_L2_WORK_B12_OnReleasedShort, 0, PUSHB_L2_WORK_B12_OnPressedShortAndHold, 0, PUSHB_L2_WORK_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L2_WORK_B13
const Command PUSHB_L2_WORK_B13_OnPressed[1]={{eApplicationID::BLIND_L2_WORK_J1, eCommandType::DOWN},};
const Command *const PUSHB_L2_WORK_B13_OnReleased=0;
const Command *const PUSHB_L2_WORK_B13_OnReleasedShort=0;
const Command *const PUSHB_L2_WORK_B13_OnPressedShortAndHold=0;
const Command PUSHB_L2_WORK_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L2_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L2_WORK_B13_LocalEvents=0;
eEventType *PUSHB_L2_WORK_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L2_WORK_B13("PUSHB_L2_WORK_B13", eApplicationID::PUSHB_L2_WORK_B13, eInput::I03, PUSHB_L2_WORK_B13_LocalEvents, 0, PUSHB_L2_WORK_B13_BusEvents, 0, PUSHB_L2_WORK_B13_OnPressed, 1, PUSHB_L2_WORK_B13_OnReleased, 0, PUSHB_L2_WORK_B13_OnReleasedShort, 0, PUSHB_L2_WORK_B13_OnPressedShortAndHold, 0, PUSHB_L2_WORK_B13_OnReleasedLong, 1);

// Blind BLIND_L2_WORK_J1
cBlind BLIND_L2_WORK_J1("BLIND_L2_WORK_J1", eApplicationID::BLIND_L2_WORK_J1, ePoweredOutput::O01, ePoweredOutput::O02, eRelayMode::INTERLOCKED, 40);

// PWM PWM___L2_WORK_S (Dimmer )
ePWMOutput PWM___L2_WORK_S_output[2]={ePWMOutput::P01,ePWMOutput::P02,};
sensact::cPWM PWM___L2_WORK_S("PWM___L2_WORK_S", eApplicationID::PWM___L2_WORK_S, PWM___L2_WORK_S_output, 2, 1, 255, false, eApplicationID::STDBY_L3_TECH_48V);

// PushButtonX PUSHB_L3_BEDR_B11
const Command PUSHB_L3_BEDR_B11_OnPressed[1]={{eApplicationID::POWIT_L2_KID2_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L3_BEDR_B11_OnReleased=0;
const Command *const PUSHB_L3_BEDR_B11_OnReleasedShort=0;
const Command *const PUSHB_L3_BEDR_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L3_BEDR_B11_OnReleasedLong=0;
eEventType *PUSHB_L3_BEDR_B11_LocalEvents=0;
eEventType *PUSHB_L3_BEDR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_BEDR_B11("PUSHB_L3_BEDR_B11", eApplicationID::PUSHB_L3_BEDR_B11, eInput::I27, PUSHB_L3_BEDR_B11_LocalEvents, 0, PUSHB_L3_BEDR_B11_BusEvents, 0, PUSHB_L3_BEDR_B11_OnPressed, 1, PUSHB_L3_BEDR_B11_OnReleased, 0, PUSHB_L3_BEDR_B11_OnReleasedShort, 0, PUSHB_L3_BEDR_B11_OnPressedShortAndHold, 0, PUSHB_L3_BEDR_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L3_BEDR_B12
const Command PUSHB_L3_BEDR_B12_OnPressed[1]={{eApplicationID::BLIND_L3_BEDR_J1, eCommandType::UP},};
const Command *const PUSHB_L3_BEDR_B12_OnReleased=0;
const Command *const PUSHB_L3_BEDR_B12_OnReleasedShort=0;
const Command *const PUSHB_L3_BEDR_B12_OnPressedShortAndHold=0;
const Command PUSHB_L3_BEDR_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L3_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L3_BEDR_B12_LocalEvents=0;
eEventType *PUSHB_L3_BEDR_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_BEDR_B12("PUSHB_L3_BEDR_B12", eApplicationID::PUSHB_L3_BEDR_B12, eInput::I04, PUSHB_L3_BEDR_B12_LocalEvents, 0, PUSHB_L3_BEDR_B12_BusEvents, 0, PUSHB_L3_BEDR_B12_OnPressed, 1, PUSHB_L3_BEDR_B12_OnReleased, 0, PUSHB_L3_BEDR_B12_OnReleasedShort, 0, PUSHB_L3_BEDR_B12_OnPressedShortAndHold, 0, PUSHB_L3_BEDR_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L3_BEDR_B13
const Command PUSHB_L3_BEDR_B13_OnPressed[1]={{eApplicationID::BLIND_L3_BEDR_J1, eCommandType::DOWN},};
const Command *const PUSHB_L3_BEDR_B13_OnReleased=0;
const Command *const PUSHB_L3_BEDR_B13_OnReleasedShort=0;
const Command *const PUSHB_L3_BEDR_B13_OnPressedShortAndHold=0;
const Command PUSHB_L3_BEDR_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L3_BEDR_J1, eCommandType::STOP},};
eEventType *PUSHB_L3_BEDR_B13_LocalEvents=0;
eEventType *PUSHB_L3_BEDR_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_BEDR_B13("PUSHB_L3_BEDR_B13", eApplicationID::PUSHB_L3_BEDR_B13, eInput::I05, PUSHB_L3_BEDR_B13_LocalEvents, 0, PUSHB_L3_BEDR_B13_BusEvents, 0, PUSHB_L3_BEDR_B13_OnPressed, 1, PUSHB_L3_BEDR_B13_OnReleased, 0, PUSHB_L3_BEDR_B13_OnReleasedShort, 0, PUSHB_L3_BEDR_B13_OnPressedShortAndHold, 0, PUSHB_L3_BEDR_B13_OnReleasedLong, 1);

// POWIT POWIT_L3_BEDR_C1
sensact::cPoweritem POWIT_L3_BEDR_C1("POWIT_L3_BEDR_C1", eApplicationID::POWIT_L3_BEDR_C1, ePoweredOutput::O03, 0);

// Blind BLIND_L3_BEDR_J1
cBlind BLIND_L3_BEDR_J1("BLIND_L3_BEDR_J1", eApplicationID::BLIND_L3_BEDR_J1, ePoweredOutput::O04, ePoweredOutput::O05, eRelayMode::INTERLOCKED, 40);

// PushButtonX PUSHB_L3_TECH_B11
const Command PUSHB_L3_TECH_B11_OnPressed[1]={{eApplicationID::POWIT_L3_TECH_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L3_TECH_B11_OnReleased=0;
const Command *const PUSHB_L3_TECH_B11_OnReleasedShort=0;
const Command *const PUSHB_L3_TECH_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_L3_TECH_B11_OnReleasedLong=0;
eEventType *PUSHB_L3_TECH_B11_LocalEvents=0;
eEventType *PUSHB_L3_TECH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_TECH_B11("PUSHB_L3_TECH_B11", eApplicationID::PUSHB_L3_TECH_B11, eInput::I06, PUSHB_L3_TECH_B11_LocalEvents, 0, PUSHB_L3_TECH_B11_BusEvents, 0, PUSHB_L3_TECH_B11_OnPressed, 1, PUSHB_L3_TECH_B11_OnReleased, 0, PUSHB_L3_TECH_B11_OnReleasedShort, 0, PUSHB_L3_TECH_B11_OnPressedShortAndHold, 0, PUSHB_L3_TECH_B11_OnReleasedLong, 0);

// POWIT POWIT_L3_TECH_C1
sensact::cPoweritem POWIT_L3_TECH_C1("POWIT_L3_TECH_C1", eApplicationID::POWIT_L3_TECH_C1, ePoweredOutput::O06, 0);

// RotaryEncoder ROTAR_L3_WORK_B11
const Command ROTAR_L3_WORK_B11_OnPressed[1]={{eApplicationID::PWM___L3_WORK_S, eCommandType::TOGGLE},};
const Command *const ROTAR_L3_WORK_B11_OnShortReleased=0;
const Command *const ROTAR_L3_WORK_B11_OnLongReleased=0;
const Command ROTAR_L3_WORK_B11_OnTurned[1]={{eApplicationID::PWM___L3_WORK_S, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L3_WORK_B11_LocalEvents=0;
eEventType *ROTAR_L3_WORK_B11_BusEvents=0;
sensact::cROTAR ROTAR_L3_WORK_B11("ROTAR_L3_WORK_B11", eApplicationID::ROTAR_L3_WORK_B11, eRotaryEncoder::ROTARYENCODER_1, eInput::ROTAR_PUSH_1, ROTAR_L3_WORK_B11_LocalEvents, 0, ROTAR_L3_WORK_B11_BusEvents, 0, ROTAR_L3_WORK_B11_OnPressed, 1, ROTAR_L3_WORK_B11_OnShortReleased, 0, ROTAR_L3_WORK_B11_OnLongReleased, 0, ROTAR_L3_WORK_B11_OnTurned, 1 );

// PushButtonX PUSHB_L3_WORK_B12
const Command PUSHB_L3_WORK_B12_OnPressed[1]={{eApplicationID::BLIND_L3_WORK_J1, eCommandType::UP},};
const Command *const PUSHB_L3_WORK_B12_OnReleased=0;
const Command *const PUSHB_L3_WORK_B12_OnReleasedShort=0;
const Command *const PUSHB_L3_WORK_B12_OnPressedShortAndHold=0;
const Command PUSHB_L3_WORK_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L3_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L3_WORK_B12_LocalEvents=0;
eEventType *PUSHB_L3_WORK_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_WORK_B12("PUSHB_L3_WORK_B12", eApplicationID::PUSHB_L3_WORK_B12, eInput::I07, PUSHB_L3_WORK_B12_LocalEvents, 0, PUSHB_L3_WORK_B12_BusEvents, 0, PUSHB_L3_WORK_B12_OnPressed, 1, PUSHB_L3_WORK_B12_OnReleased, 0, PUSHB_L3_WORK_B12_OnReleasedShort, 0, PUSHB_L3_WORK_B12_OnPressedShortAndHold, 0, PUSHB_L3_WORK_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L3_WORK_B13
const Command PUSHB_L3_WORK_B13_OnPressed[1]={{eApplicationID::BLIND_L3_WORK_J1, eCommandType::DOWN},};
const Command *const PUSHB_L3_WORK_B13_OnReleased=0;
const Command *const PUSHB_L3_WORK_B13_OnReleasedShort=0;
const Command *const PUSHB_L3_WORK_B13_OnPressedShortAndHold=0;
const Command PUSHB_L3_WORK_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L3_WORK_J1, eCommandType::STOP},};
eEventType *PUSHB_L3_WORK_B13_LocalEvents=0;
eEventType *PUSHB_L3_WORK_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_WORK_B13("PUSHB_L3_WORK_B13", eApplicationID::PUSHB_L3_WORK_B13, eInput::I08, PUSHB_L3_WORK_B13_LocalEvents, 0, PUSHB_L3_WORK_B13_BusEvents, 0, PUSHB_L3_WORK_B13_OnPressed, 1, PUSHB_L3_WORK_B13_OnReleased, 0, PUSHB_L3_WORK_B13_OnReleasedShort, 0, PUSHB_L3_WORK_B13_OnPressedShortAndHold, 0, PUSHB_L3_WORK_B13_OnReleasedLong, 1);

// PushButtonX PUSHB_L3_WORK_B14
const Command PUSHB_L3_WORK_B14_OnPressed[1]={{eApplicationID::BLIND_L3_WORK_J2, eCommandType::UP},};
const Command *const PUSHB_L3_WORK_B14_OnReleased=0;
const Command *const PUSHB_L3_WORK_B14_OnReleasedShort=0;
const Command *const PUSHB_L3_WORK_B14_OnPressedShortAndHold=0;
const Command PUSHB_L3_WORK_B14_OnReleasedLong[1]={{eApplicationID::BLIND_L3_WORK_J2, eCommandType::STOP},};
eEventType *PUSHB_L3_WORK_B14_LocalEvents=0;
eEventType *PUSHB_L3_WORK_B14_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_WORK_B14("PUSHB_L3_WORK_B14", eApplicationID::PUSHB_L3_WORK_B14, eInput::I09, PUSHB_L3_WORK_B14_LocalEvents, 0, PUSHB_L3_WORK_B14_BusEvents, 0, PUSHB_L3_WORK_B14_OnPressed, 1, PUSHB_L3_WORK_B14_OnReleased, 0, PUSHB_L3_WORK_B14_OnReleasedShort, 0, PUSHB_L3_WORK_B14_OnPressedShortAndHold, 0, PUSHB_L3_WORK_B14_OnReleasedLong, 1);

// PushButtonX PUSHB_L3_WORK_B15
const Command PUSHB_L3_WORK_B15_OnPressed[1]={{eApplicationID::BLIND_L3_WORK_J2, eCommandType::DOWN},};
const Command *const PUSHB_L3_WORK_B15_OnReleased=0;
const Command *const PUSHB_L3_WORK_B15_OnReleasedShort=0;
const Command *const PUSHB_L3_WORK_B15_OnPressedShortAndHold=0;
const Command PUSHB_L3_WORK_B15_OnReleasedLong[1]={{eApplicationID::BLIND_L3_WORK_J2, eCommandType::STOP},};
eEventType *PUSHB_L3_WORK_B15_LocalEvents=0;
eEventType *PUSHB_L3_WORK_B15_BusEvents=0;
sensact::cPushbuttonX PUSHB_L3_WORK_B15("PUSHB_L3_WORK_B15", eApplicationID::PUSHB_L3_WORK_B15, eInput::I10, PUSHB_L3_WORK_B15_LocalEvents, 0, PUSHB_L3_WORK_B15_BusEvents, 0, PUSHB_L3_WORK_B15_OnPressed, 1, PUSHB_L3_WORK_B15_OnReleased, 0, PUSHB_L3_WORK_B15_OnReleasedShort, 0, PUSHB_L3_WORK_B15_OnPressedShortAndHold, 0, PUSHB_L3_WORK_B15_OnReleasedLong, 1);

// Blind BLIND_L3_WORK_J1
cBlind BLIND_L3_WORK_J1("BLIND_L3_WORK_J1", eApplicationID::BLIND_L3_WORK_J1, ePoweredOutput::O07, ePoweredOutput::O08, eRelayMode::INTERLOCKED, 40);

// Blind BLIND_L3_WORK_J2
cBlind BLIND_L3_WORK_J2("BLIND_L3_WORK_J2", eApplicationID::BLIND_L3_WORK_J2, ePoweredOutput::O09, ePoweredOutput::O10, eRelayMode::INTERLOCKED, 40);

// PWM PWM___L3_WORK_S (Dimmer )
ePWMOutput PWM___L3_WORK_S_output[3]={ePWMOutput::P03,ePWMOutput::P04,ePWMOutput::P05,};
sensact::cPWM PWM___L3_WORK_S("PWM___L3_WORK_S", eApplicationID::PWM___L3_WORK_S, PWM___L3_WORK_S_output, 3, 1, 255, false, eApplicationID::STDBY_L3_TECH_48V);

// PushButtonX PUSHB_LS_STRS_B11
const Command PUSHB_LS_STRS_B11_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W2, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B11_OnReleased=0;
const Command *const PUSHB_LS_STRS_B11_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B11_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B11_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B11_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B11("PUSHB_LS_STRS_B11", eApplicationID::PUSHB_LS_STRS_B11, eInput::II01, PUSHB_LS_STRS_B11_LocalEvents, 0, PUSHB_LS_STRS_B11_BusEvents, 0, PUSHB_LS_STRS_B11_OnPressed, 1, PUSHB_LS_STRS_B11_OnReleased, 0, PUSHB_LS_STRS_B11_OnReleasedShort, 0, PUSHB_LS_STRS_B11_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B21
const Command PUSHB_LS_STRS_B21_OnPressed[1]={{eApplicationID::POWIT_LS_STRS_W3, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B21_OnReleased=0;
const Command *const PUSHB_LS_STRS_B21_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B21_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B21_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B21_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B21("PUSHB_LS_STRS_B21", eApplicationID::PUSHB_LS_STRS_B21, eInput::I16, PUSHB_LS_STRS_B21_LocalEvents, 0, PUSHB_LS_STRS_B21_BusEvents, 0, PUSHB_LS_STRS_B21_OnPressed, 1, PUSHB_LS_STRS_B21_OnReleased, 0, PUSHB_LS_STRS_B21_OnReleasedShort, 0, PUSHB_LS_STRS_B21_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B21_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B22
const Command PUSHB_LS_STRS_B22_OnPressed[1]={{eApplicationID::PWM___L1_CORR_S, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B22_OnReleased=0;
const Command *const PUSHB_LS_STRS_B22_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B22_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B22_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B22_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B22("PUSHB_LS_STRS_B22", eApplicationID::PUSHB_LS_STRS_B22, eInput::I17, PUSHB_LS_STRS_B22_LocalEvents, 0, PUSHB_LS_STRS_B22_BusEvents, 0, PUSHB_LS_STRS_B22_OnPressed, 1, PUSHB_LS_STRS_B22_OnReleased, 0, PUSHB_LS_STRS_B22_OnReleasedShort, 0, PUSHB_LS_STRS_B22_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B22_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B23
const Command PUSHB_LS_STRS_B23_OnPressed[2]={{eApplicationID::POWIT_L0_CORR_C1, eCommandType::TOGGLE},{eApplicationID::POWIT_LS_STRS_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B23_OnReleased=0;
const Command *const PUSHB_LS_STRS_B23_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B23_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B23_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B23("PUSHB_LS_STRS_B23", eApplicationID::PUSHB_LS_STRS_B23, eInput::I18, PUSHB_LS_STRS_B23_LocalEvents, 0, PUSHB_LS_STRS_B23_BusEvents, 0, PUSHB_LS_STRS_B23_OnPressed, 2, PUSHB_LS_STRS_B23_OnReleased, 0, PUSHB_LS_STRS_B23_OnReleasedShort, 0, PUSHB_LS_STRS_B23_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B23_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B31
const Command PUSHB_LS_STRS_B31_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W2, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B31_OnReleased=0;
const Command *const PUSHB_LS_STRS_B31_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B31_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B31_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B31_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B31_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B31("PUSHB_LS_STRS_B31", eApplicationID::PUSHB_LS_STRS_B31, eInput::II02, PUSHB_LS_STRS_B31_LocalEvents, 0, PUSHB_LS_STRS_B31_BusEvents, 0, PUSHB_LS_STRS_B31_OnPressed, 1, PUSHB_LS_STRS_B31_OnReleased, 0, PUSHB_LS_STRS_B31_OnReleasedShort, 0, PUSHB_LS_STRS_B31_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B31_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B32
const Command PUSHB_LS_STRS_B32_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W4, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B32_OnReleased=0;
const Command *const PUSHB_LS_STRS_B32_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B32_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B32_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B32_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B32_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B32("PUSHB_LS_STRS_B32", eApplicationID::PUSHB_LS_STRS_B32, eInput::II03, PUSHB_LS_STRS_B32_LocalEvents, 0, PUSHB_LS_STRS_B32_BusEvents, 0, PUSHB_LS_STRS_B32_OnPressed, 1, PUSHB_LS_STRS_B32_OnReleased, 0, PUSHB_LS_STRS_B32_OnReleasedShort, 0, PUSHB_LS_STRS_B32_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B32_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B41
const Command PUSHB_LS_STRS_B41_OnPressed[1]={{eApplicationID::POWIT_LS_STRS_W5, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B41_OnReleased=0;
const Command *const PUSHB_LS_STRS_B41_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B41_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B41_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B41_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B41_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B41("PUSHB_LS_STRS_B41", eApplicationID::PUSHB_LS_STRS_B41, eInput::I19, PUSHB_LS_STRS_B41_LocalEvents, 0, PUSHB_LS_STRS_B41_BusEvents, 0, PUSHB_LS_STRS_B41_OnPressed, 1, PUSHB_LS_STRS_B41_OnReleased, 0, PUSHB_LS_STRS_B41_OnReleasedShort, 0, PUSHB_LS_STRS_B41_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B41_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B42
const Command PUSHB_LS_STRS_B42_OnPressed[1]={{eApplicationID::PWM___L2_CORR_S, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B42_OnReleased=0;
const Command *const PUSHB_LS_STRS_B42_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B42_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B42_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B42_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B42_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B42("PUSHB_LS_STRS_B42", eApplicationID::PUSHB_LS_STRS_B42, eInput::I20, PUSHB_LS_STRS_B42_LocalEvents, 0, PUSHB_LS_STRS_B42_BusEvents, 0, PUSHB_LS_STRS_B42_OnPressed, 1, PUSHB_LS_STRS_B42_OnReleased, 0, PUSHB_LS_STRS_B42_OnReleasedShort, 0, PUSHB_LS_STRS_B42_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B42_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B43
const Command PUSHB_LS_STRS_B43_OnPressed[1]={{eApplicationID::POWIT_LS_STRS_W3, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B43_OnReleased=0;
const Command *const PUSHB_LS_STRS_B43_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B43_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B43_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B43_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B43_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B43("PUSHB_LS_STRS_B43", eApplicationID::PUSHB_LS_STRS_B43, eInput::I21, PUSHB_LS_STRS_B43_LocalEvents, 0, PUSHB_LS_STRS_B43_BusEvents, 0, PUSHB_LS_STRS_B43_OnPressed, 1, PUSHB_LS_STRS_B43_OnReleased, 0, PUSHB_LS_STRS_B43_OnReleasedShort, 0, PUSHB_LS_STRS_B43_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B43_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B51
const Command PUSHB_LS_STRS_B51_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W4, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B51_OnReleased=0;
const Command *const PUSHB_LS_STRS_B51_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B51_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B51_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B51_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B51_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B51("PUSHB_LS_STRS_B51", eApplicationID::PUSHB_LS_STRS_B51, eInput::II04, PUSHB_LS_STRS_B51_LocalEvents, 0, PUSHB_LS_STRS_B51_BusEvents, 0, PUSHB_LS_STRS_B51_OnPressed, 1, PUSHB_LS_STRS_B51_OnReleased, 0, PUSHB_LS_STRS_B51_OnReleasedShort, 0, PUSHB_LS_STRS_B51_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B51_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B52
const Command PUSHB_LS_STRS_B52_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W6, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B52_OnReleased=0;
const Command *const PUSHB_LS_STRS_B52_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B52_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B52_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B52_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B52_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B52("PUSHB_LS_STRS_B52", eApplicationID::PUSHB_LS_STRS_B52, eInput::II05, PUSHB_LS_STRS_B52_LocalEvents, 0, PUSHB_LS_STRS_B52_BusEvents, 0, PUSHB_LS_STRS_B52_OnPressed, 1, PUSHB_LS_STRS_B52_OnReleased, 0, PUSHB_LS_STRS_B52_OnReleasedShort, 0, PUSHB_LS_STRS_B52_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B52_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B61
const Command PUSHB_LS_STRS_B61_OnPressed[2]={{eApplicationID::PWM___LS_STRS_S, eCommandType::TOGGLE},{eApplicationID::POWIT_LS_STRS_W5, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B61_OnReleased=0;
const Command *const PUSHB_LS_STRS_B61_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B61_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B61_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B61_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B61_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B61("PUSHB_LS_STRS_B61", eApplicationID::PUSHB_LS_STRS_B61, eInput::I22, PUSHB_LS_STRS_B61_LocalEvents, 0, PUSHB_LS_STRS_B61_BusEvents, 0, PUSHB_LS_STRS_B61_OnPressed, 2, PUSHB_LS_STRS_B61_OnReleased, 0, PUSHB_LS_STRS_B61_OnReleasedShort, 0, PUSHB_LS_STRS_B61_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B61_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B62
const Command PUSHB_LS_STRS_B62_OnPressed[2]={{eApplicationID::PWM___LS_STRS_S, eCommandType::TOGGLE},{eApplicationID::POWIT_LS_STRS_W5, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B62_OnReleased=0;
const Command *const PUSHB_LS_STRS_B62_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B62_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B62_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B62_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B62_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B62("PUSHB_LS_STRS_B62", eApplicationID::PUSHB_LS_STRS_B62, eInput::I23, PUSHB_LS_STRS_B62_LocalEvents, 0, PUSHB_LS_STRS_B62_BusEvents, 0, PUSHB_LS_STRS_B62_OnPressed, 2, PUSHB_LS_STRS_B62_OnReleased, 0, PUSHB_LS_STRS_B62_OnReleasedShort, 0, PUSHB_LS_STRS_B62_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B62_OnReleasedLong, 0);

// PushButtonX PUSHB_LS_STRS_B71
const Command PUSHB_LS_STRS_B71_OnPressed[1]={{eApplicationID::PWM___LS_STRS_W6, eCommandType::TOGGLE},};
const Command *const PUSHB_LS_STRS_B71_OnReleased=0;
const Command *const PUSHB_LS_STRS_B71_OnReleasedShort=0;
const Command *const PUSHB_LS_STRS_B71_OnPressedShortAndHold=0;
const Command *const PUSHB_LS_STRS_B71_OnReleasedLong=0;
eEventType *PUSHB_LS_STRS_B71_LocalEvents=0;
eEventType *PUSHB_LS_STRS_B71_BusEvents=0;
sensact::cPushbuttonX PUSHB_LS_STRS_B71("PUSHB_LS_STRS_B71", eApplicationID::PUSHB_LS_STRS_B71, eInput::II06, PUSHB_LS_STRS_B71_LocalEvents, 0, PUSHB_LS_STRS_B71_BusEvents, 0, PUSHB_LS_STRS_B71_OnPressed, 1, PUSHB_LS_STRS_B71_OnReleased, 0, PUSHB_LS_STRS_B71_OnReleasedShort, 0, PUSHB_LS_STRS_B71_OnPressedShortAndHold, 0, PUSHB_LS_STRS_B71_OnReleasedLong, 0);

// PWM PWM___LS_STRS_S (Dimmer )
ePWMOutput PWM___LS_STRS_S_output[1]={ePWMOutput::P06,};
sensact::cPWM PWM___LS_STRS_S("PWM___LS_STRS_S", eApplicationID::PWM___LS_STRS_S, PWM___LS_STRS_S_output, 1, 1, 255, false, eApplicationID::STDBY_L3_TECH_48V);

// POWIT POWIT_LS_STRS_W1
sensact::cPoweritem POWIT_LS_STRS_W1("POWIT_LS_STRS_W1", eApplicationID::POWIT_LS_STRS_W1, ePoweredOutput::O11, 0);

// POWIT POWIT_LS_STRS_W3
sensact::cPoweritem POWIT_LS_STRS_W3("POWIT_LS_STRS_W3", eApplicationID::POWIT_LS_STRS_W3, ePoweredOutput::O12, 0);

// POWIT POWIT_LS_STRS_W5
sensact::cPoweritem POWIT_LS_STRS_W5("POWIT_LS_STRS_W5", eApplicationID::POWIT_LS_STRS_W5, ePoweredOutput::O13, 0);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &PUSHB_L2_WORK_B11,
    &PUSHB_L2_WORK_B12,
    &PUSHB_L2_WORK_B13,
    &BLIND_L2_WORK_J1,
    &PWM___L2_WORK_S,
    &PUSHB_L3_BEDR_B11,
    &PUSHB_L3_BEDR_B12,
    &PUSHB_L3_BEDR_B13,
    &POWIT_L3_BEDR_C1,
    &BLIND_L3_BEDR_J1,
    &PUSHB_L3_TECH_B11,
    &POWIT_L3_TECH_C1,
    &ROTAR_L3_WORK_B11,
    &PUSHB_L3_WORK_B12,
    &PUSHB_L3_WORK_B13,
    &PUSHB_L3_WORK_B14,
    &PUSHB_L3_WORK_B15,
    &BLIND_L3_WORK_J1,
    &BLIND_L3_WORK_J2,
    &PWM___L3_WORK_S,
    &PUSHB_LS_STRS_B11,
    &PUSHB_LS_STRS_B21,
    &PUSHB_LS_STRS_B22,
    &PUSHB_LS_STRS_B23,
    &PUSHB_LS_STRS_B31,
    &PUSHB_LS_STRS_B32,
    &PUSHB_LS_STRS_B41,
    &PUSHB_LS_STRS_B42,
    &PUSHB_LS_STRS_B43,
    &PUSHB_LS_STRS_B51,
    &PUSHB_LS_STRS_B52,
    &PUSHB_LS_STRS_B61,
    &PUSHB_LS_STRS_B62,
    &PUSHB_LS_STRS_B71,
    &PWM___LS_STRS_S,
    &POWIT_LS_STRS_W1,
    &POWIT_LS_STRS_W3,
    &POWIT_LS_STRS_W5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L1_KTCH_UP02_1

eNodeID NODE = eNodeID::SNSAC_L1_KTCH_UP02_1;

const char MODEL::ModelString[] ="NodeId SNSAC_L1_KTCH_UP02_1 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// RotaryEncoder ROTAR_L1_KTCH_B21
const Command ROTAR_L1_KTCH_B21_OnPressed[1]={{eApplicationID::PWM___L1_KTCH_S1, eCommandType::TOGGLE},};
const Command *const ROTAR_L1_KTCH_B21_OnShortReleased=0;
const Command *const ROTAR_L1_KTCH_B21_OnLongReleased=0;
const Command ROTAR_L1_KTCH_B21_OnTurned[1]={{eApplicationID::PWM___L1_KTCH_S1, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L1_KTCH_B21_LocalEvents=0;
eEventType *ROTAR_L1_KTCH_B21_BusEvents=0;
sensact::cROTAR ROTAR_L1_KTCH_B21("ROTAR_L1_KTCH_B21", eApplicationID::ROTAR_L1_KTCH_B21, eRotaryEncoder::ROTARYENCODER_1, eInput::ROTAR_PUSH_1, ROTAR_L1_KTCH_B21_LocalEvents, 0, ROTAR_L1_KTCH_B21_BusEvents, 0, ROTAR_L1_KTCH_B21_OnPressed, 1, ROTAR_L1_KTCH_B21_OnShortReleased, 0, ROTAR_L1_KTCH_B21_OnLongReleased, 0, ROTAR_L1_KTCH_B21_OnTurned, 1 );

// RotaryEncoder ROTAR_L1_KTCH_B22
const Command ROTAR_L1_KTCH_B22_OnPressed[1]={{eApplicationID::PWM___L1_KTCH_S2, eCommandType::TOGGLE},};
const Command *const ROTAR_L1_KTCH_B22_OnShortReleased=0;
const Command *const ROTAR_L1_KTCH_B22_OnLongReleased=0;
const Command ROTAR_L1_KTCH_B22_OnTurned[1]={{eApplicationID::PWM___L1_KTCH_S2, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L1_KTCH_B22_LocalEvents=0;
eEventType *ROTAR_L1_KTCH_B22_BusEvents=0;
sensact::cROTAR ROTAR_L1_KTCH_B22("ROTAR_L1_KTCH_B22", eApplicationID::ROTAR_L1_KTCH_B22, eRotaryEncoder::ROTARYENCODER_2, eInput::ROTAR_PUSH_2, ROTAR_L1_KTCH_B22_LocalEvents, 0, ROTAR_L1_KTCH_B22_BusEvents, 0, ROTAR_L1_KTCH_B22_OnPressed, 1, ROTAR_L1_KTCH_B22_OnShortReleased, 0, ROTAR_L1_KTCH_B22_OnLongReleased, 0, ROTAR_L1_KTCH_B22_OnTurned, 1 );

// PushButtonX PUSHB_L1_KTCH_B23
const Command PUSHB_L1_KTCH_B23_OnPressed[1]={{eApplicationID::POWIT_L1_KTCH_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_KTCH_B23_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B23_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B23_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_KTCH_B23_OnReleasedLong=0;
eEventType *PUSHB_L1_KTCH_B23_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B23_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B23("PUSHB_L1_KTCH_B23", eApplicationID::PUSHB_L1_KTCH_B23, eInput::I01, PUSHB_L1_KTCH_B23_LocalEvents, 0, PUSHB_L1_KTCH_B23_BusEvents, 0, PUSHB_L1_KTCH_B23_OnPressed, 1, PUSHB_L1_KTCH_B23_OnReleased, 0, PUSHB_L1_KTCH_B23_OnReleasedShort, 0, PUSHB_L1_KTCH_B23_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B23_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_KTCH_B24
const Command PUSHB_L1_KTCH_B24_OnPressed[1]={{eApplicationID::POWIT_L1_KTCH_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_KTCH_B24_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B24_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B24_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_KTCH_B24_OnReleasedLong=0;
eEventType *PUSHB_L1_KTCH_B24_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B24_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B24("PUSHB_L1_KTCH_B24", eApplicationID::PUSHB_L1_KTCH_B24, eInput::I02, PUSHB_L1_KTCH_B24_LocalEvents, 0, PUSHB_L1_KTCH_B24_BusEvents, 0, PUSHB_L1_KTCH_B24_OnPressed, 1, PUSHB_L1_KTCH_B24_OnReleased, 0, PUSHB_L1_KTCH_B24_OnReleasedShort, 0, PUSHB_L1_KTCH_B24_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B24_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_KTCH_B25
const Command PUSHB_L1_KTCH_B25_OnPressed[1]={{eApplicationID::BLIND_L1_KTCH_J1, eCommandType::UP},};
const Command *const PUSHB_L1_KTCH_B25_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B25_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B25_OnPressedShortAndHold=0;
const Command PUSHB_L1_KTCH_B25_OnReleasedLong[1]={{eApplicationID::BLIND_L1_KTCH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_KTCH_B25_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B25_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B25("PUSHB_L1_KTCH_B25", eApplicationID::PUSHB_L1_KTCH_B25, eInput::I03, PUSHB_L1_KTCH_B25_LocalEvents, 0, PUSHB_L1_KTCH_B25_BusEvents, 0, PUSHB_L1_KTCH_B25_OnPressed, 1, PUSHB_L1_KTCH_B25_OnReleased, 0, PUSHB_L1_KTCH_B25_OnReleasedShort, 0, PUSHB_L1_KTCH_B25_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B25_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_KTCH_B26
const Command PUSHB_L1_KTCH_B26_OnPressed[1]={{eApplicationID::BLIND_L1_KTCH_J1, eCommandType::DOWN},};
const Command *const PUSHB_L1_KTCH_B26_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B26_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B26_OnPressedShortAndHold=0;
const Command PUSHB_L1_KTCH_B26_OnReleasedLong[1]={{eApplicationID::BLIND_L1_KTCH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_KTCH_B26_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B26_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B26("PUSHB_L1_KTCH_B26", eApplicationID::PUSHB_L1_KTCH_B26, eInput::I04, PUSHB_L1_KTCH_B26_LocalEvents, 0, PUSHB_L1_KTCH_B26_BusEvents, 0, PUSHB_L1_KTCH_B26_OnPressed, 1, PUSHB_L1_KTCH_B26_OnReleased, 0, PUSHB_L1_KTCH_B26_OnReleasedShort, 0, PUSHB_L1_KTCH_B26_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B26_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_KTCH_B27
const Command PUSHB_L1_KTCH_B27_OnPressed[1]={{eApplicationID::BLIND_L1_KTCH_J2, eCommandType::UP},};
const Command *const PUSHB_L1_KTCH_B27_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B27_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B27_OnPressedShortAndHold=0;
const Command PUSHB_L1_KTCH_B27_OnReleasedLong[1]={{eApplicationID::BLIND_L1_KTCH_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_KTCH_B27_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B27_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B27("PUSHB_L1_KTCH_B27", eApplicationID::PUSHB_L1_KTCH_B27, eInput::I05, PUSHB_L1_KTCH_B27_LocalEvents, 0, PUSHB_L1_KTCH_B27_BusEvents, 0, PUSHB_L1_KTCH_B27_OnPressed, 1, PUSHB_L1_KTCH_B27_OnReleased, 0, PUSHB_L1_KTCH_B27_OnReleasedShort, 0, PUSHB_L1_KTCH_B27_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B27_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_KTCH_B28
const Command PUSHB_L1_KTCH_B28_OnPressed[1]={{eApplicationID::BLIND_L1_KTCH_J2, eCommandType::DOWN},};
const Command *const PUSHB_L1_KTCH_B28_OnReleased=0;
const Command *const PUSHB_L1_KTCH_B28_OnReleasedShort=0;
const Command *const PUSHB_L1_KTCH_B28_OnPressedShortAndHold=0;
const Command PUSHB_L1_KTCH_B28_OnReleasedLong[1]={{eApplicationID::BLIND_L1_KTCH_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_KTCH_B28_LocalEvents=0;
eEventType *PUSHB_L1_KTCH_B28_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_KTCH_B28("PUSHB_L1_KTCH_B28", eApplicationID::PUSHB_L1_KTCH_B28, eInput::I06, PUSHB_L1_KTCH_B28_LocalEvents, 0, PUSHB_L1_KTCH_B28_BusEvents, 0, PUSHB_L1_KTCH_B28_OnPressed, 1, PUSHB_L1_KTCH_B28_OnReleased, 0, PUSHB_L1_KTCH_B28_OnReleasedShort, 0, PUSHB_L1_KTCH_B28_OnPressedShortAndHold, 0, PUSHB_L1_KTCH_B28_OnReleasedLong, 1);

// PWM PWM___L1_KTCH_S1 (Dimmer )
ePWMOutput PWM___L1_KTCH_S1_output[4]={ePWMOutput::P16,ePWMOutput::P17,ePWMOutput::P18,ePWMOutput::P19,};
sensact::cPWM PWM___L1_KTCH_S1("PWM___L1_KTCH_S1", eApplicationID::PWM___L1_KTCH_S1, PWM___L1_KTCH_S1_output, 4, 1, 255, false, eApplicationID::STDBY_L0_TECH_24V);

// PWM PWM___L1_KTCH_S2 (Dimmer )
ePWMOutput PWM___L1_KTCH_S2_output[1]={ePWMOutput::P20,};
sensact::cPWM PWM___L1_KTCH_S2("PWM___L1_KTCH_S2", eApplicationID::PWM___L1_KTCH_S2, PWM___L1_KTCH_S2_output, 1, 1, 255, false, eApplicationID::STDBY_L0_TECH_24V);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &ROTAR_L1_KTCH_B21,
    &ROTAR_L1_KTCH_B22,
    &PUSHB_L1_KTCH_B23,
    &PUSHB_L1_KTCH_B24,
    &PUSHB_L1_KTCH_B25,
    &PUSHB_L1_KTCH_B26,
    &PUSHB_L1_KTCH_B27,
    &PUSHB_L1_KTCH_B28,
    &PWM___L1_KTCH_S1,
    &PWM___L1_KTCH_S2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L1_LVNG_UP02_1

eNodeID NODE = eNodeID::SNSAC_L1_LVNG_UP02_1;

const char MODEL::ModelString[] ="NodeId SNSAC_L1_LVNG_UP02_1 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// PushButtonX PUSHB_L1_LVNG_B31
const Command PUSHB_L1_LVNG_B31_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J1, eCommandType::UP},};
const Command *const PUSHB_L1_LVNG_B31_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B31_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B31_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B31_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B31_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B31_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B31("PUSHB_L1_LVNG_B31", eApplicationID::PUSHB_L1_LVNG_B31, eInput::I01, PUSHB_L1_LVNG_B31_LocalEvents, 0, PUSHB_L1_LVNG_B31_BusEvents, 0, PUSHB_L1_LVNG_B31_OnPressed, 1, PUSHB_L1_LVNG_B31_OnReleased, 0, PUSHB_L1_LVNG_B31_OnReleasedShort, 0, PUSHB_L1_LVNG_B31_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B31_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_LVNG_B32
const Command PUSHB_L1_LVNG_B32_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J1, eCommandType::DOWN},};
const Command *const PUSHB_L1_LVNG_B32_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B32_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B32_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B32_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B32_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B32_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B32("PUSHB_L1_LVNG_B32", eApplicationID::PUSHB_L1_LVNG_B32, eInput::I02, PUSHB_L1_LVNG_B32_LocalEvents, 0, PUSHB_L1_LVNG_B32_BusEvents, 0, PUSHB_L1_LVNG_B32_OnPressed, 1, PUSHB_L1_LVNG_B32_OnReleased, 0, PUSHB_L1_LVNG_B32_OnReleasedShort, 0, PUSHB_L1_LVNG_B32_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B32_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_LVNG_B33
const Command PUSHB_L1_LVNG_B33_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J2, eCommandType::UP},};
const Command *const PUSHB_L1_LVNG_B33_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B33_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B33_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B33_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B33_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B33_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B33("PUSHB_L1_LVNG_B33", eApplicationID::PUSHB_L1_LVNG_B33, eInput::I03, PUSHB_L1_LVNG_B33_LocalEvents, 0, PUSHB_L1_LVNG_B33_BusEvents, 0, PUSHB_L1_LVNG_B33_OnPressed, 1, PUSHB_L1_LVNG_B33_OnReleased, 0, PUSHB_L1_LVNG_B33_OnReleasedShort, 0, PUSHB_L1_LVNG_B33_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B33_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_LVNG_B34
const Command PUSHB_L1_LVNG_B34_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J2, eCommandType::DOWN},};
const Command *const PUSHB_L1_LVNG_B34_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B34_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B34_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B34_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J2, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B34_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B34_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B34("PUSHB_L1_LVNG_B34", eApplicationID::PUSHB_L1_LVNG_B34, eInput::I04, PUSHB_L1_LVNG_B34_LocalEvents, 0, PUSHB_L1_LVNG_B34_BusEvents, 0, PUSHB_L1_LVNG_B34_OnPressed, 1, PUSHB_L1_LVNG_B34_OnReleased, 0, PUSHB_L1_LVNG_B34_OnReleasedShort, 0, PUSHB_L1_LVNG_B34_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B34_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_LVNG_B35
const Command PUSHB_L1_LVNG_B35_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J3, eCommandType::UP},};
const Command *const PUSHB_L1_LVNG_B35_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B35_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B35_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B35_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J3, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B35_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B35_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B35("PUSHB_L1_LVNG_B35", eApplicationID::PUSHB_L1_LVNG_B35, eInput::I05, PUSHB_L1_LVNG_B35_LocalEvents, 0, PUSHB_L1_LVNG_B35_BusEvents, 0, PUSHB_L1_LVNG_B35_OnPressed, 1, PUSHB_L1_LVNG_B35_OnReleased, 0, PUSHB_L1_LVNG_B35_OnReleasedShort, 0, PUSHB_L1_LVNG_B35_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B35_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_LVNG_B36
const Command PUSHB_L1_LVNG_B36_OnPressed[1]={{eApplicationID::BLIND_L1_LVNG_J3, eCommandType::DOWN},};
const Command *const PUSHB_L1_LVNG_B36_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B36_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B36_OnPressedShortAndHold=0;
const Command PUSHB_L1_LVNG_B36_OnReleasedLong[1]={{eApplicationID::BLIND_L1_LVNG_J3, eCommandType::STOP},};
eEventType *PUSHB_L1_LVNG_B36_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B36_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B36("PUSHB_L1_LVNG_B36", eApplicationID::PUSHB_L1_LVNG_B36, eInput::I06, PUSHB_L1_LVNG_B36_LocalEvents, 0, PUSHB_L1_LVNG_B36_BusEvents, 0, PUSHB_L1_LVNG_B36_OnPressed, 1, PUSHB_L1_LVNG_B36_OnReleased, 0, PUSHB_L1_LVNG_B36_OnReleasedShort, 0, PUSHB_L1_LVNG_B36_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B36_OnReleasedLong, 1);

// RotaryEncoder ROTAR_L1_LVNG_B41
const Command ROTAR_L1_LVNG_B41_OnPressed[1]={{eApplicationID::PWM___L1_LVNG_S, eCommandType::TOGGLE},};
const Command *const ROTAR_L1_LVNG_B41_OnShortReleased=0;
const Command *const ROTAR_L1_LVNG_B41_OnLongReleased=0;
const Command ROTAR_L1_LVNG_B41_OnTurned[1]={{eApplicationID::PWM___L1_LVNG_S, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L1_LVNG_B41_LocalEvents=0;
eEventType *ROTAR_L1_LVNG_B41_BusEvents=0;
sensact::cROTAR ROTAR_L1_LVNG_B41("ROTAR_L1_LVNG_B41", eApplicationID::ROTAR_L1_LVNG_B41, eRotaryEncoder::ROTARYENCODER_2, eInput::ROTAR_PUSH_2, ROTAR_L1_LVNG_B41_LocalEvents, 0, ROTAR_L1_LVNG_B41_BusEvents, 0, ROTAR_L1_LVNG_B41_OnPressed, 1, ROTAR_L1_LVNG_B41_OnShortReleased, 0, ROTAR_L1_LVNG_B41_OnLongReleased, 0, ROTAR_L1_LVNG_B41_OnTurned, 1 );

// RotaryEncoder ROTAR_L1_LVNG_B42
const Command ROTAR_L1_LVNG_B42_OnPressed[1]={{eApplicationID::PWM___L1_LVNG_S, eCommandType::TOGGLE},};
const Command *const ROTAR_L1_LVNG_B42_OnShortReleased=0;
const Command *const ROTAR_L1_LVNG_B42_OnLongReleased=0;
const Command ROTAR_L1_LVNG_B42_OnTurned[1]={{eApplicationID::PWM___L1_LVNG_S, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L1_LVNG_B42_LocalEvents=0;
eEventType *ROTAR_L1_LVNG_B42_BusEvents=0;
sensact::cROTAR ROTAR_L1_LVNG_B42("ROTAR_L1_LVNG_B42", eApplicationID::ROTAR_L1_LVNG_B42, eRotaryEncoder::ROTARYENCODER_1, eInput::ROTAR_PUSH_1, ROTAR_L1_LVNG_B42_LocalEvents, 0, ROTAR_L1_LVNG_B42_BusEvents, 0, ROTAR_L1_LVNG_B42_OnPressed, 1, ROTAR_L1_LVNG_B42_OnShortReleased, 0, ROTAR_L1_LVNG_B42_OnLongReleased, 0, ROTAR_L1_LVNG_B42_OnTurned, 1 );

// PushButtonX PUSHB_L1_LVNG_B43
const Command PUSHB_L1_LVNG_B43_OnPressed[1]={{eApplicationID::POWIT_L1_LVNG_C1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_LVNG_B43_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B43_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B43_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_LVNG_B43_OnReleasedLong=0;
eEventType *PUSHB_L1_LVNG_B43_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B43_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B43("PUSHB_L1_LVNG_B43", eApplicationID::PUSHB_L1_LVNG_B43, eInput::I07, PUSHB_L1_LVNG_B43_LocalEvents, 0, PUSHB_L1_LVNG_B43_BusEvents, 0, PUSHB_L1_LVNG_B43_OnPressed, 1, PUSHB_L1_LVNG_B43_OnReleased, 0, PUSHB_L1_LVNG_B43_OnReleasedShort, 0, PUSHB_L1_LVNG_B43_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B43_OnReleasedLong, 0);

// PushButtonX PUSHB_L1_LVNG_B44
const Command PUSHB_L1_LVNG_B44_OnPressed[1]={{eApplicationID::POWIT_L1_LVNG_W1, eCommandType::TOGGLE},};
const Command *const PUSHB_L1_LVNG_B44_OnReleased=0;
const Command *const PUSHB_L1_LVNG_B44_OnReleasedShort=0;
const Command *const PUSHB_L1_LVNG_B44_OnPressedShortAndHold=0;
const Command *const PUSHB_L1_LVNG_B44_OnReleasedLong=0;
eEventType *PUSHB_L1_LVNG_B44_LocalEvents=0;
eEventType *PUSHB_L1_LVNG_B44_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_LVNG_B44("PUSHB_L1_LVNG_B44", eApplicationID::PUSHB_L1_LVNG_B44, eInput::I08, PUSHB_L1_LVNG_B44_LocalEvents, 0, PUSHB_L1_LVNG_B44_BusEvents, 0, PUSHB_L1_LVNG_B44_OnPressed, 1, PUSHB_L1_LVNG_B44_OnReleased, 0, PUSHB_L1_LVNG_B44_OnReleasedShort, 0, PUSHB_L1_LVNG_B44_OnPressedShortAndHold, 0, PUSHB_L1_LVNG_B44_OnReleasedLong, 0);

// PWM PWM___L1_LVNG_S (Dimmer )
ePWMOutput PWM___L1_LVNG_S_output[13]={ePWMOutput::P16,ePWMOutput::P17,ePWMOutput::P18,ePWMOutput::P19,ePWMOutput::P20,ePWMOutput::P21,ePWMOutput::P22,ePWMOutput::P23,ePWMOutput::P24,ePWMOutput::P25,ePWMOutput::P26,ePWMOutput::P27,ePWMOutput::P28,};
sensact::cPWM PWM___L1_LVNG_S("PWM___L1_LVNG_S", eApplicationID::PWM___L1_LVNG_S, PWM___L1_LVNG_S_output, 13, 1, 255, false, eApplicationID::STDBY_L2_CORR_24V);

// PWM PWM___LS_STRS_W2 (Dimmer )
ePWMOutput PWM___LS_STRS_W2_output[1]={ePWMOutput::P29,};
sensact::cPWM PWM___LS_STRS_W2("PWM___LS_STRS_W2", eApplicationID::PWM___LS_STRS_W2, PWM___LS_STRS_W2_output, 1, 1, 255, false, eApplicationID::STDBY_L2_CORR_24V);

// PWM PWM___LS_STRS_W4 (Dimmer )
ePWMOutput PWM___LS_STRS_W4_output[1]={ePWMOutput::P30,};
sensact::cPWM PWM___LS_STRS_W4("PWM___LS_STRS_W4", eApplicationID::PWM___LS_STRS_W4, PWM___LS_STRS_W4_output, 1, 1, 255, false, eApplicationID::STDBY_L2_CORR_24V);

// PWM PWM___LS_STRS_W6 (Dimmer )
ePWMOutput PWM___LS_STRS_W6_output[1]={ePWMOutput::P31,};
sensact::cPWM PWM___LS_STRS_W6("PWM___LS_STRS_W6", eApplicationID::PWM___LS_STRS_W6, PWM___LS_STRS_W6_output, 1, 1, 255, false, eApplicationID::STDBY_L2_CORR_24V);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &PUSHB_L1_LVNG_B31,
    &PUSHB_L1_LVNG_B32,
    &PUSHB_L1_LVNG_B33,
    &PUSHB_L1_LVNG_B34,
    &PUSHB_L1_LVNG_B35,
    &PUSHB_L1_LVNG_B36,
    &ROTAR_L1_LVNG_B41,
    &ROTAR_L1_LVNG_B42,
    &PUSHB_L1_LVNG_B43,
    &PUSHB_L1_LVNG_B44,
    &PWM___L1_LVNG_S,
    &PWM___LS_STRS_W2,
    &PWM___LS_STRS_W4,
    &PWM___LS_STRS_W6,
    0,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_SNSAC_L2_BATH_UP02_1

eNodeID NODE = eNodeID::SNSAC_L2_BATH_UP02_1;

const char MODEL::ModelString[] ="NodeId SNSAC_L2_BATH_UP02_1 created on 08.07.2016 23:20:54";

#include <cModel_base.inc>


// RotaryEncoder ROTAR_L2_BATH_B11
const Command ROTAR_L2_BATH_B11_OnPressed[1]={{eApplicationID::PWM___L2_BATH_S, eCommandType::TOGGLE},};
const Command *const ROTAR_L2_BATH_B11_OnShortReleased=0;
const Command *const ROTAR_L2_BATH_B11_OnLongReleased=0;
const Command ROTAR_L2_BATH_B11_OnTurned[1]={{eApplicationID::PWM___L2_BATH_S, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L2_BATH_B11_LocalEvents=0;
eEventType *ROTAR_L2_BATH_B11_BusEvents=0;
sensact::cROTAR ROTAR_L2_BATH_B11("ROTAR_L2_BATH_B11", eApplicationID::ROTAR_L2_BATH_B11, eRotaryEncoder::ROTARYENCODER_1, eInput::ROTAR_PUSH_1, ROTAR_L2_BATH_B11_LocalEvents, 0, ROTAR_L2_BATH_B11_BusEvents, 0, ROTAR_L2_BATH_B11_OnPressed, 1, ROTAR_L2_BATH_B11_OnShortReleased, 0, ROTAR_L2_BATH_B11_OnLongReleased, 0, ROTAR_L2_BATH_B11_OnTurned, 1 );

// RotaryEncoder ROTAR_L2_BATH_B12
const Command ROTAR_L2_BATH_B12_OnPressed[1]={{eApplicationID::RGBW__L2_BATH_W, eCommandType::TOGGLE},};
const Command *const ROTAR_L2_BATH_B12_OnShortReleased=0;
const Command *const ROTAR_L2_BATH_B12_OnLongReleased=0;
const Command ROTAR_L2_BATH_B12_OnTurned[1]={{eApplicationID::RGBW__L2_BATH_W, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_L2_BATH_B12_LocalEvents=0;
eEventType *ROTAR_L2_BATH_B12_BusEvents=0;
sensact::cROTAR ROTAR_L2_BATH_B12("ROTAR_L2_BATH_B12", eApplicationID::ROTAR_L2_BATH_B12, eRotaryEncoder::ROTARYENCODER_2, eInput::ROTAR_PUSH_2, ROTAR_L2_BATH_B12_LocalEvents, 0, ROTAR_L2_BATH_B12_BusEvents, 0, ROTAR_L2_BATH_B12_OnPressed, 1, ROTAR_L2_BATH_B12_OnShortReleased, 0, ROTAR_L2_BATH_B12_OnLongReleased, 0, ROTAR_L2_BATH_B12_OnTurned, 1 );

// PWM PWM___L2_BATH_S (Dimmer )
ePWMOutput PWM___L2_BATH_S_output[2]={ePWMOutput::P16,ePWMOutput::P17,};
sensact::cPWM PWM___L2_BATH_S("PWM___L2_BATH_S", eApplicationID::PWM___L2_BATH_S, PWM___L2_BATH_S_output, 2, 1, 255, false, eApplicationID::STDBY_L3_ROOF_48V);

// RGBW RGBW__L2_BATH_W (Full Color Light)
sensact::cRgbw RGBW__L2_BATH_W("RGBW__L2_BATH_W", eApplicationID::RGBW__L2_BATH_W, ePWMOutput::P18, ePWMOutput::P20, ePWMOutput::P19, ePWMOutput::P21, true, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::STDBY_L3_ROOF_48V);

// PWM PWM___L2_CORR_S (Dimmer )
ePWMOutput PWM___L2_CORR_S_output[2]={ePWMOutput::P22,ePWMOutput::P23,};
sensact::cPWM PWM___L2_CORR_S("PWM___L2_CORR_S", eApplicationID::PWM___L2_CORR_S, PWM___L2_CORR_S_output, 2, 1, 255, false, eApplicationID::STDBY_L3_ROOF_48V);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    &ROTAR_L2_BATH_B11,
    &ROTAR_L2_BATH_B12,
    &PWM___L2_BATH_S,
    &RGBW__L2_BATH_W,
    &PWM___L2_CORR_S,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif


}


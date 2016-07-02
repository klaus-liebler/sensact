/*
@file		hc_model.cpp
@author		autogen
@version	1.0
@date		06/29/2016 21:00:59 (Rendered)
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

using namespace std;

namespace sensact {

const char *MODEL::ApplicationNames[]={
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
    "STDBY_L0_24V",
    "STDBY_L0_48V",
    "PUSHB_L1_BATH_B11",
    "PWM___L1_BATH_S",
    "PUSHB_L1_BATH_B12",
    "PUSHB_L1_BATH_B13",
    "BLIND_L1_BATH_J1",
    "RGBW__L1_BATH_W1",

	};


#ifdef NODE_SNSCT_L0_TECH_HS07

eNodeID NODE = eNodeID::SNSCT_L0_TECH_HS07;

const char MODEL::ModelString[] ="NodeId SNSCT_L0_TECH_HS07 created on 29.06.2016 21:00:59";

#include <cModel_base.inc>


// PushButtonX PUSHB_L0_CORR_B11
Command PUSHB_L0_CORR_B11_OnPressed[1]={{eApplicationID::POWIT_L0_CORR_C1, eCommandType::TOGGLE},};
Command *PUSHB_L0_CORR_B11_OnReleased=0;
Command *PUSHB_L0_CORR_B11_OnReleasedShort=0;
Command *PUSHB_L0_CORR_B11_OnPressedShortAndHold=0;
Command *PUSHB_L0_CORR_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_CORR_B11_LocalEvents=0;
eEventType *PUSHB_L0_CORR_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_CORR_B11("PUSHB_L0_CORR_B11", eApplicationID::PUSHB_L0_CORR_B11, eInput::I01, PUSHB_L0_CORR_B11_LocalEvents, 0, PUSHB_L0_CORR_B11_BusEvents, 0, PUSHB_L0_CORR_B11_OnPressed, 1, PUSHB_L0_CORR_B11_OnReleased, 0, PUSHB_L0_CORR_B11_OnReleasedShort, 0, PUSHB_L0_CORR_B11_OnPressedShortAndHold, 0, PUSHB_L0_CORR_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_CORR_C1
sensact::cPoweritem POWIT_L0_CORR_C1("POWIT_L0_CORR_C1", eApplicationID::POWIT_L0_CORR_C1, ePoweredOutput::O09, 0);

// PushButtonX PUSHB_L0_PRTY_B11
Command *PUSHB_L0_PRTY_B11_OnPressed=0;
Command *PUSHB_L0_PRTY_B11_OnReleased=0;
Command PUSHB_L0_PRTY_B11_OnReleasedShort[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::TOGGLE},};
Command PUSHB_L0_PRTY_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::UP},};
Command PUSHB_L0_PRTY_B11_OnReleasedLong[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::STOP},};
eEventType *PUSHB_L0_PRTY_B11_LocalEvents=0;
eEventType *PUSHB_L0_PRTY_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_PRTY_B11("PUSHB_L0_PRTY_B11", eApplicationID::PUSHB_L0_PRTY_B11, eInput::I02, PUSHB_L0_PRTY_B11_LocalEvents, 0, PUSHB_L0_PRTY_B11_BusEvents, 0, PUSHB_L0_PRTY_B11_OnPressed, 0, PUSHB_L0_PRTY_B11_OnReleased, 0, PUSHB_L0_PRTY_B11_OnReleasedShort, 1, PUSHB_L0_PRTY_B11_OnPressedShortAndHold, 1, PUSHB_L0_PRTY_B11_OnReleasedLong, 1);

// PushButtonX PUSHB_L0_PRTY_B12
Command *PUSHB_L0_PRTY_B12_OnPressed=0;
Command *PUSHB_L0_PRTY_B12_OnReleased=0;
Command PUSHB_L0_PRTY_B12_OnReleasedShort[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::TOGGLE},};
Command PUSHB_L0_PRTY_B12_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::DOWN},};
Command PUSHB_L0_PRTY_B12_OnReleasedLong[1]={{eApplicationID::PWM___L0_PRTY_S, eCommandType::STOP},};
eEventType *PUSHB_L0_PRTY_B12_LocalEvents=0;
eEventType *PUSHB_L0_PRTY_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_PRTY_B12("PUSHB_L0_PRTY_B12", eApplicationID::PUSHB_L0_PRTY_B12, eInput::I03, PUSHB_L0_PRTY_B12_LocalEvents, 0, PUSHB_L0_PRTY_B12_BusEvents, 0, PUSHB_L0_PRTY_B12_OnPressed, 0, PUSHB_L0_PRTY_B12_OnReleased, 0, PUSHB_L0_PRTY_B12_OnReleasedShort, 1, PUSHB_L0_PRTY_B12_OnPressedShortAndHold, 1, PUSHB_L0_PRTY_B12_OnReleasedLong, 1);

// PWM PWM___L0_PRTY_S (Dimmer )
ePWMOutput PWM___L0_PRTY_S_output[4]={ePWMOutput::P01,ePWMOutput::P02,ePWMOutput::P03,ePWMOutput::P04,};
sensact::cPWM PWM___L0_PRTY_S("PWM___L0_PRTY_S", eApplicationID::PWM___L0_PRTY_S, PWM___L0_PRTY_S_output, 4, 1, 255, false, eApplicationID::STDBY_L0_48V);

// PushButtonX PUSHB_L0_STO1_B11
Command PUSHB_L0_STO1_B11_OnPressed[1]={{eApplicationID::POWIT_L0_STO1_C1, eCommandType::TOGGLE},};
Command *PUSHB_L0_STO1_B11_OnReleased=0;
Command *PUSHB_L0_STO1_B11_OnReleasedShort=0;
Command *PUSHB_L0_STO1_B11_OnPressedShortAndHold=0;
Command *PUSHB_L0_STO1_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_STO1_B11_LocalEvents=0;
eEventType *PUSHB_L0_STO1_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_STO1_B11("PUSHB_L0_STO1_B11", eApplicationID::PUSHB_L0_STO1_B11, eInput::I04, PUSHB_L0_STO1_B11_LocalEvents, 0, PUSHB_L0_STO1_B11_BusEvents, 0, PUSHB_L0_STO1_B11_OnPressed, 1, PUSHB_L0_STO1_B11_OnReleased, 0, PUSHB_L0_STO1_B11_OnReleasedShort, 0, PUSHB_L0_STO1_B11_OnPressedShortAndHold, 0, PUSHB_L0_STO1_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_STO1_C1
sensact::cPoweritem POWIT_L0_STO1_C1("POWIT_L0_STO1_C1", eApplicationID::POWIT_L0_STO1_C1, ePoweredOutput::O10, 0);

// PushButtonX PUSHB_L0_TECH_B11
Command PUSHB_L0_TECH_B11_OnPressed[1]={{eApplicationID::POWIT_L0_TECH_C1, eCommandType::TOGGLE},};
Command *PUSHB_L0_TECH_B11_OnReleased=0;
Command *PUSHB_L0_TECH_B11_OnReleasedShort=0;
Command *PUSHB_L0_TECH_B11_OnPressedShortAndHold=0;
Command *PUSHB_L0_TECH_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_TECH_B11_LocalEvents=0;
eEventType *PUSHB_L0_TECH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_TECH_B11("PUSHB_L0_TECH_B11", eApplicationID::PUSHB_L0_TECH_B11, eInput::I05, PUSHB_L0_TECH_B11_LocalEvents, 0, PUSHB_L0_TECH_B11_BusEvents, 0, PUSHB_L0_TECH_B11_OnPressed, 1, PUSHB_L0_TECH_B11_OnReleased, 0, PUSHB_L0_TECH_B11_OnReleasedShort, 0, PUSHB_L0_TECH_B11_OnPressedShortAndHold, 0, PUSHB_L0_TECH_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_TECH_C1
sensact::cPoweritem POWIT_L0_TECH_C1("POWIT_L0_TECH_C1", eApplicationID::POWIT_L0_TECH_C1, ePoweredOutput::O11, 0);

// PushButtonX PUSHB_L0_WELL_B11
Command PUSHB_L0_WELL_B11_OnPressed[1]={{eApplicationID::POWIT_L0_WELL_C1, eCommandType::TOGGLE},};
Command *PUSHB_L0_WELL_B11_OnReleased=0;
Command *PUSHB_L0_WELL_B11_OnReleasedShort=0;
Command *PUSHB_L0_WELL_B11_OnPressedShortAndHold=0;
Command *PUSHB_L0_WELL_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_WELL_B11_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B11("PUSHB_L0_WELL_B11", eApplicationID::PUSHB_L0_WELL_B11, eInput::I06, PUSHB_L0_WELL_B11_LocalEvents, 0, PUSHB_L0_WELL_B11_BusEvents, 0, PUSHB_L0_WELL_B11_OnPressed, 1, PUSHB_L0_WELL_B11_OnReleased, 0, PUSHB_L0_WELL_B11_OnReleasedShort, 0, PUSHB_L0_WELL_B11_OnPressedShortAndHold, 0, PUSHB_L0_WELL_B11_OnReleasedLong, 0);

// PushButtonX PUSHB_L0_WELL_B21
Command *PUSHB_L0_WELL_B21_OnPressed=0;
Command *PUSHB_L0_WELL_B21_OnReleased=0;
Command PUSHB_L0_WELL_B21_OnReleasedShort[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::TOGGLE},};
Command PUSHB_L0_WELL_B21_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::START},};
Command PUSHB_L0_WELL_B21_OnReleasedLong[1]={{eApplicationID::PWM___L0_WELL_S1, eCommandType::STOP},};
eEventType *PUSHB_L0_WELL_B21_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B21_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B21("PUSHB_L0_WELL_B21", eApplicationID::PUSHB_L0_WELL_B21, eInput::I07, PUSHB_L0_WELL_B21_LocalEvents, 0, PUSHB_L0_WELL_B21_BusEvents, 0, PUSHB_L0_WELL_B21_OnPressed, 0, PUSHB_L0_WELL_B21_OnReleased, 0, PUSHB_L0_WELL_B21_OnReleasedShort, 1, PUSHB_L0_WELL_B21_OnPressedShortAndHold, 1, PUSHB_L0_WELL_B21_OnReleasedLong, 1);

// PushButtonX PUSHB_L0_WELL_B22
Command *PUSHB_L0_WELL_B22_OnPressed=0;
Command *PUSHB_L0_WELL_B22_OnReleased=0;
Command PUSHB_L0_WELL_B22_OnReleasedShort[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::TOGGLE},};
Command PUSHB_L0_WELL_B22_OnPressedShortAndHold[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::START},};
Command PUSHB_L0_WELL_B22_OnReleasedLong[1]={{eApplicationID::PWM___L0_WELL_S2, eCommandType::STOP},};
eEventType *PUSHB_L0_WELL_B22_LocalEvents=0;
eEventType *PUSHB_L0_WELL_B22_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WELL_B22("PUSHB_L0_WELL_B22", eApplicationID::PUSHB_L0_WELL_B22, eInput::I08, PUSHB_L0_WELL_B22_LocalEvents, 0, PUSHB_L0_WELL_B22_BusEvents, 0, PUSHB_L0_WELL_B22_OnPressed, 0, PUSHB_L0_WELL_B22_OnReleased, 0, PUSHB_L0_WELL_B22_OnReleasedShort, 1, PUSHB_L0_WELL_B22_OnPressedShortAndHold, 1, PUSHB_L0_WELL_B22_OnReleasedLong, 1);

// POWIT POWIT_L0_WELL_C1
sensact::cPoweritem POWIT_L0_WELL_C1("POWIT_L0_WELL_C1", eApplicationID::POWIT_L0_WELL_C1, ePoweredOutput::O12, 0);

// PWM PWM___L0_WELL_S1 (Dimmer )
ePWMOutput PWM___L0_WELL_S1_output[1]={ePWMOutput::P05,};
sensact::cPWM PWM___L0_WELL_S1("PWM___L0_WELL_S1", eApplicationID::PWM___L0_WELL_S1, PWM___L0_WELL_S1_output, 1, 1, 255, false, eApplicationID::STDBY_L0_24V);

// PWM PWM___L0_WELL_S2 (Dimmer )
ePWMOutput PWM___L0_WELL_S2_output[1]={ePWMOutput::P06,};
sensact::cPWM PWM___L0_WELL_S2("PWM___L0_WELL_S2", eApplicationID::PWM___L0_WELL_S2, PWM___L0_WELL_S2_output, 1, 1, 255, false, eApplicationID::STDBY_L0_24V);

// PushButtonX PUSHB_L0_WORK_B11
Command PUSHB_L0_WORK_B11_OnPressed[1]={{eApplicationID::POWIT_L0_WORK_C1, eCommandType::TOGGLE},};
Command *PUSHB_L0_WORK_B11_OnReleased=0;
Command *PUSHB_L0_WORK_B11_OnReleasedShort=0;
Command *PUSHB_L0_WORK_B11_OnPressedShortAndHold=0;
Command *PUSHB_L0_WORK_B11_OnReleasedLong=0;
eEventType *PUSHB_L0_WORK_B11_LocalEvents=0;
eEventType *PUSHB_L0_WORK_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L0_WORK_B11("PUSHB_L0_WORK_B11", eApplicationID::PUSHB_L0_WORK_B11, eInput::I09, PUSHB_L0_WORK_B11_LocalEvents, 0, PUSHB_L0_WORK_B11_BusEvents, 0, PUSHB_L0_WORK_B11_OnPressed, 1, PUSHB_L0_WORK_B11_OnReleased, 0, PUSHB_L0_WORK_B11_OnReleasedShort, 0, PUSHB_L0_WORK_B11_OnPressedShortAndHold, 0, PUSHB_L0_WORK_B11_OnReleasedLong, 0);

// POWIT POWIT_L0_WORK_C1
sensact::cPoweritem POWIT_L0_WORK_C1("POWIT_L0_WORK_C1", eApplicationID::POWIT_L0_WORK_C1, ePoweredOutput::O13, 0);

// STDBY STDBY_L0_24V
sensact::cStandbyController STDBY_L0_24V("STDBY_L0_24V", eApplicationID::STDBY_L0_24V, ePoweredOutput::O14, 10000);

// STDBY STDBY_L0_48V
sensact::cStandbyController STDBY_L0_48V("STDBY_L0_48V", eApplicationID::STDBY_L0_48V, ePoweredOutput::O15, 10000);

// PushButtonX PUSHB_L1_BATH_B11
Command *PUSHB_L1_BATH_B11_OnPressed=0;
Command PUSHB_L1_BATH_B11_OnReleased[1]={{eApplicationID::RGBW__L1_BATH_W1, eCommandType::TOGGLE},};
Command PUSHB_L1_BATH_B11_OnReleasedShort[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::TOGGLE},};
Command PUSHB_L1_BATH_B11_OnPressedShortAndHold[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::START},};
Command PUSHB_L1_BATH_B11_OnReleasedLong[1]={{eApplicationID::PWM___L1_BATH_S, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B11_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B11_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B11("PUSHB_L1_BATH_B11", eApplicationID::PUSHB_L1_BATH_B11, eInput::I10, PUSHB_L1_BATH_B11_LocalEvents, 0, PUSHB_L1_BATH_B11_BusEvents, 0, PUSHB_L1_BATH_B11_OnPressed, 0, PUSHB_L1_BATH_B11_OnReleased, 1, PUSHB_L1_BATH_B11_OnReleasedShort, 1, PUSHB_L1_BATH_B11_OnPressedShortAndHold, 1, PUSHB_L1_BATH_B11_OnReleasedLong, 1);

// PWM PWM___L1_BATH_S (Dimmer )
ePWMOutput PWM___L1_BATH_S_output[1]={ePWMOutput::P07,};
sensact::cPWM PWM___L1_BATH_S("PWM___L1_BATH_S", eApplicationID::PWM___L1_BATH_S, PWM___L1_BATH_S_output, 1, 1, 255, false, eApplicationID::STDBY_L0_48V);

// PushButtonX PUSHB_L1_BATH_B12
Command PUSHB_L1_BATH_B12_OnPressed[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::UP},};
Command *PUSHB_L1_BATH_B12_OnReleased=0;
Command *PUSHB_L1_BATH_B12_OnReleasedShort=0;
Command *PUSHB_L1_BATH_B12_OnPressedShortAndHold=0;
Command PUSHB_L1_BATH_B12_OnReleasedLong[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B12_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B12_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B12("PUSHB_L1_BATH_B12", eApplicationID::PUSHB_L1_BATH_B12, eInput::I11, PUSHB_L1_BATH_B12_LocalEvents, 0, PUSHB_L1_BATH_B12_BusEvents, 0, PUSHB_L1_BATH_B12_OnPressed, 1, PUSHB_L1_BATH_B12_OnReleased, 0, PUSHB_L1_BATH_B12_OnReleasedShort, 0, PUSHB_L1_BATH_B12_OnPressedShortAndHold, 0, PUSHB_L1_BATH_B12_OnReleasedLong, 1);

// PushButtonX PUSHB_L1_BATH_B13
Command PUSHB_L1_BATH_B13_OnPressed[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::DOWN},};
Command *PUSHB_L1_BATH_B13_OnReleased=0;
Command *PUSHB_L1_BATH_B13_OnReleasedShort=0;
Command *PUSHB_L1_BATH_B13_OnPressedShortAndHold=0;
Command PUSHB_L1_BATH_B13_OnReleasedLong[1]={{eApplicationID::BLIND_L1_BATH_J1, eCommandType::STOP},};
eEventType *PUSHB_L1_BATH_B13_LocalEvents=0;
eEventType *PUSHB_L1_BATH_B13_BusEvents=0;
sensact::cPushbuttonX PUSHB_L1_BATH_B13("PUSHB_L1_BATH_B13", eApplicationID::PUSHB_L1_BATH_B13, eInput::I12, PUSHB_L1_BATH_B13_LocalEvents, 0, PUSHB_L1_BATH_B13_BusEvents, 0, PUSHB_L1_BATH_B13_OnPressed, 1, PUSHB_L1_BATH_B13_OnReleased, 0, PUSHB_L1_BATH_B13_OnReleasedShort, 0, PUSHB_L1_BATH_B13_OnPressedShortAndHold, 0, PUSHB_L1_BATH_B13_OnReleasedLong, 1);

// Blind BLIND_L1_BATH_J1
cBlind BLIND_L1_BATH_J1("BLIND_L1_BATH_J1", eApplicationID::BLIND_L1_BATH_J1, ePoweredOutput::O16, ePoweredOutput::O16, eRelayMode::TWO_PHASES, 40);

// RGBW RGBW__L1_BATH_W1 (Full Color Light)
sensact::cRgbw RGBW__L1_BATH_W1("RGBW__L1_BATH_W1", eApplicationID::RGBW__L1_BATH_W1, ePWMOutput::OP01, ePWMOutput::OP03, ePWMOutput::OP02, ePWMOutput::NONE, true, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::NO_APPLICATION);





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
    &STDBY_L0_24V,
    &STDBY_L0_48V,
    &PUSHB_L1_BATH_B11,
    &PWM___L1_BATH_S,
    &PUSHB_L1_BATH_B12,
    &PUSHB_L1_BATH_B13,
    &BLIND_L1_BATH_J1,
    &RGBW__L1_BATH_W1,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif


}


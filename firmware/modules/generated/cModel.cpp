/*
@file		hc_model.cpp
@author		autogen
@version	1.0
@date		06/04/2016 00:08:11 (Rendered)
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
    "BELL__DOOR",
    "STDBY_XX_XXX_1",
    "PWM___XX_XXX_1",
    "BLIND_XX_XXX_1",
    "PUSHB_LX_FRO_1",
    "PUSHB__XX_BLN__UP",
    "PUSHB__XX_BLN__DOWN",
    "PUSHB__XX_PUB_1",
    "RGBW__YY_YYY_1",
    "ROTAR_YY_YYY_1",
    "POWIT_YY_YYY_01",
    "PUSHB_YY_PUB_1",

	};


#ifdef NODE_TEST_HS07

eNodeID NODE = eNodeID::TEST_HS07;

const char MODEL::ModelString[] ="NodeId TEST_HS07 created on 04.06.2016 00:08:11";

#include <cModel_base.inc>


// DOORBELL BELL__DOOR
sensact::cBell BELL__DOOR("BELL__DOOR", eApplicationID::BELL__DOOR, &MODEL::volumeSchedule);

// STDBY STDBY_XX_XXX_1
sensact::cStandbyController STDBY_XX_XXX_1("STDBY_XX_XXX_1", eApplicationID::STDBY_XX_XXX_1, ePoweredOutput::O01, 10000);

// PWM PWM___XX_XXX_1 (Dimmer )
ePWMOutput PWM___XX_XXX_1_output[1]={ePWMOutput::P01,};
sensact::cPWM PWM___XX_XXX_1("PWM___XX_XXX_1", eApplicationID::PWM___XX_XXX_1, PWM___XX_XXX_1_output, 1, 20, 255, false, eApplicationID::STDBY_XX_XXX_1);

// Blind BLIND_XX_XXX_1
cBlind BLIND_XX_XXX_1("BLIND_XX_XXX_1", eApplicationID::BLIND_XX_XXX_1, ePoweredOutput::O03, ePoweredOutput::O02, eRelayMode::INTERLOCKED, 10);

// PushButtonX PUSHB_LX_FRO_1
Command PUSHB_LX_FRO_1_OnPressed[1]={{eApplicationID::BELL__DOOR, eCommandType::START},};
Command *PUSHB_LX_FRO_1_OnReleased=0;
Command *PUSHB_LX_FRO_1_OnReleasedShort=0;
Command *PUSHB_LX_FRO_1_OnPressedShortAndHold=0;
Command *PUSHB_LX_FRO_1_OnReleasedLong=0;
eEventType *PUSHB_LX_FRO_1_LocalEvents=0;
eEventType *PUSHB_LX_FRO_1_BusEvents=0;
sensact::cPushbuttonX PUSHB_LX_FRO_1("PUSHB_LX_FRO_1", eApplicationID::PUSHB_LX_FRO_1, eInput::I04, PUSHB_LX_FRO_1_LocalEvents, 0, PUSHB_LX_FRO_1_BusEvents, 0, PUSHB_LX_FRO_1_OnPressed, 1, PUSHB_LX_FRO_1_OnReleased, 0, PUSHB_LX_FRO_1_OnReleasedShort, 0, PUSHB_LX_FRO_1_OnPressedShortAndHold, 0, PUSHB_LX_FRO_1_OnReleasedLong, 0);

// PushButtonX PUSHB__XX_BLN__UP
Command PUSHB__XX_BLN__UP_OnPressed[1]={{eApplicationID::BLIND_XX_XXX_1, eCommandType::UP},};
Command *PUSHB__XX_BLN__UP_OnReleased=0;
Command *PUSHB__XX_BLN__UP_OnReleasedShort=0;
Command *PUSHB__XX_BLN__UP_OnPressedShortAndHold=0;
Command PUSHB__XX_BLN__UP_OnReleasedLong[1]={{eApplicationID::BLIND_XX_XXX_1, eCommandType::STOP},};
eEventType *PUSHB__XX_BLN__UP_LocalEvents=0;
eEventType *PUSHB__XX_BLN__UP_BusEvents=0;
sensact::cPushbuttonX PUSHB__XX_BLN__UP("PUSHB__XX_BLN__UP", eApplicationID::PUSHB__XX_BLN__UP, eInput::I01, PUSHB__XX_BLN__UP_LocalEvents, 0, PUSHB__XX_BLN__UP_BusEvents, 0, PUSHB__XX_BLN__UP_OnPressed, 1, PUSHB__XX_BLN__UP_OnReleased, 0, PUSHB__XX_BLN__UP_OnReleasedShort, 0, PUSHB__XX_BLN__UP_OnPressedShortAndHold, 0, PUSHB__XX_BLN__UP_OnReleasedLong, 1);

// PushButtonX PUSHB__XX_BLN__DOWN
Command PUSHB__XX_BLN__DOWN_OnPressed[1]={{eApplicationID::BLIND_XX_XXX_1, eCommandType::DOWN},};
Command *PUSHB__XX_BLN__DOWN_OnReleased=0;
Command *PUSHB__XX_BLN__DOWN_OnReleasedShort=0;
Command *PUSHB__XX_BLN__DOWN_OnPressedShortAndHold=0;
Command PUSHB__XX_BLN__DOWN_OnReleasedLong[1]={{eApplicationID::BLIND_XX_XXX_1, eCommandType::STOP},};
eEventType *PUSHB__XX_BLN__DOWN_LocalEvents=0;
eEventType *PUSHB__XX_BLN__DOWN_BusEvents=0;
sensact::cPushbuttonX PUSHB__XX_BLN__DOWN("PUSHB__XX_BLN__DOWN", eApplicationID::PUSHB__XX_BLN__DOWN, eInput::I02, PUSHB__XX_BLN__DOWN_LocalEvents, 0, PUSHB__XX_BLN__DOWN_BusEvents, 0, PUSHB__XX_BLN__DOWN_OnPressed, 1, PUSHB__XX_BLN__DOWN_OnReleased, 0, PUSHB__XX_BLN__DOWN_OnReleasedShort, 0, PUSHB__XX_BLN__DOWN_OnPressedShortAndHold, 0, PUSHB__XX_BLN__DOWN_OnReleasedLong, 1);

// PushButtonX PUSHB__XX_PUB_1
Command *PUSHB__XX_PUB_1_OnPressed=0;
Command *PUSHB__XX_PUB_1_OnReleased=0;
Command PUSHB__XX_PUB_1_OnReleasedShort[1]={{eApplicationID::PWM___XX_XXX_1, eCommandType::TOGGLE},};
Command PUSHB__XX_PUB_1_OnPressedShortAndHold[1]={{eApplicationID::PWM___XX_XXX_1, eCommandType::START},};
Command PUSHB__XX_PUB_1_OnReleasedLong[1]={{eApplicationID::PWM___XX_XXX_1, eCommandType::STOP},};
eEventType *PUSHB__XX_PUB_1_LocalEvents=0;
eEventType *PUSHB__XX_PUB_1_BusEvents=0;
sensact::cPushbuttonX PUSHB__XX_PUB_1("PUSHB__XX_PUB_1", eApplicationID::PUSHB__XX_PUB_1, eInput::I03, PUSHB__XX_PUB_1_LocalEvents, 0, PUSHB__XX_PUB_1_BusEvents, 0, PUSHB__XX_PUB_1_OnPressed, 0, PUSHB__XX_PUB_1_OnReleased, 0, PUSHB__XX_PUB_1_OnReleasedShort, 1, PUSHB__XX_PUB_1_OnPressedShortAndHold, 1, PUSHB__XX_PUB_1_OnReleasedLong, 1);





cApplication *MODEL::Glo2locCmd[] = {
    0,
    &BELL__DOOR,
    &STDBY_XX_XXX_1,
    &PWM___XX_XXX_1,
    &BLIND_XX_XXX_1,
    &PUSHB_LX_FRO_1,
    &PUSHB__XX_BLN__UP,
    &PUSHB__XX_BLN__DOWN,
    &PUSHB__XX_PUB_1,
    0,
    0,
    0,
    0,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif

#ifdef NODE_TEST_UP02

eNodeID NODE = eNodeID::TEST_UP02;

const char MODEL::ModelString[] ="NodeId TEST_UP02 created on 04.06.2016 00:08:11";

#include <cModel_base.inc>


// RGBW RGBW__YY_YYY_1 (Full Color Light)
sensact::cRgbw RGBW__YY_YYY_1("RGBW__YY_YYY_1", eApplicationID::RGBW__YY_YYY_1, ePWMOutput::P01, ePWMOutput::P03, ePWMOutput::P05, ePWMOutput::P07, false, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::NO_APPLICATION);

// RotaryEncoder ROTAR_YY_YYY_1
Command ROTAR_YY_YYY_1_OnPressed[1]={{eApplicationID::PWM___XX_XXX_1, eCommandType::TOGGLE},};
Command *ROTAR_YY_YYY_1_OnShortReleased=0;
Command *ROTAR_YY_YYY_1_OnLongReleased=0;
Command ROTAR_YY_YYY_1_OnTurned[1]={{eApplicationID::PWM___XX_XXX_1, eCommandType::STEP_VERTICAL},};
eEventType *ROTAR_YY_YYY_1_LocalEvents=0;
eEventType *ROTAR_YY_YYY_1_BusEvents=0;
sensact::cROTAR ROTAR_YY_YYY_1("ROTAR_YY_YYY_1", eApplicationID::ROTAR_YY_YYY_1, eRotaryEncoder::ROTARYENCODER_1, eInput::ROTAR_PUSH_1, ROTAR_YY_YYY_1_LocalEvents, 0, ROTAR_YY_YYY_1_BusEvents, 0, ROTAR_YY_YYY_1_OnPressed, 1, ROTAR_YY_YYY_1_OnShortReleased, 0, ROTAR_YY_YYY_1_OnLongReleased, 0, ROTAR_YY_YYY_1_OnTurned, 1 );

// POWIT POWIT_YY_YYY_01
sensact::cPoweritem POWIT_YY_YYY_01("POWIT_YY_YYY_01", eApplicationID::POWIT_YY_YYY_01, ePoweredOutput::O_LED, 3000);

// PushButtonX PUSHB_YY_PUB_1
Command PUSHB_YY_PUB_1_OnPressed[1]={{eApplicationID::POWIT_YY_YYY_01, eCommandType::ON},};
Command *PUSHB_YY_PUB_1_OnReleased=0;
Command *PUSHB_YY_PUB_1_OnReleasedShort=0;
Command *PUSHB_YY_PUB_1_OnPressedShortAndHold=0;
Command *PUSHB_YY_PUB_1_OnReleasedLong=0;
eEventType *PUSHB_YY_PUB_1_LocalEvents=0;
eEventType *PUSHB_YY_PUB_1_BusEvents=0;
sensact::cPushbuttonX PUSHB_YY_PUB_1("PUSHB_YY_PUB_1", eApplicationID::PUSHB_YY_PUB_1, eInput::I01, PUSHB_YY_PUB_1_LocalEvents, 0, PUSHB_YY_PUB_1_BusEvents, 0, PUSHB_YY_PUB_1_OnPressed, 1, PUSHB_YY_PUB_1_OnReleased, 0, PUSHB_YY_PUB_1_OnReleasedShort, 0, PUSHB_YY_PUB_1_OnPressedShortAndHold, 0, PUSHB_YY_PUB_1_OnReleasedLong, 0);





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
    &RGBW__YY_YYY_1,
    &ROTAR_YY_YYY_1,
    &POWIT_YY_YYY_01,
    &PUSHB_YY_PUB_1,

};



cApplication *MODEL::Glo2locEvt[] = { };
#endif


}


/*
@file		AppIDs
@author		autogen
@version	1.0
@date		06/29/2016 21:00:59 (Rendered)
@brief		Short
@details	Details
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */

#pragma once
#include <inttypes.h>

namespace sensact{
enum struct eApplicationID:uint16_t{
	    MASTER,
        PUSHB_L0_CORR_B11,
        POWIT_L0_CORR_C1,
        PUSHB_L0_PRTY_B11,
        PUSHB_L0_PRTY_B12,
        PWM___L0_PRTY_S,
        PUSHB_L0_STO1_B11,
        POWIT_L0_STO1_C1,
        PUSHB_L0_TECH_B11,
        POWIT_L0_TECH_C1,
        PUSHB_L0_WELL_B11,
        PUSHB_L0_WELL_B21,
        PUSHB_L0_WELL_B22,
        POWIT_L0_WELL_C1,
        PWM___L0_WELL_S1,
        PWM___L0_WELL_S2,
        PUSHB_L0_WORK_B11,
        POWIT_L0_WORK_C1,
        STDBY_L0_24V,
        STDBY_L0_48V,
        PUSHB_L1_BATH_B11,
        PWM___L1_BATH_S,
        PUSHB_L1_BATH_B12,
        PUSHB_L1_BATH_B13,
        BLIND_L1_BATH_J1,
        RGBW__L1_BATH_W1,
    	COUNT,
	NO_APPLICATION
};

enum struct eNodeID
	:uint16_t
	{
	MASTER_NODE,
	    SNSCT_L0_TECH_HS07,
    	COUNT
	
};
}


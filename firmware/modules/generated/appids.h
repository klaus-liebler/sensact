/*
@file		AppIDs
@author		autogen
@version	1.0
@date		06/04/2016 00:08:10 (Rendered)
@brief		Short
@details	Details
@copyright	(c) 2015 mail at klaus - liebler . de === GNU GPL v3
 */

#pragma once
#include <inttypes.h>

namespace sensact{
enum struct eApplicationID:uint16_t{
	    MASTER,
        BELL__DOOR,
        STDBY_XX_XXX_1,
        PWM___XX_XXX_1,
        BLIND_XX_XXX_1,
        PUSHB_LX_FRO_1,
        PUSHB__XX_BLN__UP,
        PUSHB__XX_BLN__DOWN,
        PUSHB__XX_PUB_1,
        RGBW__YY_YYY_1,
        ROTAR_YY_YYY_1,
        POWIT_YY_YYY_01,
        PUSHB_YY_PUB_1,
    	COUNT,
	NO_APPLICATION
};

enum struct eNodeID
	:uint16_t
	{
	MASTER_NODE,
	    TEST_HS07,
        TEST_UP02,
    	COUNT
	
};
}


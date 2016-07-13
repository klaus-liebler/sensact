/*
 * application.cpp
 *
 *  Created on: 12.07.2016
 *      Author: klaus
 */



#include "onewire.h"

namespace sensact{
void cOneWireApplication::ResetCb()
{
	return;
}
void cOneWireApplication::CommandCb(eOWCommand cmd, eNextAction *nextAction, volatile uint8_t *buffer)
{
}
void cOneWireApplication::MatchRomCb(eNextAction *nextAction, volatile uint8_t *buffer)
{
}
void cOneWireApplication::ActionCb(eNextAction *nextAction, volatile uint8_t *buffer)
{
}

}

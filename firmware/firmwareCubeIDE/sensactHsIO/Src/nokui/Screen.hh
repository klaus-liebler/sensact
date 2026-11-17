#pragma once

#include "stdint.h"
#include "../spilcd16bit.hh"

class Screen {
public:
	Screen(SPILCD16bit *lcd);
	virtual ~Screen();
	void Update();
	void SetActive();
	void SetInactive();
	virtual void OnTouch(int16_t x, int16_t y)=0;
	virtual void OnButtonShort(uint32_t buttonFlags)=0;
	virtual void OnButtonLong(uint32_t buttonFlags)=0;
	virtual void OnRotaryTurn(uint32_t rotaryKnobFlags, int16_t rotarySteps)=0;

protected:
	virtual void RedrawCompletely()=0;
	bool active=false;
	SPILCD16bit *lcd;
};

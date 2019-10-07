#pragma once
#include "Screen.hh"
#include "stdint.h"
#include "../spilcd16bit.hh"

class CVScreen:public Screen {
public:
	CVScreen(SPILCD16bit *lcd);
	virtual ~CVScreen();
protected:
	void RedrawCompletely() override;
	void OnRotaryTurn(uint32_t rotaryKnobFlags, int16_t rotarySteps) override;
	void OnTouch(int16_t x, int16_t y) override;
	void OnButtonShort(uint32_t buttonFlags) override;
	void OnButtonLong(uint32_t buttonFlags) override;

private:
	uint32_t VoltageSet=421;
};

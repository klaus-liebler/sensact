#include "MainScreen.hh"

#include "../spilcd16bit.hh"

//Logik
//zu jedem Zeitpunkt ist ein "Screen" aktiv; es sind aber alle instanziert
//auf den aktiven Screen zeigt ein Pointer und dieser enthält auch alle UI-Eingaben
//Beim Wechsel wird ScreenA "inaktiv", dann wird der neue Screen aktiv und dann wird der Pointer umgebogen
//Der Wechsel findet im Hauptthread statt, nicht im IRQ-Kontext
//Nur der aktive Screen darf Kommandos an den Controller senden
//wenn der RotEnc schnell gedreht wird, dann größere Schritte
//Buttons Up/Down zum Wechsel zwischen Strom und Spannung. SET geht in die Settings
//bestätigen der Eingabe mit Druck auf den Knopf

CVScreen::CVScreen(SPILCD16bit *lcd):Screen(lcd){
}

CVScreen::~CVScreen() {
}

void CVScreen::RedrawCompletely()
{
	lcd->setColors(RED, WHITE);
	lcd->fillScreenWithForegroundColor();
	lcd->setColors(GREEN, YELLOW);
	lcd->printString(5, 30, 0, 64, 0, 35, Anchor::BOTTOM_LEFT, "%d.%02d", this->VoltageSet / 100, this->VoltageSet % 100);
}

void CVScreen::OnRotaryTurn(uint32_t rotaryKnobFlags, int16_t rotarySteps)
{
	(void*)(rotaryKnobFlags);
	if(rotarySteps==0) return;
	this->VoltageSet+=10*rotarySteps;
	lcd->printString(5, 30, 0, 64, 0, 35, Anchor::BOTTOM_LEFT, "%d", this->VoltageSet);
}

void CVScreen::OnTouch(int16_t x, int16_t y){(void*)x;(void*)y;}
void CVScreen::OnButtonShort(uint32_t buttonFlags){(void*)buttonFlags;}
void CVScreen::OnButtonLong(uint32_t buttonFlags){(void*)buttonFlags;}

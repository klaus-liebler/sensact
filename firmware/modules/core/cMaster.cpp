/*
 * homeconrol_master.c
 *
 *  Created on: 31.03.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */
#include <cMaster.h>
#include "console.h"
#include "cModel.h"
#include "cBsp.h"
#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "MASTR"
#include "cLog.h"
#include "shell.h"
#include "date.h"
#ifdef STM32F1
#include "flash_info.h"
#endif

volatile uint8_t UART_buffer_pointer=0;
volatile uint64_t UART_cmdBuffer64[16];
volatile uint8_t *UART_cmdBuffer = (uint8_t *)UART_cmdBuffer64;
volatile bool BufferHasMessage=false;

using namespace date;
using namespace std::chrono;

//Was ist zu tun, um die sensactUP vom Programmieren zu befreien?
//1) IAP - kompliziert und langwierig
//2) UPs ohne eigene Verkettungslogik - Buttons und Drehimpulsgeber senden nur Events auf den Bus
//und zwar einfache Events: Pressed, Released, turned (units).
//diese Funktionalität benötigt keine app, sondern "feuert" unter der appId des sensactUP
//das Event heißt "INPUT_CHANGED" und enthält in einem 32bit-Wort die aktuelle Buttonbelegung )incl Drehgeber)
//das Event heißt "INC1_TURNED(int32 changeunits) oder INC2_TURNED(int32 changeunits)
//das Kommando heißt SET_PWM(uint32 PWM_MASK, uint16_t value): Alle mit PWM_MASK bezeichneten PWM-Ausgänge werden auf den Wert value gesetzt. Falls PWM_MASK==0, passiert also gar nicht


namespace sensact {

CANMessage cMaster::rcvMessage;
const uint32_t cMaster::CMD_EVT_OFFSET=0x400;
pIapPseudoFunction cMaster::JumpToApplication;
uint32_t cMaster::JumpAddress;
eApplicationID cMaster::heartbeatBuffer = eApplicationID::NO_APPLICATION;

/*
 * Avoids sending the save heartbeat message over and over if the same standby controller is requested from several apps of this node
 */
void cMaster::BufferHeartbeat(eApplicationID target, Time_t now)
{
	if(heartbeatBuffer != eApplicationID::NO_APPLICATION && heartbeatBuffer != target)
	{
		cApplication::SendHEARTBEATCommand(heartbeatBuffer, (uint32_t)MODEL::NodeMasterApplication, now);
	}
	heartbeatBuffer=target;
}


void cMaster::StartIAP()
{
#ifdef STM32F1
    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if (((*(__IO uint32_t*)IAP_APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (IAP_APPLICATION_ADDRESS + 4);
      JumpToApplication = (pIapPseudoFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) IAP_APPLICATION_ADDRESS);
      JumpToApplication();
    }
#endif
}

void cMaster::Run(void) {
	BSP::Init();
	uint16_t i = 0;
	uint16_t appCnt=0;
	//hier bei "1" beginnen, weil "0" der lokale/globale (?) Master ist; ggf zukï¿½nftig auch mit eigenem "Init"
	for (i = 1; i < (uint16_t) eApplicationID::CNT; i++) {
		cApplication * const ap = MODEL::Glo2locCmd[i];
		if (ap) {
			if(ap->Setup())
			{
				LOGI("App %s successfully configured", MODEL::ApplicationNames[i]);
			}
			else
			{
				LOGI("Error while configuring App %s!", MODEL::ApplicationNames[i]);
			}
			appCnt++;
		}
	}
	LOGI("%u local applications have been configured. Now, %s is pleased to be at your service.\r\n", appCnt, MODEL::ModelString);

	while (true) {
		Time_t now = BSP::GetSteadyClock();
		for (i = 0; i < (uint16_t) eApplicationID::CNT; i++) {
			cApplication * const ap = MODEL::Glo2locCmd[i];
			if (ap) {
				ap->DoEachCycle(now);
				//CanBusProcess();
			}
		}
		if(BufferHasMessage)
		{
			cShell::processCmds((uint8_t*)UART_cmdBuffer, UART_buffer_pointer);
			for(uint16_t i=0;i<UART_buffer_pointer;i++)
			{
				UART_cmdBuffer[i]='\0';
			}
			UART_buffer_pointer=0;
			BufferHasMessage=false;
		}
		CanBusProcess();
#ifdef DUMP_FRONTEND
		static uint32_t lastAllInputs = 0;
		static uint16_t lastRot1Value = 0;
		static uint16_t lastRot2Value=0;
		uint32_t inputs = BSP::GetAllOnboardInputsLowLevel();
		if(inputs != lastAllInputs)
		{
			SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::INPUT_CHANGED, (uint8_t*)&inputs, 4);
		}
		lastAllInputs=inputs;

		uint16_t rot1val = BSP::GetRotaryEncoderValue(eRotaryEncoder::ROTARYENCODER_1);
		if(rot1val!=lastRot1Value)
		{
			int16_t change =  rot1val-lastRot1Value;
			cMaster::SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::TURNED, (uint8_t*)&change ,2);
		}
		lastRot1Value=rot1val;

		uint16_t rot2val = BSP::GetRotaryEncoderValue(eRotaryEncoder::ROTARYENCODER_2);
		if(rot2val!=lastRot2Value)
		{
			int16_t change =  rot2val-lastRot2Value;
			cMaster::SendEventDirect(now, MODEL::NodeMasterApplication, eEventType::TURNED, (uint8_t*)&change ,2);
		}
		lastRot2Value=rot2val;
#endif
		BSP::DoEachCycle(now);
		//special call to release and reset message
		BufferHeartbeat(eApplicationID::NO_APPLICATION, now);
		while (BSP::GetSteadyClock()-now < 20) {
			CanBusProcess();
		}
	}
}

void cMaster::OnCommand(eCommandType command, uint8_t *data, uint8_t dataLenght, Time_t now)
{
	UNUSED(command);
	UNUSED(data);
	UNUSED(dataLenght);
	UNUSED(now);
}


bool cMaster::SendCommandToMessageBus(Time_t now, eApplicationID destinationApp, eCommandType cmd,
		const uint8_t * const payload, const uint8_t payloadLength) {
	if(destinationApp==eApplicationID::NO_APPLICATION)
	{
		return true;
	}
	if ((uint16_t) destinationApp < (uint16_t)eApplicationID::CNT) {
		cApplication * const app = MODEL::Glo2locCmd[(uint16_t) destinationApp];
		if (app != NULL) {
			//only in this case, the message can be processed local; no need to send it to can
			app->OnCommand(cmd, payload, payloadLength, now);
			return true;
		} else {
			//Send to Hardware-CAN
			CANMessage m;
			m.Id = (uint16_t) destinationApp;
			m.Length = payloadLength+1;
			m.Data[0]=(uint8_t)cmd;
			int i;
			for (i = 0; i < payloadLength; i++) {
				m.Data[i+1] = payload[i];
			}
			return BSP::SendCANMessage(&m);
		}
	}
	LOGE("Trying to send to an invalid application id %i", (uint16_t)destinationApp);
	return false;
}

void cMaster::SendEventDirect(Time_t now, const eApplicationID sourceApp, const eEventType evt, uint8_t * payload, uint8_t payloadLength)
{
	cApplication * const app = MODEL::Glo2locEvt[(uint16_t)sourceApp];
	if (app != NULL) {
		app->OnEvent(sourceApp, evt, payload, payloadLength, now);
	}
	CANMessage m;
	m.Id = (uint16_t) sourceApp + CMD_EVT_OFFSET;
	m.Length = payloadLength+1;
	m.Data[0] = (uint8_t)evt;
	int i = 0;
	for (i = 0; i < payloadLength; i++) {
		m.Data[i+1] = payload[i];
	}
	BSP::SendCANMessage(&m);
}

void cMaster::SendEvent(Time_t now, const eApplicationID sourceApp, const eEventType evt, const eEventType *const localEvts, const uint8_t localEvtsLength, const eEventType *const busEvts, const uint8_t busEvtsLength, uint8_t * payload, uint8_t payloadLength)
{
	uint8_t i;
	if ((uint16_t) sourceApp < CMD_EVT_OFFSET) {
		for(i=0;i<localEvtsLength;i++)
		{
			eEventType test = localEvts[i];
			if(evt==test)
			{
				cApplication *app = MODEL::Glo2locEvt[(uint16_t)sourceApp];
				if (app != NULL) {
					app->OnEvent(sourceApp, evt, payload, payloadLength, now);
				}
				break;
			}
		}
		for(i=0;i<busEvtsLength;i++)
		{
			eEventType test = busEvts[i];
			if(evt==test)
			{
				//Send to Hardware-CAN
				CANMessage m;
				m.Id = (uint16_t) sourceApp + CMD_EVT_OFFSET;
				m.Length = payloadLength+1;
				m.Data[0] = (uint8_t)evt;
				int i = 0;
				for (i = 0; i < payloadLength; i++) {
					m.Data[i+1] = payload[i];
				}
				BSP::SendCANMessage(&m);
				break;
			}
		}
	}
}


//Wir arbeiten mit Extended IDs
//Die niedrigsten 10 Bits definieren die Application
//Das 11. (=10!) Bit definiert, ob die App ein Event sendet (==1) oder ob ein Befehl an diese App zu senden ist (==0)
//Wenn das höchste Bit gesetzt ist, handelt es sich um einen Befehl des Bootloaders
#define HIGHEST_CAN_BIT 0x10000000
void cMaster::CanBusProcess() {

	Time_t now = BSP::GetSteadyClock();
	while (BSP::ReceiveCANMessage(&rcvMessage))
	{
		if((rcvMessage.Id & HIGHEST_CAN_BIT) != 0)
		{
			//This is a Message for the Bootloader
			return;
		}
		uint32_t appId = rcvMessage.Id & (CMD_EVT_OFFSET - 1);
		if(appId >= (uint32_t)eApplicationID::CNT)
		{
			LOGD("Unknown applicationID %i", appId);
			return;
		}
		if (rcvMessage.Id < CMD_EVT_OFFSET) {
			//appId is the id of the target app
			if(MODEL::TRACE_COMMANDS)
			{
				LOGX("Command to id:%s; len:%d; payload:", MODEL::ApplicationNames[appId], rcvMessage.Length);
				for (int i = 0; i < rcvMessage.Length; i++)
				{
					Console::Write("0x%02X, ", rcvMessage.Data[i]);
				}
				Console::Writeln("");
			}
			cApplication *app = MODEL::Glo2locCmd[appId];
			if (app != NULL) {
				app->OnCommand((eCommandType)rcvMessage.Data[0], &(rcvMessage.Data[1]), rcvMessage.Length, now);
			}
		}
		else
		{
			//appId is the id of the source app
			if(MODEL::TRACE_EVENTS)
			{
				LOGX("Event from id:%s; len:%d; payload:", MODEL::ApplicationNames[appId], rcvMessage.Length);
				for (int i = 0; i < rcvMessage.Length; i++)
				{
					Console::Write("0x%02X, ", rcvMessage.Data[i]);
				}
				Console::Writeln("");
			}
			cApplication *app = MODEL::Glo2locEvt[appId];
			if (app != NULL) {
				app->OnEvent((eApplicationID) appId, (eEventType)rcvMessage.Data[0],  &(rcvMessage.Data[1]), rcvMessage.Length, now);
			}
		}
	}

}

std::chrono::hours
cMaster::utc_offset_cet(std::chrono::system_clock::time_point tp)
{
    constexpr auto CET = hours(1);
    constexpr auto CEST = hours(2);
    const auto y = year_month_day(floor<days>(tp)).year();
    const auto begin = day_point(sun[last]/mar/y) + hours(1); // CEST begins at this UTC time
    const auto end   = day_point(sun[last]/10/y) + hours(1); // CEST ends at this UTC time
    if (tp < begin || end <= tp)
        return CET;
    return CEST;
}

std::chrono::hours
cMaster::cet_offset_utc(std::chrono::system_clock::time_point tp)
{

    constexpr auto CET = -hours(1);
    constexpr auto CEST = -hours(2);
    const auto y = year_month_day(floor<days>(tp)).year();
    const auto begin = day_point(sun[last]/mar/y) + hours(2); // CEST begins at this CET time
    const auto end   = day_point(sun[last]/10/y) + hours(3); // CEST ends at this CET time
    if (tp < begin || end <= tp)
        return CET;
    return CEST;
}


Time_t cMaster::Date2unixtimeMillis(uint16_t jahr, uint8_t monat, uint8_t tag, uint8_t stunde, uint8_t minute, uint8_t sekunde)
{
	const uint16_t tage_seit_jahresanfang[12] = /* Anzahl der Tage seit Jahresanfang ohne Tage des aktuellen Monats und ohne Schalttag */
    {0,31,59,90,120,151,181,212,243,273,304,334};

  int schaltjahre = ((jahr-1)-1968)/4 /* Anzahl der Schaltjahre seit 1970 (ohne das evtl. laufende Schaltjahr) */
                  - ((jahr-1)-1900)/100
                  + ((jahr-1)-1600)/400;

  long tage_seit_1970 = (jahr-1970)*365 + schaltjahre
                      + tage_seit_jahresanfang[monat-1] + tag-1;

  if ( (monat>2) && (jahr%4==0 && (jahr%100!=0 || jahr%400==0)) )
    tage_seit_1970 += 1; /* +Schalttag, wenn jahr Schaltjahr ist */
  return (sekunde + 60 * ( minute + 60 * (stunde + 24*tage_seit_1970) ))*1000;
}

}


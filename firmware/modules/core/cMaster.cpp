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

volatile uint8_t UART_buffer_pointer=0;
volatile uint64_t UART_cmdBuffer64[16];
volatile uint8_t *UART_cmdBuffer = (uint8_t *)UART_cmdBuffer64;
volatile bool BufferHasMessage=false;

using namespace date;
using namespace std::chrono;

namespace sensact {

CANMessage cMaster::rcvMessage;
const uint32_t cMaster::CMD_EVT_OFFSET=0x400;

void cMaster::Run(void) {
	BSP::Init();
	uint16_t i = 0;
	uint16_t appCnt=0;
	//hier bei "1" beginnen, weil "0" der lokale/globale (?) Master ist; ggf zuk�nftig auch mit eigenem "Init"
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
	LOGI("%u local applications have been configured. Now, sensact is pleased to be at your service.\r\n", appCnt);

	while (true) {
		Time_t now = BSP::GetTime();
		for (i = 0; i < (uint16_t) eApplicationID::CNT; i++) {
			cApplication * const ap = MODEL::Glo2locCmd[i];
			if (ap) {
				ap->DoEachCycle(now);
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
		while (BSP::ReceiveCANMessage(&rcvMessage)) {
			//Process CAN Message
			ReceiveFromMessageBus();
		}
		BSP::DoEachCycle(now);
		BSP::WaitAtLeastSinceLastCycle(20);

	}
}

bool cMaster::SendCommandToMessageBus(Time_t now, eApplicationID destinationApp, eCommandType cmd,
		uint8_t *payload, uint8_t payloadLength) {
	if(destinationApp==eApplicationID::NO_APPLICATION)
	{
		return true;
	}
	if ((uint16_t) destinationApp < CMD_EVT_OFFSET) {
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
			BSP::SendCANMessage(&m);
		}
		return true;
	}
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
void cMaster::ReceiveFromMessageBus() {
	uint32_t appId = rcvMessage.Id & (CMD_EVT_OFFSET - 1);
	Time_t now = BSP::GetTime();

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


/*
  * hc_pwm.cpp
 *
 *  Created on: 10.05.2015
 *      Author: klaus
 */
#include <cMaster.h>
#include <cPwm.h>
#define LOGLEVEL LEVEL_WARN
#define LOGNAME "PWM  "
#include <cLog.h>
namespace sensact {

static const int MAXIMUM_LEVEL =UINT8_MAX;
static const int TIME_TO_FORGET_DIM_DIRECTION=5000;
static const int DIM_TO_TARGET_STEP=5;

//targetValue absolut setzen oder aktuellen targetValue verändern mit einem sint16_t
//oder ausschalten, sonst geht der targetLevel nicht auf 0

cPWM::cPWM(const char* name, const eApplicationID id, const ePWMOutput *const output, const uint8_t outputLength, const uint8_t minimalLevel, const uint8_t initialStoredTargetLevel,  const bool lowMeansLampOn, const eApplicationID standbyController, const Time_t autoOffIntervalMsecs) :
		cApplication(name, id, eAppType::PWM),
		output(output),
		outputLength(outputLength),
		minimalOnLevel(minimalLevel),
		lowMeansLampOn(lowMeansLampOn),
		standbyController(standbyController),
		autoOffIntervalMsecs(autoOffIntervalMsecs),
		currentLevel(0),
		storedTargetLevel(initialStoredTargetLevel),
		lastUserSignal(0),
		OneButtonDimDirection(eDirection::UP),
		autoDimDirection(eDirection::STOP),
		targetLevel(0),
		lastHeartbeatToStandbycontroller(0),
		autoOffTime(TIME_MAX)
{

}

void cPWM::OnSET_VERTICAL_TARGETCommand(uint16_t target, Time_t now)
{
	SetTargetAbsolute(target, now);
}

void cPWM::SetTargetAbsolute(uint8_t level, Time_t now)
{
	lastUserSignal=now;
	level = max(level, minimalOnLevel);
	this->targetLevel = level;
}

void cPWM::SetTargetRelative(int step, Time_t now)
{
	lastUserSignal=now;
	if(targetLevel + step > MAXIMUM_LEVEL )
	{
		targetLevel = MAXIMUM_LEVEL;
	}
	else if ( targetLevel + step < minimalOnLevel)
	{
		targetLevel = minimalOnLevel;
	}
	else
	{
		targetLevel+=step;
	}
}

void cPWM::MoveInDirection(eDirection dir, Time_t now)
{
	lastUserSignal=now;
	autoDimDirection=dir;
}

void cPWM::StopMove(Time_t now)
{
	lastUserSignal=now;
	autoDimDirection=eDirection::STOP;
}

//gesendet vom Inkrementalgeber
void cPWM::OnSTEP_VERTICALCommand(int16_t step, Time_t now) {

	if(targetLevel==0)
	{
		if(step>0)
			SetTargetAbsolute(minimalOnLevel, now);
		else
			SetTargetAbsolute(MAXIMUM_LEVEL, now);
	}
	else
	{
		SetTargetRelative(4*step, now);
	}
	LOGD("%s sets PWM Value to %d", this->Id, currentLevel);
}

//gesendet vom 1BP
void cPWM::OnSTARTCommand(Time_t now)
{
	if(now-this->lastUserSignal> TIME_TO_FORGET_DIM_DIRECTION)
	{
		this->OneButtonDimDirection=eDirection::UP;
	}
	else
	{
		this->OneButtonDimDirection = OneButtonDimDirection!=eDirection::UP?eDirection::UP:eDirection::DOWN;
	}

	MoveInDirection(this->OneButtonDimDirection, now);
}

//gesendet vom 1PB
void cPWM::OnSTOPCommand(Time_t now)
{
	StopMove(now);
}

void cPWM::OnUPCommand(Time_t now)
{
	MoveInDirection(eDirection::UP, now);
}


void cPWM::OnTOGGLECommand(Time_t now)
{
	UNUSED(now);
	if (targetLevel == 0) {
		targetLevel=storedTargetLevel;
	}
	else
	{
		storedTargetLevel=targetLevel;
		targetLevel=0;
	}
}

void cPWM::OnONCommand(uint32_t autoReturnToOffMsecs, Time_t now) {
	UNUSED(autoReturnToOffMsecs);
	UNUSED(now);
	targetLevel=UINT8_MAX;
	if(autoOffIntervalMsecs!=0)
	{
		autoOffTime=now+autoOffIntervalMsecs;
	}
}



void cPWM::OnDOWNCommand(Time_t now) {
	MoveInDirection(eDirection::DOWN, now);
}

void cPWM::DoEachCycle(volatile Time_t now) {
	if(autoOffIntervalMsecs!=0 && autoOffTime<now && targetLevel>0)
	{
		targetLevel=0;
		autoOffTime =TIME_MAX;
	}

	switch (autoDimDirection) {
		case eDirection::STOP:
			break;
		case eDirection::UP:
			if(targetLevel<MAXIMUM_LEVEL)
			{
				targetLevel++;
			}
			break;
		case eDirection::DOWN:
			if(targetLevel>minimalOnLevel)
			{
				targetLevel--;
			}
			break;
		default:
			break;
	}
	if(targetLevel>currentLevel)
	{
		if(currentLevel+DIM_TO_TARGET_STEP>targetLevel)
		{
			currentLevel=targetLevel;
		}
		else
		{
			currentLevel+=DIM_TO_TARGET_STEP;
		}
		SetDimLevel(currentLevel);
	}
	else if(targetLevel<currentLevel)
	{
		if(currentLevel-DIM_TO_TARGET_STEP<targetLevel)
		{
			currentLevel=targetLevel;
		}
		else
		{
			currentLevel-=DIM_TO_TARGET_STEP;
		}
		SetDimLevel(currentLevel);
	}

	if(standbyController!=eApplicationID::NO_APPLICATION && currentLevel>0 && now-lastHeartbeatToStandbycontroller>10000)
	{
		cMaster::SendCommandToMessageBus(now, standbyController, eCommandType::HEARTBEAT, 0, 0);
		lastHeartbeatToStandbycontroller=now;
	}
	return;
}

void cPWM::SetDimLevel(uint8_t level) {

	LOGD("SetDimLevel = %d", level);
	currentLevel = level;
	uint8_t i;
	uint16_t val = cPWM::level2brightness[level];
	if(!lowMeansLampOn)
	{
		val=UINT16_MAX-val;
	}
	for(i=0;i<outputLength;i++)
	{
		BSP::SetPWM(this->output[i], val);
	}
}

bool cPWM::Setup() {
	uint8_t i;
	for(i=0;i<outputLength;i++)
	{
		if(!BSP::RequestPWM(this->output[i], this->lowMeansLampOn))
		{
			return false;
		}
	}
	SetDimLevel(0);
	return true;
}

const uint16_t cPWM::level2brightness[] = { 0, 66, 67, 69, 71, 73, 75, 77,
		79, 81, 84, 86, 88, 91, 93, 96, 99, 101, 104, 107, 110, 113, 116, 119,
		122, 126, 129, 133, 137, 140, 144, 148, 152, 156, 161, 165, 170, 174,
		179, 184, 189, 194, 200, 205, 211, 217, 223, 229, 235, 242, 248, 255,
		262, 270, 277, 285, 292, 301, 309, 317, 326, 335, 344, 354, 364, 374,
		384, 394, 405, 417, 428, 440, 452, 464, 477, 490, 504, 518, 532, 547,
		562, 577, 593, 610, 626, 644, 661, 680, 698, 718, 737, 758, 779, 800,
		822, 845, 868, 892, 917, 942, 968, 994, 1022, 1050, 1079, 1109, 1139,
		1171, 1203, 1236, 1270, 1305, 1341, 1378, 1416, 1455, 1495, 1537, 1579,
		1622, 1667, 1713, 1760, 1809, 1859, 1910, 1963, 2017, 2072, 2130, 2188,
		2249, 2311, 2374, 2440, 2507, 2576, 2647, 2720, 2795, 2872, 2951, 3033,
		3116, 3202, 3290, 3381, 3474, 3570, 3669, 3770, 3874, 3981, 4090, 4203,
		4319, 4438, 4560, 4686, 4815, 4948, 5084, 5225, 5369, 5517, 5669, 5825,
		5986, 6151, 6320, 6494, 6673, 6857, 7046, 7241, 7440, 7645, 7856, 8073,
		8295, 8524, 8759, 9001, 9249, 9504, 9766, 10035, 10312, 10596, 10888,
		11188, 11497, 11814, 12139, 12474, 12818, 13171, 13534, 13907, 14291,
		14685, 15090, 15506, 15933, 16372, 16824, 17288, 17764, 18254, 18757,
		19274, 19806, 20352, 20913, 21489, 22082, 22690, 23316, 23959, 24619,
		25298, 25996, 26712, 27449, 28205, 28983, 29782, 30603, 31447, 32314,
		33205, 34120, 35061, 36027, 37020, 38041, 39090, 40168, 41275, 42413,
		43582, 44784, 46018, 47287, 48591, 49930, 51307, 52721, 54175, 55668,
		57203, 58780, 60400, 62066, 63777, 65535 };

}

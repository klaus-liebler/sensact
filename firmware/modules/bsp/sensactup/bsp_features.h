/*
 * sensact_bsp_features.h
 *
 *  Created on: 08.09.2015
 *      Author: klaus
 */

#pragma once

namespace sensact {

#ifdef SENSACTUP02
#define HAS_DS2482
#endif

const uint8_t RGBLED_SIZE=0;

enum struct ePoweredOutput : uint32_t
{
	O01,
	O02,//C13
	O03,
	O04,//C14
	O05,
	O06,//C15
	O07,
	O_LED,
	CNT=4,
};

enum struct ePWMOutput:uint8_t
{
	P01=0,//T4.3, B6-9, Default
	P03=2,//T4.2
	P05=4,//T4.1
	P07=6,//T4.4
	P16=64,
	P17=65,
	P18=66,
	P19=67,
	P20=68,
	P21=69,
	P22=70,
	P23=71,
	P24=72,
	P25=73,
	P26=74,
	P27=75,
	P28=76,
	P29=77,
	P30=78,
	P31=79,
	CNT=20,
	NONE=255,
};

enum struct eRotaryEncoder:uint8_t
{

#ifdef SENSACTUP01
	ROTARYENCODER_1, //T4, B6+B7, Button auf B8, aktiv high
	CNT=1
#endif
#ifdef SENSACTUP02
	ROTARYENCODER_1, //T2, A0+1, Default, button auf B15
	ROTARYENCODER_2, //T3, B4+5, Remap
	CNT=2
#endif
};

enum struct eInput : uint8_t
{
	I01=0,//A6
	I02=1,//A7
	I03=2,//B0
	I04=3,//B1
	I05=4,//A4
	I06=5,//A5
	I07=6,//A2
	I08=7,//A3
#ifdef SENSACTUP01
	ROTAR_PUSH_1=8,
	CNT=8,
#endif
#ifdef SENSACTUP02
	ROTAR_PUSH_1=8,//B15
	ROTAR_PUSH_2=9,//B3
	CNT=9,
#endif
};

}

/*
 * sensact_bsp_features.h
 *
 *  Created on: 08.09.2015
 *      Author: klaus
 */

#pragma once

namespace sensact {


#ifdef SENSACTHS04
const uint8_t RGBLED_SIZE=0;
enum struct ePoweredOutput : uint32_t
{
	O6_1=8+32,
	O6_2=9+32,
	O6_3=10+32,
	O6_4=11+32,
	O6_5=12+32,
	O6_6=13+32,
	O6_7=14+32,
	O6_8=15+32,
	O5_1=0+32,
	O5_2=1+32,
	O5_3=2+32,
	O5_4=3+32,
	O5_5=4+32,
	O5_6=5+32,
	O5_7=6+32,
	O5_8=7+32,
	O7_5=0,
	O7_6=1,
	O7_7=2,
	CNT=19
};

enum struct ePWMOutput:uint8_t
{
	O1_1,
	O1_2,
	O2_1,
	O2_2,
	O3_1,
	O3_2,
	O4_1,
	O4_2,
	O7_2,
	O7_3,
	O7_4,
	CNT
};

enum struct eRotaryEncoder:uint8_t
{
	ROTARYENCODER_1,
	ROTARYENCODER_2,
	CNT=2
};

enum struct eInput : uint8_t
{
	I01=0,
	I02=1,
	I03=2,
	I04=3,
	I05=4,
	I06=5,
	I07=6,
	I08=7,
	I09=8,
	I10=9,
	I11=10,
	I12=11,
	I13=12,
	I14=13,
	I15=14,
	CNT=15
};
#endif
#ifdef SENSACTHS07
const uint8_t RGBLED_SIZE=0;
enum struct ePoweredOutput
	: uint8_t
	{
	O01 = 0,
	O02 = 1,
	O03 = 2,
	O04 = 3,
	O05 = 4,
	O06 = 5,
	O07 = 6,
	O08 = 7,

	O09 = 48,
	O10 = 49,
	O11 = 50,
	O12 = 51,
	O13 = 52,
	O14 = 53,
	O15 = 54,
	O16 = 55,
	O17 = 40,
	O18 = 41,
	O19 = 42,
	O20 = 43,
	O21 = 44,
	O22 = 45,
	O23 = 46,
	O24 = 47,
	O25 = 32,
	O26 = 33,
	O27 = 34,
	O28 = 35,
	O29 = 36,
	O30 = 37,
	O31 = 38,
	O32 = 39,
};

enum struct ePWMOutput
	:uint8_t
	{
	NONE=UINT8_MAX,
	P01=8+64,
	P02=9+64,
	P03=10+64,
	P04=11+64,
	P05=12+64,
	P06=13+64,
	P07=14+64,
	P08=15+64,
	P09=7+64,
	P10=6+64,
	P11=5+64,
	P12=4+64,
	P13=3+64,
	P14=2+64,
	P15=1+64,
	P16=0+64,
	P17=16+8+64,
	P18=16+9+64,
	P19=16+10+64,
	P20=16+11+64,
	P21=16+12+64,
	P22=16+13+64,
	P23=16+14+64,
	P24=16+15+64,
	P25=16+7+64,
	P26=16+6+64,
	P27=16+5+64,
	P28=16+4+64,
	P29=16+3+64,
	P30=16+2+64,
	P31=16+1+64,
	P32=16+0+64,
	OP01=0,
	OP02=1,
	OP03=2,
	OP04=3,
	OP05=4,
	OP06=5,
	OP07=6,
	OP08=7,
};

enum struct eRotaryEncoder
	:uint8_t
	{
		ROTARYENCODER_1, CNT = 1
};

enum struct eInput
	: uint8_t
	{
	I01 = 16 + 13 + 64,
	I02 = 16 + 12 + 64,
	I03 = 16 + 14 + 64,
	I04 = 16 + 15 + 64,
	I05 = 16 + 10 + 64,
	I06 = 16 + 11 + 64,
	I07 = 16 + 9 + 64,
	I08 = 16 + 8 + 64,
	I09 = 16 + 6 + 64,
	I10 = 16 + 7 + 64,
	I11 = 16 + 5 + 64,
	I12 = 16 + 4 + 64,
	I13 = 16 + 1 + 64,
	I14 = 16 + 0 + 64,
	I15 = 16 + 2 + 64,
	I16 = 16 + 3 + 64,
	I17 = 13 + 64,
	I18 = 12 + 64,
	I19 = 14 + 64,
	I20 = 15 + 64,
	I21 = 10 + 64,
	I22 = 11 + 64,
	I23 = 9 + 64,
	I24 = 8 + 64,
	I25 = 6 + 64,
	I26 = 7 + 64,
	I27 = 5 + 64,
	I28 = 4 + 64,
	I29 = 1 + 64,
	I30 = 0 + 64,
	I31 = 2 + 64,
	I32 = 3 + 64,
	ROTAR_PUSH_1=0,
	II01=1,
	II02=2,
	II03=3,
	II04=4,
	II05=5,
	II06=6,
	II07=7,
	II08=8,
	II09=9,
	II10=10,
	II11=11,
	II12=12,
	II13=13,
	CNT = 46,
};
#endif

}

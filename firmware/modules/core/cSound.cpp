/*
 * hc_blind.cpp
 *
 *  Created on: 18.04.2015
 *      Author: Klaus Liebler
 *	   Contact: mail@klaus-liebler.de
 *     Licence: none
 */

#include <cMaster.h>
#include <cModel.h>
#include <cSound.h>
#include <chrono>

#define LOGLEVEL LEVEL_DEBUG
#define LOGNAME "SOUND"
#include <cLog.h>


namespace sensact {

static uint8_t send_buf[10] = {0x7E, 0xFF,0x06,0,0,0,0,0,0,0xEF};
//static bool is_reply = false;


//
static void fill_uint16_bigend (uint8_t *thebuf, uint16_t data)
{
	*thebuf =	(uint8_t)(data>>8);
	*(thebuf+1) =	(uint8_t)data;
}


//calc checksum (1~6 byte)
static uint16_t mp3_get_checksum (uint8_t *thebuf)
{
	uint16_t sum = 0;
	for (int i=1; i<7; i++)
	{
		sum += thebuf[i];
	}
	return -sum;
}

//fill checksum to send_buf (7~8 byte)
static void mp3_fill_checksum ()
{
	uint16_t checksum = mp3_get_checksum (send_buf);
	fill_uint16_bigend (send_buf+7, checksum);
}

static void send_func ()
{
	HAL_UART_Transmit(&BSP::BELL, send_buf, 10, 1000);
}




static void mp3_send_cmd (uint8_t cmd, uint16_t arg)
{
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), arg);
	mp3_fill_checksum ();
	send_func();
}





//play mp3 file in mp3 folder in your tf card
static void mp3_play (uint16_t num) {
	mp3_send_cmd (0x12, num);
}



static void mp3_set_volume (uint16_t volume)
{
	mp3_send_cmd (0x06, volume);
}

/*
static void mp3_send_cmd (uint8_t cmd)
{
	send_buf[3] = cmd;
	fill_uint16_bigend ((send_buf+5), 0);
	mp3_fill_checksum ();
	send_func();
}
static void mp3_set_reply (bool state)
{
	is_reply = state;
	send_buf[4] = is_reply;
}


static void mp3_play ()
{
	mp3_send_cmd (0x0d);
}
//
static void mp3_play_physical (uint16_t num)
{
	mp3_send_cmd (0x03, num);
}
//
static void mp3_play_physical ()
{
	mp3_send_cmd (0x03);
}

//
static void mp3_next ()
{
	mp3_send_cmd (0x01);
}

//
static void mp3_prev ()
{
	mp3_send_cmd (0x02);
}

//0x06 set volume 0-30


//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
static void mp3_set_EQ (uint16_t eq)
{
	mp3_send_cmd (0x07, eq);
}

//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
static void mp3_set_device (uint16_t device)
{
	mp3_send_cmd (0x09, device);
}

//
static void mp3_sleep ()
{
	mp3_send_cmd (0x0a);
}

//
static void mp3_reset ()
{
	mp3_send_cmd (0x0c);
}

//


//
static void mp3_pause ()
{
	mp3_send_cmd (0x0e);
}

//
static void mp3_stop () {
	mp3_send_cmd (0x16);
}



//
static void mp3_get_state () {
	mp3_send_cmd (0x42);
}

//
static void mp3_get_volume () {
	mp3_send_cmd (0x43);
}


//
static void mp3_get_u_sum () {
	mp3_send_cmd (0x47);
}

//
static void mp3_get_tf_sum () {
	mp3_send_cmd (0x48);
}

//
static void mp3_get_flash_sum () {
	mp3_send_cmd (0x49);
}


//
static void mp3_get_tf_current () {
	mp3_send_cmd (0x4c);
}

//
static void mp3_get_u_current () {
	mp3_send_cmd (0x4b);
}


//
static void mp3_get_flash_current () {
	mp3_send_cmd (0x4d);
}

//
static void mp3_single_loop (bool state) {
	mp3_send_cmd (0x19, !state);
}

//add
static void mp3_single_play (uint16_t num) {
	mp3_play (num);
	HAL_Delay(10);
	mp3_single_loop (true);
	//mp3_send_cmd (0x19, !state);
}

//
static void mp3_DAC (bool state) {
	mp3_send_cmd (0x1a, !state);
}

//
static void mp3_random_play () {
	mp3_send_cmd (0x18);
}
*/
bool cSound::Setup() {
	return true;
}

void cSound::OnSET_SIGNALCommand(uint16_t signal, Time_t now)
{
	UNUSED(now);
	//mp3_stop();
	//volumeSchedule->DoEachCycle(std::chrono::system_clock::now());
	//uint32_t vol = volumeSchedule->GetCurrentValue();
	mp3_set_volume(15);
	HAL_Delay(20);
	mp3_play(signal);
}

void cSound::OnSTARTCommand(Time_t now)
{
	OnSET_SIGNALCommand(1, now);
}

void cSound::DoEachCycle(Time_t now)
{
	UNUSED(now);
}
}


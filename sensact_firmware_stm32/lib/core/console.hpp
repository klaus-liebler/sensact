#pragma once

#include <inttypes.h>
#include <stdarg.h>
#include <sensacthal.hpp>

namespace sensactcore {

class Console {
private:
	static char* bf;
	static char buf[12];
	static int32_t num;
	static char uc;
	static char zs;
	static sensacthal::SensactHAL *hal;
	static void out(char c);
	static void outDgt(char dgt);
	static void divOut(uint32_t div);

public:
	static void SetHAL(sensacthal::SensactHAL *hal){Console::hal=hal;}
	static void Writeln(const char *fmt, ...);
	static void Write(const char *fmt, va_list argp);
	static void Write(const char *fmt, ...);
	static Time_t Timestamp(){
		Time_t now;
		hal->GetSteadyClock(&now);
		return now;
	}
};
}


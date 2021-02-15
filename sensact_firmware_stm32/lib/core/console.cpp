#include "console.hpp"

namespace sensactcore {

char* Console::bf;
char Console::buf[12];
int32_t Console::num;
char Console::uc;
char Console::zs;
sensacthal::SensactHAL *Console::hal;



void Console::out(char c) {
	*bf++ = c;
}

void Console::outDgt(char dgt) {
	out(dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10));
	zs = 1;
}

void Console::divOut(uint32_t div) {
	unsigned char dgt = 0;
	//num &= 0xffff; // just for testing the code  with 32 bit ints
	while ((uint32_t)num >= div) {
		num -= div;
		dgt++;
	}
	if (zs || dgt > 0)
		outDgt(dgt);
}

void Console::Writeln(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	Console::Write(fmt, va);
	va_end(va);
	Console::Write("\r\n");
}

void Console::Write(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	Console::Write(fmt, va);
	va_end(va);
}

void Console::Write(const char *fmt, va_list va) {
	char ch;
	char* p;
	while ((ch = *(fmt++))) {
		if (ch != '%') {
			hal->Write2Console(ch);
		} else {
			char lz = 0;
			char w = 0;
			ch = *(fmt++);
			if (ch == '0') {
				ch = *(fmt++);
				lz = 1;
			}
			if (ch >= '0' && ch <= '9') {
				w = 0;
				while (ch >= '0' && ch <= '9') {
					w = (((w << 2) + w) << 1) + ch - '0';
					ch = *fmt++;
				}
			}
			bf = buf;
			p = bf;
			zs = 0;
			switch (ch) {
			case 0:
				goto abort;
			case 'u':
			case 'd':
				num = va_arg(va, int);
				if (ch == 'd' && (int) num < 0) {
					num = -(int) num;
					out('-');
				}
				divOut(10000000L);
				divOut(1000000L);
				divOut(100000L);
				divOut(10000);
				divOut(1000);
				divOut(100);
				divOut(10);
				outDgt(num);
				break;
			case 'x':
			case 'X':
				uc = ch == 'X';
				num = va_arg(va, unsigned int);
				divOut(0x10000000);
				divOut(0x01000000);
				divOut(0x00100000);
				divOut(0x00010000);
				divOut(0x00001000);
				divOut(0x00000100);
				divOut(0x00000010);
				outDgt(num);
				break;
			case 'c':
				out((char) (va_arg(va, int)));
				break;
			case 's':
				p = va_arg(va, char*);
				break;
			case '%':
				out('%');
				break;
			default:
				break;
			}
			*bf = 0;
			bf = p;
			while (*bf++ && w > 0)
				w--;
			while (w-- > 0)
				hal->Write2Console(lz ? '0' : ' ');
			while ((ch = *p++))
				hal->Write2Console(ch);
		}
	}
	abort:;

}
}

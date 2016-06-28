#include "console.h"
#include "stm32f4xx_hal.h"

char* bf;
char buf[12];
int32_t num;
char uc;
char zs;

static void putcharX(char c);
static void out(char c);
static void outDgt(char dgt);
static void divOut(uint16_t div);
void Console_WriteInt(const char *fmt, va_list va);

void putcharX(char c) {
	while (!(USART1->SR & USART_SR_TXE))
		;
	USART1->DR = c;
}

void out(char c) {
	*bf++ = c;
}

void outDgt(char dgt) {
	out(dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10));
	zs = 1;
}

void divOut(uint16_t div) {
	unsigned char dgt = 0;
	//num &= 0xffff; // just for testing the code  with 32 bit ints
	while (num >= div) {
		num -= div;
		dgt++;
	}
	if (zs || dgt > 0)
		outDgt(dgt);
}



void Console_WriteInt(const char *fmt, va_list va) {
	char ch;
	char* p;
	while ((ch = *(fmt++))) {
		if (ch != '%') {
			putcharX (ch);
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
				divOut(0x1000);
				divOut(0x100);
				divOut(0x10);
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
				putcharX(lz ? '0' : ' ');
			while ((ch = *p++))
				putcharX (ch);
		}
	}
	abort:;

}


void Console_Writeln(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	Console_WriteInt(fmt, va);
	va_end(va);
	Console_Write("\r\n");
}

void Console_Write(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	Console_WriteInt(fmt, va);
	va_end(va);
}

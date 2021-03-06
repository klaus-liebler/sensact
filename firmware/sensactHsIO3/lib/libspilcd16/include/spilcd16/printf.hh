///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2014-2018, PALANDesign Hannover, Germany
// \author (c) Klaus Liebler (klaus.liebler@w-hs.de) transform to C++ class
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Tiny printf, sprintf and snprintf implementation, optimized for speed on
//        embedded systems with a very limited resources.
//        Use this instead of bloated standard/newlib printf.
//        These routines are thread safe and reentrant.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdarg.h>
#include <stddef.h>

class Print {
private:

	size_t _vsnprintf(char* buffer, const size_t maxlen, const char* format, va_list va);
	size_t _ntoa_format(char* buffer, size_t idx, size_t maxlen, char* buf, size_t len, bool negative, unsigned int base, unsigned int prec, unsigned int width, unsigned int flags);
	size_t _ntoa_long(char* buffer, size_t idx, size_t maxlen, unsigned long value, bool negative, unsigned long base, unsigned int prec, unsigned int width, unsigned int flags);
	size_t _ntoa_long_long(char* buffer, size_t idx, size_t maxlen, unsigned long long value, bool negative, unsigned long long base, unsigned int prec, unsigned int width, unsigned int flags);
	size_t _ftoa(char* buffer, size_t idx, size_t maxlen, double value, unsigned int prec, unsigned int width, unsigned int flags);
protected:
	/**
	 * Output a character to a custom device like UART, used by the printf() function
	 * This function is declared here only. You have to write your custom implementation in the derived class
	 * \param character Character to output
	 */
	virtual void out(char character, void* buffer, size_t idx, size_t maxlen);
public:
	virtual ~Print() {
	}
	;

	/**
	 * Tiny printf implementation
	 * You have to implement _putchar if you use printf()
	 * To avoid conflicts with the regular printf() API it is overridden by macro defines
	 * and internal underscore-appended functions like printf_() are used
	 * \param format A string that specifies the format of the output
	 * \return The number of characters that are written into the array, not counting the terminating null character
	 */
#define printf printf_
	size_t printf_(const char* format, ...);

	/**
	 * Tiny sprintf implementation
	 * Due to security reasons (buffer overflow) YOU SHOULD CONSIDER USING (V)SNPRINTF INSTEAD!
	 * \param buffer A pointer to the buffer where to store the formatted string. MUST be big enough to store the output!
	 * \param format A string that specifies the format of the output
	 * \return The number of characters that are WRITTEN into the buffer, not counting the terminating null character
	 */
#define sprintf sprintf_
	size_t sprintf_(char* buffer, const char* format, ...);

	/**
	 * Tiny snprintf/vsnprintf implementation
	 * \param buffer A pointer to the buffer where to store the formatted string
	 * \param count The maximum number of characters to store in the buffer, including a terminating null character
	 * \param format A string that specifies the format of the output
	 * \return The number of characters that are WRITTEN into the buffer, not counting the terminating null character
	 *         If the formatted string is truncated the buffer size (count) is returned
	 */
#define snprintf  snprintf_
#define vsnprintf vsnprintf_
	size_t snprintf_(char* buffer, size_t count, const char* format, ...);
	size_t vsnprintf_(char* buffer, size_t count, const char* format, va_list va);


};

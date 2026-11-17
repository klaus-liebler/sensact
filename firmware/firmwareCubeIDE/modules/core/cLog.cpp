#include "cLog.h"
#include "cBsp.h"
#include "console.h"
#include "stdarg.h"

namespace sensact{

CLogger CLogger::m_pThis;


CLogger* CLogger::getLogger(){
    return &m_pThis;
}

char CLogger::level2char[] = {'T', 'D', 'I', 'W', 'E'};

void CLogger::Trace(const char * format, const char *logname, ...)
{

	va_list argp;
	va_start(argp, logname);
	DoLog(Level::TRA, format, logname, argp);
	va_end(argp);
}
void CLogger::Debug(const char * format, const char *logname, ...)
{
	va_list argp;
	va_start(argp, logname);
	DoLog(Level::DEB, format, logname, argp);
	va_end(argp);
}
void CLogger::Info(const char * format, const char *logname, ...)
{
	va_list argp;
	va_start(argp, logname);
	DoLog(Level::INF, format, logname, argp);
	va_end(argp);
}

void CLogger::Warn(const char * format, const char *logname, ...)
{
	va_list argp;
	va_start(argp, logname);
	DoLog(Level::WAR, format, logname, argp);
	va_end(argp);
}
void CLogger::Error(const char * format, const char *logname, ...)
{
	va_list argp;
	va_start(argp, logname);
	DoLog(Level::ERR, format, logname, argp);
	va_end(argp);
}

void CLogger::DoLog(Level level, const char * format, const char *logname, va_list argp)
{
	Console::Write("%c %06d %s :", level2char[(uint8_t)level], BSP::GetSteadyClock(), logname);
	Console::Write(format, argp);
	Console::Writeln("");
}
}

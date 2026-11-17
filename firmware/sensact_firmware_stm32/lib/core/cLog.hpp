#pragma once
#include <cstdarg>
#include <string>
using namespace std;


#if !defined(LOGLEVEL)
	#define LOGLEVEL LEVEL_INFO
#endif
#if !defined(LOGNAME)
	#define LOGNAME "NONAME"
#endif
#define LOG sensactcore::CLogger::getLogger()
#define LOGX(x, ...)    do { if (true)LOG->Info(x, LOGNAME, ##__VA_ARGS__); } while (0)
#if LOGLEVEL ==  LEVEL_ALL
	#define LOGT(x, ...)    do { if (true)LOG->Trace(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGD(x, ...)    do { if (true)LOG->Debug(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGI(x, ...)    do { if (true)LOG->Info(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGW(x, ...)    do { if (true)LOG->Warn(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGE(x, ...)    do { if (true)LOG->Error(x, LOGNAME, ##__VA_ARGS__); } while (0)
#elif LOGLEVEL == LEVEL_DEBUG
	#define LOGT(x, ...)    do { if (false)LOG->Trace(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGD(x, ...)    do { if (true)LOG->Debug(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGI(x, ...)    do { if (true)LOG->Info(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGW(x, ...)    do { if (true)LOG->Warn(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGE(x, ...)    do { if (true)LOG->Error(x, LOGNAME, ##__VA_ARGS__); } while (0)
#elif LOGLEVEL == LEVEL_INFO
	#define LOGT(x, ...)    do { if (false)LOG->Trace(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGD(x, ...)    do { if (false)LOG->Debug(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGI(x, ...)    do { if (true)LOG->Info(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGW(x, ...)    do { if (true)LOG->Warn(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGE(x, ...)    do { if (true)LOG->Error(x, LOGNAME, ##__VA_ARGS__); } while (0)
#elif LOGLEVEL == LEVEL_WARN
	#define LOGT(x, ...)    do { if (false)LOG->Trace(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGD(x, ...)    do { if (false)LOG->Debug(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGI(x, ...)    do { if (false)LOG->Info(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGW(x, ...)    do { if (true)LOG->Warn(x, LOGNAME, ##__VA_ARGS__); } while (0)
	#define LOGE(x, ...)    do { if (true)LOG->Error(x, LOGNAME, ##__VA_ARGS__); } while (0)
#elif LOGLEVEL == LEVEL_ERROR
	#define LOGT(x, ...)    do { if (false)LOG->Trace(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGD(x, ...)    do { if (false)LOG->Debug(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGI(x, ...)    do { if (false)LOG->Info(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGW(x, ...)    do { if (false)LOG->Warn(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGE(x, ...)    do { if (true)LOG->Error(x, LOGNAME, ##__VA_ARGS__) } while (0)
#else
	#define LOGT(x, ...)    do { if (false)LOG->Trace(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGD(x, ...)    do { if (false)LOG->Debug(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGI(x, ...)    do { if (false)LOG->Info(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGW(x, ...)    do { if (false)LOG->Warn(x, LOGNAME, ##__VA_ARGS__) } while (0)
	#define LOGE(x, ...)    do { if (false)LOG->Error(x, LOGNAME, ##__VA_ARGS__) } while (0)
#endif /* DEBUG */

namespace sensactcore
{

	enum struct Level
	{
		TRA,
		DEB,
		INF,
		WAR,
		ERR
	};
	/**
 *   Singleton Logger Class.
 */
	class CLogger
	{
	public:
		/**
	 *   Logs a message
	 *   @param sMessage message to be logged.
	 */
		void Log(const std::string &sMessage);
		/**
	 *   Variable Length Logger function
	 *   @param format string for the message to be logged.
	 */
		void DoLog(Level level, const char *format, const char *logname, va_list argp);
		void Trace(const char *format, const char *logname, ...);
		void Debug(const char *format, const char *logname, ...);
		void Info(const char *format, const char *logname, ...);
		void Warn(const char *format, const char *logname, ...);
		void Error(const char *format, const char *logname, ...);

		/**
	 *   Funtion to create the instance of logger class.
	 *   @return singleton object of Clogger class..
	 */
		static CLogger *getLogger();
		CLogger()
		{
		}

	private:
		/**
	 *    Default constructor for the Logger class.
	 */

		/**
	 *   copy constructor for the Logger class.
	 */
		CLogger(const CLogger &){}; // copy constructor is private
		/**
	 *   assignment operator for the Logger class.
	 */
		CLogger &operator=(const CLogger &)
		{
			return *this;
		}; // assignment operator is private
		/**
	 *   Log file name.
	 **/

		/**
	 *   Singleton logger class object pointer.
	 **/
		static CLogger m_pThis;
		/**
	 *   Log file stream object.
	 **/

		static char level2char[];
	};

} // namespace sensactcore

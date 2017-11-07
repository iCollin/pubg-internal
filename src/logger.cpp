#include "logger.h"

Logger::Logger()
{
}

void Logger::Log(const char* format, ...)
{
	if (!mLogFile)
		return;

}

void Logger::Debug(const char* format, ... Values)
{
	if (!mDebugFile)
		return;

	fprintf(mDebugFile, format, ...);
}

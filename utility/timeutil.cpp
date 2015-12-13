#include "timeutil.h"
#include "stringutil.h"

TimeUtil::TimeUtil()
{
}

TimeUtil::~TimeUtil()
{
}

string TimeUtil::getTimeNow(string pattern)
{
	return "";
}

string TimeUtil::getTimeNow()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	char timeStr[100];
	sprintf(timeStr, "%0.4d-%0.2d-%0.2d %2d:%0.2d:%0.2d ", sysTime.wYear, sysTime.wMonth, 
		sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);	
	return string(timeStr);
}

string TimeUtil::getTimeNow4Log()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	char timeStr[100];
	sprintf(timeStr, "%0.4d-%0.2d-%0.2d %2d:%0.2d:%0.2d", sysTime.wYear, sysTime.wMonth, 
		sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);	
	return string(timeStr);
}
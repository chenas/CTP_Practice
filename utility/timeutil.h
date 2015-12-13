#pragma once;
#ifndef TIMEUTIL_H
#define TIMEUTIL_H
#include <string>
#include <Windows.h>
#include <time.h>

using namespace std;

class TimeUtil
{
public:
	TimeUtil();
	~TimeUtil();
	static string getTimeNow(string pattern);
	static string getTimeNow();
	static string getTimeNow4Log();
};

#endif
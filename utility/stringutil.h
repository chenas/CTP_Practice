#pragma once
#ifndef STRINGUTIL_H
#define STRINGUITL_H

#include "../common/DataDefine.h"

class StringUtil
{
public:
	StringUtil();
	~StringUtil();
	static string shortToStr(short s);
	static string intToStr(int d);
	static int stringToInt(string s);
	static double stringToDouble(string s);
	///字符串需要以空格隔开
	static void stringToVector(string source, string separator, vector<string>& v);
	static void stringToVector(string source, string separator, vector<int>& v);
	///除去字符串中的数字
	static string delNum(string s);
};

#endif
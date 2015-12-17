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
	///�ַ�����Ҫ�Կո����
	static void stringToVector(string source, string separator, vector<string>& v);
	static void stringToVector(string source, string separator, vector<int>& v);
	///��ȥ�ַ����е�����
	static string delNum(string s);
};

#endif
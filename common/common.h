#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include "../utility/timeutil.h"
#include "../utility/stringutil.h"

static string TradingLogFilePath = "../cfg/trading_log.txt";

class Common
{

public:
	//将内容写入到文本文件中
	///buySell 买卖方向 0.买  1.卖
	///openClose 开平标志  0.开仓   1.平仓  3.平今
	static void record2File(const char *instrumentId, int buySell, int openClose, int volume, double price);
	static void record2File(const char *msg);

	static void log2File(const char *msg);

	//将内容显示在cmd上
	static void record2Stdout(const char *instrumentId, int buySell, int openClose, int volume, double price);

	static void findInstruments(string pattern, set<string> source, vector<string>& v);

	static void copyVector(vector<string> source, vector<string>& destination);

	static void erasefiles();

private:
	

};


#endif
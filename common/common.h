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
	//������д�뵽�ı��ļ���
	///buySell �������� 0.��  1.��
	///openClose ��ƽ��־  0.����   1.ƽ��  3.ƽ��
	static void record2File(const char *instrumentId, int buySell, int openClose, int volume, double price);
	static void record2File(const char *msg);

	static void log2File(const char *msg);

	//��������ʾ��cmd��
	static void record2Stdout(const char *instrumentId, int buySell, int openClose, int volume, double price);

	static void findInstruments(string pattern, set<string> source, vector<string>& v);

	static void copyVector(vector<string> source, vector<string>& destination);

	static void erasefiles();

private:
	

};


#endif
#pragma once
#ifndef _CASE_H_
#define _CASE_H_
#include <map>
#include <vector>
#include "../utility/stringutil.h"
#include "../utility/timeutil.h"
#include "../common/common.h"
#include "../common/trader.h"

struct CaseCfg
{
	string ID;
	string Function;
	string ProductOrInstrument;
	string Timeout;
	string Change;
	string MaxVolume;
	string Frequency;
};

struct PriceData
{
	char InstrumentId[10];
	double LowestPrice;
	double HighestPrice;
	double PriceTick;
	int TickCount;  ///需要多少个PriceTick才到LowestPrice/HighestPrice
	double CurPrice; //最新价,可能会有延迟
	int TimeOut;
	int MaxVolume;
	int Frequency;
	double Change;
};

extern set<string> AllInstrumentId;

class Case
{
public:
	~Case();
	Case(Trader* pTrader);
	void setID(string id);
	void setFunctions(string _case);
	void setInstruments(string productOrInstrument);
	void setTimeouts(string timeout);
	void setChanges(string change);
	void setMaxVolume(string maxVolume);
	void setFrequencys(string frequency);

	void setPriceData();

	map<int, vector<PriceData *>> getFunctionWithData();
	
	int ID;
	void show();

private:
	
	vector<int> vFunction;
	vector<string> vProductOrInstrument;
	vector<int> vTimeout;
	vector<int> vChange;
	vector<int> vMaxVolume;
	vector<int> vFrequency;

	map<int, vector<PriceData *>> mFunctionWithData;

	Trader* pTrader;

	double findLowestPrice(const char* instrumentId, double currentPrice, double change);
	double findHighestPrice(const char* instrumentId, double currentPrice, double change);

	PriceData* initPriceData(const char* instrumemtId, double currentPrice, double change, bool isUp, int timeout);
	PriceData* initPriceData(const char* instrumemtId, double currentPrice, double change, int timeout, int maxVolume, int frequency);

};

class CaseFactory
{
public:
	~CaseFactory();
	CaseFactory();
	Case* createCase(Trader* pTrader, CaseCfg* caseCfg);


};

#endif
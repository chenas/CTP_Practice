#pragma once
#ifndef _CASES_H_
#define _CASES_H_
#include "trader.h"
#include <vector>
#include <conio.h> 
#include "../common/common.h"
#include "../utility/stringutil.h"

extern string cfgFilePath;

extern set<string> AllInstrumentId;


///涨停板价
extern TThostFtdcPriceType	UpperLimitPrice;
///跌停板价
extern TThostFtdcPriceType	LowerLimitPrice;
///最新价
extern TThostFtdcPriceType	LastPrice;
///最小变动价
extern TThostFtdcPriceType	PriceTick;
///开盘价
extern TThostFtdcPriceType	OpenPrice;
///开盘价
extern TThostFtdcPriceType	PreSettlementPrice;

#define Random(X) (rand()%X)

struct PriceData
{
	char InstrumentId[10];
	double LowestPrice;
	double HighestPrice;
	double PriceTick;
	double CurPrice; //最新价
};

class Cases
{

public:
	Cases();
	~Cases(){};

	void run();
	
private:

	Trader* pTrader;

	CfgUtil* getCfg;

	string instrumentId;
	vector<string> vInstrIds;
	int vInstrSize;

	void MainMenu();
	///limitPrice 将市场上的合约推至此价格
	bool makeLimitPrice(const char* instrumentId, double limitPrice);

	///根据涨跌幅算出价格区间
	///下单允许的最高价，最低价
	///change 涨跌幅 0.00%
	double findLowestPrice(const char* instrumentId, double currentPrice, double change);
	double findHighestPrice(const char* instrumentId, double currentPrice, double change);
	
	PriceData* getPriceData(const char* instrumentId, double currentPrice, double change);

	void Case1();
	void Case2();
	void Case3();
	void Case4();
	void Case5();
	void Case6();

};


#endif
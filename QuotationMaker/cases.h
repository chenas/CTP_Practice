#pragma once
#ifndef _CASES_H_
#define _CASES_H_
#include "../common/trader.h"
#include "../common/marketutil.h"
#include <vector>
#include <conio.h> 
#include "../utility/stringutil.h"
#include "case.h"

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
///申买价一
extern TThostFtdcPriceType	BidPrice1;
///申卖价一
extern TThostFtdcPriceType	AskPrice1;
///未成交的挂单手数 
extern int NoTradedNumber;


#define Random(X) (rand()%X)


struct Data4Thread
{
	PriceData* data;
	int volume;
	Trader* pTrader;
	bool isUp;
	int holdVolume;
};

class Cases
{

public:
	Cases();
	~Cases(){};

	void run();
	
private:

	Trader* pTrader;
	MarketUtil* pMarketUtil;

	CfgUtil* getCfg;
	CaseFactory* pCaseFactory;

	vector<Case *> vCases;
	
	////maker_rsh.cfg配置的参数, 以下两个参数迁移到cases.cfg
	///每笔单的手数
	//int volume;
	///挂单数量
	//int holdVolume;
	///未成交数量 废弃
	///int noTradedVolume;

	///是否开启平仓功能
	int IsUseClosePosition;

	//废弃
	void MainMenu();

	///limitPrice 将市场上的合约推至此价格
	bool makeLimitPrice(const char* instrumentId, double limitPrice, int volume);
	bool makeLimitPrice(const char* instrumentId, double limitPrice, bool isUp, int volume);

	///根据涨跌幅算出价格区间
	///下单允许的最高价，最低价
	///change 涨跌幅 0.00%
	double findLowestPrice(const char* instrumentId, double currentPrice, double change);
	double findHighestPrice(const char* instrumentId, double currentPrice, double change);
	
	PriceData* getPriceData(const char* instrumentId, double currentPrice, double change);
	PriceData* getPriceData(const char* instrumentId, double currentPrice, double change, bool isUp);
	
	void initData();

	///制作涨停/跌停价
	///isUp true 则为涨停
	void makeLimit(vector<PriceData *> data, bool isUp);
	
	///单个合约上涨
	friend DWORD WINAPI holdChangeWithOne(LPVOID pPriceData);

	///匀速上涨/下跌
	///change 涨跌幅度
	void holdChane(vector<PriceData *> data, bool isUp);

	///随机下单
	void sendOrderRandom(vector<PriceData *> data);

};


#endif
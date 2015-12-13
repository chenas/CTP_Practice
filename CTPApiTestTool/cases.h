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


///��ͣ���
extern TThostFtdcPriceType	UpperLimitPrice;
///��ͣ���
extern TThostFtdcPriceType	LowerLimitPrice;
///���¼�
extern TThostFtdcPriceType	LastPrice;
///��С�䶯��
extern TThostFtdcPriceType	PriceTick;
///���̼�
extern TThostFtdcPriceType	OpenPrice;
///���̼�
extern TThostFtdcPriceType	PreSettlementPrice;

#define Random(X) (rand()%X)

struct PriceData
{
	char InstrumentId[10];
	double LowestPrice;
	double HighestPrice;
	double PriceTick;
	double CurPrice; //���¼�
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
	///limitPrice ���г��ϵĺ�Լ�����˼۸�
	bool makeLimitPrice(const char* instrumentId, double limitPrice);

	///�����ǵ�������۸�����
	///�µ��������߼ۣ���ͼ�
	///change �ǵ��� 0.00%
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
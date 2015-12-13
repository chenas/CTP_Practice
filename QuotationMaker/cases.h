#pragma once
#ifndef _CASES_H_
#define _CASES_H_
#include "../common/trader.h"
#include <vector>
#include <conio.h> 
#include "../common/common.h"
#include "../utility/stringutil.h"
#include "case.h"

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
///�����һ
extern TThostFtdcPriceType	BidPrice1;
///������һ
extern TThostFtdcPriceType	AskPrice1;
///δ�ɽ��Ĺҵ����� 
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

	CfgUtil* getCfg;
	CaseFactory* pCaseFactory;

	vector<Case *> vCases;
	
	////maker_rsh.cfg���õĲ���
	///ÿ�ʵ�������
	int volume;
	///�ҵ�����
	int holdVolume;
	///δ�ɽ�����
	int noTradedVolume;

	//����
	void MainMenu();

	///limitPrice ���г��ϵĺ�Լ�����˼۸�
	bool makeLimitPrice(const char* instrumentId, double limitPrice);
	bool makeLimitPrice(const char* instrumentId, double limitPrice, bool isUp);

	///�����ǵ�������۸�����
	///�µ��������߼ۣ���ͼ�
	///change �ǵ��� 0.00%
	double findLowestPrice(const char* instrumentId, double currentPrice, double change);
	double findHighestPrice(const char* instrumentId, double currentPrice, double change);
	
	PriceData* getPriceData(const char* instrumentId, double currentPrice, double change);
	PriceData* getPriceData(const char* instrumentId, double currentPrice, double change, bool isUp);
	
	void initData();

	///������ͣ/��ͣ��
	///isUp true ��Ϊ��ͣ
	void makeLimit(vector<PriceData *> data, bool isUp);
	
	///������Լ����
	friend DWORD WINAPI holdChangeWithOne(LPVOID pPriceData);

	///��������/�µ�
	///change �ǵ�����
	void holdChane(vector<PriceData *> data, bool isUp);

	///����µ�
	void sendOrderRandom(vector<PriceData *> data);

};


#endif
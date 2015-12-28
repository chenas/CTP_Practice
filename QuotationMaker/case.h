#pragma once
#ifndef _CASE_H_
#define _CASE_H_
#include <map>
#include <vector>
#include "../utility/stringutil.h"
#include "../utility/timeutil.h"
#include "../common/common.h"
#include "../common/trader.h"
#include "../common/marketutil.h"

///�ĳ�builderģʽ

extern set<string> AllInstrumentId;
extern map< string, double > AllInstrumentIdWithPriceTick;
///��������ʱ��������飬������
extern std::map< std::string, CThostFtdcDepthMarketDataField > FirstDepthMarketData;

///���������ѯʱ����
extern std::map< std::string, CThostFtdcDepthMarketDataField > MidDepthMarketData;

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
	void setVolume(string volume);
	void SetHoldVolume(string holdVolume);

	void setPriceData();

	void getPriceData(int function, vector<PriceData*> &vPriceData);

	map<int, vector<PriceData *>> getFunctionWithData();
	
	int ID;
	void show();
	void show(int function);

private:
	
	vector<int> vFunction;
	vector<string> vProductOrInstrument;
	vector<int> vTimeout;
	vector<int> vChange;
	vector<int> vMaxVolume;
	vector<int> vFrequency;
	///ÿ�ʵ�������
	vector<int> vVolume;
	///ÿ�ʵ�����������Ϊ�ҵ�
	vector<int> vHoldVolume;

	map<int, vector<PriceData *>> mFunctionWithData;

	Trader* pTrader;
	
	double findLowestPrice(const char* instrumentId, double currentPrice, double change);
	double findLowestPrice(double lowerLimitPrice, double currentPrice, double change, double priceTick);
	double findHighestPrice(const char* instrumentId, double currentPrice, double change);
	double findHighestPrice(double upperLimitPrice, double currentPrice, double change, double priceTick);
	
	PriceData* initPriceData(const char* instrumemtId, double currentPrice, double change, 
		bool isUp, int timeout, int volume, int holdVolume);
	PriceData* initPriceData(CThostFtdcDepthMarketDataField DepthMarketDataField, double change,
		double priceTick, bool isUp, int timeout, int volume, int holdVolume);

	PriceData* initPriceData(const char* instrumemtId, double currentPrice, double change, 
		int timeout, int maxVolume, int frequency, int volume, int holdVolume);
	PriceData* initPriceData(CThostFtdcDepthMarketDataField DepthMarketDataField, double change, 
		double priceTick, int timeout, int maxVolume, int frequency, int volume, int holdVolume);

	void reInitPriceData(PriceData& data, bool isUp);
};

class CaseFactory
{
public:
	~CaseFactory();
	CaseFactory();
	Case* createCase(Trader* pTrader, CaseCfg* caseCfg);


};

#endif
#ifndef _MARKET_UTIL_H_
#define _MARKET_UTIL_H_
#include "../apiImpl/mdspimpl.h"
#include "../utility/cfgutil.h"
#include "DataDefine.h"

extern std::string cfgFilePath;
extern HANDLE  g_hEvent;

//最新深度行情
extern std::map< std::string, CThostFtdcDepthMarketDataField > LastDepthMarketData;

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
///前结算价
extern TThostFtdcPriceType	PreSettlementPrice;

///申买价一
extern TThostFtdcPriceType	BidPrice1;
///申卖价一
extern TThostFtdcPriceType	AskPrice1;

class MarketUtil
{
public:
	MarketUtil();
	~MarketUtil();
	
	///行情订阅请求
	int subcribeMarketData(vector<PriceData *> vData);
	///取消行情订阅
	int unSubscribeMarketData(vector<PriceData *> vData);

	///打开记录行情数据文件
	bool openMdLog();
	bool closeMdLog();
	void writeSeparator(const char* msg);
	
	///获得涨停板价
	double getUpperLimitPrice(const char* instrumentId) const;

	///获得跌停板价
	double getLowerLimitPrice(const char* instrumentId) const;

	///获得最新价
	double getLastPrice(const char* instrumentId) const;
		
	///昨结算价
	double getPreSettlementPrice(const char* instrumentId) const;
	
	///买一价
	double getBidPrice(const char* instrumentId) const;
	///买一价与买一量
	void getBidPrice(const char* instrumentId, double& bidPrice1, int& bidVolume1) const;

	///卖一价
	double getAskPrice(const char* instrumentId) const;
	///买一价与买一量
	void getAskPrice(const char* instrumentId, double& askPrice1, int& askVolume1) const;

private:

	CThostFtdcMdApi* _pMdApi;
	CfgUtil* _getCfg;
	MdRspImpl* _mdRspImpl;

	string _mdIp;
	int _reqId;
	
	///登录
	void userLogin();
};

#endif
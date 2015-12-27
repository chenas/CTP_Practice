#ifndef _MARKET_UTIL_H_
#define _MARKET_UTIL_H_
#include "../apiImpl/mdspimpl.h"
#include "../utility/cfgutil.h"
#include "DataDefine.h"

extern std::string cfgFilePath;
extern HANDLE  g_hEvent;

//�����������
extern std::map< std::string, CThostFtdcDepthMarketDataField > LastDepthMarketData;

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
///ǰ�����
extern TThostFtdcPriceType	PreSettlementPrice;

///�����һ
extern TThostFtdcPriceType	BidPrice1;
///������һ
extern TThostFtdcPriceType	AskPrice1;

class MarketUtil
{
public:
	MarketUtil();
	~MarketUtil();
	
	///���鶩������
	int subcribeMarketData(vector<PriceData *> vData);
	///ȡ�����鶩��
	int unSubscribeMarketData(vector<PriceData *> vData);

	///�򿪼�¼���������ļ�
	bool openMdLog();
	bool closeMdLog();
	void writeSeparator(const char* msg);
	
	///�����ͣ���
	double getUpperLimitPrice(const char* instrumentId) const;

	///��õ�ͣ���
	double getLowerLimitPrice(const char* instrumentId) const;

	///������¼�
	double getLastPrice(const char* instrumentId) const;
		
	///������
	double getPreSettlementPrice(const char* instrumentId) const;
	
	///��һ��
	double getBidPrice(const char* instrumentId) const;
	///��һ������һ��
	void getBidPrice(const char* instrumentId, double& bidPrice1, int& bidVolume1) const;

	///��һ��
	double getAskPrice(const char* instrumentId) const;
	///��һ������һ��
	void getAskPrice(const char* instrumentId, double& askPrice1, int& askVolume1) const;

private:

	CThostFtdcMdApi* _pMdApi;
	CfgUtil* _getCfg;
	MdRspImpl* _mdRspImpl;

	string _mdIp;
	int _reqId;
	
	///��¼
	void userLogin();
};

#endif
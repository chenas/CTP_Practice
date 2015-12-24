#ifndef _MARKET_UTIL_H_
#define _MARKET_UTIL_H_
#include "../apiImpl/mdspimpl.h"
#include "../utility/cfgutil.h"
#include "DataDefine.h"

extern std::string cfgFilePath;
extern HANDLE  g_hEvent;

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
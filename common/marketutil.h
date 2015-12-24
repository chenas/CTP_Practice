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
	
	///行情订阅请求
	int subcribeMarketData(vector<PriceData *> vData);
	///取消行情订阅
	int unSubscribeMarketData(vector<PriceData *> vData);

	///打开记录行情数据文件
	bool openMdLog();
	bool closeMdLog();
	void writeSeparator(const char* msg);

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
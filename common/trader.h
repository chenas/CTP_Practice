#pragma once

#ifndef _TRADER_H_
#define _TRADER_H_
#include <iostream>
#include <windows.h>
#include "../utility/cfgutil.h"
#include "../apiImpl/traderspimpl.h"
#include "common.h"

extern string cfgFilePath;

extern HANDLE  g_hEvent;
///报单引用
extern TThostFtdcOrderRefType	OrderRef;
///会话编号
extern TThostFtdcSessionIDType	SessionID;
///前置编号
extern TThostFtdcFrontIDType	FrontID;

///交易所代码
extern TThostFtdcExchangeIDType	ExchangeID;
///报单编号
extern TThostFtdcOrderSysIDType	OrderSysID;
///合约代码
extern TThostFtdcInstrumentIDType	InstrumentID;

///预埋编号
extern TThostFtdcParkedOrderIDType	ParkedOrderID;

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

///全市场合约
extern std::set<string> AllInstrumentId;
///未成交的委托
extern std::vector<OrderActionPackge> NoTradedOrder; 
extern int NoTradedNumber;
///持仓
extern std::vector<PositionActionPackage> Position;

class Trader
{

public:

	Trader();
	~Trader();

	///报单
	///instrumentId 合约
	///buySell 买卖方向 0.买  1.卖
	///openClose 开平标志  0.开仓   1.平仓
	///volume 手数
	///price 价格
	void sendOrder(const char* instrumentId, int buySell, int openClose, int volume, double price);
	
	///获得涨停板价
	double getUpperLimitPrice(const char* instrumentId);

	///获得跌停板价
	double getLowerLimitPrice(const char* instrumentId);

	///获得最新价
	double getLastPrice(const char* instrumentId);

	///获得开盘价
	double getOpenPrice(const char* instrumentId);

	///获得最小变动价位
	double getTickPrice(const char* instrumentId);

	///昨结算价
	double getPreSettlementPrice(const char* instrumentId);

	///买一价
	double getBidPrice(const char* instrumentId);

	///卖一价
	double getAskPrice(const char* instrumentId);

	///撤单
	void orderAction();

	///查询委托数量
	int qryOrder(const char* instrumentId);

	///查持仓
	void qryPosition();


private:

	///登录
	void userLogin();
	///结算确认
	void settleComfirm();

	CfgUtil* getCfg;

	CThostFtdcTraderApi *tradeApi;
	
	string brokerId;
	string userId;
	string passwd;
	string tradeIp;
	string mdIp;
	string productInfo;
	string authCode;


	//请求编号
	int reqId;

	int testMode;

};

#endif
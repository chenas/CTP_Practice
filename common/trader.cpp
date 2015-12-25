#pragma once

#include "trader.h"


Trader::Trader()
{
	Common::erasefiles(); //清除缓存
	reqId = 0;
	getCfg = CfgUtil::getInstance(cfgFilePath);
	brokerId = getCfg->getPara("BrokerId");
	userId = getCfg->getPara("BrokerUser");
	passwd = getCfg->getPara("BrokerUserPasswd");
	tradeIp = getCfg->getPara("TradeFrontIp");
	mdIp = getCfg->getPara("MarketFrontIp");
	productInfo = getCfg->getPara("UserProductInfo");
	authCode = getCfg->getPara("AuthCode");
	testMode = StringUtil::stringToInt(getCfg->getPara("TestMode"));
	std::cerr << "BrokerId: " << brokerId << std::endl;
	std::cerr << "UserId: " << userId << std::endl;
	std::cerr << "UserPasswd: " << passwd << std::endl;
	std::cerr << "TradeIp: " << tradeIp << std::endl;
	std::cerr << "MdIp: " << mdIp << std::endl;
	std::cerr << "productInfo: " << productInfo << std::endl;
	//std::cerr << "authCode: " << authCode << std::endl;

	TradeRspImpl *tradeRspImpl = new TradeRspImpl;
	tradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi("./");
	tradeApi->RegisterFront(const_cast<char*>(tradeIp.c_str()));
	tradeApi->RegisterSpi(tradeRspImpl);
	tradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	tradeApi->SubscribePublicTopic(THOST_TERT_QUICK);
	tradeApi->Init();
	WaitForSingleObject(g_hEvent, INFINITE);

	userLogin();

	settleComfirm();

	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	tradeApi->ReqQryInstrument(&instrument, ++reqId);
	WaitForSingleObject(g_hEvent,INFINITE);

	std::cerr << "市场总合约个数：" << AllInstrumentId.size() << "  开始初始化..." << std::endl;
	Sleep(1000);
}

Trader::~Trader()
{
	SetEvent(g_hEvent);
	tradeApi->Release();
}

///报单
///instrumentId 合约
///buySell 买卖方向 0.买  1.卖
///openClose 开平标志  0.开仓   1.平仓  3.平今  4.平作
///volume 手数
///price 价格
void Trader::sendOrder(const char* instrumentId, int buySell, int openClose, int volume, double price)
{
	CThostFtdcInputOrderField inputOrder;
	memset(&inputOrder, 0, sizeof(inputOrder));
	//brokerid
	strcpy(inputOrder.BrokerID, brokerId.c_str());
	//投资者代码
	strcpy(inputOrder.InvestorID, userId.c_str());
	strcpy(inputOrder.UserID, userId.c_str());

	strcpy(inputOrder.InstrumentID, instrumentId);
	inputOrder.VolumeTotalOriginal = volume;
	inputOrder.LimitPrice = price;

	if (buySell == 0)
		inputOrder.Direction = THOST_FTDC_D_Buy;
	else if (buySell == 1)
		inputOrder.Direction = THOST_FTDC_D_Sell;

	if (openClose == 0)
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
	else if (openClose == 1)
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	else if (openClose == 3)
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	else if (openClose == 4)
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday; //平昨

	///组合投机套保标志
	inputOrder.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//投机

	///有效期类型: 当日有效
	inputOrder.TimeCondition = THOST_FTDC_TC_GFD;
	//成交量类型
	inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
	//报价类型
	inputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice; 
	//限价单标识
	inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
	///非强平
	inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///用户强评标志
	inputOrder.UserForceClose = 0;

	if(!testMode)
	{
		int rtn = tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
		std::cerr << "---->>>发送报单请求" << (rtn == 0 ? "成功":"失败") << std::endl;
	}
	Common::record2Stdout(instrumentId, buySell, openClose, volume, price);
	Common::record2File(instrumentId, buySell, openClose, volume, price);

}

///涨停价
double Trader::getUpperLimitPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return UpperLimitPrice;
}

///跌停价
double Trader::getLowerLimitPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return LowerLimitPrice;
}

///最新价
double Trader::getLastPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return LastPrice;
}

///开盘价
double Trader::getOpenPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return OpenPrice;
}

///昨结算价
double Trader::getPreSettlementPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return PreSettlementPrice;
}

///买一价
double Trader::getBidPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return BidPrice1;
}

///卖一价
double Trader::getAskPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,100);
	return AskPrice1;
}

///最小变动价
double Trader::getTickPrice(const char* instrumentId)
{
	ResetEvent(g_hEvent);
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));	
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);;
	std::cerr << "---->>>发送查询合约请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
	return PriceTick;
}

///查询整个市场深度行情
void Trader::qryDepthMarketData()
{
	ResetEvent(g_hEvent);
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent,INFINITE);
}

///登录
void Trader::userLogin()
{
	ResetEvent(g_hEvent);
	CThostFtdcReqUserLoginField loginField;
	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, brokerId.c_str());
	strcpy(loginField.UserID, userId.c_str());
	strcpy(loginField.Password, passwd.c_str());
	strcpy(loginField.UserProductInfo, productInfo.c_str());
	int rtn = tradeApi->ReqUserLogin(&loginField, ++reqId);
	std::cerr << "---->>>发送登录请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
}

///结算结果确认
void Trader::settleComfirm()
{
	ResetEvent(g_hEvent);
	CThostFtdcSettlementInfoConfirmField SettlementInfoConfirm;
	strcpy(SettlementInfoConfirm.BrokerID, brokerId.c_str());
	strcpy(SettlementInfoConfirm.InvestorID, userId.c_str());
	int rtn = tradeApi->ReqSettlementInfoConfirm(&SettlementInfoConfirm, ++reqId);
	std::cerr << "---->>>发送结算确认请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
}

///撤掉全部未成交的单子
void Trader::orderAction()
{
	ResetEvent(g_hEvent);
	CThostFtdcQryOrderField order;
	strcpy(order.BrokerID, brokerId.c_str());
	strcpy(order.InvestorID, userId.c_str());
	memset(&order, 0, sizeof(order));
	int rtn = tradeApi->ReqQryOrder(&order, ++reqId);
	std::cerr << "---->>>发送委托查询" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);

	std::cout << "--------开始撤单--------" << std::endl;

	for (int i=0; i<NoTradedOrder.size(); i++)
	{
		CThostFtdcInputOrderActionField order;
		memset(&order, 0, sizeof(order));
		strcpy(order.BrokerID, NoTradedOrder[i].BrokerID.c_str());
		strcpy(order.InvestorID, NoTradedOrder[i].InvestorID.c_str());
		strcpy(order.InstrumentID, NoTradedOrder[i].InstrumentID.c_str());
		strcpy(order.OrderSysID, NoTradedOrder[i].OrderSysID.c_str());
		strcpy(order.ExchangeID, NoTradedOrder[i].ExchangeID.c_str());
		order.VolumeChange = NoTradedOrder[i].VolumeChange;
		order.ActionFlag = '0';
		int rtn = tradeApi->ReqOrderAction(&order, ++reqId);
		std::cerr << "---->>>发送撤单请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	}

	vector<OrderActionPackge>().swap(NoTradedOrder);
	std::cout << "--------撤单完成--------" << std::endl;

}

///查询挂单数量
int Trader::qryOrder(const char* instrumentId)
{

	ResetEvent(g_hEvent);
	CThostFtdcQryOrderField order;
	strcpy(order.BrokerID, brokerId.c_str());
	strcpy(order.InvestorID, userId.c_str());
	strcpy(order.InstrumentID, instrumentId);
	memset(&order, 0, sizeof(order));
	int rtn = tradeApi->ReqQryOrder(&order, ++reqId);
	std::cerr << "---->>>发送委托查询" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
	return NoTradedNumber;
}

///查询持仓，然后平仓
void Trader::qryPosition(vector<PriceData *> vData)
{	
	ResetEvent(g_hEvent);
	CThostFtdcQryInvestorPositionField QryInvestorPosition;
	memset(&QryInvestorPosition, 0, sizeof(QryInvestorPosition));
	strcpy(QryInvestorPosition.BrokerID, brokerId.c_str());
	strcpy(QryInvestorPosition.InvestorID, userId.c_str());
	int rtn = tradeApi->ReqQryInvestorPosition(&QryInvestorPosition, ++reqId);
	std::cerr << "---->>>发送查询持仓请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
	std::cout << "--------平仓开始--------" << std::endl;
	//Position.size()
	for (int i=0; i<Position.size(); i++)
	{
		double closePrice = 0.0;
		int vSize = vData.size();
		for (int j=0; j<vSize; j++)
		{
			if (vData[j]->InstrumentId == Position[i].InstrumentID)
			{
				closePrice = vData[j]->CurPrice;
			}
		}
		if (closePrice == 0.0)
		{
			closePrice = getLowerLimitPrice(Position[i].InstrumentID.c_str());
			Sleep(1000);
		}
		/// Direction;  //2、多，3、空
		///buySell 买卖方向 0.买  1.卖
		int buySell = (Position[i].Direction == 2 ? 1:0);
		///openClose 开平标志  0.开仓   1.平仓  3.平今  4.平昨
		if (Position[i].Position > 0)  ///平今
		{
			sendOrder(Position[i].InstrumentID.c_str(), buySell, 3, Position[i].Position, closePrice);
		}
		if (Position[i].YdPosition > 0)  ///平昨
		{
			sendOrder(Position[i].InstrumentID.c_str(), buySell, 4, Position[i].YdPosition, closePrice);
		}
	}
	std::cout << "--------平仓完成--------" << std::endl;
}
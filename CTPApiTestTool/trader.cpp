#pragma once

#include "trader.h"
#include "../common/common.h"

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

	Sleep(3000);

	userLogin();
	Sleep(100);
	settleComfirm();

	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	tradeApi->ReqQryInstrument(&instrument, ++reqId);
	Sleep(3500);
	std::cerr << "市场总合约个数：" << AllInstrumentId.size() << "  初始化中..." << std::endl;
	Sleep(4500);

	WaitForSingleObject(g_hEvent,INFINITE);
}

Trader::~Trader()
{
	SetEvent(g_hEvent);
	tradeApi->Release();
}


///报单
///instrumentId 合约
///buySell 买卖方向 0.买  1.卖
///openClose 开平标志  0.开仓   1.平仓  3.平今
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

	int rtn = tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
	std::cerr << "---->>>发送报单请求" << (rtn == 0 ? "成功":"失败") << std::endl;

}

///涨停价
double Trader::getUpperLimitPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return UpperLimitPrice;
}

///跌停价
double Trader::getLowerLimitPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return LowerLimitPrice;
}

///最新价
double Trader::getLastPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>发送查询深度行情请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return LastPrice;
}

///开盘价
double Trader::getOpenPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>发送合约查询请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return OpenPrice;
}

///最小变动价
double Trader::getTickPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>发送合约查询请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return PriceTick;
}

///昨结算价
double Trader::getPreSettlementPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>发送合约查询请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	Sleep(500);
	return PreSettlementPrice;
}

///登录
void Trader::userLogin()
{
	CThostFtdcReqUserLoginField loginField;
	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, brokerId.c_str());
	strcpy(loginField.UserID, userId.c_str());
	strcpy(loginField.Password, passwd.c_str());
	strcpy(loginField.UserProductInfo, productInfo.c_str());
	int rtn = tradeApi->ReqUserLogin(&loginField, ++reqId);
	std::cerr << "---->>>发送登录请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
}

///结算结果确认
void Trader::settleComfirm()
{

	CThostFtdcSettlementInfoConfirmField SettlementInfoConfirm;
	strcpy(SettlementInfoConfirm.BrokerID, brokerId.c_str());
	strcpy(SettlementInfoConfirm.InvestorID, userId.c_str());
	int rtn = tradeApi->ReqSettlementInfoConfirm(&SettlementInfoConfirm, ++reqId);
	std::cerr << "---->>>发送结算确认请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
}
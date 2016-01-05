#pragma once  
#include "traderspimpl.h"


extern HANDLE  g_hEvent;

///撤单组合一
///报单引用
TThostFtdcOrderRefType	OrderRef;
///会话编号
TThostFtdcSessionIDType	SessionID;
///前置编号
TThostFtdcFrontIDType	FrontID;

///撤单组合二
///交易所代码
TThostFtdcExchangeIDType	ExchangeID;
///报单编号
TThostFtdcOrderSysIDType	OrderSysID;
///合约代码
TThostFtdcInstrumentIDType	InstrumentID;

//用于删除预埋单
///预埋编号
TThostFtdcParkedOrderIDType	ParkedOrderID;

///涨停板价
TThostFtdcPriceType	UpperLimitPrice = 0;
///跌停板价
TThostFtdcPriceType	LowerLimitPrice = 0;
///最新价
TThostFtdcPriceType	LastPrice = 0;
///开盘价
TThostFtdcPriceType	OpenPrice = 0;
///最小变动价
TThostFtdcPriceType	PriceTick = 0;
///上次结算价
TThostFtdcPriceType	PreSettlementPrice = 0;
///申买价一
TThostFtdcPriceType	BidPrice1 = 0;
///申卖价一
TThostFtdcPriceType	AskPrice1 = 0;

///整个市场的合约
std::set<std::string> AllInstrumentId;
std::map< std::string, double > AllInstrumentIdWithPriceTick;

///未成交委托
std::vector<OrderActionPackge> NoTradedOrder;
int NoTradedNumber = 0;

///持仓
std::vector<PositionActionPackage> Position;

///程序启动时的深度行情，不更新
std::map< std::string, CThostFtdcDepthMarketDataField > FirstDepthMarketData;

///主动发起查询时更新
std::map< std::string, CThostFtdcDepthMarketDataField > MidDepthMarketData;

//最新深度行情
std::map< std::string, CThostFtdcDepthMarketDataField > LastDepthMarketData;

void TradeRspImpl::OnFrontConnected()
{
	std::cerr.precision(2); 
	std::cerr.setf(std::ios::fixed); 
	//std::cerr.setf(std::cerr.showpoint);
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	SetEvent(g_hEvent);
}

void TradeRspImpl::OnFrontDisconnected(int nReason)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "--->>> " << "reason: " << nReason << std::endl;
	ResetEvent(g_hEvent);
}

void TradeRspImpl::OnHeartBeatWarning(int nTimeLapse)
{	
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "--->>> " << nTimeLapse << std::endl;
}

///客户端认证响应
void TradeRspImpl::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pRspAuthenticateField->BrokerID << " UserID: " << pRspAuthenticateField->UserID << " ProductInfo: " << pRspAuthenticateField->UserProductInfo << std::endl;
	std::cerr << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///登录请求响应
void TradeRspImpl::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	SessionID = pRspUserLogin->SessionID;	
	FrontID = pRspUserLogin->FrontID;
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pRspUserLogin->BrokerID << " UserID: " << pRspUserLogin->UserID << " ProductInfo: " << std::endl;
	std::cerr << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///登出请求响应
void TradeRspImpl::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pUserLogout->BrokerID << " UserID: " << pUserLogout->UserID << std::endl;
	std::cerr << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	SetEvent(g_hEvent);
}

///修改密码响应
void TradeRspImpl::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pUserPasswordUpdate->BrokerID << " UserID: " << pUserPasswordUpdate->UserID
		<< " oldPasswd: " << pUserPasswordUpdate->OldPassword << " newPasswd: " << pUserPasswordUpdate->NewPassword << std::endl;
	std::cerr << "--->>> " << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	SetEvent(g_hEvent);
}

///错误应答
void TradeRspImpl::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "ErrorID: " << pRspInfo->ErrorID << " ErrorMsg: " << pRspInfo->ErrorMsg
		<< std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///报单录入请求响应
void TradeRspImpl::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	//std::cerr << "brokerID: " << pInputOrder->BrokerID << " UserID: " << pInputOrder->UserID
	//	<< " instrumentId: " << pInputOrder->InstrumentID << " 买卖方向: " << (pInputOrder->Direction == '0'? "买":"卖") 
	//	<< " 组合开平标志: " << (pInputOrder->CombOffsetFlag[0] == '0'?"开仓":"平仓") << std::endl;
	//std::cerr << "--->>> " << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///投资者结算结果确认响应
void TradeRspImpl::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pSettlementInfoConfirm->BrokerID << " UserID: " << pSettlementInfoConfirm->InvestorID
		<< std::endl;
	std::cerr << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///报单通知
void TradeRspImpl::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	strcpy(InstrumentID, pOrder->InstrumentID);
	strcpy(OrderRef, pOrder->OrderRef);
	strcpy(ExchangeID, pOrder->ExchangeID);
	strcpy(OrderSysID, pOrder->OrderSysID);

	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InvestorID: " << pOrder->InvestorID << " InstrumentID: " << pOrder->InstrumentID 
	//	<< " OrderStatus: " << pOrder->OrderStatus << " OrderRef" << pOrder->OrderRef 
	//	<< " OrderType: " << pOrder->OrderType << std::endl;

	/*if (pOrder->OrderStatus == '5')
	{
	std::cout << "出现错单" << std::endl;
	}*/

}

///成交通知
void TradeRspImpl::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InvestorID: " << pTrade->InvestorID << " InstrumentID: " << pTrade->InstrumentID << " OrderSysID: " << pTrade->OrderSysID << std::endl;
}

///报单录入错误回报
void TradeRspImpl::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InvestorID: " << pInputOrder->InvestorID << " InstrumentID: " << pInputOrder->InstrumentID << " OrderRef: " << pInputOrder->OrderRef << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;

}

///报单操作错误回报
void TradeRspImpl::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InvestorID: " << pOrderAction->InvestorID << " InstrumentID: " << pOrderAction->InstrumentID << " OrderRef: " << pOrderAction->OrderRef << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
}

///报单操作请求响应
void TradeRspImpl::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InvestorID: " << pInputOrderAction->InvestorID << " InstrumentID: " << pInputOrderAction->InstrumentID << " OrderRef: " << pInputOrderAction->OrderRef << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///请求查询投资者持仓响应
void TradeRspImpl::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "今持仓：" << pInvestorPosition->TodayPosition << " 昨持仓：" << pInvestorPosition->YdPosition 
		<< " 合约代码：" << pInvestorPosition->InstrumentID << " 占用的保证金：" << pInvestorPosition->UseMargin
		<< " PosiDirection " << pInvestorPosition->PosiDirection 
		<< std::endl;
	PositionActionPackage p;
	memset(&p, 0, sizeof(p));
	p.InstrumentID = pInvestorPosition->InstrumentID;
	p.BrokerID = pInvestorPosition->BrokerID;
	p.InvestorID = pInvestorPosition->InvestorID;
	p.Position = pInvestorPosition->TodayPosition;
	p.Direction = pInvestorPosition->PosiDirection - '0';
	p.YdPosition = pInvestorPosition->YdPosition;

	std::vector<PositionActionPackage>::iterator pit;
	pit = find(Position.begin(), Position.end(), p);

	if (pit == Position.end())
	{
		Position.push_back(p);
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///请求查询深度行情响应
void TradeRspImpl::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InstrumentID: " << pDepthMarketData->InstrumentID << " UpdateTime: " << pDepthMarketData->UpdateTime << std::endl;
	//std::cerr << " 涨停板： " << pDepthMarketData->UpperLimitPrice << " 跌停板： " << pDepthMarketData->LowerLimitPrice << std::endl;

	UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
	LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
	if (pDepthMarketData->LastPrice == 0)
	{
		LastPrice = pDepthMarketData->PreSettlementPrice;
	}
	else
	{		
		LastPrice = pDepthMarketData->LastPrice;
	}
	OpenPrice = pDepthMarketData->OpenPrice;
	PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
	///申买价一
	BidPrice1 = pDepthMarketData->BidPrice1;
	///申卖价一
	AskPrice1 = pDepthMarketData->AskPrice1;

	std::string key = pDepthMarketData->InstrumentID;

	///作为平仓数据使用
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = FirstDepthMarketData.find(key);
	if (mit == FirstDepthMarketData.end())
	{
		FirstDepthMarketData.insert(make_pair(key, *pDepthMarketData));
	}
	else
	{
		//mit->second = *pDepthMarketData;
	}
	
	///作为每个功能的初始化数据，主动查询时更新
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator midMit = MidDepthMarketData.find(key);
	if (midMit == MidDepthMarketData.end())
	{
		MidDepthMarketData.insert(make_pair(key, *pDepthMarketData));
	}
	else
	{
		midMit->second = *pDepthMarketData;
	}

	///最新行情初始化，订阅深度行情，跟着交易所行情一起更新
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator lastmit = LastDepthMarketData.find(key);
	if (lastmit == LastDepthMarketData.end())
	{
		LastDepthMarketData.insert(make_pair(key, *pDepthMarketData));
	}
	else
	{
		lastmit->second = *pDepthMarketData;
	}

	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///预埋单录入请求响应
void TradeRspImpl::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	strcpy(InstrumentID, pParkedOrder->InstrumentID);
	strcpy(ParkedOrderID, pParkedOrder->ParkedOrderID);
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InvestorID: " << pParkedOrder->InvestorID << " InstrumentID: " << pParkedOrder->InstrumentID 
		<< " Status: " << pParkedOrder->Status << " OrderRef: " << pParkedOrder->OrderRef << " ParkedOrderID: " << pParkedOrder->ParkedOrderID << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///预埋撤单操作请求响应
void TradeRspImpl::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InvestorID: " << pParkedOrderAction->InvestorID << " InstrumentID: " << pParkedOrderAction->InstrumentID 
		<< " Status: " << pParkedOrderAction->Status << " OrderRef: " << pParkedOrderAction->OrderRef << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///删除预埋单响应
void TradeRspImpl::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "BrokerID: " << pRemoveParkedOrder->BrokerID << " InvestorID: " << pRemoveParkedOrder->InvestorID
		<< " ParkedOrderID: " << pRemoveParkedOrder->ParkedOrderID << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///删除预埋撤单响应
void TradeRspImpl::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///请求查询报单响应
void TradeRspImpl::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	if (pOrder != NULL)
	{
		if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing || pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing || 
			pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing || pOrder->OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing)
		{
			OrderActionPackge order;
			memset(&order, 0, sizeof(order));
			order.BrokerID = pOrder->BrokerID;		
			order.InvestorID = pOrder->InvestorID;
			order.InstrumentID = pOrder->InstrumentID;
			order.OrderSysID = pOrder->OrderSysID;
			order.ExchangeID = pOrder->ExchangeID;
			order.VolumeChange = pOrder->VolumeTotal;
			std::vector<OrderActionPackge>::iterator it;
			it = find(NoTradedOrder.begin(), NoTradedOrder.end(), order);
			if (it == NoTradedOrder.end())
			{
				NoTradedOrder.push_back(order);			
			}
			NoTradedNumber += pOrder->VolumeTotal;
			//std::cerr << "IstrumentID: " << pOrder->InstrumentID << " OrderStatus: " << pOrder->OrderStatus
			//<< " VolumeTotal: " << pOrder->VolumeTotal << std::endl;

		}
	}
	if (pRspInfo != NULL)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast)
	{
		SetEvent(g_hEvent);
	}
}

///请求查询成交响应
void TradeRspImpl::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InstrumentID: " << pTrade->InstrumentID << " OrderRef: " << pTrade->OrderRef << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///请求查询资金账户响应
void TradeRspImpl::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "AccountID: " << pTradingAccount->AccountID << " Available: " << pTradingAccount->Available 
		<< " Commission: " << pTradingAccount->Commission << " CloseProfit: " << pTradingAccount->CloseProfit 
		<< " PositionProfit: " << pTradingAccount->PositionProfit << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
}

///请求查询合约手续费率响应
void TradeRspImpl::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InstrumentID " << pInstrumentCommissionRate->InstrumentID << std::endl;
	std::cerr << "CloseRatioByMoney " << pInstrumentCommissionRate->CloseRatioByMoney << " CloseRatioByVolume " << pInstrumentCommissionRate->CloseRatioByVolume << std::endl; 
	std::cerr << " CloseTodayRatioByMoney " << pInstrumentCommissionRate->CloseTodayRatioByMoney << " CloseTodayRatioByVolume " << pInstrumentCommissionRate->CloseTodayRatioByVolume << std::endl;
	std::cerr << " OpenRatioByMoney " << pInstrumentCommissionRate->OpenRatioByMoney << " OpenRatioByVolume " << pInstrumentCommissionRate->OpenRatioByVolume << std::endl;
	if (pRspInfo)
	{
		std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
	}
}

///请求查询合约响应
void TradeRspImpl::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "--->>>" << pInstrument->InstrumentID << std::endl;
	PriceTick = pInstrument->PriceTick;
	if ( pInstrument->IsTrading == 1)
	{
		AllInstrumentId.insert(pInstrument->InstrumentID);
		std::string key = pInstrument->InstrumentID;
		std::map< std::string, double >::iterator mit = AllInstrumentIdWithPriceTick.find(key);
		if (mit == AllInstrumentIdWithPriceTick.end())
		{
			AllInstrumentIdWithPriceTick.insert(make_pair(key, pInstrument->PriceTick));
		}
	}	
	if(pInstrument->IsTrading == 0)
		std::cerr << "isTrading  " << pInstrument->IsTrading << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///合约交易状态通知
void TradeRspImpl::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) 
{
	std::cerr << pInstrumentStatus->InstrumentStatus << std::endl;
}

#pragma once  
#include "traderspimpl.h"


extern HANDLE  g_hEvent;

///�������һ
///��������
TThostFtdcOrderRefType	OrderRef;
///�Ự���
TThostFtdcSessionIDType	SessionID;
///ǰ�ñ��
TThostFtdcFrontIDType	FrontID;

///������϶�
///����������
TThostFtdcExchangeIDType	ExchangeID;
///�������
TThostFtdcOrderSysIDType	OrderSysID;
///��Լ����
TThostFtdcInstrumentIDType	InstrumentID;

//����ɾ��Ԥ��
///Ԥ����
TThostFtdcParkedOrderIDType	ParkedOrderID;

///��ͣ���
TThostFtdcPriceType	UpperLimitPrice = 0;
///��ͣ���
TThostFtdcPriceType	LowerLimitPrice = 0;
///���¼�
TThostFtdcPriceType	LastPrice = 0;
///���̼�
TThostFtdcPriceType	OpenPrice = 0;
///��С�䶯��
TThostFtdcPriceType	PriceTick = 0;
///�ϴν����
TThostFtdcPriceType	PreSettlementPrice = 0;
///�����һ
TThostFtdcPriceType	BidPrice1 = 0;
///������һ
TThostFtdcPriceType	AskPrice1 = 0;

///�����г��ĺ�Լ
std::set<std::string> AllInstrumentId;
std::map< std::string, double > AllInstrumentIdWithPriceTick;

///δ�ɽ�ί��
std::vector<OrderActionPackge> NoTradedOrder;
int NoTradedNumber = 0;

///�ֲ�
std::vector<PositionActionPackage> Position;

///��������ʱ��������飬������
std::map< std::string, CThostFtdcDepthMarketDataField > FirstDepthMarketData;

///���������ѯʱ����
std::map< std::string, CThostFtdcDepthMarketDataField > MidDepthMarketData;

//�����������
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

///�ͻ�����֤��Ӧ
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

///��¼������Ӧ
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

///�ǳ�������Ӧ
void TradeRspImpl::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pUserLogout->BrokerID << " UserID: " << pUserLogout->UserID << std::endl;
	std::cerr << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	SetEvent(g_hEvent);
}

///�޸�������Ӧ
void TradeRspImpl::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "brokerID: " << pUserPasswordUpdate->BrokerID << " UserID: " << pUserPasswordUpdate->UserID
		<< " oldPasswd: " << pUserPasswordUpdate->OldPassword << " newPasswd: " << pUserPasswordUpdate->NewPassword << std::endl;
	std::cerr << "--->>> " << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	SetEvent(g_hEvent);
}

///����Ӧ��
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

///����¼��������Ӧ
void TradeRspImpl::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	//std::cerr << "brokerID: " << pInputOrder->BrokerID << " UserID: " << pInputOrder->UserID
	//	<< " instrumentId: " << pInputOrder->InstrumentID << " ��������: " << (pInputOrder->Direction == '0'? "��":"��") 
	//	<< " ��Ͽ�ƽ��־: " << (pInputOrder->CombOffsetFlag[0] == '0'?"����":"ƽ��") << std::endl;
	//std::cerr << "--->>> " << "RspInfo: " << pRspInfo->ErrorMsg << std::endl;
	if(bIsLast) 
	{
		SetEvent(g_hEvent);
	}
}

///Ͷ���߽�����ȷ����Ӧ
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

///����֪ͨ
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
	std::cout << "���ִ�" << std::endl;
	}*/

}

///�ɽ�֪ͨ
void TradeRspImpl::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InvestorID: " << pTrade->InvestorID << " InstrumentID: " << pTrade->InstrumentID << " OrderSysID: " << pTrade->OrderSysID << std::endl;
}

///����¼�����ر�
void TradeRspImpl::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InvestorID: " << pInputOrder->InvestorID << " InstrumentID: " << pInputOrder->InstrumentID << " OrderRef: " << pInputOrder->OrderRef << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;

}

///������������ر�
void TradeRspImpl::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "InvestorID: " << pOrderAction->InvestorID << " InstrumentID: " << pOrderAction->InstrumentID << " OrderRef: " << pOrderAction->OrderRef << std::endl;
	std::cerr << "ErrorMsg: " << pRspInfo->ErrorMsg << std::endl;
}

///��������������Ӧ
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

///�����ѯͶ���ֲ߳���Ӧ
void TradeRspImpl::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	std::cerr << "��ֲ֣�" << pInvestorPosition->TodayPosition << " ��ֲ֣�" << pInvestorPosition->YdPosition 
		<< " ��Լ���룺" << pInvestorPosition->InstrumentID << " ռ�õı�֤��" << pInvestorPosition->UseMargin
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

///�����ѯ���������Ӧ
void TradeRspImpl::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//std::cerr << "--->>>" << __FUNCTION__ <<std::endl;
	//std::cerr << "InstrumentID: " << pDepthMarketData->InstrumentID << " UpdateTime: " << pDepthMarketData->UpdateTime << std::endl;
	//std::cerr << " ��ͣ�壺 " << pDepthMarketData->UpperLimitPrice << " ��ͣ�壺 " << pDepthMarketData->LowerLimitPrice << std::endl;

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
	///�����һ
	BidPrice1 = pDepthMarketData->BidPrice1;
	///������һ
	AskPrice1 = pDepthMarketData->AskPrice1;

	std::string key = pDepthMarketData->InstrumentID;

	///��Ϊƽ������ʹ��
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = FirstDepthMarketData.find(key);
	if (mit == FirstDepthMarketData.end())
	{
		FirstDepthMarketData.insert(make_pair(key, *pDepthMarketData));
	}
	else
	{
		//mit->second = *pDepthMarketData;
	}
	
	///��Ϊÿ�����ܵĳ�ʼ�����ݣ�������ѯʱ����
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator midMit = MidDepthMarketData.find(key);
	if (midMit == MidDepthMarketData.end())
	{
		MidDepthMarketData.insert(make_pair(key, *pDepthMarketData));
	}
	else
	{
		midMit->second = *pDepthMarketData;
	}

	///���������ʼ��������������飬���Ž���������һ�����
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

///Ԥ��¼��������Ӧ
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

///Ԥ�񳷵�����������Ӧ
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

///ɾ��Ԥ����Ӧ
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

///ɾ��Ԥ�񳷵���Ӧ
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

///�����ѯ������Ӧ
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

///�����ѯ�ɽ���Ӧ
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

///�����ѯ�ʽ��˻���Ӧ
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

///�����ѯ��Լ����������Ӧ
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

///�����ѯ��Լ��Ӧ
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

///��Լ����״̬֪ͨ
void TradeRspImpl::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) 
{
	std::cerr << pInstrumentStatus->InstrumentStatus << std::endl;
}

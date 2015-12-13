#pragma once

#include "trader.h"
#include "common.h"


Trader::Trader()
{
	Common::erasefiles(); //�������
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

	Sleep(3000);

	userLogin();
	Sleep(100);
	settleComfirm();

	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	tradeApi->ReqQryInstrument(&instrument, ++reqId);

	Sleep(3500);
	std::cerr << "�г��ܺ�Լ������" << AllInstrumentId.size() << "  ��ʼ����..." << std::endl;
	Sleep(4500);

	WaitForSingleObject(g_hEvent,INFINITE);
}

Trader::~Trader()
{
	SetEvent(g_hEvent);
	tradeApi->Release();
}


///����
///instrumentId ��Լ
///buySell �������� 0.��  1.��
///openClose ��ƽ��־  0.����   1.ƽ��  3.ƽ��
///volume ����
///price �۸�
void Trader::sendOrder(const char* instrumentId, int buySell, int openClose, int volume, double price)
{
	CThostFtdcInputOrderField inputOrder;
	memset(&inputOrder, 0, sizeof(inputOrder));
	//brokerid
	strcpy(inputOrder.BrokerID, brokerId.c_str());
	//Ͷ���ߴ���
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

	///���Ͷ���ױ���־
	inputOrder.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//Ͷ��

	///��Ч������: ������Ч
	inputOrder.TimeCondition = THOST_FTDC_TC_GFD;
	//�ɽ�������
	inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
	//��������
	inputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice; 
	//�޼۵���ʶ
	inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
	///��ǿƽ
	inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�û�ǿ����־
	inputOrder.UserForceClose = 0;

	if(!testMode)
	{
		int rtn = tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
		std::cerr << "---->>>���ͱ�������" << (rtn == 0 ? "�ɹ�":"ʧ��") << std::endl;
	}
	Common::record2Stdout(instrumentId, buySell, openClose, volume, price);
	Common::record2File(instrumentId, buySell, openClose, volume, price);

}

///��ͣ��
double Trader::getUpperLimitPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return UpperLimitPrice;
}

///��ͣ��
double Trader::getLowerLimitPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return LowerLimitPrice;
}

///���¼�
double Trader::getLastPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return LastPrice;
}

///���̼�
double Trader::getOpenPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return OpenPrice;
}

///��С�䶯��
double Trader::getTickPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));	
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);;
	std::cerr << "---->>>���Ͳ�ѯ��Լ����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return PriceTick;
}

///������
double Trader::getPreSettlementPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return PreSettlementPrice;
}

///��һ��
double Trader::getBidPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return BidPrice1;
}

///��һ��
double Trader::getAskPrice(const char* instrumentId)
{
	CThostFtdcQryDepthMarketDataField QryDepthMarketData;
	memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
	strcpy(QryDepthMarketData.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�����������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return AskPrice1;
}

///��¼
void Trader::userLogin()
{
	CThostFtdcReqUserLoginField loginField;
	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, brokerId.c_str());
	strcpy(loginField.UserID, userId.c_str());
	strcpy(loginField.Password, passwd.c_str());
	strcpy(loginField.UserProductInfo, productInfo.c_str());
	int rtn = tradeApi->ReqUserLogin(&loginField, ++reqId);
	std::cerr << "---->>>���͵�¼����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
}

///������ȷ��
void Trader::settleComfirm()
{

	CThostFtdcSettlementInfoConfirmField SettlementInfoConfirm;
	strcpy(SettlementInfoConfirm.BrokerID, brokerId.c_str());
	strcpy(SettlementInfoConfirm.InvestorID, userId.c_str());
	int rtn = tradeApi->ReqSettlementInfoConfirm(&SettlementInfoConfirm, ++reqId);
	std::cerr << "---->>>���ͽ���ȷ������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
}

///����ȫ��δ�ɽ��ĵ���
void Trader::orderAction()
{
	Sleep(1000);
	CThostFtdcQryOrderField order;
	strcpy(order.BrokerID, brokerId.c_str());
	strcpy(order.InvestorID, userId.c_str());
	memset(&order, 0, sizeof(order));
	int rtn = tradeApi->ReqQryOrder(&order, ++reqId);
	std::cerr << "---->>>����ί�в�ѯ" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	std::cout << "--------��ʼ����--------" << std::endl;

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
		std::cerr << "---->>>���ͳ�������" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	}
	vector<OrderActionPackge>().swap(NoTradedOrder);
	std::cout << "--------�������--------" << std::endl;

}

///��ѯ�ҵ�����
int Trader::qryOrder(const char* instrumentId)
{
	CThostFtdcQryOrderField order;
	strcpy(order.BrokerID, brokerId.c_str());
	strcpy(order.InvestorID, userId.c_str());
	strcpy(order.InstrumentID, instrumentId);
	memset(&order, 0, sizeof(order));
	int rtn = tradeApi->ReqQryOrder(&order, ++reqId);
	std::cerr << "---->>>����ί�в�ѯ" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(300);
	return NoTradedNumber;
}

///��ѯ�ֲ֣�Ȼ��ƽ��
void Trader::qryPosition()
{
	CThostFtdcQryInvestorPositionField QryInvestorPosition;
	memset(&QryInvestorPosition, 0, sizeof(QryInvestorPosition));
	strcpy(QryInvestorPosition.BrokerID, brokerId.c_str());
	strcpy(QryInvestorPosition.InvestorID, userId.c_str());
	int rtn = tradeApi->ReqQryInvestorPosition(&QryInvestorPosition, ++reqId);
	std::cerr << "---->>>���Ͳ�ѯ�ֲ�����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(3000);
	std::cout << "--------ƽ�ֿ�ʼ--------" << std::endl;
	for (int i=0; i<Position.size(); i++)
	{
		int buySell = Position[i].Direction == 2 ? 1:0;
		if (Position[i].Position > 0)  ///ƽ��
		{
			sendOrder(Position[i].InstrumentID.c_str(), buySell, 3, Position[i].Position, getLowerLimitPrice(Position[i].InstrumentID.c_str()));
		}
		if (Position[i].YdPosition > 0)  ///ƽ��
		{
			sendOrder(Position[i].InstrumentID.c_str(), buySell, 1, Position[i].YdPosition, getLowerLimitPrice(Position[i].InstrumentID.c_str()));
		}
		Sleep(100);
	}
	std::cout << "--------ƽ�����--------" << std::endl;
}
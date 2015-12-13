#pragma once

#include "trader.h"
#include "../common/common.h"

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

	int rtn = tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
	std::cerr << "---->>>���ͱ�������" << (rtn == 0 ? "�ɹ�":"ʧ��") << std::endl;

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
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>���ͺ�Լ��ѯ����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return OpenPrice;
}

///��С�䶯��
double Trader::getTickPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>���ͺ�Լ��ѯ����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return PriceTick;
}

///������
double Trader::getPreSettlementPrice(const char* instrumentId)
{
	CThostFtdcQryInstrumentField instrument;
	memset(&instrument, 0, sizeof(instrument));
	strcpy(instrument.InstrumentID, instrumentId);
	int rtn = tradeApi->ReqQryInstrument(&instrument, ++reqId);
	std::cerr << "---->>>���ͺ�Լ��ѯ����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	Sleep(500);
	return PreSettlementPrice;
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
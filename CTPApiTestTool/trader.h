#pragma once

#ifndef _TRADER_H_
#define _TRADER_H_
#include <iostream>
#include <windows.h>
#include "../utility/cfgutil.h"
#include "../apiImpl/traderspimpl.h"

extern string cfgFilePath;

extern HANDLE  g_hEvent;
///��������
extern TThostFtdcOrderRefType	OrderRef;
///�Ự���
extern TThostFtdcSessionIDType	SessionID;
///ǰ�ñ��
extern TThostFtdcFrontIDType	FrontID;

///����������
extern TThostFtdcExchangeIDType	ExchangeID;
///�������
extern TThostFtdcOrderSysIDType	OrderSysID;
///��Լ����
extern TThostFtdcInstrumentIDType	InstrumentID;

///Ԥ����
extern TThostFtdcParkedOrderIDType	ParkedOrderID;

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
///���̼�
extern TThostFtdcPriceType	PreSettlementPrice;

///ȫ�г���Լ
extern std::set<string> AllInstrumentId;

class Trader
{

public:

	Trader();
	~Trader();

	///����
	///instrumentId ��Լ
	///buySell �������� 0.��  1.��
	///openClose ��ƽ��־  0.����   1.ƽ��
	///volume ����
	///price �۸�
	void sendOrder(const char* instrumentId, int buySell, int openClose, int volume, double price);
	
	///�����ͣ���
	double getUpperLimitPrice(const char* instrumentId);

	///��õ�ͣ���
	double getLowerLimitPrice(const char* instrumentId);

	///������¼�
	double getLastPrice(const char* instrumentId);

	///��ÿ��̼�
	double getOpenPrice(const char* instrumentId);

	///�����С�䶯��λ
	double getTickPrice(const char* instrumentId);

	///������
	double getPreSettlementPrice(const char* instrumentId);

private:

	///��¼
	void userLogin();
	///����ȷ��
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


	//������
	int reqId;

};

#endif
#pragma once
#ifndef _TRADER_H_
#define _TRADER_H_

#include <iostream>
#include <windows.h>
#include "../utility/cfgutil.h"
#include "../apiImpl/traderspimpl.h"

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


class Trader
{

public:

	Trader()
	{
		reqId = 0;
		getCfg = CfgUtil::getInstance("../cfg/rsh.cfg");
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
		std::cerr << "authCode: " << authCode << std::endl;

		TradeRspImpl *tradeRspImpl = new TradeRspImpl;
		tradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi("./");
		tradeApi->RegisterFront(const_cast<char*>(tradeIp.c_str()));
		tradeApi->RegisterSpi(tradeRspImpl);
		tradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);
		tradeApi->SubscribePublicTopic(THOST_TERT_QUICK);
		tradeApi->Init();
		WaitForSingleObject(g_hEvent,INFINITE);
	}

	~Trader()
	{
		SetEvent(g_hEvent);
		//tradeApi->Join();
		tradeApi->Release();

	}

	const void run()
	{
		int c = 0;
		for (;;)
		{
			Sleep(1000);
			menu();
			std::cin >> c;
			switch (c)
			{
			case EXIT:
				exit(0);
			case LOGIN:
				login();
				break;
			case LOGOUT:
				logout();
				break;
			case UPDATE_PASSWD:
				updatePasswd();
				break;
			case SETTLE_CONFIRM:
				settleConfirm();
				break;
			case ORDERINSERT:
				orderInsert();
				break;
			case ORDERACTION:
				orderAction();
				break;
			case QRYPOSITION:
				qryPosition();
				break;
			case QRYDEPTHMARKETDATA:
				qryDepthMarketData();
				break;
			case PARKED_ORDER:
				parkedOrder();
				break;
			case PARKED_ORDER_ACTION:
				parkedOrderAction();
				break;
			case QRYORDER:
				qryOrder();
				break;
			case QRYTRADE:
				qryTrade();
				break;
			case QRY_TRADING_ACCOUNT:
				qryTradingAccount();
				break;
			case QRY_INSTRUMENT_RATIO:
				qryInstrumentRatio();
				break;
			case REQ_AUTH:
				reqAuth();
				break;
			default:
				break;
			}
		}
	}

protected:

	const void menu()
	{
		std::cerr << "--->>>��ѡ��" << std::endl;
		std::cerr << "-1.�˳�" << std::endl;
		std::cerr << "1. ��¼\t\t";
		std::cerr << "2. �ǳ�\t\t";
		std::cerr << "3. �޸�����" << std::endl;
		std::cerr << "4. ������ȷ��\t";
		std::cerr << "5. ����\t\t";
		std::cerr << "6. ����" << std::endl;
		std::cerr << "7. �ֲֲ�ѯ\t";
		std::cerr << "8. ��������ѯ\t" << std::endl;
		std::cerr << "9. ����Ԥ��\t";
		std::cerr << "10. ��Ԥ��\t" << std::endl;
		std::cerr << "11. ��ѯ����\t";
		std::cerr << "12. ��ѯ�ɽ�\t" << std::endl;
		std::cerr << "13. ��ѯ�ʽ��˻�\t" 
			<< "14. ��ѯ��Լ��������\t" << std::endl;
	}

	const void login()
	{
		CThostFtdcReqUserLoginField loginField;
		memset(&loginField,0,sizeof(loginField));
		strcpy(loginField.BrokerID, brokerId.c_str());
		strcpy(loginField.UserID, userId.c_str());
		strcpy(loginField.Password, passwd.c_str());
		strcpy(loginField.UserProductInfo, "ashu");
		tradeApi->ReqUserLogin(&loginField, ++reqId);
	}

	const void logout()
	{
		CThostFtdcUserLogoutField userLogout;
		memset(&userLogout,0,sizeof(userLogout));
		strcpy(userLogout.BrokerID, brokerId.c_str());
		strcpy(userLogout.UserID, userId.c_str());
		tradeApi->ReqUserLogout(&userLogout, ++reqId);

	}

	///�޸�����
	const void updatePasswd()
	{
		string newPasswd;
		CThostFtdcUserPasswordUpdateField UserPasswordUpdate;
		memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
		strcpy(UserPasswordUpdate.BrokerID, brokerId.c_str());
		strcpy(UserPasswordUpdate.UserID, userId.c_str());
		strcpy(UserPasswordUpdate.OldPassword, passwd.c_str());
		std::cerr << "�����������룺" << std::endl;
		std::cin >> newPasswd;
		strcpy(UserPasswordUpdate.NewPassword, newPasswd.c_str());
		tradeApi->ReqUserPasswordUpdate(&UserPasswordUpdate, ++reqId);
	}

	///������ȷ��
	const void settleConfirm()
	{
		CThostFtdcSettlementInfoConfirmField SettlementInfoConfirm;
		strcpy(SettlementInfoConfirm.BrokerID, brokerId.c_str());
		strcpy(SettlementInfoConfirm.InvestorID, userId.c_str());
		tradeApi->ReqSettlementInfoConfirm(&SettlementInfoConfirm, ++reqId);
	}

	///����
	const void orderInsert()
	{
		CThostFtdcInputOrderField inputOrder; //���뱨������
		memset(&inputOrder,0,sizeof(inputOrder));
		//brokerid
		strcpy(inputOrder.BrokerID, brokerId.c_str());
		//Ͷ���ߴ���
		strcpy(inputOrder.InvestorID, userId.c_str());
		strcpy(inputOrder.UserID, userId.c_str());

		string instrumentId;
		int volume = 1;
		std::cerr << "�������Լ��" << std::endl;
		std::cin >> instrumentId;		
		strcpy(inputOrder.InstrumentID, instrumentId.c_str());
		std::cerr << "������������" << std::endl;
		std::cin >> volume;		
				
		//����
		inputOrder.VolumeTotalOriginal = volume;
		//��Ч������ --������Ч
		inputOrder.TimeCondition = THOST_FTDC_TC_GFD;
		//���Ͷ���ױ���־
		inputOrder.CombHedgeFlag[0] = THOST_FTDC_ECIDT_Hedge;
		//�ɽ�������
		inputOrder.VolumeCondition = THOST_FTDC_VC_AV;

		//��������
		inputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //�޼۵���ʶ

		//������
		inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
		inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		inputOrder.UserForceClose = 0;
		inputOrder.Direction = THOST_FTDC_D_Buy;
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

		std::cerr << "��ѡ��" << std::endl;
		std::cerr << "1. �򿪣�" << std::endl;
		std::cerr << "2. ������" << std::endl;
		std::cerr << "3. ��ƽ��" << std::endl;
		std::cerr << "4. ��ƽ��" << std::endl;
		std::cerr << "5. FAK��" << std::endl;
		std::cerr << "6. FOK��" << std::endl;
		std::cerr << "7. �����������¼۴��������ۣ���" << std::endl;
		std::cerr << "8. �м۵���" << std::endl;
		std::cerr << "9. ������ֹ��/ֹӯ��" << std::endl;
		int oType = 1; //������ֹ��ֹӯ���õ��ñ���
		int type;
		std::cin >> type;
		switch(type)
		{
		case 1:
			inputOrder.Direction = THOST_FTDC_D_Buy;
			inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			break;
		case 2:
			inputOrder.Direction = THOST_FTDC_D_Sell;
			inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			break;
		case 3:
			inputOrder.Direction = THOST_FTDC_D_Buy;
			inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
			break;
		case 4:
			inputOrder.Direction = THOST_FTDC_D_Sell;
			inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			break;
		case 5:
			inputOrder.TimeCondition = THOST_FTDC_TC_IOC; //�����ɽ���������
			inputOrder.VolumeCondition = THOST_FTDC_VC_AV; //~MV������
			inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 6: 
			inputOrder.TimeCondition = THOST_FTDC_TC_IOC; //�����ɽ���������
			inputOrder.VolumeCondition = THOST_FTDC_VC_CV; //ȫ������
			inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 7:
			inputOrder.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterThanStopPrice; //���¼۴���������
			break;
		case 8:
			//�м۵�
			inputOrder.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			inputOrder.LimitPrice = 0;
			inputOrder.TimeCondition = THOST_FTDC_TC_IOC;
			inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
			inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 9:
			inputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			inputOrder.TimeCondition = THOST_FTDC_TC_GFD;
			inputOrder.VolumeCondition = THOST_FTDC_VC_AV;
			std::cerr << "1. ֹ��" << std::endl;
			std::cerr << "2. ֹӮ" << std::endl;
			cin >> oType;
			if (oType == 1)
				inputOrder.ContingentCondition = THOST_FTDC_CC_Touch;
			else if (oType == 2)
				inputOrder.ContingentCondition = THOST_FTDC_CC_TouchProfit;
			break;
		default:
			break;
		}
		if (type != 8)
		{
			std::cerr << "�����뱨���۸�" << std::endl;
			std::cin >> inputOrder.LimitPrice;	
		}

		tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
	}

	///����
	const void orderAction()
	{
		CThostFtdcInputOrderActionField InputOrderAction;
		memset(&InputOrderAction, 0, sizeof(InputOrderAction));
		strcpy(InputOrderAction.BrokerID, brokerId.c_str());
		strcpy(InputOrderAction.InvestorID, userId.c_str());
		int c = 0;;
		std::cerr << "1. �������һ: " << " FrontID: " << FrontID << " SessionID: " << SessionID << " OrderRef: " << OrderRef << endl; 
		std::cerr << "2. ������϶�: " << " ExchangeID: " << ExchangeID << " OrderSysID: " << OrderSysID << endl; 
		std::cerr << "��ѡ�񳷵���ϣ� " << std::endl;
		cin >> c;
		switch (c)
		{
		case 1:
			InputOrderAction.FrontID = FrontID;
			InputOrderAction.SessionID = SessionID;
			strcpy(InputOrderAction.OrderRef, OrderRef);
			break;
		case 2:
			strcpy(InputOrderAction.ExchangeID, ExchangeID);
			strcpy(InputOrderAction.OrderSysID, OrderSysID);
			break;
		default:
			break;
		}
		InputOrderAction.ActionFlag = THOST_FTDC_AF_Delete;
		strcpy(InputOrderAction.InstrumentID, InstrumentID);  //����Ǳ����
		tradeApi->ReqOrderAction(&InputOrderAction, ++reqId);
	}

	//��ֲ�
	const void qryPosition()
	{
		int c = 0;
		char _instrumentId[10];
		CThostFtdcQryInvestorPositionField QryInvestorPosition;
		memset(&QryInvestorPosition, 0, sizeof(QryInvestorPosition));
		strcpy(QryInvestorPosition.BrokerID, brokerId.c_str());
		strcpy(QryInvestorPosition.InvestorID, userId.c_str());
		std::cerr << "1. ��ѯ���еĳֲ�" << std::endl;
		std::cerr << "2. ��ѯ������Լ�ĳֲ�" << std::endl;
		std::cerr << "��ѡ��" << std::endl;
		cin >> c;
		switch (c)
		{
		case 1:
			break;
		case 2:
			std::cerr << "�������Լ���룺" << std::endl;
			cin >> _instrumentId;
			strcpy(QryInvestorPosition.InstrumentID, _instrumentId);
			break;
		default:
			break;
		}
		tradeApi->ReqQryInvestorPosition(&QryInvestorPosition, ++reqId);
	}

	///��������ѯ
	const void qryDepthMarketData()
	{
		char _instrumentId[10];
		CThostFtdcQryDepthMarketDataField QryDepthMarketData;
		memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
		std::cerr << "�������Լ���룺" << std::endl;
		cin >> _instrumentId;
		strcpy(QryDepthMarketData.InstrumentID, _instrumentId);
		tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
		//tradeApi->ReqQryInstrument();
	}

	///Ԥ��
	const void parkedOrder()
	{
		char _instrumentId[10];
		int _volume = 0;
		double _price = 0;
		CThostFtdcParkedOrderField ParkedOrder;
		memset(&ParkedOrder, 0, sizeof(ParkedOrder));
		strcpy(ParkedOrder.BrokerID, brokerId.c_str());
		strcpy(ParkedOrder.UserID, userId.c_str());
		strcpy(ParkedOrder.InvestorID, userId.c_str());
		std::cerr << "�������Լ��" << std::endl;
		cin >> _instrumentId;		
		strcpy(ParkedOrder.InstrumentID, _instrumentId);
		std::cerr << "������������" << std::endl;
		std::cin >> _volume;		
				
		//����
		ParkedOrder.VolumeTotalOriginal = _volume;
		//��Ч������ --������Ч
		ParkedOrder.TimeCondition = THOST_FTDC_TC_GFD;
		//���Ͷ���ױ���־
		ParkedOrder.CombHedgeFlag[0] = THOST_FTDC_ECIDT_Hedge;
		//�ɽ�������
		ParkedOrder.VolumeCondition = THOST_FTDC_VC_AV;
		//��������
		ParkedOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		//������
		ParkedOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
		ParkedOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		ParkedOrder.UserForceClose = 0;
		ParkedOrder.Direction = THOST_FTDC_D_Buy;
		ParkedOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

		std::cerr << "������۸�" << std::endl;
		cin >> _price;
		ParkedOrder.LimitPrice = _price;

		tradeApi->ReqParkedOrderInsert(&ParkedOrder, ++reqId);
	}

	///��Ԥ��
	const void parkedOrderAction()
	{
		CThostFtdcParkedOrderActionField ParkedOrderAction;

		CThostFtdcRemoveParkedOrderField RemoveParkedOrder;

		memset(&RemoveParkedOrder, 0, sizeof(RemoveParkedOrder));
		strcpy(RemoveParkedOrder.BrokerID, brokerId.c_str());
		strcpy(RemoveParkedOrder.InvestorID, userId.c_str());
		strcpy(RemoveParkedOrder.ParkedOrderID, ParkedOrderID);
		
		tradeApi->ReqRemoveParkedOrder(&RemoveParkedOrder, ++reqId);
	}

	///��ѯ����
	const void qryOrder()
	{
		CThostFtdcQryOrderField QryOrder;
		memset(&QryOrder, 0, sizeof(QryOrder));
		strcpy(QryOrder.BrokerID, brokerId.c_str());
		strcpy(QryOrder.InvestorID, userId.c_str());
		
		tradeApi->ReqQryOrder(&QryOrder, ++reqId);
	}

	///��ѯ�ɽ�
	const void qryTrade()
	{
		CThostFtdcQryTradeField QryTrade;
		memset(&QryTrade, 0, sizeof(QryTrade));
		tradeApi->ReqQryTrade(&QryTrade, ++reqId);
	}

	///��ѯ�ʽ��˻�
	const void qryTradingAccount()
	{
		CThostFtdcQryTradingAccountField QryTradingAccount;
		memset(&QryTradingAccount, 0, sizeof(QryTradingAccount));
		strcpy(QryTradingAccount.BrokerID, brokerId.c_str());
		strcpy(QryTradingAccount.InvestorID, userId.c_str());
		tradeApi->ReqQryTradingAccount(&QryTradingAccount, ++reqId);
	}

	///��ѯ��Լ����
	const void qryInstrumentRatio()
	{
		char _instrumentId[10];
		CThostFtdcQryInstrumentCommissionRateField QryInstrumentCommissionRate;
		memset(&QryInstrumentCommissionRate, 0, sizeof(QryInstrumentCommissionRate));
		strcpy(QryInstrumentCommissionRate.BrokerID, brokerId.c_str());
		strcpy(QryInstrumentCommissionRate.InvestorID, userId.c_str());
		std::cerr << "�������Լ��" << std::endl;
		cin >> _instrumentId;
		strcpy(QryInstrumentCommissionRate.InstrumentID, _instrumentId);
		tradeApi->ReqQryInstrumentCommissionRate(&QryInstrumentCommissionRate, ++reqId);
	}
	
	//����ͻ�����֤
	const void reqAuth()
	{
		CThostFtdcReqAuthenticateField authField;
		memset(&authField, 0, sizeof(authField));
		strcpy(authField.BrokerID, brokerId.c_str());
		strcpy(authField.UserID, userId.c_str());
		strcpy(authField.UserProductInfo, productInfo.c_str());
		strcpy(authField.AuthCode, "GKB6TBKCK7P1B7UA");
		tradeApi->ReqAuthenticate(&authField, ++reqId);
	}


private:

	enum function
	{
		EXIT = -1,
		LOGIN = 1,
		LOGOUT,
		UPDATE_PASSWD,
		SETTLE_CONFIRM,
		ORDERINSERT,
		ORDERACTION,
		QRYPOSITION,
		QRYDEPTHMARKETDATA,
		PARKED_ORDER,
		PARKED_ORDER_ACTION,
		QRYORDER,
		QRYTRADE,
		QRY_TRADING_ACCOUNT,
		QRY_INSTRUMENT_RATIO,
		REQ_AUTH
	};

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
#pragma once
#ifndef _TRADER_H_
#define _TRADER_H_

#include <iostream>
#include <windows.h>
#include "../utility/cfgutil.h"
#include "../apiImpl/traderspimpl.h"

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
		std::cerr << "--->>>请选择：" << std::endl;
		std::cerr << "-1.退出" << std::endl;
		std::cerr << "1. 登录\t\t";
		std::cerr << "2. 登出\t\t";
		std::cerr << "3. 修改密码" << std::endl;
		std::cerr << "4. 结算结果确认\t";
		std::cerr << "5. 报单\t\t";
		std::cerr << "6. 撤单" << std::endl;
		std::cerr << "7. 持仓查询\t";
		std::cerr << "8. 深度行情查询\t" << std::endl;
		std::cerr << "9. 报入预埋单\t";
		std::cerr << "10. 撤预埋单\t" << std::endl;
		std::cerr << "11. 查询报单\t";
		std::cerr << "12. 查询成交\t" << std::endl;
		std::cerr << "13. 查询资金账户\t" 
			<< "14. 查询合约手续费率\t" << std::endl;
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

	///修改密码
	const void updatePasswd()
	{
		string newPasswd;
		CThostFtdcUserPasswordUpdateField UserPasswordUpdate;
		memset(&UserPasswordUpdate, 0, sizeof(UserPasswordUpdate));
		strcpy(UserPasswordUpdate.BrokerID, brokerId.c_str());
		strcpy(UserPasswordUpdate.UserID, userId.c_str());
		strcpy(UserPasswordUpdate.OldPassword, passwd.c_str());
		std::cerr << "请输入新密码：" << std::endl;
		std::cin >> newPasswd;
		strcpy(UserPasswordUpdate.NewPassword, newPasswd.c_str());
		tradeApi->ReqUserPasswordUpdate(&UserPasswordUpdate, ++reqId);
	}

	///结算结果确认
	const void settleConfirm()
	{
		CThostFtdcSettlementInfoConfirmField SettlementInfoConfirm;
		strcpy(SettlementInfoConfirm.BrokerID, brokerId.c_str());
		strcpy(SettlementInfoConfirm.InvestorID, userId.c_str());
		tradeApi->ReqSettlementInfoConfirm(&SettlementInfoConfirm, ++reqId);
	}

	///报单
	const void orderInsert()
	{
		CThostFtdcInputOrderField inputOrder; //插入报单数据
		memset(&inputOrder,0,sizeof(inputOrder));
		//brokerid
		strcpy(inputOrder.BrokerID, brokerId.c_str());
		//投资者代码
		strcpy(inputOrder.InvestorID, userId.c_str());
		strcpy(inputOrder.UserID, userId.c_str());

		string instrumentId;
		int volume = 1;
		std::cerr << "请输入合约：" << std::endl;
		std::cin >> instrumentId;		
		strcpy(inputOrder.InstrumentID, instrumentId.c_str());
		std::cerr << "请输入手数：" << std::endl;
		std::cin >> volume;		
				
		//手数
		inputOrder.VolumeTotalOriginal = volume;
		//有效期类型 --当日有效
		inputOrder.TimeCondition = THOST_FTDC_TC_GFD;
		//组合投机套保标志
		inputOrder.CombHedgeFlag[0] = THOST_FTDC_ECIDT_Hedge;
		//成交量类型
		inputOrder.VolumeCondition = THOST_FTDC_VC_AV;

		//报价类型
		inputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //限价单标识

		//立即单
		inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
		inputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		inputOrder.UserForceClose = 0;
		inputOrder.Direction = THOST_FTDC_D_Buy;
		inputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

		std::cerr << "请选择：" << std::endl;
		std::cerr << "1. 买开：" << std::endl;
		std::cerr << "2. 卖开：" << std::endl;
		std::cerr << "3. 买平：" << std::endl;
		std::cerr << "4. 卖平：" << std::endl;
		std::cerr << "5. FAK：" << std::endl;
		std::cerr << "6. FOK：" << std::endl;
		std::cerr << "7. 条件单（最新价大于条件价）：" << std::endl;
		std::cerr << "8. 市价单：" << std::endl;
		std::cerr << "9. 大商所止损/止盈：" << std::endl;
		int oType = 1; //大商所止损止盈单用到该变量
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
			inputOrder.TimeCondition = THOST_FTDC_TC_IOC; //立即成交，否则撤销
			inputOrder.VolumeCondition = THOST_FTDC_VC_AV; //~MV都可以
			inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 6: 
			inputOrder.TimeCondition = THOST_FTDC_TC_IOC; //立即成交，否则撤销
			inputOrder.VolumeCondition = THOST_FTDC_VC_CV; //全部数量
			inputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
			break;
		case 7:
			inputOrder.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterThanStopPrice; //最新价大于条件价
			break;
		case 8:
			//市价单
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
			std::cerr << "1. 止损" << std::endl;
			std::cerr << "2. 止赢" << std::endl;
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
			std::cerr << "请输入报单价格：" << std::endl;
			std::cin >> inputOrder.LimitPrice;	
		}

		tradeApi->ReqOrderInsert(&inputOrder, ++reqId);
	}

	///撤单
	const void orderAction()
	{
		CThostFtdcInputOrderActionField InputOrderAction;
		memset(&InputOrderAction, 0, sizeof(InputOrderAction));
		strcpy(InputOrderAction.BrokerID, brokerId.c_str());
		strcpy(InputOrderAction.InvestorID, userId.c_str());
		int c = 0;;
		std::cerr << "1. 撤单组合一: " << " FrontID: " << FrontID << " SessionID: " << SessionID << " OrderRef: " << OrderRef << endl; 
		std::cerr << "2. 撤单组合二: " << " ExchangeID: " << ExchangeID << " OrderSysID: " << OrderSysID << endl; 
		std::cerr << "请选择撤单组合： " << std::endl;
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
		strcpy(InputOrderAction.InstrumentID, InstrumentID);  //这个是必须的
		tradeApi->ReqOrderAction(&InputOrderAction, ++reqId);
	}

	//查持仓
	const void qryPosition()
	{
		int c = 0;
		char _instrumentId[10];
		CThostFtdcQryInvestorPositionField QryInvestorPosition;
		memset(&QryInvestorPosition, 0, sizeof(QryInvestorPosition));
		strcpy(QryInvestorPosition.BrokerID, brokerId.c_str());
		strcpy(QryInvestorPosition.InvestorID, userId.c_str());
		std::cerr << "1. 查询所有的持仓" << std::endl;
		std::cerr << "2. 查询单个合约的持仓" << std::endl;
		std::cerr << "请选择：" << std::endl;
		cin >> c;
		switch (c)
		{
		case 1:
			break;
		case 2:
			std::cerr << "请输入合约代码：" << std::endl;
			cin >> _instrumentId;
			strcpy(QryInvestorPosition.InstrumentID, _instrumentId);
			break;
		default:
			break;
		}
		tradeApi->ReqQryInvestorPosition(&QryInvestorPosition, ++reqId);
	}

	///深度行情查询
	const void qryDepthMarketData()
	{
		char _instrumentId[10];
		CThostFtdcQryDepthMarketDataField QryDepthMarketData;
		memset(&QryDepthMarketData, 0, sizeof(QryDepthMarketData));
		std::cerr << "请输入合约代码：" << std::endl;
		cin >> _instrumentId;
		strcpy(QryDepthMarketData.InstrumentID, _instrumentId);
		tradeApi->ReqQryDepthMarketData(&QryDepthMarketData, ++reqId);
		//tradeApi->ReqQryInstrument();
	}

	///预埋单
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
		std::cerr << "请输入合约：" << std::endl;
		cin >> _instrumentId;		
		strcpy(ParkedOrder.InstrumentID, _instrumentId);
		std::cerr << "请输入手数：" << std::endl;
		std::cin >> _volume;		
				
		//手数
		ParkedOrder.VolumeTotalOriginal = _volume;
		//有效期类型 --当日有效
		ParkedOrder.TimeCondition = THOST_FTDC_TC_GFD;
		//组合投机套保标志
		ParkedOrder.CombHedgeFlag[0] = THOST_FTDC_ECIDT_Hedge;
		//成交量类型
		ParkedOrder.VolumeCondition = THOST_FTDC_VC_AV;
		//报价类型
		ParkedOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		//立即单
		ParkedOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
		ParkedOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		ParkedOrder.UserForceClose = 0;
		ParkedOrder.Direction = THOST_FTDC_D_Buy;
		ParkedOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;

		std::cerr << "请输入价格：" << std::endl;
		cin >> _price;
		ParkedOrder.LimitPrice = _price;

		tradeApi->ReqParkedOrderInsert(&ParkedOrder, ++reqId);
	}

	///撤预埋单
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

	///查询报单
	const void qryOrder()
	{
		CThostFtdcQryOrderField QryOrder;
		memset(&QryOrder, 0, sizeof(QryOrder));
		strcpy(QryOrder.BrokerID, brokerId.c_str());
		strcpy(QryOrder.InvestorID, userId.c_str());
		
		tradeApi->ReqQryOrder(&QryOrder, ++reqId);
	}

	///查询成交
	const void qryTrade()
	{
		CThostFtdcQryTradeField QryTrade;
		memset(&QryTrade, 0, sizeof(QryTrade));
		tradeApi->ReqQryTrade(&QryTrade, ++reqId);
	}

	///查询资金账户
	const void qryTradingAccount()
	{
		CThostFtdcQryTradingAccountField QryTradingAccount;
		memset(&QryTradingAccount, 0, sizeof(QryTradingAccount));
		strcpy(QryTradingAccount.BrokerID, brokerId.c_str());
		strcpy(QryTradingAccount.InvestorID, userId.c_str());
		tradeApi->ReqQryTradingAccount(&QryTradingAccount, ++reqId);
	}

	///查询合约费率
	const void qryInstrumentRatio()
	{
		char _instrumentId[10];
		CThostFtdcQryInstrumentCommissionRateField QryInstrumentCommissionRate;
		memset(&QryInstrumentCommissionRate, 0, sizeof(QryInstrumentCommissionRate));
		strcpy(QryInstrumentCommissionRate.BrokerID, brokerId.c_str());
		strcpy(QryInstrumentCommissionRate.InvestorID, userId.c_str());
		std::cerr << "请输入合约：" << std::endl;
		cin >> _instrumentId;
		strcpy(QryInstrumentCommissionRate.InstrumentID, _instrumentId);
		tradeApi->ReqQryInstrumentCommissionRate(&QryInstrumentCommissionRate, ++reqId);
	}
	
	//请求客户端认证
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

	//请求编号
	int reqId;

};

#endif
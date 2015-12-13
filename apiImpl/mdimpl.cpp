#pragma once
#include "mdimpl.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "../utility/timeutil.h"
#include "../utility/cfgutil.h"


MiniMd::MiniMd()
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile.open("./log/minimdlog.log",ios_base::app);
	mdlogfile << "minimd construtor" << endl;
}

MiniMd::~MiniMd()
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile.close();
}

//初始化参数
void MiniMd::iniMd(string user, string passwd, string brokerid, string ipaddress,
		const int bIsUsingUdp, const int bIsMultcast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	
	mdlogfile<< TimeUtil::getTimeNow4Log() << " MiniMd" << endl;
	UserId=user;
	Password=passwd;
	Brokerid=brokerid;
	Ipaddress=ipaddress;
	UserProductInfo="mini";

	bool udp = bIsUsingUdp == 1 ? true : false;
	bool mult = bIsMultcast == 1 ? true : false;

	//初始化行情接口
	g_pMdStockApi = CThostFtdcMdApi::CreateFtdcMdApi("./" , udp, mult);
	g_pMdStockApi->RegisterSpi(this); 
	g_pMdStockApi->RegisterFront(const_cast<char*>(Ipaddress.c_str()));
	g_pMdStockApi->Init();
	g_pMdStockApi->Join();

	
}

//连接前置
void MiniMd ::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	
	mdlogfile<< TimeUtil::getTimeNow4Log() << " 已连接到前置" << endl;

	//用户登录请求数据结构
	CThostFtdcReqUserLoginField reqStockfld;
	memset(&reqStockfld,0,sizeof(reqStockfld));

	strcpy_s(reqStockfld.BrokerID,Brokerid.c_str());
	strcpy_s(reqStockfld.UserID,UserId.c_str());
	strcpy_s(reqStockfld.Password,Password.c_str());
	strcpy_s(reqStockfld.UserProductInfo,UserProductInfo.c_str());

	//发起登录
	int nResult =g_pMdStockApi->ReqUserLogin(&reqStockfld,0);

	CfgUtil* getCfg = CfgUtil::getInstance("../cfg/rsh.cfg");
	string instrumentId = getCfg->getPara("InstrumentId");
	char **ppInstrumentID = new char * [3];
	char *arraya = new char[10];
	char *arraya1 = new char[10];
	strcpy(arraya, instrumentId.c_str());
	ppInstrumentID[0] = arraya;

	string instrumentId1 = getCfg->getPara("InstrumentId1");
	strcpy(arraya1, instrumentId1.c_str());
	ppInstrumentID[1] = arraya1;


	//订阅行情
	g_pMdStockApi->SubscribeMarketData(ppInstrumentID, 1);
}


//断开连接时被调用
void MiniMd::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile << TimeUtil::getTimeNow4Log() << " 断开前置连接 reason：" << nReason << endl;
}

//登录请求回调函数
void MiniMd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	if(pRspInfo->ErrorID == 0)
	{
		mdlogfile<< TimeUtil::getTimeNow4Log()<< "登录成功" << endl;
	}
	else
	{
		mdlogfile<< TimeUtil::getTimeNow4Log()<< "登录失败 reason："<<pRspInfo->ErrorMsg << endl;
	}
}

//非法请求响应
void MiniMd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile<< TimeUtil::getTimeNow4Log()<< "非法请求 reason："<< pRspInfo->ErrorMsg << endl;
}

//行情订阅失败响应
void MiniMd::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cout << TimeUtil::getTimeNow4Log() << " OnRspSubMarketData called  " << pSpecificInstrument->InstrumentID << endl;
}

//接收深度行情
void MiniMd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) 
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cout << "ExchangeID: " << pDepthMarketData->ExchangeID << " | "<< pDepthMarketData->InstrumentID << "\t" << pDepthMarketData->LastPrice << "\t" << pDepthMarketData->UpdateTime << "\t"
		<< pDepthMarketData->UpperLimitPrice << "\t"<< pDepthMarketData->LowerLimitPrice << endl;
}

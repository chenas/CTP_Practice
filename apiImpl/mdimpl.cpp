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

//��ʼ������
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

	//��ʼ������ӿ�
	g_pMdStockApi = CThostFtdcMdApi::CreateFtdcMdApi("./" , udp, mult);
	g_pMdStockApi->RegisterSpi(this); 
	g_pMdStockApi->RegisterFront(const_cast<char*>(Ipaddress.c_str()));
	g_pMdStockApi->Init();
	g_pMdStockApi->Join();

	
}

//����ǰ��
void MiniMd ::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	
	mdlogfile<< TimeUtil::getTimeNow4Log() << " �����ӵ�ǰ��" << endl;

	//�û���¼�������ݽṹ
	CThostFtdcReqUserLoginField reqStockfld;
	memset(&reqStockfld,0,sizeof(reqStockfld));

	strcpy_s(reqStockfld.BrokerID,Brokerid.c_str());
	strcpy_s(reqStockfld.UserID,UserId.c_str());
	strcpy_s(reqStockfld.Password,Password.c_str());
	strcpy_s(reqStockfld.UserProductInfo,UserProductInfo.c_str());

	//�����¼
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


	//��������
	g_pMdStockApi->SubscribeMarketData(ppInstrumentID, 1);
}


//�Ͽ�����ʱ������
void MiniMd::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile << TimeUtil::getTimeNow4Log() << " �Ͽ�ǰ������ reason��" << nReason << endl;
}

//��¼����ص�����
void MiniMd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	if(pRspInfo->ErrorID == 0)
	{
		mdlogfile<< TimeUtil::getTimeNow4Log()<< "��¼�ɹ�" << endl;
	}
	else
	{
		mdlogfile<< TimeUtil::getTimeNow4Log()<< "��¼ʧ�� reason��"<<pRspInfo->ErrorMsg << endl;
	}
}

//�Ƿ�������Ӧ
void MiniMd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;	
	mdlogfile<< TimeUtil::getTimeNow4Log()<< "�Ƿ����� reason��"<< pRspInfo->ErrorMsg << endl;
}

//���鶩��ʧ����Ӧ
void MiniMd::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cout << TimeUtil::getTimeNow4Log() << " OnRspSubMarketData called  " << pSpecificInstrument->InstrumentID << endl;
}

//�����������
void MiniMd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) 
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	cout << "ExchangeID: " << pDepthMarketData->ExchangeID << " | "<< pDepthMarketData->InstrumentID << "\t" << pDepthMarketData->LastPrice << "\t" << pDepthMarketData->UpdateTime << "\t"
		<< pDepthMarketData->UpperLimitPrice << "\t"<< pDepthMarketData->LowerLimitPrice << endl;
}

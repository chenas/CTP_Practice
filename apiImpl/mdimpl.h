#pragma once
#ifndef MINIMD_H
#define MINIMD_H

#include <string>
#include <fstream>
#include <vector>
#include "../ctpapi/ThostFtdcMdfastApi.h"  //֧���鲥����
//#include "userapi\ThostFtdcMdApi.h"

using namespace std;

class MiniMd : public CThostFtdcMdSpi
{

public :
	// ���캯������Ҫһ����Ч��ָ��CThostFtdcMdApiʵ����ָ�� 
	//CMdSpiQH(CThostFtdcMdApi *g_pMdStockApi):mg_pMdStockApi(g_pMdStockApi){}
		
	MiniMd();
	~MiniMd();

	//����־�ļ�
	fstream mdlogfile;
	
	//��ʼ����ȡ��Ϣ
	string Brokerid;
	string Ipaddress;
	string UserId     ;
	string Password   ;
	string UserProductInfo;


	/*
	* user �û���
	* passwd ����
	* brokerid ���͹�˾��Ԫ��
	* ipaddress ����ǰ�õ�ַ ��ʽ��tcp://x.x.x.x:y
	*/
	void iniMd(string user, string passwd, string brokerid, string ipaddress,
		const int bIsUsingUdp=0, const int bIsMutlticast=0);

	//����ǰ��
	void OnFrontConnected();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	void OnFrontDisconnected(int nReason);

	//��¼�¼���Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��,�������ϵͳ�޷�ʶ��ͻ��˷��͵���Ϣ����ͨ������������ش�����Ϣ
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///���鶩������
	void subcribeMarketData(char *ppInstrumentID[], int alength);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

public:
	//�����������
	CThostFtdcDepthMarketDataField *depthMarketData;
	//��Ӧ��Ϣ	
	CThostFtdcRspInfoField infoField;
protected:
	CThostFtdcMdApi *g_pMdStockApi;

};

#endif
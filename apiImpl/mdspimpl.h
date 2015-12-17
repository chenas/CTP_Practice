#pragma once
#ifndef _MDRSPIMPL_H_
#define _MDRSPIMPL_H_

#include <iostream>
#include <windows.h>
#include <set>
#include <string>
#include <algorithm>  //find ����
#include <vector>
#include <fstream>
#include "../utility/cfgutil.h"
#include "../ctpapi/ThostFtdcMdfastApi.h"  //֧���鲥����

extern HANDLE g_hEvent;

class MdRspImpl : public CThostFtdcMdSpi
{
public:

	MdRspImpl();

	bool openFile();
	bool closeFile();
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
	
	///����Ӧ��,�������ϵͳ�޷�ʶ��ͻ��˷��͵���Ϣ����ͨ������������ش�����Ϣ
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//��¼�¼���Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///��������Ӧ��
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///ȡ����������Ӧ��
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	
private:
	std::fstream mdlogstream;
	int isPrintPrice;
	CfgUtil* cfgUtil;

};

#endif
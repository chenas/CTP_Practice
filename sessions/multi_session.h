///////////////////////////
///�̳н���api�ӿ�
///created 2015-07-13
///////////////////////////

#pragma once
#ifndef MUTIL_SESSION_H
#define MUTIL_SESSION_H

#include "../ctpapi/ThostFtdcTraderApi.h"
#include "../ctpapi/ThostFtdcUserApiStruct.h"
#include "../ctpapi/ThostFtdcUserApiDataType.h"

#include <iostream>
#include <windows.h>

using namespace std;

class Session : public CThostFtdcTraderSpi
{

public:

	Session();
	~Session();
	
	//����ǰ��
	void OnFrontConnected();

	//�Ͽ�ǰ��
	void OnFrontDisconnected(int nReason);

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnHeartBeatWarning(int nTimeLapse);
	
		
public:
		
	//��¼�ɹ��󣬻��������������	
	//ǰ�ñ��
	TThostFtdcFrontIDType	frontID;
	///�Ự���
	TThostFtdcSessionIDType sessionID;
	///��󱨵�����
	TThostFtdcOrderRefType orderRef;
	//������
	int reqId;

};

#endif
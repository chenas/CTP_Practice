///////////////////////////
///继承交易api接口
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
	
	//连接前置
	void OnFrontConnected();

	//断开前置
	void OnFrontDisconnected(int nReason);

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnHeartBeatWarning(int nTimeLapse);
	
		
public:
		
	//登录成功后，获得下面三个参数	
	//前置编号
	TThostFtdcFrontIDType	frontID;
	///会话编号
	TThostFtdcSessionIDType sessionID;
	///最大报单引用
	TThostFtdcOrderRefType orderRef;
	//请求编号
	int reqId;

};

#endif
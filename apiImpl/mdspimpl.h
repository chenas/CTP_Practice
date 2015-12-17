#pragma once
#ifndef _MDRSPIMPL_H_
#define _MDRSPIMPL_H_

#include <iostream>
#include <windows.h>
#include <set>
#include <string>
#include <algorithm>  //find 函数
#include <vector>
#include <fstream>
#include "../utility/cfgutil.h"
#include "../ctpapi/ThostFtdcMdfastApi.h"  //支持组播行情

extern HANDLE g_hEvent;

class MdRspImpl : public CThostFtdcMdSpi
{
public:

	MdRspImpl();

	bool openFile();
	bool closeFile();
	//连接前置
	void OnFrontConnected();
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	void OnFrontDisconnected(int nReason);
	
	///错误应答,如果交易系统无法识别客户端发送的消息，就通过这个函数返回错误消息
	void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	//登录事件响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///订阅行情应答
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///取消订阅行情应答
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	
private:
	std::fstream mdlogstream;
	int isPrintPrice;
	CfgUtil* cfgUtil;

};

#endif
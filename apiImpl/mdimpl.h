#pragma once
#ifndef MINIMD_H
#define MINIMD_H

#include <string>
#include <fstream>
#include <vector>
#include "../ctpapi/ThostFtdcMdfastApi.h"  //支持组播行情
//#include "userapi\ThostFtdcMdApi.h"

using namespace std;

class MiniMd : public CThostFtdcMdSpi
{

public :
	// 构造函数，需要一个有效的指向CThostFtdcMdApi实例的指针 
	//CMdSpiQH(CThostFtdcMdApi *g_pMdStockApi):mg_pMdStockApi(g_pMdStockApi){}
		
	MiniMd();
	~MiniMd();

	//落日志文件
	fstream mdlogfile;
	
	//初始化获取信息
	string Brokerid;
	string Ipaddress;
	string UserId     ;
	string Password   ;
	string UserProductInfo;


	/*
	* user 用户名
	* passwd 密码
	* brokerid 经纪公司单元号
	* ipaddress 行情前置地址 格式：tcp://x.x.x.x:y
	*/
	void iniMd(string user, string passwd, string brokerid, string ipaddress,
		const int bIsUsingUdp=0, const int bIsMutlticast=0);

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

	//登录事件响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答,如果交易系统无法识别客户端发送的消息，就通过这个函数返回错误消息
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///行情订阅请求
	void subcribeMarketData(char *ppInstrumentID[], int alength);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

public:
	//深度行情数据
	CThostFtdcDepthMarketDataField *depthMarketData;
	//响应信息	
	CThostFtdcRspInfoField infoField;
protected:
	CThostFtdcMdApi *g_pMdStockApi;

};

#endif
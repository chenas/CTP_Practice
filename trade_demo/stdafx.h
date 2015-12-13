// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						
#pragma warning(disable : 4129)
#pragma warning(disable : 4996)
#include  <io.h>
#include <stdio.h>
#include <afx.h>
#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <string>
#include <vector>
#include <map>
using namespace  std;
#include ".\API_CTP\ThostFtdcTraderApi.h"
#include ".\API_CTP\ThostFtdcMdApi.h"
#include "TradeCTP.h"
#include "QuoteCTP.h"
#include<direct.h>



typedef struct strLocalID
{
	TThostFtdcFrontIDType		FrontID;
	TThostFtdcSessionIDType		SessionID;
	TThostFtdcOrderRefType		OrderRef_1;
	TThostFtdcOrderSysIDType	OrderSysID_1;
	TThostFtdcOrderRefType		OrderRef_2;
	TThostFtdcOrderSysIDType	OrderSysID_2;
	void Init()
	{
		FrontID = 0;
		SessionID = 0;
		memset(OrderRef_1,0,sizeof(TThostFtdcOrderRefType));
		memset(OrderSysID_1,0,sizeof(TThostFtdcOrderSysIDType));
		memset(OrderRef_2,0,sizeof(TThostFtdcOrderRefType));
		memset(OrderSysID_2,0,sizeof(TThostFtdcOrderSysIDType));
	}
}strLocalID;

typedef struct strMD_K
{

	///交易日
	TThostFtdcDateType	TradingDay;
	///最后修改时间
	TThostFtdcTimeType	UpdateTime;	
	///合约代码
	TThostFtdcInstrumentIDType	InstrumentID;
	///开盘价
	TThostFtdcPriceType	Price_Open;
	///最高价
	TThostFtdcPriceType	Price_High;
	///最低价
	TThostFtdcPriceType	Price_Low;
	///收盘价
	TThostFtdcPriceType	Price_Close;
	///成交量 
	TThostFtdcVolumeType Vol;
	void Init()
	{
		memset(TradingDay,0,sizeof(TThostFtdcDateType));
		memset(UpdateTime,0,sizeof(TThostFtdcTimeType));
		memset(InstrumentID,0,sizeof(TThostFtdcInstrumentIDType));
		Price_Open=0.0;
		Price_High=0.0;
		Price_Close=0.0;
		Price_Low=10000000.0;
		Vol=0;
	}

}strMD_K;
///Key是 FRONT_ID_SESSION_ID_OrderRef
typedef map<string,strLocalID*> map_str_pstrLocalID;  
///Key是TradeID
typedef map<string,CThostFtdcTradeField> map_string_Trade;
///Key是OrderSysID
typedef map<string,CThostFtdcOrderField> map_string_Order;
///本地保存的InputOrder
typedef std::vector< CThostFtdcInputOrderField >     vector_InputOrder;
///本地持仓
typedef map<string,CThostFtdcInvestorPositionField >       map_str_InvestorPosition;
///K线数据
typedef std::vector< strMD_K>                              vector_MD_K;
///当前合约信息
typedef map<string,CThostFtdcInstrumentField>			   map_str_InstrumentInfo;
///保证金率
typedef map<string,CThostFtdcInstrumentMarginRateField>    map_str_MarginRate;
///手续费率
typedef map<string,CThostFtdcInstrumentCommissionRateField> map_str_CommissionRate;
///最新行情列表
typedef map<string,CThostFtdcDepthMarketDataField>          map_srt_DepthMarketData;
///str_str的map                              
typedef map<string,string>                                  map_str_str;
///str的vector   
typedef std::vector<std::string >				 vector_str;
///成交
typedef std::vector<CThostFtdcTradeField>        vector_Trade;
// TODO: 在此处引用程序需要的其他头文件
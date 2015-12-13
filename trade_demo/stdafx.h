// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
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

	///������
	TThostFtdcDateType	TradingDay;
	///����޸�ʱ��
	TThostFtdcTimeType	UpdateTime;	
	///��Լ����
	TThostFtdcInstrumentIDType	InstrumentID;
	///���̼�
	TThostFtdcPriceType	Price_Open;
	///��߼�
	TThostFtdcPriceType	Price_High;
	///��ͼ�
	TThostFtdcPriceType	Price_Low;
	///���̼�
	TThostFtdcPriceType	Price_Close;
	///�ɽ��� 
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
///Key�� FRONT_ID_SESSION_ID_OrderRef
typedef map<string,strLocalID*> map_str_pstrLocalID;  
///Key��TradeID
typedef map<string,CThostFtdcTradeField> map_string_Trade;
///Key��OrderSysID
typedef map<string,CThostFtdcOrderField> map_string_Order;
///���ر����InputOrder
typedef std::vector< CThostFtdcInputOrderField >     vector_InputOrder;
///���سֲ�
typedef map<string,CThostFtdcInvestorPositionField >       map_str_InvestorPosition;
///K������
typedef std::vector< strMD_K>                              vector_MD_K;
///��ǰ��Լ��Ϣ
typedef map<string,CThostFtdcInstrumentField>			   map_str_InstrumentInfo;
///��֤����
typedef map<string,CThostFtdcInstrumentMarginRateField>    map_str_MarginRate;
///��������
typedef map<string,CThostFtdcInstrumentCommissionRateField> map_str_CommissionRate;
///���������б�
typedef map<string,CThostFtdcDepthMarketDataField>          map_srt_DepthMarketData;
///str_str��map                              
typedef map<string,string>                                  map_str_str;
///str��vector   
typedef std::vector<std::string >				 vector_str;
///�ɽ�
typedef std::vector<CThostFtdcTradeField>        vector_Trade;
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
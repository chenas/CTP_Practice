#pragma once
#include "stdafx.h"
typedef std::vector< CThostFtdcDepthMarketDataField >							 vect_MarketData;
typedef map<string, vect_MarketData>											 Time_vect_MarketData;			// string->20141121 10:01:59
class CGet_MD_K
{
public:
	CGet_MD_K(void);
	~CGet_MD_K(void);

	Time_vect_MarketData      m_m_Time_vect_MarketData;
	 ///得到历史K线数据
	 void GetHistoryData(vector_MD_K & v_MD_K, TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin);
	 ///取得历史数据
	 void ReadHisData(TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin);

	///得到t_Min分前的时间
	void GetBackTime(int t_Min,CThostFtdcDepthMarketDataField  t_md,string & strTradingDay ,string & strUpdateTime,int &offSet);
	///得到从19000101 00:00:01到现在的总秒数
	time_t GetSecondFrom1900(string strTradingDay ,string  strUpdateTime);

	int GetCurrentPath(char buf[],char *pFileName);  
	///判断总共有多少个分隔符，目的是在main函数中构造相应的arr指针数组
	int strsplinum(char *str, const char*del);
	///字符分割函数的简单定义和实现
	void split( char **arr, char *str, const char *del);

	///取nMin的Mod
	void GetTimeMod(string & strTradingDay ,string & strUpdateTime,int nMin);


};


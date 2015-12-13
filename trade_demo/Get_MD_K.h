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
	 ///�õ���ʷK������
	 void GetHistoryData(vector_MD_K & v_MD_K, TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin);
	 ///ȡ����ʷ����
	 void ReadHisData(TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin);

	///�õ�t_Min��ǰ��ʱ��
	void GetBackTime(int t_Min,CThostFtdcDepthMarketDataField  t_md,string & strTradingDay ,string & strUpdateTime,int &offSet);
	///�õ���19000101 00:00:01�����ڵ�������
	time_t GetSecondFrom1900(string strTradingDay ,string  strUpdateTime);

	int GetCurrentPath(char buf[],char *pFileName);  
	///�ж��ܹ��ж��ٸ��ָ�����Ŀ������main�����й�����Ӧ��arrָ������
	int strsplinum(char *str, const char*del);
	///�ַ��ָ���ļ򵥶����ʵ��
	void split( char **arr, char *str, const char *del);

	///ȡnMin��Mod
	void GetTimeMod(string & strTradingDay ,string & strUpdateTime,int nMin);


};


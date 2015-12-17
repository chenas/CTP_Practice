#ifndef _DATA_DEFINE_H_
#define _DATA_DEFINE_H_

#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct CaseCfg
{
	string ID;
	string Function;
	string ProductOrInstrument;
	string Timeout;
	string Change;
	string MaxVolume;
	string Frequency;
};

struct PriceData
{
	char InstrumentId[10];
	double LowestPrice;
	double HighestPrice;
	double PriceTick;
	int TickCount;  ///��Ҫ���ٸ�PriceTick�ŵ�LowestPrice/HighestPrice
	double CurPrice; //���¼�,���ܻ����ӳ�
	int TimeOut;
	int MaxVolume;
	int Frequency;
	double Change;
};

#endif
#ifndef _CASE_BUILDER_H_
#define _CASE_BUILDER_H_

#include "../common/DataDefine.h"
#include "../common/trader.h"

class CaseBuilder
{
public:
	CaseBuilder(Trader* pTrader);

	void setID(string id);
	void setFunctions(string _case);
	void setInstruments(string productOrInstrument);
	void setTimeouts(string timeout);
	void setChanges(string change);
	void setMaxVolume(string maxVolume);
	void setFrequencys(string frequency);
	void setVolume(string volume);
	void SetHoldVolume(string holdVolume);
	
	double findLowestPrice(double currentPrice, double change, double priceTick, double lowLimitPrice);

	double findHighestPrice(double currentPrice, double change, double priceTick, double upperLimitPrice);

	PriceData* initPriceData(const char* instrumemtId, double currentPrice, double change, 
		int timeout, int volume, int holdVolume);
}

#endif
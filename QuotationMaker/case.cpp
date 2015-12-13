#include "case.h"

string separator = "/";

Case::~Case()
{
}

Case::Case(Trader* pTrader)
{
	this->pTrader = pTrader;
	map<int, vector<PriceData *>>().swap(mFunctionWithData);
}

map<int, vector<PriceData *>> Case::getFunctionWithData()
{
	return mFunctionWithData;
}

void Case::setID(string id)
{
	ID = StringUtil::stringToInt(id);
}

void Case::setFunctions(string _case)
{
	StringUtil::stringToVector(_case, ",", this->vFunction);
}

void Case::setChanges(string change)
{
	StringUtil::stringToVector(change, separator, vChange);
}

void Case::setFrequencys(string frequency)
{
	StringUtil::stringToVector(frequency, separator, vFrequency);
}

void Case::setInstruments(string productOrInstrument)
{
	StringUtil::stringToVector(productOrInstrument, separator, vProductOrInstrument);
}

void Case::setMaxVolume(string maxVolume)
{
	StringUtil::stringToVector(maxVolume, separator, vMaxVolume);
}

void Case::setTimeouts(string timeout)
{
	StringUtil::stringToVector(timeout, separator, vTimeout);
}

void Case::setPriceData()
{
	vector<int>::iterator it;
	vector<string>::iterator sit;
	int index = 0;
	for (it=vFunction.begin(); it!=vFunction.end(); it++)
	{
		vector<PriceData*> data;
		vector<string> vTemp;
		vector<string> vTempInstrument;
		string POI = vProductOrInstrument[index];
		int timeout = vTimeout[index];
		int change = vChange[index];
		int maxVolume = vMaxVolume[index];
		int frequency = vFrequency[index];

		StringUtil::stringToVector(POI, ",", vTemp);
		for(int i=0; i<vTemp.size(); i++)
		{
			Common::findInstruments(vTemp[i], AllInstrumentId, vTempInstrument);
		}

		switch(*it)
		{
		case 1:  //保持涨停

			for (int i=0; i<vTempInstrument.size(); i++)
			{
				double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, 100, true, timeout));
			}
			break;

		case 2:  //保持跌停
			
			for (int i=0; i<vTempInstrument.size(); i++)
			{
				double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, 100, false, timeout));
			}
			break;

		case 3:  //持续上涨
			
			for (int i=0; i<vTempInstrument.size(); i++)
			{
				double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, true, timeout));
			}
			break;

		case 4: //持续下跌
			
			for (int i=0; i<vTempInstrument.size(); i++)
			{
				double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, false, timeout));
			}
			break;
			
		case 5:  //随机报单
			
			for (int i=0; i<vTempInstrument.size(); i++)
			{
				double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, timeout, maxVolume, frequency));
			}
			break;

		default:
			break;
		}
		index++;
		mFunctionWithData.insert(pair<int, vector<PriceData*>> (*it, data));
		vector<PriceData*>().swap(data);
		vector<string>().swap(vTemp);
		vector<string>().swap(vTempInstrument);
	}
}

void Case::show()
{
	
	std::ofstream o_file("../cfg/log_case.txt",std::ios::app);			
	map<int, vector<PriceData*>>::iterator mit;
	o_file << TimeUtil::getTimeNow() << std::endl;
	for (mit=mFunctionWithData.begin(); mit!=mFunctionWithData.end(); mit++)
	{
		//std::cout << "NO: " << ID << " Function: " << mit->first << " ";
		o_file << "NO: " << ID << " Function: " << mit->first << " ";
		vector<PriceData*> data = mit->second;
		vector<PriceData*>::iterator it;
		for (it = data.begin(); it != data.end(); it++)
		{
			//std::cout << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
				//<< " PriceTick: " << (*it)->PriceTick << " TickCount: " << (*it)->TickCount << std::endl;
			o_file << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
				<< " CurrentPrice: " << (*it)->CurPrice << " PriceTick: " << (*it)->PriceTick << " TickCount: " << (*it)->TickCount << std::endl;
		}
	}
	o_file << std::endl;
	o_file.close();
}


///找到最低价
double Case::findLowestPrice(const char* instrumentId, double currentPrice, double change)
{
	if (change == 100)
	{
		return pTrader->getLowerLimitPrice(instrumentId);
	}
	double lowerPrice = pTrader->getLowerLimitPrice(instrumentId);
	double priceTick = PriceTick;
	double price = 0.0;
	double changePrice = currentPrice * change/100;

	if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
	{
		price = currentPrice - changePrice;
	}
	else if ( fmod(changePrice, priceTick) != 0)
	{
		price = currentPrice - (changePrice - fmod(changePrice, priceTick));
	}
	if (price <= lowerPrice)
	{
		price = lowerPrice;
	}
	return price;
}

///找到最高价
double Case::findHighestPrice(const char* instrumentId, double currentPrice, double change)
{
	double upperPrice = pTrader->getUpperLimitPrice(instrumentId);
	Sleep(1000);
	if (change == 100)
	{
		return upperPrice;
	}
	
	double priceTick = PriceTick;
	double price = 0.0;
	double changePrice = currentPrice * change/100;

	if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
	{
		price = currentPrice + changePrice;
	}
	else if ( fmod(changePrice, priceTick) != 0)
	{
		price = currentPrice + (changePrice - fmod(changePrice, priceTick));  //fmod取余
	}
	if (price >= upperPrice)
	{
		price = upperPrice;
	}
	return price;
}


PriceData* Case::initPriceData(const char* instrumemtId, double currentPrice, double change, bool isUp, int timeout)
{
	if (currentPrice == 0.0)
		return 0;
	PriceData* data = new PriceData;

	data->PriceTick = pTrader->getTickPrice(instrumemtId);

	if (isUp)
	{
		data->HighestPrice = findHighestPrice(instrumemtId, currentPrice, change);
		Sleep(300);
		data->TickCount = ceil((data->HighestPrice - currentPrice)/PriceTick); ///向上取整
		
	}
	else
	{
		data->LowestPrice = findLowestPrice(instrumemtId, currentPrice, change);
		Sleep(300);
		data->TickCount = ceil((currentPrice - data->LowestPrice)/PriceTick);
		
	}
	
	data->PriceTick = PriceTick;
	data->CurPrice = currentPrice;
	strcpy(data->InstrumentId, instrumemtId);
	data->TimeOut = timeout;
	data->Change = change;

	return data;
}

PriceData* Case::initPriceData(const char* instrumemtId, double currentPrice, double change, int timeout, int maxVolume, int frequency)
{
	if (currentPrice == 0.0)
		return 0;
	PriceData* data = new PriceData;
	
	data->PriceTick = pTrader->getTickPrice(instrumemtId);

	data->HighestPrice = findHighestPrice(instrumemtId, currentPrice, change);
	Sleep(500);
	data->LowestPrice = findLowestPrice(instrumemtId, currentPrice, change);

	data->PriceTick = PriceTick;
	data->CurPrice = currentPrice;
	strcpy(data->InstrumentId, instrumemtId);
	data->TimeOut = timeout;
	data->TickCount = ceil((currentPrice * change/100)/PriceTick);
	data->Change = change;
	data->MaxVolume = maxVolume;
	data->Frequency = frequency;

	return data;
}

CaseFactory::~CaseFactory()
{
}

CaseFactory::CaseFactory()
{
}


Case* CaseFactory::createCase(Trader* pTrader, CaseCfg* caseCfg)
{
	Case* c = new Case(pTrader);
	c->setChanges(caseCfg->Change);
	c->setFunctions(caseCfg->Function);
	c->setFrequencys(caseCfg->Frequency);
	c->setID(caseCfg->ID);
	c->setInstruments(caseCfg->ProductOrInstrument);
	c->setMaxVolume(caseCfg->MaxVolume);
	c->setTimeouts(caseCfg->Timeout);
	c->setPriceData();
	return c;
}

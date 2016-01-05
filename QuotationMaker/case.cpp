#include "case.h"

string separator = "/";

Case::~Case()
{
}

Case::Case(Trader* pTrader)
{
	this->pTrader = pTrader;
	map<int, vector<PriceData *> >().swap(mFunctionWithData);
}

map<int, vector<PriceData *> > Case::getFunctionWithData()
{
	return mFunctionWithData;
}

vector<pair<int, vector<PriceData *>> > Case::getVFunctionWithData()
{
	return this->vFunctionWithData;
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

	int vFunctionSize = vFunction.size();
	int paraSize = vChange.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vChange.push_back(vChange[paraSize-1]);
		}
	}
}

void Case::setFrequencys(string frequency)
{
	StringUtil::stringToVector(frequency, separator, vFrequency);

	int vFunctionSize = vFunction.size();
	int paraSize = vFrequency.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vFrequency.push_back(vFrequency[paraSize-1]);
		}
	}
}

void Case::setInstruments(string productOrInstrument)
{
	StringUtil::stringToVector(productOrInstrument, separator, vProductOrInstrument);

	int vFunctionSize = vFunction.size();
	int paraSize = vProductOrInstrument.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vProductOrInstrument.push_back(vProductOrInstrument[paraSize-1]);
		}
	}
}

void Case::setMaxVolume(string maxVolume)
{
	StringUtil::stringToVector(maxVolume, separator, vMaxVolume);

	int vFunctionSize = vFunction.size();
	int paraSize = vMaxVolume.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vMaxVolume.push_back(vMaxVolume[paraSize-1]);
		}
	}
}

void Case::setTimeouts(string timeout)
{
	StringUtil::stringToVector(timeout, separator, vTimeout);

	int vFunctionSize = vFunction.size();
	int paraSize = vTimeout.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vTimeout.push_back(vTimeout[paraSize-1]);
		}
	}
}

void Case::setVolume(string volume)
{
	StringUtil::stringToVector(volume, separator, vVolume);

	int vFunctionSize = vFunction.size();
	int paraSize = vVolume.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vVolume.push_back(vVolume[paraSize-1]);
		}
	}
}

void Case::SetHoldVolume(string holdVolume)
{
	StringUtil::stringToVector(holdVolume, separator, vHoldVolume);

	int vFunctionSize = vFunction.size();
	int paraSize = vHoldVolume.size();	
	if (vFunctionSize > 1 && (paraSize > 0 && paraSize < vFunctionSize))
	{
		int diff = vFunctionSize - paraSize;
		for (int i=1; i<=diff; i++)
		{
			vHoldVolume.push_back(vHoldVolume[paraSize-1]);
		}
	}
}

void Case::setPriceData()
{
	pTrader->qryDepthMarketData(); //查询最新行情
	vector<int>::iterator it;
	vector<string>::iterator sit;
	int index = 0;
	for (it=vFunction.begin(); it!=vFunction.end(); it++)
	{
		vector<PriceData*> data;  //一个合约一个PriceData
		vector<string> vTemp;
		vector<string> vTempInstrument;
		string POI = vProductOrInstrument[index];
		int timeout = vTimeout[index];
		int change = vChange[index];
		int maxVolume = vMaxVolume[index];
		int frequency = vFrequency[index];
		int volume = vVolume[index];
		int holdVolume = vHoldVolume[index];

		StringUtil::stringToVector(POI, ",", vTemp);
		for(unsigned int i=0; i<vTemp.size(); i++)
		{
			Common::findInstruments(vTemp[i], AllInstrumentId, vTempInstrument);
		}

		switch(*it)
		{
		case 1:  //保持涨停

			for (unsigned int i=0; i<vTempInstrument.size(); i++)
			{
				std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(vTempInstrument[i]);
				std::map< string, double >::iterator mitTick = AllInstrumentIdWithPriceTick.find(vTempInstrument[i]);
				if (mit == MidDepthMarketData.end())
				{
					std::cout << "can not find DepthMarketData for " << vTempInstrument[i] << std::endl;
					continue;
				}
				else if (mitTick == AllInstrumentIdWithPriceTick.end())
				{
					std::cout << "can not find PriceTick for " << vTempInstrument[i] << std::endl;
					continue;
				}
				//double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				//data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, 100, true, timeout,volume, holdVolume));
				//Sleep(1000);
				data.push_back(initPriceData(mit->second, 100, mitTick->second, true, timeout, volume, holdVolume));
			}
			break;

		case 2:  //保持跌停

			for (unsigned int i=0; i<vTempInstrument.size(); i++)
			{
				std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(vTempInstrument[i]);
				std::map< string, double >::iterator mitTick = AllInstrumentIdWithPriceTick.find(vTempInstrument[i]);
				if (mit == MidDepthMarketData.end())
				{
					std::cout << "can not find DepthMarketData for " << vTempInstrument[i] << std::endl;
					continue;
				}
				else if (mitTick == AllInstrumentIdWithPriceTick.end())
				{
					std::cout << "can not find PriceTick for " << vTempInstrument[i] << std::endl;
					continue;
				}
				//double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				//data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, 100, false, timeout,volume, holdVolume));
				//Sleep(1000);
				data.push_back(initPriceData(mit->second, 100, mitTick->second, false, timeout, volume, holdVolume));
			}
			break;

		case 3:  //持续上涨

			for (unsigned int i=0; i<vTempInstrument.size(); i++)
			{
				std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(vTempInstrument[i]);
				std::map< string, double >::iterator mitTick = AllInstrumentIdWithPriceTick.find(vTempInstrument[i]);
				if (mit == MidDepthMarketData.end())
				{
					std::cout << "can not find DepthMarketData for " << vTempInstrument[i] << std::endl;
					continue;
				}
				else if (mitTick == AllInstrumentIdWithPriceTick.end())
				{
					std::cout << "can not find PriceTick for " << vTempInstrument[i] << std::endl;
					continue;
				}
				//double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				//data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, true, timeout,volume, holdVolume));
				//Sleep(1000);
				data.push_back(initPriceData(mit->second, change, mitTick->second, true, timeout, volume, holdVolume));
			}
			break;

		case 4: //持续下跌

			for (unsigned int i=0; i<vTempInstrument.size(); i++)
			{
				std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(vTempInstrument[i]);
				std::map< string, double >::iterator mitTick = AllInstrumentIdWithPriceTick.find(vTempInstrument[i]);
				if (mit == MidDepthMarketData.end())
				{
					std::cout << "can not find DepthMarketData for " << vTempInstrument[i] << std::endl;
					continue;
				}
				else if (mitTick == AllInstrumentIdWithPriceTick.end())
				{
					std::cout << "can not find PriceTick for " << vTempInstrument[i] << std::endl;
					continue;
				}
				//double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				//data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, false, timeout,volume, holdVolume));
				//Sleep(1000);
				data.push_back(initPriceData(mit->second, change, mitTick->second, false, timeout, volume, holdVolume));
			}
			break;

		case 5:  //随机报单

			for (unsigned int i=0; i<vTempInstrument.size(); i++)
			{
				std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(vTempInstrument[i]);
				std::map< string, double >::iterator mitTick = AllInstrumentIdWithPriceTick.find(vTempInstrument[i]);
				if (mit == MidDepthMarketData.end())
				{
					std::cout << "can not find DepthMarketData for " << vTempInstrument[i] << std::endl;
					continue;
				}
				else if (mitTick == AllInstrumentIdWithPriceTick.end())
				{
					std::cout << "can not find PriceTick for " << vTempInstrument[i] << std::endl;
					continue;
				}
				//double curPrice = pTrader->getLastPrice(vTempInstrument[i].c_str());
				//data.push_back(initPriceData(vTempInstrument[i].c_str(), curPrice, change, timeout, maxVolume, frequency,volume, holdVolume));
				//Sleep(1000);
				data.push_back(initPriceData(mit->second, change, mitTick->second, timeout, maxVolume, frequency,volume, holdVolume));
			}
			break;

		default:
			break;
		}
		index++;
		mFunctionWithData.insert(pair<int, vector<PriceData *> > (*it, data));
		vFunctionWithData.push_back(make_pair<int, vector<PriceData *>> (*it, data));
		vector<PriceData*>().swap(data);
		vector<string>().swap(vTemp);
		vector<string>().swap(vTempInstrument);
	}
}

///最新数据
void Case::getPriceData(int function, vector<PriceData*> &vPriceData)
{
	switch(function)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		pTrader->qryDepthMarketData(); //查询最新行情
		for (int i=0; i<vPriceData.size(); i++)
		{
			reInitPriceData(*vPriceData[i], true);
		}
		break;
	case 4:
		pTrader->qryDepthMarketData(); //查询最新行情
		for (int i=0; i<vPriceData.size(); i++)
		{
			reInitPriceData(*vPriceData[i], false);
		}
		break;
	case 5:
		break;
	default:
		break;
	}
}

void Case::reInitPriceData(PriceData &data, bool isUp)
{
	std::map< std::string, CThostFtdcDepthMarketDataField >::iterator mit = MidDepthMarketData.find(data.InstrumentId);

	if (mit == MidDepthMarketData.end())
	{
		std::cout << "can not find DepthMarketData for " << data.InstrumentId << std::endl;
	}
	else
	{
		double currentPrice = (mit->second).LastPrice == 0 ? (mit->second).PreSettlementPrice : (mit->second).LastPrice;
		data.CurPrice = currentPrice;
		if (isUp)
		{

			data.HighestPrice = findHighestPrice((mit->second).UpperLimitPrice, currentPrice, data.Change, data.PriceTick);
			data.TickCount = ceil((data.HighestPrice - currentPrice)/PriceTick); ///向上取整
		}
		else
		{
			data.LowestPrice = findLowestPrice((mit->second).LowerLimitPrice, currentPrice, data.Change, data.PriceTick);
			data.TickCount = ceil((currentPrice - data.LowestPrice)/PriceTick);			
		}
	}


}

///分解后的case写入到文件
void Case::show()
{

	std::string strLog =  "\r\n"; 
	BOOL bRet = WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);

	map<int, vector<PriceData *> >::iterator mit;
	for (mit=mFunctionWithData.begin(); mit!=mFunctionWithData.end(); mit++)
	{
		strLog = "NO: " + StringUtil::intToStr(ID) + " Function: " + StringUtil::intToStr(mit->first) + "\r\n";
		WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
		vector<PriceData*> data = mit->second;
		vector<PriceData*>::iterator it;
		for (it = data.begin(); it != data.end(); it++)
		{
			//std::cout << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
			//<< " PriceTick: " << (*it)->PriceTick << " TickCount: " << (*it)->TickCount << std::endl;
			strLog = " instrument: " + StringUtil::arrayToStr((*it)->InstrumentId) + " timeout: " + StringUtil::intToStr((*it)->TimeOut )
				+ " Change: " + StringUtil::doubleToStr((*it)->Change) 
				+ " CurrentPrice: " + StringUtil::doubleToStr((*it)->CurPrice)+ " PriceTick: " + StringUtil::intToStr((*it)->PriceTick) 
				+ " TickCount: " + StringUtil::intToStr((*it)->TickCount) + " Volume: " + StringUtil::intToStr((*it)->Volume) 
				+ " HoldVolume: " + StringUtil::intToStr((*it)->HoldVolume) + "\r\n";
			WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
		}
	}

	//std::ofstream o_file("../cfg/trading_log.txt",std::ios::app);
	//o_file << TimeUtil::getTimeNow() << std::endl;
	//map<int, vector<PriceData *> >::iterator mit;
	//for (mit=mFunctionWithData.begin(); mit!=mFunctionWithData.end(); mit++)
	//{
	//	//std::cout << "NO: " << ID << " Function: " << mit->first << " ";
	//	o_file << "NO: " << ID << " Function: " << mit->first << std::endl;
	//	vector<PriceData*> data = mit->second;
	//	vector<PriceData*>::iterator it;
	//	for (it = data.begin(); it != data.end(); it++)
	//	{
	//		//std::cout << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
	//		//<< " PriceTick: " << (*it)->PriceTick << " TickCount: " << (*it)->TickCount << std::endl;
	//		o_file << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
	//			<< " Change: " << (*it)->Change 
	//			<< " CurrentPrice: " << (*it)->CurPrice << " PriceTick: " << (*it)->PriceTick 
	//			<< " TickCount: " << (*it)->TickCount << " Volume: " << (*it)->Volume 
	//			<< " HoldVolume: " << (*it)->HoldVolume<< std::endl;
	//	}
	//}
	//o_file << std::endl;
	//o_file.close();
}

///分解后的case写入到文件
void Case::show(int function)
{
	std::string strLog =  "\r\n";
	BOOL bRet = WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);

	map<int, vector<PriceData *> >::iterator mit = mFunctionWithData.find(function);
	if (mit == mFunctionWithData.end())
	{		
		strLog = "can not find function with data for \r\n";
		WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
	}
	else
	{
		strLog = "NO: " + StringUtil::intToStr(ID) + " Function: " + StringUtil::intToStr(mit->first) + "\r\n";
		WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
		vector<PriceData*> data = mit->second;
		vector<PriceData*>::iterator it;
		for (it = data.begin(); it != data.end(); it++)
		{
			strLog = " instrument: " + StringUtil::arrayToStr((*it)->InstrumentId) + " timeout: " + StringUtil::intToStr((*it)->TimeOut )
				+ " Change: " + StringUtil::doubleToStr((*it)->Change) 
				+ " CurrentPrice: " + StringUtil::doubleToStr((*it)->CurPrice) + " PriceTick: " + StringUtil::intToStr((*it)->PriceTick) 
				+ " TickCount: " + StringUtil::intToStr((*it)->TickCount) + " Volume: " + StringUtil::intToStr((*it)->Volume) 
				+ " HoldVolume: " + StringUtil::intToStr((*it)->HoldVolume) + "\r\n";
			WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
		}
	}


	//std::ofstream o_file("../cfg/trading_log.txt",std::ios::app);
	//o_file << TimeUtil::getTimeNow() << std::endl;
	//map<int, vector<PriceData *> >::iterator mit = mFunctionWithData.find(function);
	//if (mit == mFunctionWithData.end())
	//{
	//	o_file << "can not find function with data for " << function << std::endl;
	//}
	//else
	//{
	//	o_file << "NO: " << ID << " Function: " << mit->first << std::endl;
	//	vector<PriceData*> data = mit->second;
	//	vector<PriceData*>::iterator it;
	//	for (it = data.begin(); it != data.end(); it++)
	//	{
	//		o_file << " instrument: " << (*it)->InstrumentId << " timeout: " << (*it)->TimeOut 
	//			<< " Change: " << (*it)->Change 
	//			<< " CurrentPrice: " << (*it)->CurPrice << " PriceTick: " << (*it)->PriceTick 
	//			<< " TickCount: " << (*it)->TickCount << " Volume: " << (*it)->Volume 
	//			<< " HoldVolume: " << (*it)->HoldVolume<< std::endl;
	//	}
	//}
	//o_file << std::endl;
	//o_file.close();
}
void Case::show(int function, DWORD usedTime)
{
	map<int, vector<PriceData *> >::iterator mit;
	for (mit=mFunctionWithData.begin(); mit!=mFunctionWithData.end(); mit++)
	{
		if (mit->first == function)
		{			
			std::string strLog =  " Function: " + StringUtil::intToStr(mit->first) + " UsedTime: " + StringUtil::intToStr(usedTime) + "ms \r\n";
			WriteFile(g_hFile,strLog.c_str(),strlen(strLog.c_str()),&g_dwWritenSize,NULL);
		}
	}
}

///找到最低价
double Case::findLowestPrice(const char* instrumentId, double currentPrice, double change)
{
	double lowerPrice = pTrader->getLowerLimitPrice(instrumentId);
	if (change == 100)
	{
		return LowerLimitPrice;
	}
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
double Case::findLowestPrice(double lowerLimitPrice, double currentPrice, double change, double priceTick)
{
	if (change == 100)
	{
		return lowerLimitPrice;
	}
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
	if (price < lowerLimitPrice)
	{
		price = lowerLimitPrice;
	}
	return price;
}

///找到最高价
double Case::findHighestPrice(const char* instrumentId, double currentPrice, double change)
{
	double upperPrice = pTrader->getUpperLimitPrice(instrumentId);

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
double Case::findHighestPrice(double upperLimitPrice, double currentPrice, double change, double priceTick)
{
	if (change == 100)
	{
		return upperLimitPrice;
	}

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
	if (price > upperLimitPrice)
	{
		price = upperLimitPrice;
	}
	return price;
}

PriceData* Case::initPriceData(const char* instrumemtId, double currentPrice, double change,
	bool isUp, int timeout, int volume, int holdVolume)
{
	if (currentPrice == 0.0)
		return 0;
	PriceData* data = new PriceData;

	data->PriceTick = pTrader->getTickPrice(instrumemtId);

	if (isUp)
	{
		data->HighestPrice = findHighestPrice(instrumemtId, currentPrice, change);
		data->TickCount = ceil((data->HighestPrice - currentPrice)/PriceTick); ///向上取整
	}
	else
	{
		data->LowestPrice = findLowestPrice(instrumemtId, currentPrice, change);
		data->TickCount = ceil((currentPrice - data->LowestPrice)/PriceTick);
	}

	data->PriceTick = PriceTick;
	data->CurPrice = LastPrice;
	strcpy(data->InstrumentId, instrumemtId);
	data->TimeOut = timeout;
	data->Change = change;
	data->Volume = volume;
	data->HoldVolume = holdVolume;
	return data;
}

PriceData* Case::initPriceData(CThostFtdcDepthMarketDataField DepthMarketDataField, double change,
	double priceTick, bool isUp, int timeout, int volume, int holdVolume)
{
	PriceData* data = new PriceData;

	data->PriceTick = priceTick;

	double currentPrice = DepthMarketDataField.LastPrice == 0 ? DepthMarketDataField.PreSettlementPrice : DepthMarketDataField.LastPrice;
	if (isUp)
	{

		data->HighestPrice = findHighestPrice(DepthMarketDataField.UpperLimitPrice, currentPrice, change, priceTick);
		data->TickCount = ceil((data->HighestPrice - currentPrice)/PriceTick); ///向上取整
	}
	else
	{
		data->LowestPrice = findLowestPrice(DepthMarketDataField.LowerLimitPrice, currentPrice, change, priceTick);
		data->TickCount = ceil((currentPrice - data->LowestPrice)/PriceTick);
	}

	data->PriceTick = PriceTick;
	data->CurPrice = currentPrice;
	strcpy(data->InstrumentId, DepthMarketDataField.InstrumentID);
	data->TimeOut = timeout;
	data->Change = change;
	data->Volume = volume;
	data->HoldVolume = holdVolume;
	return data;
}


PriceData* Case::initPriceData(const char* instrumemtId, double currentPrice, double change, 
	int timeout, int maxVolume, int frequency, int volume, int holdVolume)
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
	data->Volume = volume;
	data->HoldVolume = holdVolume;

	return data;
}

PriceData* Case::initPriceData(CThostFtdcDepthMarketDataField DepthMarketDataField, double change, 
	double priceTick, int timeout, int maxVolume, int frequency, int volume, int holdVolume)
{
	PriceData* data = new PriceData;

	data->PriceTick = priceTick;
	double currentPrice = DepthMarketDataField.LastPrice == 0 ? DepthMarketDataField.PreSettlementPrice : DepthMarketDataField.LastPrice;
	data->HighestPrice = findHighestPrice(DepthMarketDataField.UpperLimitPrice, currentPrice, change, priceTick);
	data->LowestPrice = findLowestPrice(DepthMarketDataField.LowerLimitPrice, currentPrice, change, priceTick);

	data->CurPrice = currentPrice;
	strcpy(data->InstrumentId, DepthMarketDataField.InstrumentID);
	data->TimeOut = timeout;
	data->TickCount = ceil((currentPrice * change/100)/PriceTick);
	data->Change = change;
	data->MaxVolume = maxVolume;
	data->Frequency = frequency;
	data->Volume = volume;
	data->HoldVolume = holdVolume;

	return data;
}

CaseFactory::~CaseFactory()
{
}

CaseFactory::CaseFactory()
{
}

///创建case实例
Case* CaseFactory::createCase(Trader* pTrader, CaseCfg* caseCfg)
{
	Case* c = new Case(pTrader);
	c->setID(caseCfg->ID);
	c->setFunctions(caseCfg->Function);
	c->setChanges(caseCfg->Change);
	c->setFrequencys(caseCfg->Frequency);
	c->setInstruments(caseCfg->ProductOrInstrument);
	c->setMaxVolume(caseCfg->MaxVolume);
	c->setTimeouts(caseCfg->Timeout);
	c->setVolume(caseCfg->Volume);
	c->SetHoldVolume(caseCfg->HoldVolume);

	c->setPriceData();
	return c;
}

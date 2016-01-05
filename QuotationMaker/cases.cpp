#pragma once

#include "cases.h"

string cfgFilePath = "../cfg/maker_rsh.cfg";
string caseFilePath = "../cfg/cases.cfg";

DWORD g_dwStart = 0; //功能1，2使用

Cases::Cases()
{
	pTrader = new Trader();
	pMarketUtil = new MarketUtil();
	getCfg = CfgUtil::getInstance(cfgFilePath);
	//volume = StringUtil::stringToInt(getCfg->getPara("Volume"));
	//holdVolume = StringUtil::stringToInt(getCfg->getPara("HoldVolume"));
	//noTradedVolume = StringUtil::stringToInt(getCfg->getPara("NoTradedVolume"));
	IsUseClosePosition = getCfg->getParaInt("IsUseClosePosition");
	pCaseFactory = new CaseFactory();
	initData();
}

void Cases::run()
{
	Sleep(300);
	DWORD dwStart = 0;
	pMarketUtil->openMdLog();
	for (unsigned int i=0; i<vCases.size(); i++)
	{
		Common::record2File((TimeUtil::getTimeNow() + " case:  " + StringUtil::intToStr(vCases[i]->ID) + " begin \r\n").c_str());
		///行情开始标记
		pMarketUtil->writeSeparator((TimeUtil::getTimeNow() + " case:  " + StringUtil::intToStr(vCases[i]->ID) + " begin").c_str());
		vCases[i]->show();
		//map<int, vector<PriceData *>, DisableCompare<int> > mFunctionWithData = vCases[i]->getFunctionWithData();
		vector<pair<int, vector<PriceData *>> > mFunctionWithData = vCases[i]->getVFunctionWithData();
		//map<int, vector<PriceData *>, DisableCompare<int> >::iterator mit;
		vector<pair<int, vector<PriceData *>> >::iterator mit;
		for (mit = mFunctionWithData.begin(); mit != mFunctionWithData.end(); mit++)
		{
			switch ((*mit).first)
			{
			case 1:
				pMarketUtil->subcribeMarketData((*mit).second);
				std::cout << "function 1 begin" << std::endl;
				makeLimit((*mit).second, true);
				std::cout << "function 1 end" << std::endl;
				vCases[i]->show(1, GetTickCount() - g_dwStart);
				pMarketUtil->unSubscribeMarketData((*mit).second);
				Sleep(100);
				break;
			case 2:
				pMarketUtil->subcribeMarketData((*mit).second);								
				std::cout << "function 2 begin" << std::endl;
				makeLimit((*mit).second, false);
				std::cout << "function 2 end" << std::endl;
				vCases[i]->show(2, GetTickCount() - g_dwStart);
				pMarketUtil->unSubscribeMarketData((*mit).second);
				Sleep(100);
				break;
			case 3:
				vCases[i]->getPriceData(3, (*mit).second); //更新数据
				vCases[i]->show(3);
				pMarketUtil->subcribeMarketData((*mit).second);
				dwStart = GetTickCount();
				std::cout << "function 3 begin" << std::endl;
				holdChane((*mit).second, true);
				std::cout << "function 3 end" << std::endl;
				vCases[i]->show(3, GetTickCount() - dwStart);
				pMarketUtil->unSubscribeMarketData((*mit).second);
				Sleep(100);
				break;
			case 4:
				vCases[i]->getPriceData(4, (*mit).second);  //更新数据
				vCases[i]->show(4);
				pMarketUtil->subcribeMarketData((*mit).second);
				dwStart = GetTickCount();
				std::cout << "function 4 begin" << std::endl;
				holdChane((*mit).second, false);
				std::cout << "function 4 end" << std::endl;
				vCases[i]->show(4, GetTickCount() - dwStart);
				pMarketUtil->unSubscribeMarketData((*mit).second);
				Sleep(100);
				break;
			case 5:
				pMarketUtil->subcribeMarketData((*mit).second);
				dwStart = GetTickCount();
				std::cout << "function 5 begin" << std::endl;
				sendOrderRandom((*mit).second);
				std::cout << "function 5 end" << std::endl;
				vCases[i]->show(5, GetTickCount() - dwStart);
				pMarketUtil->unSubscribeMarketData((*mit).second);
				Sleep(100);
				break;
			case -1:
				break;

			default:
				break;
			}
		}
		Common::record2File((TimeUtil::getTimeNow() + " case:  " + StringUtil::intToStr(vCases[i]->ID) + " end \r\n").c_str());
		pMarketUtil->writeSeparator((TimeUtil::getTimeNow() + " case:  " + StringUtil::intToStr(vCases[i]->ID) + " end").c_str());
		///撤单
		pTrader->orderAction();

		///查询持仓，平仓
		if (IsUseClosePosition == 1)
		{
			for (mit = mFunctionWithData.begin(); mit != mFunctionWithData.end(); mit++)
			{
				pTrader->qryPosition((*mit).second);
			}			
		}	
	}
	pMarketUtil->closeMdLog();

}

///将最新价提高到指定价格
bool Cases::makeLimitPrice(const char* instrumentId, double limitPrice, int volume, bool isUp)
{
	if (isUp)
	{
		pTrader->sendOrder(instrumentId, 1, 0, volume, limitPrice);
		Sleep(5);
		pTrader->sendOrder(instrumentId, 0, 0, volume, limitPrice);
		Sleep(100);
	}
	else
	{
		pTrader->sendOrder(instrumentId, 0, 0, volume, limitPrice);
		Sleep(5);
		pTrader->sendOrder(instrumentId, 1, 0, volume, limitPrice);
		Sleep(100);
	}
	double lastPrice = pMarketUtil->getLastPrice(instrumentId);
	if (limitPrice == lastPrice)
		return true;
	else
		return false;
}

///废弃： 将最新价提高到指定价格
bool Cases::makeLimitPrice(const char* instrumentId, double limitPrice, bool isUp, int volume)
{
	//pTrader->getLastPrice(instrumentId);
	double lastPrice = pMarketUtil->getLastPrice(instrumentId);
	if (isUp)
	{
		pTrader->sendOrder(instrumentId, 0, 0, volume, limitPrice);
		pTrader->sendOrder(instrumentId, 1, 0, volume, limitPrice);
		if (limitPrice == lastPrice)
		{
			return true;
		}
	}
	else
	{
		pTrader->sendOrder(instrumentId, 0, 0, volume, limitPrice);
		pTrader->sendOrder(instrumentId, 1, 0, volume, limitPrice);
		if (limitPrice == lastPrice)
		{
			return true;
		}
	}

	//Sleep(1000);
	return false;
}

///初始化应用数据
void Cases::initData()
{
	ifstream fin( "../cfg/cases.cfg" ); 
	string line;
	string s;
	getline(fin, line);
	while (getline(fin, line))
	{
		if (line == "")
			break;
		if (string::npos != line.rfind("#", 1))
			continue;
		CaseCfg *c = new CaseCfg;
		std::stringstream w(line);

		w >> c->ID;
		w >> c->Function;
		w >> c->ProductOrInstrument;
		w >> c->Timeout;
		w >> c->Change;
		w >> c->MaxVolume;
		w >> c->Frequency;
		w >> c->Volume;
		w >> c->HoldVolume;

		std::cout << "case " << c->ID << std::endl;
		std::cout << "ProductOrInstrument: " << c->ProductOrInstrument << std::endl;

		vCases.push_back(pCaseFactory->createCase(pTrader, c));

	}

	std::cout << "---->>>初始化数据结束" << std::endl;
	//Sleep(1000);
	//vector<Case *>::iterator cit;
	//for (cit=vCases.begin(); cit!=vCases.end(); cit++)
	//{
	//	(*cit)->show();
	//}
	std::cout << "-----------------------------" << std::endl;

	//pTrader->qryPosition(vCases[0]->getFunctionWithData().find(1)->second);
	//ResetEvent(g_hEvent);
	//WaitForSingleObject(g_hEvent, INFINITE);
}

///保持涨停/跌停
void Cases::makeLimit(vector<PriceData *> data, bool isUp)
{
	g_dwStart = GetTickCount(); //先初始化
	if (data.size() < 1)
		return;
	DWORD dwUsed = 0; //消耗的时间
	double price = 0;	
	for (unsigned int i=0; i<data.size(); i++)
	{
		if (isUp)
			price = data[i]->HighestPrice;
		else
			price = data[i]->LowestPrice;

		if (price == 0)
		{
			data.erase(data.begin() + i);  ///删除没有价格的合约
			continue;
		}

		while(!makeLimitPrice(data[i]->InstrumentId, price, data[i]->Volume, isUp))  ///待增加超时机制 **********
		{
		}	
		if (isUp)
		{
			pTrader->sendOrder(data[i]->InstrumentId, 0, 0, data[i]->HoldVolume, data[i]->HighestPrice);
		}
		else
		{
			pTrader->sendOrder(data[i]->InstrumentId, 1, 0, data[i]->HoldVolume, data[i]->LowestPrice);
		}
	}
	DWORD dwStart = GetTickCount(); //取windows启动到现在的流逝时间(毫秒)
	g_dwStart = GetTickCount();

	while (dwUsed <= (data[0]->TimeOut * 1000))
	{
		for (unsigned int i=0; i<data.size(); i++)
		{
			NoTradedNumber =0;  //重置
			//int _noTraded = 0;
			//pTrader->qryOrder(data[i]->InstrumentId);
			//Sleep(1000);
			if (isUp)
			{
				int bidVolume = 0;
				double bidPrice = 0.0;
				pMarketUtil->getBidPrice(data[i]->InstrumentId, bidPrice, bidVolume);
				//double bidPrice = pTrader->getBidPrice(data[i]->InstrumentId);  ///买一价				
				//std::cout << "NoTradedNumber  " << NoTradedNumber << std::endl;
				Sleep(500);
				//std::cout << "------买一量："<< bidVolume <<"--------买一价："<< bidPrice << "------涨停价：" << data[i]->HighestPrice << "-----------" << data[i]->InstrumentId << std::endl;
				if ((data[i]->HoldVolume <= bidVolume) && (bidPrice == data[i]->HighestPrice))
				{
					continue;
				}
				else
				{
					pTrader->sendOrder(data[i]->InstrumentId, 0, 0, data[i]->HoldVolume, data[i]->HighestPrice);

				}
			}
			else
			{
				double askPrice = 0.0;
				int askVolume = 0;
				pMarketUtil->getAskPrice(data[i]->InstrumentId, askPrice, askVolume);
				//askPrice = pTrader->getAskPrice(data[i]->InstrumentId);				
				//std::cout << "NoTradedNumber  " << NoTradedNumber << std::endl;
				Sleep(500);
				//std::cout << "------卖一量："<< askVolume <<"--------卖一价："<< askPrice << "------跌停价：" << data[i]->LowestPrice << "-----------" << data[i]->InstrumentId << std::endl;
				if ((data[i]->HoldVolume <= askVolume) && (askPrice == data[i]->LowestPrice))
				{
					continue;
				}
				else 
				{
					pTrader->sendOrder(data[i]->InstrumentId, 1, 0, data[i]->HoldVolume, data[i]->LowestPrice);

				}
			}
		}
		dwUsed = GetTickCount() - dwStart; 
	}
}

///匀速上涨/下跌
void Cases::holdChane(vector<PriceData *> data, bool isUp)
{
	int dataSize = data.size();
	if (dataSize < 1)
		return;
	double price = 0;
	double changePrice = 0;  //涨跌幅度值
	if (data[0]->TimeOut == 0)  //瞬间涨/跌
	{
		if (data[0]->Change = 100)//涨停/跌停
		{
			for (unsigned int i=0; i<data.size(); i++)
			{
				if (isUp)
					price = data[i]->HighestPrice;
				else
					price = data[i]->LowestPrice;
				while (!makeLimitPrice(data[i]->InstrumentId, price, data[i]->Volume, isUp))
				{
				}	
				if (isUp)
				{
					pTrader->sendOrder(data[i]->InstrumentId, 0, 0, data[i]->HoldVolume, data[i]->HighestPrice);
				}
				else
				{
					pTrader->sendOrder(data[i]->InstrumentId, 1, 0, data[i]->HoldVolume, data[i]->LowestPrice);
				}
			}
		}
		else  //到指定价格
		{
			for (int i=0; i<dataSize; i++)
			{
				double lastPrice = pTrader->getLastPrice(data[i]->InstrumentId);
				while (!makeLimitPrice(data[i]->InstrumentId, data[i]->HighestPrice, data[i]->Volume, isUp))
				{}

				if (isUp)
				{
					pTrader->sendOrder(data[i]->InstrumentId, 0, 0, data[i]->HoldVolume, data[i]->HighestPrice);
				}
				else
				{
					pTrader->sendOrder(data[i]->InstrumentId, 1, 0, data[i]->HoldVolume, data[i]->LowestPrice);
				}
			}
		}
	}
	else
	{
		HANDLE  lphandles[500];
		for (int i=0; i<dataSize; i++)
		{
			Data4Thread *pData = new Data4Thread;
			pData->data = data[i];
			pData->pTrader = pTrader;
			pData->volume = data[i]->Volume;
			pData->isUp = isUp;
			pData->holdVolume = data[i]->HoldVolume;
			lphandles[i] = CreateThread(NULL,0,holdChangeWithOne,(LPVOID)pData,0,NULL);
			pData = NULL;
		}
		WaitForMultipleObjects(dataSize, lphandles, true, INFINITE);
		for (int h=0; h<dataSize; h++)
		{
			if (lphandles[h] != NULL)
				CloseHandle(lphandles[h]);
		}
		//Sleep((data[0]->TimeOut +1) * 1000);
	}
}

DWORD WINAPI holdChangeWithOne(LPVOID pParam)
{
	Data4Thread* pData = (Data4Thread *)pParam;
	PriceData* data = pData->data;
	Trader* pTrader = pData->pTrader;
	int volume  = pData->volume;
	int _holdVolume = pData->holdVolume;
	double mod = fmod((double)data->TimeOut * 1000, (double)data->TickCount);
	int timeEachTick = 0;
	if (mod == 0 && data->TickCount != 0)
	{
		timeEachTick  = floor(double(data->TimeOut * 1000 / data->TickCount));	 //向下取整
	}
	else if(mod != 0 && data->TickCount != 0)
	{
		timeEachTick = floor(double(data->TimeOut * 1000 / data->TickCount));
	}
	else if(data->TickCount == 0)
	{
		std::cout << data->InstrumentId <<"---->>>已经是涨/跌停板价" << std::endl;
	}
	//Common::log2File("timeEachTick " + timeEachTick);
	DWORD dwUsed = 0;
	DWORD dwStart = GetTickCount(); ///开始时间
	for (int i=1; i<= data->TickCount; i++)
	{
		//if (dwUsed >= (timeEachTick * data->TickCount))
		//break;
		if (pData->isUp)
		{
			double inputPrice = data->CurPrice + (i * data->PriceTick);
			if (inputPrice > data->HighestPrice)
				inputPrice = data->HighestPrice;
			pTrader->sendOrder(data->InstrumentId, 0, 0 , volume, inputPrice);
			pTrader->sendOrder(data->InstrumentId, 1, 0 , volume, inputPrice);
			//挂单
			pTrader->sendOrder(data->InstrumentId, 0, 0 , _holdVolume, inputPrice);
			//pTrader->sendOrder(data->InstrumentId, 1, 3 , volume, inputPrice);
		}
		else
		{
			double inputPrice = data->CurPrice - (i * data->PriceTick);
			if (inputPrice < data->LowestPrice)
				inputPrice = data->LowestPrice;
			pTrader->sendOrder(data->InstrumentId, 1, 0 , volume, inputPrice);
			pTrader->sendOrder(data->InstrumentId, 0, 0 , volume, inputPrice);
			//挂单
			pTrader->sendOrder(data->InstrumentId, 1, 0 , _holdVolume, inputPrice);
			//pTrader->sendOrder(data->InstrumentId, 0, 3 , volume, inputPrice);
		}
		Sleep(timeEachTick);
		//dwUsed = GetTickCount() - dwStart;
	}
	return 1;
}

///随机下单, 价格、手数、合约随机
void Cases::sendOrderRandom(vector<PriceData *> data)
{
	if (data.size() < 1)
		return;

	DWORD dwUsed = 0; //消耗的时间
	DWORD dwStart = GetTickCount(); //毫秒
	srand((int)time(0));  //设置随机种子
	int dataIndex = 0;
	double price = 0.0;
	int _volume = 1;
	int buyOrSell = 0;

	while(dwUsed <= (data[0]->TimeOut * 1000))
	{
		dataIndex = Random(data.size());
		_volume = Random(data[0]->MaxVolume) + 1;
		if(Random(2))
		{
			price = data[dataIndex]->CurPrice + (Random(data[dataIndex]->TickCount)+1) * data[dataIndex]->PriceTick;
		}
		else
		{
			price =  data[dataIndex]->CurPrice - (Random(data[dataIndex]->TickCount)+1) * data[dataIndex]->PriceTick;
		}
		if (price >= data[dataIndex]->HighestPrice)
			price = data[dataIndex]->HighestPrice;
		if (price <= data[dataIndex]->LowestPrice)
			price = data[dataIndex]->LowestPrice;

		buyOrSell = Random(2);

		pTrader->sendOrder(data[dataIndex]->InstrumentId, buyOrSell, 0, _volume, price);
		Sleep(data[0]->Frequency);
		dwUsed = GetTickCount() - dwStart; 
	}
}

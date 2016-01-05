#pragma once

#include "cases.h"

string cfgFilePath = "../cfg/maker_rsh.cfg";
string caseFilePath = "../cfg/cases.cfg";

DWORD g_dwStart = 0; //����1��2ʹ��

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
		///���鿪ʼ���
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
				vCases[i]->getPriceData(3, (*mit).second); //��������
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
				vCases[i]->getPriceData(4, (*mit).second);  //��������
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
		///����
		pTrader->orderAction();

		///��ѯ�ֲ֣�ƽ��
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

///�����¼���ߵ�ָ���۸�
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

///������ �����¼���ߵ�ָ���۸�
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

///��ʼ��Ӧ������
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

	std::cout << "---->>>��ʼ�����ݽ���" << std::endl;
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

///������ͣ/��ͣ
void Cases::makeLimit(vector<PriceData *> data, bool isUp)
{
	g_dwStart = GetTickCount(); //�ȳ�ʼ��
	if (data.size() < 1)
		return;
	DWORD dwUsed = 0; //���ĵ�ʱ��
	double price = 0;	
	for (unsigned int i=0; i<data.size(); i++)
	{
		if (isUp)
			price = data[i]->HighestPrice;
		else
			price = data[i]->LowestPrice;

		if (price == 0)
		{
			data.erase(data.begin() + i);  ///ɾ��û�м۸�ĺ�Լ
			continue;
		}

		while(!makeLimitPrice(data[i]->InstrumentId, price, data[i]->Volume, isUp))  ///�����ӳ�ʱ���� **********
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
	DWORD dwStart = GetTickCount(); //ȡwindows���������ڵ�����ʱ��(����)
	g_dwStart = GetTickCount();

	while (dwUsed <= (data[0]->TimeOut * 1000))
	{
		for (unsigned int i=0; i<data.size(); i++)
		{
			NoTradedNumber =0;  //����
			//int _noTraded = 0;
			//pTrader->qryOrder(data[i]->InstrumentId);
			//Sleep(1000);
			if (isUp)
			{
				int bidVolume = 0;
				double bidPrice = 0.0;
				pMarketUtil->getBidPrice(data[i]->InstrumentId, bidPrice, bidVolume);
				//double bidPrice = pTrader->getBidPrice(data[i]->InstrumentId);  ///��һ��				
				//std::cout << "NoTradedNumber  " << NoTradedNumber << std::endl;
				Sleep(500);
				//std::cout << "------��һ����"<< bidVolume <<"--------��һ�ۣ�"<< bidPrice << "------��ͣ�ۣ�" << data[i]->HighestPrice << "-----------" << data[i]->InstrumentId << std::endl;
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
				//std::cout << "------��һ����"<< askVolume <<"--------��һ�ۣ�"<< askPrice << "------��ͣ�ۣ�" << data[i]->LowestPrice << "-----------" << data[i]->InstrumentId << std::endl;
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

///��������/�µ�
void Cases::holdChane(vector<PriceData *> data, bool isUp)
{
	int dataSize = data.size();
	if (dataSize < 1)
		return;
	double price = 0;
	double changePrice = 0;  //�ǵ�����ֵ
	if (data[0]->TimeOut == 0)  //˲����/��
	{
		if (data[0]->Change = 100)//��ͣ/��ͣ
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
		else  //��ָ���۸�
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
		timeEachTick  = floor(double(data->TimeOut * 1000 / data->TickCount));	 //����ȡ��
	}
	else if(mod != 0 && data->TickCount != 0)
	{
		timeEachTick = floor(double(data->TimeOut * 1000 / data->TickCount));
	}
	else if(data->TickCount == 0)
	{
		std::cout << data->InstrumentId <<"---->>>�Ѿ�����/��ͣ���" << std::endl;
	}
	//Common::log2File("timeEachTick " + timeEachTick);
	DWORD dwUsed = 0;
	DWORD dwStart = GetTickCount(); ///��ʼʱ��
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
			//�ҵ�
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
			//�ҵ�
			pTrader->sendOrder(data->InstrumentId, 1, 0 , _holdVolume, inputPrice);
			//pTrader->sendOrder(data->InstrumentId, 0, 3 , volume, inputPrice);
		}
		Sleep(timeEachTick);
		//dwUsed = GetTickCount() - dwStart;
	}
	return 1;
}

///����µ�, �۸���������Լ���
void Cases::sendOrderRandom(vector<PriceData *> data)
{
	if (data.size() < 1)
		return;

	DWORD dwUsed = 0; //���ĵ�ʱ��
	DWORD dwStart = GetTickCount(); //����
	srand((int)time(0));  //�����������
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

#pragma once

#include "cases.h"


Cases::Cases()
{
	pTrader = new Trader();

	getCfg = CfgUtil::getInstance(cfgFilePath);

	//string strumentIds = getCfg->getPara("InstrumentIds");
	//StringUtil::stringToVector(strumentIds, ",", vInstrIds);
	//vInstrSize = vInstrIds.size();
	//instrumentId = getCfg->getPara("InstrumentId");

	//std::cerr << "InstrumentIds: " << strumentIds << std::endl;
}


void Cases::run()
{
	//double lower = pTrader->getLowerLimitPrice("a1703");
	//double upper = pTrader->getUpperLimitPrice("a1703");

	//pTrader->sendOrder("ag1509", 0, 0, 2, 3800);

	//_record2File("ag1509", 0, 0, 2, 3800);
	//_record2Stdout("ag1509", 0, 0, 2, 3800);

	//std::cerr << "" << lower << " " << upper << std::endl;

	//std::cerr << pTrader->getTickPrice("a1703") << std::endl;
	//std::cerr << AllInstrumentId.size() << std::endl;


	//vector<string> temVInstrIds;
	//string instrumentIds = "b";
	//StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
	//vInstrSize = vInstrIds.size();
	//for (int j=0; j< vInstrSize; j++)
	//{
	//	std::cerr << vInstrIds[j] << std::endl;
	//	Common::findInstruments(vInstrIds[j], AllInstrumentId, temVInstrIds);
	//}

	//vector<string>::iterator it;
	//for (it = temVInstrIds.begin(); it!=temVInstrIds.end(); it++)
	//{
	//	std::cerr << *it << std::endl;
	//}

	Sleep(3000);

	int c;
	bool loop = true;
	while (loop)
	{
		Sleep(100);
		MainMenu();

		std::cin >> c;
		switch (c)
		{
		case 1:
			Case1();
			break;
		case 2:
			Case2();
			break;
		case 3:
			Case3();
			break;
		case 4:
			Case4();
			break;
		case 5:
			Case5();
			break;
		case 6:
			Case6();
			break;
		case -1:
			loop = false;
			break;

		default:
			break;
		}
	}

	system("pause");
}


void Cases::MainMenu()
{
	system("cls");
	std::cout << "------程序化交易软件测试工具------" << std::endl;
	std::cout << "1、	涨停/跌停" << std::endl;
	std::cout << "2、	涨停-跌停/跌停-涨停" << std::endl;
	std::cout << "3、	连续上涨/下跌" << std::endl;
	std::cout << "4、	瞬涨后瞬跌/瞬跌后瞬涨" << std::endl;
	std::cout << "5、	在某价格区间上大量下单" << std::endl;
	std::cout << "6、	随机下单" << std::endl;
	std::cout << "-1、	退出程序" << std::endl;
}

bool Cases::makeLimitPrice(const char* instrumentId, double limitPrice)
{
	pTrader->sendOrder(instrumentId, 0, 0, 1, limitPrice);
	Sleep(100);
	Common::record2File(instrumentId, 0, 0, 1, limitPrice);
	Common::record2Stdout(instrumentId, 0, 0, 1, limitPrice);
	pTrader->sendOrder(instrumentId, 1, 0, 1, limitPrice);
	Sleep(600);
	Common::record2File(instrumentId, 1, 0, 1, limitPrice);
	Common::record2Stdout(instrumentId, 1, 0, 1, limitPrice);
	if (limitPrice == pTrader->getLastPrice(instrumentId))
		return true;
	else
		return false;
}

double Cases::findLowestPrice(const char* instrumentId, double currentPrice, double change)
{
	double lowerPrice = pTrader->getLowerLimitPrice(instrumentId);
	double priceTick = PriceTick;
	double price = 0.0;
	double changePrice = currentPrice * change;

	if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
	{
		price = currentPrice - changePrice;
	}
	else if ( fmod(changePrice, priceTick) != 0)
	{
		price = currentPrice - (changePrice - fmod(changePrice, priceTick));
	}
	if (price < lowerPrice)
	{
		price = lowerPrice;
	}
	return price;
}

double Cases::findHighestPrice(const char* instrumentId, double currentPrice, double change)
{
	double upperPrice = pTrader->getUpperLimitPrice(instrumentId);
	Sleep(1000);
	double priceTick = PriceTick;
	double price = 0.0;
	double changePrice = currentPrice * change;

	if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
	{
		price = currentPrice + changePrice;
	}
	else if ( fmod(changePrice, priceTick) != 0)
	{
		price = currentPrice + (changePrice - fmod(changePrice, priceTick));  //fmod取余
	}
	if (price > upperPrice)
	{
		price = upperPrice;
	}
	return price;
}

PriceData* Cases::getPriceData(const char* instrumemtId, double currentPrice, double change)
{
	if (currentPrice == 0.0)
		return 0;
	PriceData* data = new PriceData;
	Sleep(300);
	data->HighestPrice = findHighestPrice(instrumemtId, currentPrice, change);
	Sleep(300);
	data->LowestPrice = findLowestPrice(instrumemtId, currentPrice, change);
	data->PriceTick = pTrader->getTickPrice(instrumemtId);
	data->CurPrice = currentPrice;
	strcpy(data->InstrumentId, instrumemtId);
	return data;
}

///涨停/跌停
void Cases::Case1()
{
	int c;
	bool loop = true;
	string instrumentIds;
	vector<string> temVInstrIds; //for UpperLimitPrice/LowerLimitPrice
	vector<string> tempVInstrIds5; //for 5
	int tempVInstrIds5Size=0;
	vector<string> vTemp(AllInstrumentId.begin(), AllInstrumentId.end()); //报单时使用
	int temVInstrIdSize=0;
	while (loop)
	{		

		bool isLimitPrice = false;  //到涨跌停板时需要停止报单
		std::cout << "------case1------" << std::endl; 
		std::cout << "1. 涨停" << std::endl; 
		std::cout << "2. 跌停" << std::endl;
		std::cout << "3. 开始报单且有成交" << std::endl;
		std::cout << "4. 开始报单且无成交" << std::endl;
		std::cout << "5. 重置合约价格(将最新价恢复到昨结算价)" << std::endl;
		std::cout << "-1. 返回上一级" << std::endl; 

		std::cin >> c;
		switch (c)
		{
		case -1:
			loop = false;
			break;
		case 1:
			vector <string>().swap(temVInstrIds); //需要清除，否则会累积
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();

			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, temVInstrIds);
			}

			Common::copyVector(temVInstrIds, tempVInstrIds5);
			temVInstrIdSize = temVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<temVInstrIdSize; i++)
				{
					double upperLimitPrice = pTrader->getUpperLimitPrice(temVInstrIds[i].c_str());
					if (makeLimitPrice(temVInstrIds[i].c_str(), upperLimitPrice) || upperLimitPrice == 0.0)
					{
						temVInstrIds.erase(temVInstrIds.begin() + i);
						temVInstrIdSize--;
					}
				}
				if (temVInstrIdSize == 0)
				{
					std::cout << "已经全部涨停" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 2:		//跌停
			vector <string>().swap(temVInstrIds); //需要清除，否则会累积
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, temVInstrIds);
			}
			Common::copyVector(temVInstrIds, tempVInstrIds5);
			temVInstrIdSize = temVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<temVInstrIdSize; i++)
				{
					//跌停价
					double lowerLimitPrice = pTrader->getLowerLimitPrice(temVInstrIds[i].c_str());
					if (makeLimitPrice(temVInstrIds[i].c_str(), lowerLimitPrice) || lowerLimitPrice == 0.0)
					{
						temVInstrIds.erase(temVInstrIds.begin() + i);
						temVInstrIdSize--;
					}
				}
				if (temVInstrIdSize == 0)
				{
					std::cout << "已经全部跌停" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 3:
			std::cout << "按住空格键持续报单，按ESC退出" << std::endl;
			while(getch()!=0x1b)   //esc
			{
				///自买自卖
				///
				int index = Random(vTemp.size());
				double lastPrice = pTrader->getLastPrice(vTemp[index].c_str());
				pTrader->sendOrder(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Sleep(100);
				Common::record2File(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Common::record2Stdout(vTemp[index].c_str(), 0, 0, 1, lastPrice);

				pTrader->sendOrder(vTemp[index].c_str(), 1, 0, 1, lastPrice);
				Sleep(100);
				Common::record2File(vTemp[index].c_str(), 1, 0, 1, lastPrice);
				Common::record2Stdout(vTemp[index].c_str(), 1, 0, 1, lastPrice);
				Sleep(100);
			}
			break;

		case 4:

			std::cout << "按住空格键持续报单，按ESC退出" << std::endl;
			while(getch()!=0x1b)   //esc
			{
				///单边买
				///
				int index = Random(vTemp.size());
				double lastPrice = pTrader->getLastPrice(vTemp[index].c_str());
				pTrader->sendOrder(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Sleep(100);
				Common::record2File(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Common::record2Stdout(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Sleep(100);
			}
			break;

		case 5:
			tempVInstrIds5Size = tempVInstrIds5.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tempVInstrIds5Size; i++)
				{
					//开盘价
					double preSettlementPrice = pTrader->getPreSettlementPrice(tempVInstrIds5[i].c_str());
					if (makeLimitPrice(tempVInstrIds5[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tempVInstrIds5.erase(tempVInstrIds5.begin() + i);
						tempVInstrIds5Size--;
					}
				}
				if (tempVInstrIds5Size == 0)
				{
					std::cout << "已经还原" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		default:
			break;
		}
	}
}

///从一个停板到另一个停板
void Cases::Case2()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	int waiteTime = 0;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> _tmpVInstrIds; //从一个停板价到另一个停板价使用 tmpVInstrIds的复制
	int _tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 2----" << std::endl;
		std::cout << "1. 涨停—跌停(先涨停，然后间隔一段时间后跌停)" << std::endl;
		std::cout << "2. 跌停—涨停(先跌停，然后间隔一段时间后涨停)" << std::endl;
		std::cout << "3. 重置合约价格(将最新价恢复到开盘价)" << std::endl;
		std::cout << "-1. 返回上一级" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:  ///1. 涨停—跌停(先涨停，然后间隔一段时间后跌停)
			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入从一个停板价到另一个停板价的时间间隔（单位秒）：" << std::endl;
			std::cin >> waiteTime;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);
			Common::copyVector(tmpVInstrIds, _tmpVInstrIds);
			tmpVInstrIdSize = tmpVInstrIds.size();		
			_tmpVInstrIdSize = tmpVInstrIdSize;
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize; i++)
				{
					double upperLimitPrice = pTrader->getUpperLimitPrice(tmpVInstrIds[i].c_str());
					if (makeLimitPrice(tmpVInstrIds[i].c_str(), upperLimitPrice) || upperLimitPrice == 0.0)
					{
						tmpVInstrIds.erase(tmpVInstrIds.begin() + i);
						tmpVInstrIdSize--;
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "已经全部涨停" << std::endl;
					isLimitPrice = true;
				}

				Sleep(waiteTime * 1000);

				if (isLimitPrice)
				{
					isLimitPrice = false;

					for (int i=0; i<_tmpVInstrIdSize; i++)
					{
						double lowerLimitPrice = pTrader->getLowerLimitPrice(_tmpVInstrIds[i].c_str());
						if (makeLimitPrice(_tmpVInstrIds[i].c_str(), lowerLimitPrice) || lowerLimitPrice == 0.0)
						{
							_tmpVInstrIds.erase(_tmpVInstrIds.begin() + i);
							_tmpVInstrIdSize--;
						}
					}
					if (_tmpVInstrIdSize == 0)
					{
						std::cout << "已经全部跌停" << std::endl;
						isLimitPrice = true;
					}
				}
			}
			break;
		case 2:		///2. 跌停—涨停(先跌停，然后间隔一段时间后涨停)
			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入从一个停板价到另一个停板价的时间间隔（单位秒）：" << std::endl;
			std::cin >> waiteTime;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
			Common::copyVector(tmpVInstrIds, _tmpVInstrIds);  //涨停需这些合约
			tmpVInstrIdSize = tmpVInstrIds.size();		
			_tmpVInstrIdSize = tmpVInstrIdSize;
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize; i++)
				{
					double lowerLimitPrice = pTrader->getLowerLimitPrice(tmpVInstrIds[i].c_str());
					if (makeLimitPrice(tmpVInstrIds[i].c_str(), lowerLimitPrice) || lowerLimitPrice == 0.0)
					{
						tmpVInstrIds.erase(tmpVInstrIds.begin() + i);
						tmpVInstrIdSize--;
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "已经全部跌停" << std::endl;
					isLimitPrice = true;
				}

				Sleep(waiteTime * 1000);

				if (isLimitPrice)
				{
					isLimitPrice = false;

					for (int i=0; i<_tmpVInstrIdSize; i++)
					{
						double upperLimitPrice = pTrader->getUpperLimitPrice(_tmpVInstrIds[i].c_str());
						if (makeLimitPrice(_tmpVInstrIds[i].c_str(), upperLimitPrice) || upperLimitPrice == 0.0)
						{
							_tmpVInstrIds.erase(_tmpVInstrIds.begin() + i);
							_tmpVInstrIdSize--;
						}
					}
					if (_tmpVInstrIdSize == 0)
					{
						std::cout << "已经全部涨停" << std::endl;
						isLimitPrice = true;
					}
				}
			}
			break;
		case 3:
			tmpVInstrIdSize3 = tmpVInstrIds3.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize3; i++)
				{
					//开盘价
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "已经还原" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case -1:
			loop = false;
			break;
		default:
			break;
		}
	}
}

///连续涨停/跌停
void Cases::Case3()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	double change = 0.0; //涨幅
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 3----" << std::endl;
		std::cout << "1. 连续上涨" << std::endl;
		std::cout << "2. 连续下跌" << std::endl;
		std::cout << "3. 重置合约价格(将最新价恢复到昨结算价)" << std::endl;
		std::cout << "-1. 返回上一级" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:

			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
			tmpVInstrIdSize = tmpVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize; i++)
				{
					double lastPrice = lastPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
					double upperPrice = UpperLimitPrice;
					double priceTick = PriceTick;
					double price = 0.0;

					if (upperPrice == 0.0 || lastPrice == upperPrice)
					{
						tmpVInstrIds.erase(tmpVInstrIds.begin() + i);
						tmpVInstrIdSize--;
						continue;
					}
					while(lastPrice != upperPrice)
					{
						lastPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
						double changePrice = lastPrice * change;
						if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
						{
							price = lastPrice + changePrice;
						}
						else if ( fmod(changePrice, priceTick) != 0)
						{
							price = lastPrice + (changePrice - fmod(changePrice, priceTick));
						}
						if (price > upperPrice)
						{
							price = upperPrice;
						}
						while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
						{
							Sleep(500); //延迟看效果
						}
						Sleep(500);  //延迟看效果
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "已经涨停" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 2:   //连续下跌

			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
			tmpVInstrIdSize = tmpVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize; i++)
				{
					double lowerPrice = pTrader->getLowerLimitPrice(tmpVInstrIds[i].c_str());
					double priceTick = pTrader->getTickPrice(tmpVInstrIds[i].c_str());
					double lastPrice = lastPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
					double price = 0.0;

					if (lowerPrice == 0.0 || lastPrice == lowerPrice)
					{
						tmpVInstrIds.erase(tmpVInstrIds.begin() + i);
						tmpVInstrIdSize--;
						continue;
					}
					while(lastPrice != lowerPrice)
					{
						lastPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
						double changePrice = lastPrice * change;
						if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
						{
							price = lastPrice - changePrice;
						}
						else if ( fmod(changePrice, priceTick) != 0)
						{
							price = lastPrice - (changePrice - fmod(changePrice, priceTick));
						}
						if (price < lowerPrice)
						{
							price = lowerPrice;
						}
						while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
						{
							Sleep(500); //延迟看效果
						}
						Sleep(500);  //延迟看效果
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "已经跌停" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 3:
			tmpVInstrIdSize3 = tmpVInstrIds3.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize3; i++)
				{
					//昨结算价
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "已经还原" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case -1:
			loop = false;
			break;
		}
	}
}

///瞬涨后瞬跌、瞬跌后瞬涨
void Cases::Case4()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	double change = 0.0; //涨跌幅
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 4----" << std::endl;
		std::cout << "1. 瞬涨后瞬跌" << std::endl;
		std::cout << "2. 瞬跌后瞬涨" << std::endl;
		std::cout << "3. 重置合约价格(将最新价恢复到昨结算价)" << std::endl;
		std::cout << "-1. 返回上一级" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:

			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
			tmpVInstrIdSize = tmpVInstrIds.size();

			for (int i=0; i<tmpVInstrIdSize; i++)
			{
				double currentPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
				double lowerPrice = LowerLimitPrice;
				double upperPrice = UpperLimitPrice;
				double priceTick = PriceTick;
				double changePrice = currentPrice * change;
				double price = 0.0;
				//涨
				if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
				{
					price = currentPrice + changePrice;
				}
				else if ( fmod(changePrice, priceTick) != 0)
				{
					price = currentPrice + (changePrice - fmod(changePrice, priceTick));
				}
				if (price > upperPrice)
				{
					price = upperPrice;
				}
				while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
				{
					Sleep(1000); //延迟
				}
				//跌
				if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
				{
					price = currentPrice - changePrice;
				}
				else if ( fmod(changePrice, priceTick) != 0)
				{
					price = currentPrice - (changePrice - fmod(changePrice, priceTick));
				}
				if (price < lowerPrice)
				{
					price = lowerPrice;
				}
				while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
				{
					Sleep(1000); //延迟
				}
			}
			break;

		case 2: //瞬跌后瞬涨

			vector <string>().swap(tmpVInstrIds);
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
			tmpVInstrIdSize = tmpVInstrIds.size();

			for (int i=0; i<tmpVInstrIdSize; i++)
			{
				double currentPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
				double upperPrice = UpperLimitPrice;
				double lowerPrice = LowerLimitPrice;
				double priceTick = PriceTick;
				double changePrice = currentPrice * change;
				double price = 0.0;
				//跌
				if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
				{
					price = currentPrice - changePrice;
				}
				else if ( fmod(changePrice, priceTick) != 0)
				{
					price = currentPrice - (changePrice - fmod(changePrice, priceTick));
				}
				if (price < lowerPrice)
				{
					price = lowerPrice;
				}
				while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
				{
					Sleep(1000); //延迟
				}
				//涨
				if (fmod(changePrice, priceTick) == 0)  //最小变动价的整数倍
				{
					price = currentPrice + changePrice;
				}
				else if ( fmod(changePrice, priceTick) != 0)
				{
					price = currentPrice + (changePrice - fmod(changePrice, priceTick));
				}
				if (price > upperPrice)
				{
					price = upperPrice;
				}
				while (!makeLimitPrice(tmpVInstrIds[i].c_str(), price))
				{
					Sleep(1000); //延迟
				}
			}
			break;

		case 3:   ///重置合约价格

			tmpVInstrIdSize3 = tmpVInstrIds3.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize3; i++)
				{
					//昨结算价
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "已经还原" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case -1:
			loop = false;
			break;
		default:
			break;
		}
	}
}

///在价格区间上大量下单
void Cases::Case5()
{
	int c;
	string instrumentIds = "";
	double change = 0.0; //涨跌幅
	int volume = 1;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //重置价格时需要这些合约
	int tmpVInstrIdSize3;
	bool isLimitPrice = false;

	vector <string>().swap(tmpVInstrIds);
	std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
	std::cin >> instrumentIds;
	std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
	std::cin >> change;
	change = change / 100;

	std::cout << "请输入每笔报单的手数：" << std::endl;
	std::cin >> volume;

	StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
	vInstrSize = vInstrIds.size();
	for (int j=0; j< vInstrSize; j++)
	{
		Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
	}
	Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
	tmpVInstrIdSize = tmpVInstrIds.size();

	vector<PriceData *> data;
	for (int i=0; i<tmpVInstrIdSize; i++)
	{
		double curPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
		data.push_back(getPriceData(tmpVInstrIds[i].c_str(), curPrice, change));
	}

	std::cout << "--->>" << "初始化数据结束，请按住空格键持续报单，ESC键退出" << std::endl;
	int dataSize = data.size();
	int i = 0;
	while (getch()!=0x1b)
	{
		if (dataSize != 0)
		{
			if (i == dataSize)
				i=0;		
			double bidOrAskPrice = 0.0;	
			srand((int)time(0));  //设置随机种子
			if (Random(2) == 1)
			{
				bidOrAskPrice = data[i]->CurPrice + ((Random(20) + 1) * data[i]->PriceTick);	
			}
			else
			{
				bidOrAskPrice = data[i]->CurPrice - ((Random(20) + 1) * data[i]->PriceTick);	
			}		
			//std::cout << "bidOrAskPrice--->>" << bidOrAskPrice << std::endl;

			if (bidOrAskPrice < data[i]->LowestPrice || bidOrAskPrice > data[i]->HighestPrice)
			{
				continue;
			}
			pTrader->sendOrder(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			Sleep(100);
			Common::record2File(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			Common::record2Stdout(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			pTrader->sendOrder(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			Sleep(600);
			Common::record2File(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			Common::record2Stdout(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			i++;
		}
		else 
		{
			std::cout << "没有找到合约行情，请按ESC退出" << std::endl;
		}
	}

	//tmpVInstrIdSize3 = tmpVInstrIds3.size();
	//while (!isLimitPrice)
	//{
	//	for (int i=0; i<tmpVInstrIdSize3; i++)
	//	{
	//		//昨结算价
	//		double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
	//		if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
	//		{
	//			tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
	//			tmpVInstrIdSize3--;
	//		}
	//	}
	//	if (tmpVInstrIdSize3 == 0)
	//	{
	//		std::cout << "已经还原" << std::endl;
	//		isLimitPrice = true;
	//	}
	//}
}

///随机下单
void Cases::Case6()
{
	string instrumentIds = "";
	double change = 0.0; //涨跌幅
	int maxVolume = 0;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //重置价格时需要这些合约
	//int tmpVInstrIdSize3;
	bool isLimitPrice = false;

	vector <string>().swap(tmpVInstrIds);
	std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
	std::cin >> instrumentIds;
	std::cout << "请输入涨跌幅（格式0.00）%：" << std::endl;
	std::cin >> change;
	change = change / 100;

	std::cout << "请输入每笔报单的最大手数：" << std::endl;
	std::cin >> maxVolume;

	StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
	vInstrSize = vInstrIds.size();
	for (int j=0; j< vInstrSize; j++)
	{
		Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
	}
	Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //重置价格时需要这些合约
	tmpVInstrIdSize = tmpVInstrIds.size();

	vector<PriceData *> data;
	for (int i=0; i<tmpVInstrIdSize; i++)
	{
		double curPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
		data.push_back(getPriceData(tmpVInstrIds[i].c_str(), curPrice, change));
	}

	std::cout << "--->>" << "初始化数据结束，请按住空格键持续报单，ESC键退出" << std::endl;

	int dataSize = data.size();
	int i = 0;
	while (getch()!=0x1b)
	{
		if (dataSize != 0)
		{
			if (dataSize == i)
				i = 0;
			double bidOrAskPrice = 0.0;
			srand((int)time(0));  //设置随机种子
			if (Random(2) == 1)
			{
				bidOrAskPrice = data[i]->CurPrice + ((Random(20) + 1) * data[i]->PriceTick);	
			}
			else
			{
				bidOrAskPrice = data[i]->CurPrice - ((Random(20) + 1) * data[i]->PriceTick);	
			}
			
			//std::cout << "bidOrAskPrice--->>" << bidOrAskPrice << std::endl;
			int volume = Random(maxVolume) +1;
			if (bidOrAskPrice < data[i]->LowestPrice || bidOrAskPrice > data[i]->HighestPrice)
			{
				continue;
			}
			pTrader->sendOrder(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			Sleep(100);
			Common::record2File(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			Common::record2Stdout(data[i]->InstrumentId, 0, 0, volume, bidOrAskPrice);
			pTrader->sendOrder(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			Sleep(100);
			Common::record2File(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			Common::record2Stdout(data[i]->InstrumentId, 1, 0, volume, bidOrAskPrice);
			i++;
		}
		else 
		{
			std::cout << "没找到合约行情，请按ESC退出" << std::endl;
		}
	}

	//tmpVInstrIdSize3 = tmpVInstrIds3.size();
	//while (!isLimitPrice)
	//{
	//	for (int i=0; i<tmpVInstrIdSize3; i++)
	//	{
	//		//昨结算价
	//		double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
	//		if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
	//		{
	//			tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
	//			tmpVInstrIdSize3--;
	//		}
	//	}
	//	if (tmpVInstrIdSize3 == 0)
	//	{
	//		std::cout << "已经还原" << std::endl;
	//		isLimitPrice = true;
	//	}
	//}
}


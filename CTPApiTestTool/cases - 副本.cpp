#pragma once

#include "cases.h"


Cases::Cases()
{
	pTrader = new Trader();

	getCfg = CfgUtil::getInstance(cfgFilePath);

	string strumentIds = getCfg->getPara("InstrumentIds");
	StringUtil::stringToVector(strumentIds, ",", vInstrIds);
	vInstrSize = vInstrIds.size();
	instrumentId = getCfg->getPara("InstrumentId");

	//std::cerr << "InstrumentIds: " << strumentIds << std::endl;
}


void Cases::run()
{
	//double lower = pTrader->getLowerLimitPrice("ag1511");
	//double upper = pTrader->getUpperLimitPrice("ag1509");

	//pTrader->sendOrder("ag1509", 0, 0, 2, 3800);

	//_record2File("ag1509", 0, 0, 2, 3800);
	//_record2Stdout("ag1509", 0, 0, 2, 3800);

	//std::cerr << "" << lower << " " << upper << std::endl;

	//std::cerr << pTrader->getTickPrice("ag1509") << std::endl;
	//std::cerr << AllInstrumentId.size() << std::endl;

	Sleep(1000);

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
	std::cout << "5、	某价格上大量下单" << std::endl;
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

void Cases::Case1()
{
	int c;
	bool loop = true;
	string instrumentIds;
	vector<string> temVInstrIds;	
	vector<string> vTemp(AllInstrumentId.begin(), AllInstrumentId.end()); //报单时使用
	int temVInstrIdSize;
	bool isLimitPrice = false;  //到涨跌停板时需要停止报单
	while (loop)
	{
		std::cout << "------case1------" << std::endl; 
		std::cout << "1. 涨停" << std::endl; 
		std::cout << "2. 跌停" << std::endl;
		std::cout << "3. 开始报单且有成交" << std::endl;
		std::cout << "4. 开始报单且无成交" << std::endl;
		std::cout << "-1. 返回上一级" << std::endl; 

		std::cin >> c;
		std::cout << c << std::endl;
		switch (c)
		{
		case -1:
			loop = false;
			break;
		case 1:
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, temVInstrIds);
			}

			temVInstrIdSize = temVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<temVInstrIdSize; i++)
				{
					double upperLimitPrice = pTrader->getUpperLimitPrice(temVInstrIds[i].c_str());
					if (makeLimitPrice(temVInstrIds[i].c_str(), upperLimitPrice))
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

		case 2:
			std::cout << "请输入合约/品种名称，多个以逗号隔开：" << std::endl;
			std::cin >> instrumentIds;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, temVInstrIds);
			}

			temVInstrIdSize = temVInstrIds.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<temVInstrIdSize; i++)
				{
					//跌停价
					double lowerLimitPrice = pTrader->getLowerLimitPrice(temVInstrIds[i].c_str());
					pTrader->sendOrder(temVInstrIds[i].c_str(), 0, 0, 1, lowerLimitPrice);
					Sleep(100);
					Common::record2File(temVInstrIds[i].c_str(), 0, 0, 1, lowerLimitPrice);
					Common::record2Stdout(temVInstrIds[i].c_str(), 0, 0, 1, lowerLimitPrice);
					pTrader->sendOrder(temVInstrIds[i].c_str(), 1, 0, 1, lowerLimitPrice);
					Sleep(100);
					Common::record2File(temVInstrIds[i].c_str(), 1, 0, 1, lowerLimitPrice);
					Common::record2Stdout(temVInstrIds[i].c_str(), 1, 0, 1, lowerLimitPrice);
					if (temVInstrIdSize > 0)
					{
						if (lowerLimitPrice == pTrader->getLastPrice(temVInstrIds[i].c_str()))
						{
							temVInstrIds.erase(temVInstrIds.begin() + i);
							temVInstrIdSize--;
						}
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
				int index = Random(vTemp.size());
				double lastPrice = pTrader->getLastPrice(vTemp[index].c_str());
				pTrader->sendOrder(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Sleep(100);
				Common::record2File(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Common::record2Stdout(vTemp[index].c_str(), 0, 0, 1, lastPrice);
				Sleep(100);
			}
			break;

		default:
			break;
		}
	}
}



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
	std::cout << "------���򻯽���������Թ���------" << std::endl;
	std::cout << "1��	��ͣ/��ͣ" << std::endl;
	std::cout << "2��	��ͣ-��ͣ/��ͣ-��ͣ" << std::endl;
	std::cout << "3��	��������/�µ�" << std::endl;
	std::cout << "4��	˲�Ǻ�˲��/˲����˲��" << std::endl;
	std::cout << "5��	��ĳ�۸������ϴ����µ�" << std::endl;
	std::cout << "6��	����µ�" << std::endl;
	std::cout << "-1��	�˳�����" << std::endl;
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

	if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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

	if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
	{
		price = currentPrice + changePrice;
	}
	else if ( fmod(changePrice, priceTick) != 0)
	{
		price = currentPrice + (changePrice - fmod(changePrice, priceTick));  //fmodȡ��
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

///��ͣ/��ͣ
void Cases::Case1()
{
	int c;
	bool loop = true;
	string instrumentIds;
	vector<string> temVInstrIds; //for UpperLimitPrice/LowerLimitPrice
	vector<string> tempVInstrIds5; //for 5
	int tempVInstrIds5Size=0;
	vector<string> vTemp(AllInstrumentId.begin(), AllInstrumentId.end()); //����ʱʹ��
	int temVInstrIdSize=0;
	while (loop)
	{		

		bool isLimitPrice = false;  //���ǵ�ͣ��ʱ��Ҫֹͣ����
		std::cout << "------case1------" << std::endl; 
		std::cout << "1. ��ͣ" << std::endl; 
		std::cout << "2. ��ͣ" << std::endl;
		std::cout << "3. ��ʼ�������гɽ�" << std::endl;
		std::cout << "4. ��ʼ�������޳ɽ�" << std::endl;
		std::cout << "5. ���ú�Լ�۸�(�����¼ۻָ���������)" << std::endl;
		std::cout << "-1. ������һ��" << std::endl; 

		std::cin >> c;
		switch (c)
		{
		case -1:
			loop = false;
			break;
		case 1:
			vector <string>().swap(temVInstrIds); //��Ҫ�����������ۻ�
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
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
					std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 2:		//��ͣ
			vector <string>().swap(temVInstrIds); //��Ҫ�����������ۻ�
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
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
					//��ͣ��
					double lowerLimitPrice = pTrader->getLowerLimitPrice(temVInstrIds[i].c_str());
					if (makeLimitPrice(temVInstrIds[i].c_str(), lowerLimitPrice) || lowerLimitPrice == 0.0)
					{
						temVInstrIds.erase(temVInstrIds.begin() + i);
						temVInstrIdSize--;
					}
				}
				if (temVInstrIdSize == 0)
				{
					std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 3:
			std::cout << "��ס�ո��������������ESC�˳�" << std::endl;
			while(getch()!=0x1b)   //esc
			{
				///��������
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

			std::cout << "��ס�ո��������������ESC�˳�" << std::endl;
			while(getch()!=0x1b)   //esc
			{
				///������
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
					//���̼�
					double preSettlementPrice = pTrader->getPreSettlementPrice(tempVInstrIds5[i].c_str());
					if (makeLimitPrice(tempVInstrIds5[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tempVInstrIds5.erase(tempVInstrIds5.begin() + i);
						tempVInstrIds5Size--;
					}
				}
				if (tempVInstrIds5Size == 0)
				{
					std::cout << "�Ѿ���ԭ" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		default:
			break;
		}
	}
}

///��һ��ͣ�嵽��һ��ͣ��
void Cases::Case2()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	int waiteTime = 0;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> _tmpVInstrIds; //��һ��ͣ��۵���һ��ͣ���ʹ�� tmpVInstrIds�ĸ���
	int _tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 2----" << std::endl;
		std::cout << "1. ��ͣ����ͣ(����ͣ��Ȼ����һ��ʱ����ͣ)" << std::endl;
		std::cout << "2. ��ͣ����ͣ(�ȵ�ͣ��Ȼ����һ��ʱ�����ͣ)" << std::endl;
		std::cout << "3. ���ú�Լ�۸�(�����¼ۻָ������̼�)" << std::endl;
		std::cout << "-1. ������һ��" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:  ///1. ��ͣ����ͣ(����ͣ��Ȼ����һ��ʱ����ͣ)
			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������һ��ͣ��۵���һ��ͣ��۵�ʱ��������λ�룩��" << std::endl;
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
					std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
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
						std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
						isLimitPrice = true;
					}
				}
			}
			break;
		case 2:		///2. ��ͣ����ͣ(�ȵ�ͣ��Ȼ����һ��ʱ�����ͣ)
			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������һ��ͣ��۵���һ��ͣ��۵�ʱ��������λ�룩��" << std::endl;
			std::cin >> waiteTime;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
			Common::copyVector(tmpVInstrIds, _tmpVInstrIds);  //��ͣ����Щ��Լ
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
					std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
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
						std::cout << "�Ѿ�ȫ����ͣ" << std::endl;
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
					//���̼�
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "�Ѿ���ԭ" << std::endl;
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

///������ͣ/��ͣ
void Cases::Case3()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	double change = 0.0; //�Ƿ�
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 3----" << std::endl;
		std::cout << "1. ��������" << std::endl;
		std::cout << "2. �����µ�" << std::endl;
		std::cout << "3. ���ú�Լ�۸�(�����¼ۻָ���������)" << std::endl;
		std::cout << "-1. ������һ��" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:

			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
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
						if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
							Sleep(500); //�ӳٿ�Ч��
						}
						Sleep(500);  //�ӳٿ�Ч��
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "�Ѿ���ͣ" << std::endl;
					isLimitPrice = true;
				}
			}
			break;

		case 2:   //�����µ�

			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
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
						if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
							Sleep(500); //�ӳٿ�Ч��
						}
						Sleep(500);  //�ӳٿ�Ч��
					}
				}
				if (tmpVInstrIdSize == 0)
				{
					std::cout << "�Ѿ���ͣ" << std::endl;
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
					//������
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "�Ѿ���ԭ" << std::endl;
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

///˲�Ǻ�˲����˲����˲��
void Cases::Case4()
{
	int c;
	bool loop = true;
	string instrumentIds = "";
	double change = 0.0; //�ǵ���
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //for choose 3
	int tmpVInstrIdSize3;
	while (loop)
	{
		bool isLimitPrice = false;
		std::cout << "----case 4----" << std::endl;
		std::cout << "1. ˲�Ǻ�˲��" << std::endl;
		std::cout << "2. ˲����˲��" << std::endl;
		std::cout << "3. ���ú�Լ�۸�(�����¼ۻָ���������)" << std::endl;
		std::cout << "-1. ������һ��" << std::endl;
		std::cin>>c;
		switch (c)
		{
		case 1:

			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
			tmpVInstrIdSize = tmpVInstrIds.size();

			for (int i=0; i<tmpVInstrIdSize; i++)
			{
				double currentPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
				double lowerPrice = LowerLimitPrice;
				double upperPrice = UpperLimitPrice;
				double priceTick = PriceTick;
				double changePrice = currentPrice * change;
				double price = 0.0;
				//��
				if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
					Sleep(1000); //�ӳ�
				}
				//��
				if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
					Sleep(1000); //�ӳ�
				}
			}
			break;

		case 2: //˲����˲��

			vector <string>().swap(tmpVInstrIds);
			std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
			std::cin >> instrumentIds;
			std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
			std::cin >> change;
			change = change / 100;
			StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
			vInstrSize = vInstrIds.size();
			for (int j=0; j< vInstrSize; j++)
			{
				Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
			}
			Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
			tmpVInstrIdSize = tmpVInstrIds.size();

			for (int i=0; i<tmpVInstrIdSize; i++)
			{
				double currentPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
				double upperPrice = UpperLimitPrice;
				double lowerPrice = LowerLimitPrice;
				double priceTick = PriceTick;
				double changePrice = currentPrice * change;
				double price = 0.0;
				//��
				if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
					Sleep(1000); //�ӳ�
				}
				//��
				if (fmod(changePrice, priceTick) == 0)  //��С�䶯�۵�������
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
					Sleep(1000); //�ӳ�
				}
			}
			break;

		case 3:   ///���ú�Լ�۸�

			tmpVInstrIdSize3 = tmpVInstrIds3.size();
			while (!isLimitPrice)
			{
				for (int i=0; i<tmpVInstrIdSize3; i++)
				{
					//������
					double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
					if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
					{
						tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
						tmpVInstrIdSize3--;
					}
				}
				if (tmpVInstrIdSize3 == 0)
				{
					std::cout << "�Ѿ���ԭ" << std::endl;
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

///�ڼ۸������ϴ����µ�
void Cases::Case5()
{
	int c;
	string instrumentIds = "";
	double change = 0.0; //�ǵ���
	int volume = 1;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //���ü۸�ʱ��Ҫ��Щ��Լ
	int tmpVInstrIdSize3;
	bool isLimitPrice = false;

	vector <string>().swap(tmpVInstrIds);
	std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
	std::cin >> instrumentIds;
	std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
	std::cin >> change;
	change = change / 100;

	std::cout << "������ÿ�ʱ�����������" << std::endl;
	std::cin >> volume;

	StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
	vInstrSize = vInstrIds.size();
	for (int j=0; j< vInstrSize; j++)
	{
		Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
	}
	Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
	tmpVInstrIdSize = tmpVInstrIds.size();

	vector<PriceData *> data;
	for (int i=0; i<tmpVInstrIdSize; i++)
	{
		double curPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
		data.push_back(getPriceData(tmpVInstrIds[i].c_str(), curPrice, change));
	}

	std::cout << "--->>" << "��ʼ�����ݽ������밴ס�ո������������ESC���˳�" << std::endl;
	int dataSize = data.size();
	int i = 0;
	while (getch()!=0x1b)
	{
		if (dataSize != 0)
		{
			if (i == dataSize)
				i=0;		
			double bidOrAskPrice = 0.0;	
			srand((int)time(0));  //�����������
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
			std::cout << "û���ҵ���Լ���飬�밴ESC�˳�" << std::endl;
		}
	}

	//tmpVInstrIdSize3 = tmpVInstrIds3.size();
	//while (!isLimitPrice)
	//{
	//	for (int i=0; i<tmpVInstrIdSize3; i++)
	//	{
	//		//������
	//		double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
	//		if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
	//		{
	//			tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
	//			tmpVInstrIdSize3--;
	//		}
	//	}
	//	if (tmpVInstrIdSize3 == 0)
	//	{
	//		std::cout << "�Ѿ���ԭ" << std::endl;
	//		isLimitPrice = true;
	//	}
	//}
}

///����µ�
void Cases::Case6()
{
	string instrumentIds = "";
	double change = 0.0; //�ǵ���
	int maxVolume = 0;
	vector<string> tmpVInstrIds;
	int tmpVInstrIdSize;
	vector<string> tmpVInstrIds3; //���ü۸�ʱ��Ҫ��Щ��Լ
	//int tmpVInstrIdSize3;
	bool isLimitPrice = false;

	vector <string>().swap(tmpVInstrIds);
	std::cout << "�������Լ/Ʒ�����ƣ�����Զ��Ÿ�����" << std::endl;
	std::cin >> instrumentIds;
	std::cout << "�������ǵ�������ʽ0.00��%��" << std::endl;
	std::cin >> change;
	change = change / 100;

	std::cout << "������ÿ�ʱ��������������" << std::endl;
	std::cin >> maxVolume;

	StringUtil::stringToVector(instrumentIds, ",", vInstrIds);
	vInstrSize = vInstrIds.size();
	for (int j=0; j< vInstrSize; j++)
	{
		Common::findInstruments(vInstrIds[j], AllInstrumentId, tmpVInstrIds);
	}
	Common::copyVector(tmpVInstrIds, tmpVInstrIds3);  //���ü۸�ʱ��Ҫ��Щ��Լ
	tmpVInstrIdSize = tmpVInstrIds.size();

	vector<PriceData *> data;
	for (int i=0; i<tmpVInstrIdSize; i++)
	{
		double curPrice = pTrader->getLastPrice(tmpVInstrIds[i].c_str());
		data.push_back(getPriceData(tmpVInstrIds[i].c_str(), curPrice, change));
	}

	std::cout << "--->>" << "��ʼ�����ݽ������밴ס�ո������������ESC���˳�" << std::endl;

	int dataSize = data.size();
	int i = 0;
	while (getch()!=0x1b)
	{
		if (dataSize != 0)
		{
			if (dataSize == i)
				i = 0;
			double bidOrAskPrice = 0.0;
			srand((int)time(0));  //�����������
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
			std::cout << "û�ҵ���Լ���飬�밴ESC�˳�" << std::endl;
		}
	}

	//tmpVInstrIdSize3 = tmpVInstrIds3.size();
	//while (!isLimitPrice)
	//{
	//	for (int i=0; i<tmpVInstrIdSize3; i++)
	//	{
	//		//������
	//		double preSettlementPrice = pTrader->getPreSettlementPrice(tmpVInstrIds3[i].c_str());
	//		if (makeLimitPrice(tmpVInstrIds3[i].c_str(), preSettlementPrice) || preSettlementPrice == 0.0)
	//		{
	//			tmpVInstrIds3.erase(tmpVInstrIds3.begin() + i);
	//			tmpVInstrIdSize3--;
	//		}
	//	}
	//	if (tmpVInstrIdSize3 == 0)
	//	{
	//		std::cout << "�Ѿ���ԭ" << std::endl;
	//		isLimitPrice = true;
	//	}
	//}
}


#pragma once
#include "../common/common.h"

//将内容写入到文本文件中
///buySell 买卖方向 0.买  1.卖
///openClose 开平标志  0.开仓   1.平仓  3.平今
void Common::record2File(const char *instrumentId, int buySell, int openClose, int volume, double price)
{
	std::ofstream o_file(TradingLogFilePath,std::ios::app);
	o_file << TimeUtil::getTimeNow4Log() << "_" << instrumentId << "_" << (buySell == 0?"买":"卖")
		<< "_" << (openClose == 0?"开仓":"平仓") << "_" << volume << "_" << price << std::endl; 
	o_file.close();						//关闭文件
}

void Common::record2File(const char *msg)
{
	std::ofstream o_file(TradingLogFilePath,std::ios::app);
	o_file << "--------------------------" << msg <<"--------------------------"<< std::endl; 
	o_file.close();				//关闭文件
}

void Common::log2File(const char* msg)
{
	std::ofstream o_file("./log.txt",std::ios::app);
	o_file << "--------------------------" << msg <<"--------------------------"<< std::endl; 
	o_file.close();				//关闭文件
}

//将内容显示在cmd上
void Common::record2Stdout(const char *instrumentId, int buySell, int openClose, int volume, double price)
{
	std::cerr << TimeUtil::getTimeNow4Log() << "_" << instrumentId << "_" << (buySell == 0?"买":"卖")
		<< "_" << (openClose == 0?"开仓":"平仓") << "_" << volume << "_" << price << std::endl; 
}

///根据pattern从一个集合里面找出需要的合约，或者产品对应的合约
void Common::findInstruments(string pattern, set<string> source, vector<string>& v)
{
	set<string>::iterator its;

	for (its=source.begin(); its != source.end(); its++)
	{
		///产品
		if (pattern.size()>0 && pattern.size() <=2 )
		{
			string product = StringUtil::delNum(*its);
			if (product == pattern)
			{
				v.push_back(*its);
			}
		}
		else if (pattern.size() > 2)
		{
			if ((*its) == pattern)
				v.push_back(*its);
		}
	}

}

///两个vector的深复制
void Common::copyVector(vector<string> source, vector<string>& destination)
{
	vector<string>().swap(destination);
	vector<string>::iterator it;
	for (it = source.begin(); it != source.end(); it++)
	{
		destination.push_back(*it);
	}

}

void Common::erasefiles()
{
	system("del .\\DialogRsp.con");
	system("del .\\Private.con");
	system("del .\\Public.con");

	system("del .\\QueryRsp.con");
	system("del .\\TradingDay.con");
}
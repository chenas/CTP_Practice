#pragma once
#include "../common/common.h"

//������д�뵽�ı��ļ���
///buySell �������� 0.��  1.��
///openClose ��ƽ��־  0.����   1.ƽ��  3.ƽ��
void Common::record2File(const char *instrumentId, int buySell, int openClose, int volume, double price)
{
	std::ofstream o_file(TradingLogFilePath,std::ios::app);
	o_file << TimeUtil::getTimeNow4Log() << "_" << instrumentId << "_" << (buySell == 0?"��":"��")
		<< "_" << (openClose == 0?"����":"ƽ��") << "_" << volume << "_" << price << std::endl; 
	o_file.close();						//�ر��ļ�
}

void Common::record2File(const char *msg)
{
	std::ofstream o_file(TradingLogFilePath,std::ios::app);
	o_file << "--------------------------" << msg <<"--------------------------"<< std::endl; 
	o_file.close();				//�ر��ļ�
}

void Common::log2File(const char* msg)
{
	std::ofstream o_file("./log.txt",std::ios::app);
	o_file << "--------------------------" << msg <<"--------------------------"<< std::endl; 
	o_file.close();				//�ر��ļ�
}

//��������ʾ��cmd��
void Common::record2Stdout(const char *instrumentId, int buySell, int openClose, int volume, double price)
{
	std::cerr << TimeUtil::getTimeNow4Log() << "_" << instrumentId << "_" << (buySell == 0?"��":"��")
		<< "_" << (openClose == 0?"����":"ƽ��") << "_" << volume << "_" << price << std::endl; 
}

///����pattern��һ�����������ҳ���Ҫ�ĺ�Լ�����߲�Ʒ��Ӧ�ĺ�Լ
void Common::findInstruments(string pattern, set<string> source, vector<string>& v)
{
	set<string>::iterator its;

	for (its=source.begin(); its != source.end(); its++)
	{
		///��Ʒ
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

///����vector�����
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
#pragma once  
#include "../apiImpl/mdspimpl.h"

bool MdRspImpl::openFile()
{
	mdlogstream.open("../cfg/marketdata.txt",std::ios_base::app);
	return true;
}

bool MdRspImpl::closeFile()
{
	mdlogstream.close();
	return true;
}

void MdRspImpl::writeSeparator(const char* msg)
{	
	if (isPrintPrice)
	{
		mdlogstream << "--------------------------" << msg << "--------------------------" << std::endl;
	}
}

MdRspImpl::MdRspImpl():isPrintPrice(0)
{
	cfgUtil = CfgUtil::getInstance("../cfg/maker_rsh.cfg");
	isPrintPrice = cfgUtil->getParaInt("IsPrintPrice");
}

//连接前置
void MdRspImpl ::OnFrontConnected()
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	SetEvent(g_hEvent);
}

void MdRspImpl::OnFrontDisconnected(int nReason)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
}

//非法请求响应
void MdRspImpl::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	std::cerr << "非法请求 reason："<< pRspInfo->ErrorMsg << std::endl;
	if (bIsLast)
	{
		SetEvent(g_hEvent);
	}
}

//登录请求回调函数
void MdRspImpl::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	if (bIsLast)
	{
		SetEvent(g_hEvent);
	}
}

//行情订阅失败响应
void MdRspImpl::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	if (pRspInfo->ErrorID != 0)
	{
		std::cout << pRspInfo->ErrorMsg << std::endl;
	}
	if (bIsLast)
	{
		SetEvent(g_hEvent);
	}
}

void MdRspImpl::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	if (pRspInfo->ErrorID != 0)
	{
		std::cout << pRspInfo->ErrorMsg << std::endl;
	}
	if (bIsLast)
	{
		SetEvent(g_hEvent);
	}
}

//接收深度行情
void MdRspImpl::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) 
{
	//std::cerr << "--->>> " << __FUNCTION__ << std::endl;
	//std::cout << "InstrumentID" << pDepthMarketData->InstrumentID << " LastPrice: " << pDepthMarketData->LastPrice << " UpdateTime: " << pDepthMarketData->UpdateTime << std::endl;
	if (isPrintPrice)
	{
		mdlogstream << "InstrumentID: " << pDepthMarketData->InstrumentID << " LastPrice: " << pDepthMarketData->LastPrice << " UpdateTime: " << pDepthMarketData->UpdateTime << std::endl;
	}	
}




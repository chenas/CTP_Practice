#include "stdafx.h"
#include "TradeCTP.h"
#include <iostream>
using namespace std;

#include ".\API_CTP\ThostFtdcTraderApi.h"
#pragma warning(disable : 4996)
// USER_API参数
extern CThostFtdcTraderApi* g_pTradeApi;
extern CQuoteCTP*           g_pQuoteCTP;

// 配置参数
extern char FRONT_ADDR[];		// 前置地址
extern char BROKER_ID[];		// 经纪公司代码
extern char INVESTOR_ID[];		// 投资者代码
extern char PASSWORD[];			// 用户密码
extern TThostFtdcPriceType	LIMIT_PRICE;	// 价格
extern TThostFtdcDirectionType	DIRECTION;	// 买卖方向

// 请求编号
extern int g_iRequestID;
extern TThostFtdcFrontIDType	FRONT_ID;	//前置编号
extern TThostFtdcSessionIDType	SESSION_ID;	//会话编号
extern TThostFtdcOrderRefType	ORDER_REF;	//报单引用

extern map_str_pstrLocalID g_m_Local_Ref;
extern map_string_Trade       g_m_Trade;
extern map_string_Order       g_m_Order;
extern char                   g_cKey[1024];
extern map_str_InvestorPosition   g_m_InvestorPosition;
extern int                    g_iReturn;
extern map_str_InstrumentInfo     g_m_InstrumentInfo;
extern map_str_MarginRate         g_m_MarginRate;
extern map_str_CommissionRate     g_m_CommissionRate;
extern map_srt_DepthMarketData    g_m_DepthMarketData;
extern map_str_str                g_m_Instrument_ProductID;
extern vector_Trade               g_v_Trade;
extern CThostFtdcTradingAccountField   g_TradingAccount;

///在平仓时，查询平仓单位成交的部分
int GetFrozenNumOnClose(TThostFtdcInstrumentIDType	InstrumentID, TThostFtdcPosiDirectionType PosiDirection)
{

	int cDirection;
	if(PosiDirection==THOST_FTDC_PD_Long)
	{///平仓对象的仓位是多头时，必是卖出
		cDirection=THOST_FTDC_D_Sell;
	}
	else
	{
		cDirection=THOST_FTDC_D_Buy;
	}
	///冻结数量
	int iFrozenNum=0;

	///未成交的数量
	int iLeft=0;

	map_string_Order::iterator iter;
	for(iter=g_m_Order.begin();iter!=g_m_Order.end();iter++)
	{
		if(!(iter->second.OrderStatus == THOST_FTDC_OST_AllTraded || iter->second.OrderStatus ==THOST_FTDC_OST_Canceled) &&  iter->second.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
		{///未成交和为撤单的平仓

			if(strcmp(iter->second.InstrumentID,InstrumentID)==0)
			{///合约

				if(iter->second.Direction==cDirection)
				{///对应的买卖方向

					iLeft = iter->second.VolumeTotal-iter->second.VolumeTraded;
					iFrozenNum+=iLeft;
				}
			}
		}
	}
	return iFrozenNum;
}

///在平仓时通过FIFO得到均价
double GetAveragePriceOnClose(CThostFtdcInvestorPositionField In_Position,int iVolume)
{
	double db_Price=0;
	int iPostion=In_Position.Position-iVolume;
	int iOffSet=0;
	if(iPostion<1)
	{
		return db_Price;
	}
	int nVolume=0;
	TThostFtdcDirectionType  Direction;
	if(In_Position.PosiDirection==THOST_FTDC_PD_Long)
	{
		Direction=THOST_FTDC_D_Buy;
	}
	else
	{
		Direction=THOST_FTDC_D_Sell;
	}
	db_Price=In_Position.PreSettlementPrice;
	double db_Total=0;
	vector_Trade::reverse_iterator  iter=g_v_Trade.rbegin();
	for(iter=g_v_Trade.rbegin();iter!=g_v_Trade.rend();iter++)
	{
		if(strcmp(iter->InstrumentID,In_Position.InstrumentID)==0)
		{///合约匹配
			if(iter->OffsetFlag==THOST_FTDC_OF_Open)
			{///开仓
				if(iter->Direction==Direction)
				{///方向

					db_Total+=iter->Volume*iter->Price;
					nVolume+=iter->Volume;
					if(nVolume==iPostion)
					{
						db_Price=db_Total/iPostion;
						break;
					}
					if(nVolume>iPostion)
					{
						iOffSet=nVolume-iPostion;
						db_Total-=iOffSet*iter->Price;
						db_Price=db_Total/iPostion;
						break;
					}
				}
			}
		}
	}
	return db_Price;
}
///得到费率
CThostFtdcInstrumentCommissionRateField * GetCommissionRate(char * pInstrument)
{
	string strInstrument=pInstrument;
	string strProductId;
	map_str_str::iterator iter=g_m_Instrument_ProductID.find(strInstrument);
	if(iter!=g_m_Instrument_ProductID.end())
	{
		strProductId = iter->second;
		map_str_CommissionRate::iterator iter_CommissionRate=g_m_CommissionRate.find(strProductId);
		if(iter_CommissionRate!=g_m_CommissionRate.end())
		{
			return &iter_CommissionRate->second;
		}
	}
	return NULL;
}

///写费率
void WriteInstrumentInfo2File()
{
	char pPath[256];
	_getcwd( pPath, 256 );
	string strPath=pPath;
	strPath+="\\InstrumentInfo.csv";
	char pContent[1024];
	map_str_InstrumentInfo::iterator iter;
	FILE *      pFile=fopen(strPath.c_str(),"w+");
	if(NULL!=pFile)
	{
		fseek( pFile, 0, SEEK_SET );
		for(iter = g_m_InstrumentInfo.begin();iter!=g_m_InstrumentInfo.end();iter++)
		{
			memset(pContent,0,sizeof(pContent));
			sprintf(pContent,"%s,%s,%s,%s,%s,%c,%d,%d,%d,%d,%d,%d,%d,%f,%s,%s,%s,%s,%s,%c,%d,%c,%c,%f,%f,%c\n",
				iter->second.InstrumentID,
				iter->second.ExchangeID,
				iter->second.InstrumentName,
				iter->second.ExchangeInstID,
				iter->second.ProductID,
				iter->second.ProductClass,
				iter->second.DeliveryYear,
				iter->second.DeliveryMonth,
				iter->second.MaxMarketOrderVolume,
				iter->second.MinMarketOrderVolume,
				iter->second.MaxLimitOrderVolume,
				iter->second.MinLimitOrderVolume,
				iter->second.VolumeMultiple,
				iter->second.PriceTick,
				iter->second.CreateDate,
				iter->second.OpenDate,
				iter->second.ExpireDate,
				iter->second.StartDelivDate,
				iter->second.EndDelivDate,
				iter->second.InstLifePhase,
				iter->second.IsTrading,
				iter->second.PositionType,
				iter->second.PositionDateType,
				iter->second.LongMarginRatio,
				iter->second.ShortMarginRatio,
				iter->second.MaxMarginSideAlgorithm
				);

			fwrite(pContent,strlen(pContent),1,pFile);
		}
		fclose(pFile);
	}

}

///写费率
void WriteCommissionRate2File()
{
	char pPath[256];
	_getcwd( pPath, 256 );
	string strPath=pPath;
	strPath+="\\CommissionRate.csv";
	char pContent[1024];
	map_str_CommissionRate::iterator iter = g_m_CommissionRate.begin();
	FILE *      pFile=fopen(strPath.c_str(),"w+");
	if(NULL!=pFile)
	{
		fseek( pFile, 0, SEEK_SET );
		for(iter = g_m_CommissionRate.begin();iter!=g_m_CommissionRate.end();iter++)
		{
			memset(pContent,0,sizeof(pContent));
			sprintf(pContent,"%s,%c,%s,%s,%f,%f,%f,%f,%f,%f\n",iter->second.InstrumentID,iter->second.InvestorRange,iter->second.BrokerID,iter->second.InvestorID,
				iter->second.OpenRatioByMoney,iter->second.OpenRatioByVolume,
				iter->second.CloseRatioByMoney,iter->second.CloseRatioByVolume,
				iter->second.CloseTodayRatioByMoney,iter->second.CloseTodayRatioByVolume);
			fwrite(pContent,strlen(pContent),1,pFile);
		}
		fclose(pFile);
	}
}
///写保证金率
void WriteMarginRate2File()
{
	char pPath[256];
	_getcwd( pPath, 256 );
	string strPath=pPath;
	strPath+="\\MarginRate.csv";
	char pContent[1024];
	map_str_MarginRate::iterator iter = g_m_MarginRate.begin();
	FILE *      pFile=fopen(strPath.c_str(),"w+");
	if(NULL!=pFile)
	{
		fseek( pFile, 0, SEEK_SET );
		for(iter = g_m_MarginRate.begin();iter!=g_m_MarginRate.end();iter++)
		{
			memset(pContent,0,sizeof(pContent));
			sprintf(pContent,"%s,%c,%s,%s,%c,%f,%f,%f,%f,%d\n",iter->second.InstrumentID,iter->second.InvestorRange,iter->second.BrokerID,iter->second.InvestorID,iter->second.HedgeFlag,
				iter->second.LongMarginRatioByMoney,iter->second.LongMarginRatioByVolume,
				iter->second.ShortMarginRatioByMoney,iter->second.ShortMarginRatioByVolume,
				iter->second.IsRelative);
			fwrite(pContent,strlen(pContent),1,pFile);
		}
		fclose(pFile);
	}
}

DWORD WINAPI Thread_QryRate(LPVOID pParam)
{
	string strKey;
	map_str_InstrumentInfo::iterator iter;// = g_m_InstrumentInfo.find(strKey);
	map_str_str tmp_m_str_str;
	map_str_str::iterator iter_Type;
	int iSize = g_m_InstrumentInfo.size();
	int iCnt=0;
	int nCnt=0;
	int iSleepMic_Secend=1000/4;
	int iResult;
	for(iter = g_m_InstrumentInfo.begin();iter != g_m_InstrumentInfo.end();iter++)
	{
		strKey=iter->second.ProductID;
		iter_Type=tmp_m_str_str.find(strKey);
		if(iter_Type==tmp_m_str_str.end())
		{
			if(1==iter->second.IsTrading)
			{
				tmp_m_str_str.insert(make_pair(strKey,strKey));
				
				///取手续费率
				CThostFtdcQryInstrumentCommissionRateField QryInstrumentCommissionRate;
				memset(&QryInstrumentCommissionRate,0,sizeof(QryInstrumentCommissionRate));
				strcpy(QryInstrumentCommissionRate.BrokerID, BROKER_ID);
				strcpy(QryInstrumentCommissionRate.InvestorID, INVESTOR_ID);
				strcpy(QryInstrumentCommissionRate.InstrumentID,iter->second.InstrumentID);
				Sleep(iSleepMic_Secend);
				iResult = g_pTradeApi->ReqQryInstrumentCommissionRate(&QryInstrumentCommissionRate,++g_iRequestID);
				///continue;
				
				///取保证金率
				CThostFtdcQryInstrumentMarginRateField QryInstrumentMarginRate;
				memset(&QryInstrumentMarginRate,0,sizeof(QryInstrumentMarginRate));
				strcpy(QryInstrumentMarginRate.BrokerID, BROKER_ID);
				strcpy(QryInstrumentMarginRate.InvestorID, INVESTOR_ID);
				strcpy(QryInstrumentMarginRate.InstrumentID,iter->second.InstrumentID);
				///投机
				Sleep(iSleepMic_Secend);
				QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Speculation;
				iResult = g_pTradeApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++g_iRequestID);

				///套利
				Sleep(iSleepMic_Secend);
				QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Arbitrage;
				iResult = g_pTradeApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++g_iRequestID);

				///套保
				Sleep(iSleepMic_Secend);
				QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Hedge;
				iResult = g_pTradeApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++g_iRequestID);

				nCnt++;
			}
			else
			{
				int a = 0;
			}
		}
		iCnt++;
	}
	if(iSize==iCnt)
	{
		///存储
		int a = 0;
	}
	return 1;
}

void Show()
{
	///return;
	system("cls"); 
	char pTmp[1024];
	map_str_InvestorPosition::iterator iter;
	int i=1;
	double Average_Prive = 0;
	double PositionProfit=0;

	///费用
	g_TradingAccount.Commission=0;

	///可用资金
	g_TradingAccount.Balance=0;

	///平仓盈亏
	g_TradingAccount.CloseProfit=0;

	///持仓盈亏
	g_TradingAccount.PositionProfit=0;

	///占用保证金
	g_TradingAccount.CurrMargin=0;

	double dB_Balance=0;

	for(iter = g_m_InvestorPosition.begin();iter!=g_m_InvestorPosition.end();iter++)
	{
		printf("*******************持仓*****************************\r\n");
		g_TradingAccount.Commission+=iter->second.Commission;

		g_TradingAccount.CloseProfit+=iter->second.CloseProfit;

		g_TradingAccount.CurrMargin+=iter->second.UseMargin;

		if(iter->second.Position>0)
		{ 
			string strKey = iter->second.InstrumentID;
			map_str_InstrumentInfo::iterator iter_Info = g_m_InstrumentInfo.find(strKey);
			map_srt_DepthMarketData::iterator iter_MD = g_m_DepthMarketData.find(strKey);
			memset(pTmp,0,1024);
			if(iter_Info != g_m_InstrumentInfo.end()  && iter_MD != g_m_DepthMarketData.end())
			{///有合约信息时，可以通过乘数来计算均价
				 Average_Prive = 0;
				 PositionProfit=0;
				 Average_Prive = (iter->second.PositionCost)/(iter->second.Position*iter_Info->second.VolumeMultiple);
				 PositionProfit = (iter_MD->second.LastPrice-Average_Prive)*iter->second.Position*iter_Info->second.VolumeMultiple;
				 ///空仓盈利处理
				 if(iter->second.PosiDirection==THOST_FTDC_PD_Short)
				 {
					 PositionProfit=0-PositionProfit;
				 }
				 g_TradingAccount.PositionProfit+=PositionProfit;

				if(iter->second.PosiDirection==THOST_FTDC_PD_Long)
				{
					sprintf(pTmp,"%02d %s 多 总:%d 昨:%d 今仓:%d 可平:%d 均价:%g 持仓盈亏:%.1f 保证金:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.ShortFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
				else
				{
					sprintf(pTmp,"%02d %s 空 总:%d 昨:%d 今仓:%d 可平:%d 均价:%g 持仓盈亏:%.1f 保证金:%.1f \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.LongFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
			}
			else
			{///无合约信息时，用结算价
				 Average_Prive = (iter->second.PositionCost)/(iter->second.Position*iter_Info->second.VolumeMultiple);;
				 PositionProfit=iter->second.PositionProfit;

				 g_TradingAccount.PositionProfit+=PositionProfit;

				if(iter->second.PosiDirection==THOST_FTDC_PD_Long)
				{	
					
					sprintf(pTmp,"%02d %s 多 总:%d 昨:%d 今仓:%d 可平:%d 均价:%g 持仓盈亏:%.1f 保证金:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.ShortFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
				else
				{
			
					sprintf(pTmp,"%02d %s 空 总:%d 昨:%d 今仓:%d 可平:%d 均价:%g 持仓盈亏:%.1f 保证金:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.LongFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}

			}
			printf(pTmp);
			i++;
		}
	}
	///动态权益
	g_TradingAccount.Balance=g_TradingAccount.PreBalance+g_TradingAccount.CloseProfit+g_TradingAccount.PositionProfit-g_TradingAccount.Commission;
	double db_Per = 0;
	if(g_TradingAccount.Balance>0.1)
	{
		db_Per= 100*g_TradingAccount.CurrMargin/g_TradingAccount.Balance;
	}

	printf("\r\n***************权益与风险***************************");
	memset(pTmp,0,sizeof(pTmp));
	sprintf(pTmp,"\r\n静态权益:%.0f 平仓盈亏:%.0f 持仓盈亏:%.0f 权利金:0 动态权益:%.0f 占用保证金:%.0f 下单冻结:0 冻结权利金:0 可用资金:%.0f 风险度:%.1f ",
		g_TradingAccount.PreBalance,
		g_TradingAccount.CloseProfit,
		g_TradingAccount.PositionProfit,
		g_TradingAccount.Balance,
		g_TradingAccount.CurrMargin,
		g_TradingAccount.Balance-g_TradingAccount.CurrMargin,
		db_Per);
	printf(pTmp);
	printf("\r\n****************************************************");
}

DWORD WINAPI Thread_Show(LPVOID pParam)
{
	while(true)
	{
		Sleep(500);
		Show();
	}
}

DWORD WINAPI Cala_Position_Trade(LPVOID pTrade)
{///因成交而修改持仓
	CThostFtdcTradeField Trade = *(CThostFtdcTradeField*)pTrade;
	char pcTmp[255];
	memset(pcTmp,0,sizeof(pcTmp));
	if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
	{
		if(Trade.Direction==THOST_FTDC_D_Buy)
		{
			///此order为开多仓
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Long);
		}
		else
		{
			///此order为开空仓
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Short);
		}
	}
	else
	{
		if(Trade.Direction==THOST_FTDC_D_Buy)
		{
			///此order为平空仓
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Short);
		}
		else
		{
			///此order为平多仓
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Long);
		}

	}
	///strKey为“IF1505_3”
	string strKey=pcTmp;
	map_str_InvestorPosition::iterator iter = g_m_InvestorPosition.find(strKey);
	strKey = Trade.InstrumentID;
	///strKey为“IF1505”
	map_str_InstrumentInfo::iterator iter_Info = g_m_InstrumentInfo.find(strKey);
	///strKey为“IF1505_1”
	sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,Trade.HedgeFlag);
	strKey = pcTmp;
	map_str_MarginRate::iterator     iter_MarginRate = g_m_MarginRate.find(strKey);
	///strKey为“IF1505”
	strKey = Trade.InstrumentID;
	map_srt_DepthMarketData::iterator iter_MD = g_m_DepthMarketData.find(strKey);

	CThostFtdcInstrumentCommissionRateField *pCommissionRate=GetCommissionRate(Trade.InstrumentID);
	int YdPosition=0;
	double dTotalCost=0;
	///本次费用
	double dCommission=0;


	///得到平仓后的均价
    double db_Average_Now = 0;

	///上次均价
    double db_Average_Last=0;

	///本次平仓盈利
	double db_CloseProfit=0;

	if(iter_Info == g_m_InstrumentInfo.end())
	{  ///没有得到合约信息之前不处理
		return 0;
	}
	if(iter_MarginRate == g_m_MarginRate.end())
	{  ///没有得到保证金率之前不处理
		return 0;
	}
	if(iter==g_m_InvestorPosition.end() )
	{
		///没有该方向的持仓合约，肯定是开仓，不会是开仓以外的形式
		if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
		{
			
			///此时要在本地生成一个持仓
			CThostFtdcInvestorPositionField InvestorPosition;

			memset(&InvestorPosition,0,sizeof(InvestorPosition));

			///合约代码
			strcpy(InvestorPosition.InstrumentID,Trade.InstrumentID);

			///经纪公司代码
			strcpy(InvestorPosition.BrokerID,Trade.BrokerID);

			///投资者代码
			strcpy(InvestorPosition.InvestorID,Trade.InvestorID);

			///持仓多空方向
			if(Trade.Direction == THOST_FTDC_D_Buy)
			{
				///买为多头
				InvestorPosition.PosiDirection=THOST_FTDC_PD_Long;
			}
			else
			{   ///卖为空头
				InvestorPosition.PosiDirection=THOST_FTDC_PD_Short;
			}

			///投机套保标志
			InvestorPosition.HedgeFlag=Trade.HedgeFlag;

			///持仓日期
			InvestorPosition.PositionDate=THOST_FTDC_PSD_Today;

			///上日持仓
			InvestorPosition.YdPosition=0;

			///总仓
			InvestorPosition.Position=Trade.Volume;

			///多头冻结
			InvestorPosition.LongFrozen = 0;

			///空头冻结
			InvestorPosition.ShortFrozen = 0;

			///开仓冻结金额
			InvestorPosition.LongFrozenAmount=0;

			///开仓冻结金额
			InvestorPosition.ShortFrozenAmount=0;

			///开仓量
			InvestorPosition.OpenVolume=Trade.Volume;

			///平仓量
			InvestorPosition.CloseVolume=0;

			///开仓金额
			InvestorPosition.OpenAmount=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///平仓金额
			InvestorPosition.CloseAmount=0;

			///持仓成本
			InvestorPosition.PositionCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;


			///上次占用的保证金
			InvestorPosition.PreMargin=0;

			///占用的保证金
			if(InvestorPosition.PosiDirection==THOST_FTDC_PD_Long)
			{
				if(iter_MarginRate->second.LongMarginRatioByVolume>0)
				{
					InvestorPosition.UseMargin=Trade.Volume*iter_MarginRate->second.LongMarginRatioByVolume;
				}
				else
				{
					InvestorPosition.UseMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_MarginRate->second.LongMarginRatioByMoney;
				}
				
			}
			else
			{
				if(iter_MarginRate->second.ShortMarginRatioByVolume>0)
				{
					InvestorPosition.UseMargin=Trade.Volume*iter_MarginRate->second.ShortMarginRatioByVolume;
				}
				else
				{
					InvestorPosition.UseMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_MarginRate->second.ShortMarginRatioByMoney;
				}
			}

			///冻结的保证金
			InvestorPosition.FrozenMargin=0;

			///冻结的资金
			InvestorPosition.FrozenCash=0;

			///冻结的手续费
			InvestorPosition.FrozenCommission=0;

			///资金差额
			InvestorPosition.CashIn=0;

			///手续费
			InvestorPosition.Commission=0;
			if(NULL!=pCommissionRate)
			{
				if(pCommissionRate->OpenRatioByVolume>0)
				{
					dCommission = pCommissionRate->OpenRatioByVolume*Trade.Volume;
					InvestorPosition.Commission+=dCommission;
				}
				else
				{
					dCommission=pCommissionRate->OpenRatioByVolume*InvestorPosition.OpenAmount;
					InvestorPosition.Commission+=dCommission;
				}
			}

			///把手续费放在占用的保证金
			///InvestorPosition.UseMargin+=dCommission;

			///平仓盈亏
			InvestorPosition.CloseProfit=0;


			///持仓盈亏
			if(iter_MD != g_m_DepthMarketData.end())
			{
				///有行情时，按照行情计算
				InvestorPosition.PositionProfit = Trade.Volume * iter_MD->second.LastPrice*iter_Info->second.VolumeMultiple -InvestorPosition.PositionCost;
			}
			else
			{
				///无行情时，按照结算价计算
				InvestorPosition.PositionProfit = Trade.Volume * Trade.Price *iter_Info->second.VolumeMultiple-InvestorPosition.PositionCost;
			}
			///上次结算价
			InvestorPosition.PreSettlementPrice=Trade.Price;

			///本次结算价
			InvestorPosition.SettlementPrice=Trade.Price;

			///交易日
			strcpy(InvestorPosition.TradingDay,Trade.TradingDay);

			///结算编号
			InvestorPosition.SettlementID=0;

			///开仓成本
			InvestorPosition.OpenCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///交易所保证金
			InvestorPosition.ExchangeMargin=0;
		    if(InvestorPosition.PosiDirection == THOST_FTDC_PD_Long)
			{
				InvestorPosition.ExchangeMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_Info->second.LongMarginRatio;
			}
			else
			{
				InvestorPosition.ExchangeMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_Info->second.ShortMarginRatio;
			}

			///组合成交形成的持仓
			InvestorPosition.CombPosition=0;

			///组合多头冻结
			InvestorPosition.CombLongFrozen=0;

			///组合空头冻结
			InvestorPosition.CombShortFrozen=0;

			///逐日盯市平仓盈亏
			InvestorPosition.CloseProfitByDate = InvestorPosition.CloseProfit;

			///逐笔对冲平仓盈亏
			InvestorPosition.CloseProfitByTrade=0;

			///今日持仓
			InvestorPosition.TodayPosition=Trade.Volume;

			///保证金率
			InvestorPosition.MarginRateByMoney=0;
		   if(InvestorPosition.PosiDirection == THOST_FTDC_PD_Long)
			{
				InvestorPosition.MarginRateByMoney =iter_MarginRate->second.LongMarginRatioByMoney;
				InvestorPosition.MarginRateByVolume=iter_MarginRate->second.LongMarginRatioByVolume;
			}
			else
			{
				InvestorPosition.MarginRateByMoney=iter_MarginRate->second.ShortMarginRatioByMoney;
				InvestorPosition.MarginRateByVolume=iter_MarginRate->second.ShortMarginRatioByVolume;
			}


			///保证金率(按手数)
			InvestorPosition.MarginRateByVolume=0;



			char pcTmp[255];
			string strKey;
			memset(pcTmp,0,sizeof(pcTmp));
			sprintf(pcTmp,"%s_%c",InvestorPosition.InstrumentID,InvestorPosition.PosiDirection);

			strKey = pcTmp;
			///插入本地内存
			g_m_InvestorPosition.insert(make_pair(strKey,InvestorPosition));

		}
		return 1;
	}
	else
	{
		if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
		{///开仓

			///仓位
			iter->second.Position+=Trade.Volume;

			///开仓量
			iter->second.OpenVolume+=Trade.Volume;

			///今仓
			iter->second.TodayPosition+=Trade.Volume;

			///昨仓
			YdPosition = iter->second.Position-iter->second.TodayPosition;
			iter->second.YdPosition=YdPosition;

			///开仓金额
			iter->second.OpenAmount+=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///持仓成本
			iter->second.PositionCost+= Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

  			  ///占用的保证金
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				if(iter_MarginRate->second.LongMarginRatioByVolume>0)
				{
		
					iter->second.UseMargin=iter->second.Position*iter_MarginRate->second.LongMarginRatioByVolume;
				}
				else
				{
					iter->second.UseMargin= iter->second.PositionCost*iter_MarginRate->second.LongMarginRatioByMoney;
				}
				
			}
			else
			{
				if(iter_MarginRate->second.ShortMarginRatioByVolume>0)
				{
					iter->second.UseMargin=iter->second.Position*iter_MarginRate->second.ShortMarginRatioByVolume;
				}
				else
				{
					iter->second.UseMargin= iter->second.PositionCost*iter_MarginRate->second.ShortMarginRatioByMoney;
				}
					
			}

			///手续费
			if(NULL!=pCommissionRate)
			{
				if(pCommissionRate->OpenRatioByVolume>0)
				{
					dCommission=pCommissionRate->OpenRatioByVolume*Trade.Volume;
					iter->second.Commission+=dCommission;
				}
				else
				{
					dTotalCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;
					dCommission=pCommissionRate->OpenRatioByVolume*dTotalCost;
					iter->second.Commission+=dCommission;
				}
			}


	
			///本次结算价(是均价)
			iter->second.SettlementPrice=Trade.Price;

			///持仓盈亏
			db_Average_Now =iter->second.PositionCost/(iter->second.Position*iter_Info->second.VolumeMultiple);
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.PositionProfit=(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			else
			{
				iter->second.PositionProfit=-(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}

			///开仓成本
			iter->second.OpenCost+=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///交易所保证金
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.ExchangeMargin+=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple*iter_Info->second.LongMarginRatio;
			}
			else
			{
				iter->second.ExchangeMargin+=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple*iter_Info->second.ShortMarginRatio;
			}


		}
		else
		{///平仓
			///1、减少仓位
			///2、同时减少可平量
			///3、处理今仓
			///4、昨仓
			///减少仓位

			///得到平仓后的均价
			db_Average_Now = GetAveragePriceOnClose(iter->second,Trade.Volume);

			///上次均价
			db_Average_Last=iter->second.PositionCost/(iter->second.Position*iter_Info->second.VolumeMultiple);

			///
			db_CloseProfit=0;

			iter->second.Position-=Trade.Volume;

			if(iter->second.PosiDirection==THOST_FTDC_PD_Long)
			{
				iter->second.ShortFrozen=GetFrozenNumOnClose(iter->second.InstrumentID,iter->second.PosiDirection);
			}
			else
			{
				iter->second.LongFrozen=GetFrozenNumOnClose(iter->second.InstrumentID,iter->second.PosiDirection);
			}


			///平仓量
			iter->second.CloseVolume+=Trade.Volume;

			///平仓金额
			iter->second.CloseAmount+=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple;
			

			///今仓
			if(Trade.OffsetFlag!=THOST_FTDC_OF_Open)
			{///处理今仓
				if( Trade.Volume < iter->second.YdPosition )
				{

				}
				else
				{
					if(iter->second.TodayPosition>0 )
						iter->second.TodayPosition-=Trade.Volume;
				}
				
			}

			///昨仓
			YdPosition = iter->second.Position-iter->second.TodayPosition;
			iter->second.YdPosition=YdPosition;

			///持仓成本
			iter->second.PositionCost=db_Average_Now*iter->second.Position*iter_Info->second.VolumeMultiple;
			
			///开仓成本
			iter->second.OpenCost -=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple;

			///占用的保证金
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				if(iter_MarginRate->second.LongMarginRatioByVolume>0)
				{
					iter->second.UseMargin=iter->second.Position*iter_MarginRate->second.LongMarginRatioByVolume;
				}
				else
				{
					iter->second.UseMargin=iter->second.PositionCost*iter_MarginRate->second.LongMarginRatioByMoney;
				}
				
			}
			else
			{
				if(iter_MarginRate->second.ShortMarginRatioByVolume>0)
				{
					iter->second.UseMargin=iter->second.Position*iter_MarginRate->second.ShortMarginRatioByVolume;
				}
				else
				{
					iter->second.UseMargin=iter->second.PositionCost*iter_MarginRate->second.ShortMarginRatioByMoney;
				}
					
			}

			///手续费
			if(NULL!=pCommissionRate)
			{///这里一定是平仓
				if(Trade.OffsetFlag!=THOST_FTDC_OF_CloseToday)
				{///不是平今的平仓
					if(pCommissionRate->CloseRatioByVolume>0)
					{
						dCommission=pCommissionRate->CloseRatioByVolume*Trade.Volume;
						iter->second.Commission+=dCommission;
					}
					else
					{
						dTotalCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;
						dCommission=pCommissionRate->CloseRatioByMoney*dTotalCost;
						iter->second.Commission+=dCommission;
					}
				}
				else
				{///平今
					if(pCommissionRate->CloseTodayRatioByVolume>0)
					{
						dCommission=pCommissionRate->CloseTodayRatioByVolume*Trade.Volume;
						iter->second.Commission+=dCommission;
					}
					else
					{
						dTotalCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;
						dCommission=pCommissionRate->CloseTodayRatioByVolume*dTotalCost;
						iter->second.Commission+=dCommission;
					}
				}
			}

		    ///交易所保证金
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.ExchangeMargin=iter->second.PositionCost*iter_Info->second.LongMarginRatio;
			}
			else
			{
				iter->second.ExchangeMargin=iter->second.PositionCost*iter_Info->second.ShortMarginRatio;
			}

			///本次结算价
			iter->second.SettlementPrice = Trade.Price;
			///平仓盈亏
			db_CloseProfit=(Trade.Price-db_Average_Last)*iter_Info->second.VolumeMultiple*Trade.Volume;

			if(iter->second.PosiDirection == THOST_FTDC_PD_Short)
			{
				db_CloseProfit=0-db_CloseProfit;
			}
			iter->second.CloseProfit+=db_CloseProfit;
			//iter->second.CloseProfit = YdPosition*iter->second.PreSettlementPrice*iter_Info->second.VolumeMultiple
			//						  +iter->second.OpenAmount
			//						  -iter->second.CloseAmount
			//						  +iter->second.CloseProfitByDate
			//						  -iter->second.PositionCost;
			///持仓盈亏
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.PositionProfit=(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			else
			{
				iter->second.PositionProfit=0-(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			
			///逐日盯市平仓盈亏
			iter->second.CloseProfitByDate +=db_CloseProfit;

			///逐笔对冲平仓盈亏
			iter->second.CloseProfitByTrade+=db_CloseProfit;
		}
	}
	if(iter->second.Position==0)
	{
		iter->second.ShortFrozen=0;
		iter->second.LongFrozen=0;
		iter->second.PositionCost=0;
		iter->second.UseMargin=0;
		iter->second.PositionProfit=0;
		iter->second.OpenCost=0;
		iter->second.TodayPosition = 0;
	}
#if _DEBUG
	Show();
#endif
	return  1;
}

DWORD WINAPI Cala_Position_Order(LPVOID pOrder)
{///因平仓下单而修改冻结量
	CThostFtdcOrderField Order = *(CThostFtdcOrderField*)pOrder;
	char pcTmp[255];
	string strKey;
	memset(pcTmp,0,sizeof(pcTmp));
	map_str_InvestorPosition::iterator iter;
	///存在该方向的持仓合约，处理冻结数量
	if(   Order.CombOffsetFlag[0]!=THOST_FTDC_OF_Open 
		)
	{///该订单是非开仓
		if(Order.Direction==THOST_FTDC_D_Buy)
		{    ///此订单为买入平空

			sprintf_s(pcTmp,"%s_%c",Order.InstrumentID,THOST_FTDC_PD_Short);
			strKey=pcTmp;
			iter = g_m_InvestorPosition.find(strKey);
			if(iter !=g_m_InvestorPosition.end())
			{
				iter->second.LongFrozen = GetFrozenNumOnClose(Order.InstrumentID,THOST_FTDC_PD_Short);

			}
		}
		else
		{///此订单为卖出平多
			sprintf_s(pcTmp,"%s_%c",Order.InstrumentID,THOST_FTDC_PD_Long);
			strKey=pcTmp;
			iter = g_m_InvestorPosition.find(strKey);
			if(iter !=g_m_InvestorPosition.end())
			{
				iter->second.ShortFrozen = GetFrozenNumOnClose(Order.InstrumentID,THOST_FTDC_PD_Long);

			}
		}
	}
	else
	{
		///开仓订单不会影响仓位冻结
	}
#if _DEBUG
	Show();
#endif
	return  1;
}
CTradeCTP::CTradeCTP(void)
{
}


CTradeCTP::~CTradeCTP(void)
{
}
void CTradeCTP::OnFrontDisconnected(int nReason)
{
	Beep(263,1000);
}
void CTradeCTP::OnHeartBeatWarning(int nTimeLapse)
{
}
// 是否收到Success的响应
bool CTradeCTP::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	return false;
}
///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTradeCTP::OnFrontConnected()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = g_pTradeApi->ReqUserLogin(&req, ++g_iRequestID);
	
}
///登录请求响应
void CTradeCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
		///cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		m_iNextOrderRef = iNextOrderRef;

		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///获取当前交易日
		///cerr << "--->>> 获取当前交易日 = " << g_pTradeApi->GetTradingDay() << endl;
		///投资者结算结果确认
		CThostFtdcSettlementInfoConfirmField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		int iResult = g_pTradeApi->ReqSettlementInfoConfirm(&req, ++g_iRequestID);
	}
}
///投资者结算结果确认响应
void CTradeCTP::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


	if(bIsLast)
	{
	   ///查询持仓
		CThostFtdcQryInvestorPositionField req;
     	memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		g_m_InvestorPosition.clear();
		int iResult = g_pTradeApi->ReqQryInvestorPosition(&req, ++g_iRequestID);

		///查询合约
		CThostFtdcQryInstrumentField QryInstrument;
		memset(&QryInstrument,0,sizeof(QryInstrument));
		iResult = g_pTradeApi->ReqQryInstrument(&QryInstrument, ++g_iRequestID);

		///查询成交
		CThostFtdcQryTradeField QryTrade;
		memset(&QryTrade,0,sizeof(QryTrade));
		iResult = g_pTradeApi->ReqQryTrade(&QryTrade, ++g_iRequestID);


		memset(&g_TradingAccount,0,sizeof(g_TradingAccount));
		///请求查询资金账户
		CThostFtdcQryTradingAccountField QryTradingAccount;
		memset(&QryTradingAccount,0,sizeof(QryTradingAccount));
	    iResult = g_pTradeApi->ReqQryTradingAccount(&QryTradingAccount, ++g_iRequestID);

	}
}
///请求查询资金账户响应
void CTradeCTP::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pTradingAccount)
	{
	    g_TradingAccount=*pTradingAccount;
		Show();
	}
}

///请求查询合约响应
 void CTradeCTP::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	string strKey;
	string strProductID;
	if(NULL!=pInstrument)
	{
		strKey=pInstrument->InstrumentID;
		///if(strcmp(strKey.c_str(),"IF1505")!=0) 
		strProductID = pInstrument->ProductID;
		map_str_InstrumentInfo::iterator  iter = g_m_InstrumentInfo.find(strKey);
		if(iter== g_m_InstrumentInfo.end())
		{
		    g_m_InstrumentInfo.insert(make_pair(strKey,*pInstrument));
		}
		else
		{
		
			iter->second=*pInstrument;
		}
		g_m_Instrument_ProductID.insert(make_pair(strKey,strProductID));
	}
	if(bIsLast)
	{
		WriteInstrumentInfo2File();
		///CreateThread(NULL,0,Thread_QryRate,NULL,0,NULL);
	}
}
 ///报单通知
void CTradeCTP::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	/////cerr << "--->>> " << "OnRtnOrder"  << endl;
	if(NULL!=pOrder)
	{
		string strKey;
		if(atol(pOrder->OrderSysID)>0 )
		{///只取交易所的回报，排除CTP回报
			strKey=pOrder->OrderSysID;
			///记录订单,
			map_string_Order::iterator iter_Tmp = g_m_Order.find(strKey);
			if(iter_Tmp == g_m_Order.end())
			{
				g_m_Order.insert(make_pair(strKey,*pOrder));
			}
			else
			{
				///处理撤单时，用以撤单的回报订单修改原来的订单
				iter_Tmp->second = *pOrder;
			}
			///修改本地可平仓数量
			Cala_Position_Order((LPVOID) pOrder);

		}
		else
		{
			///有CTP的订单回报，就订阅行情，撤单、平仓不处理
		   strKey = pOrder->InstrumentID;
		   map_srt_DepthMarketData::iterator iter_MD = g_m_DepthMarketData.find(strKey);
		   if(iter_MD == g_m_DepthMarketData.end())
		   { 
				char * pInstrumnet[1];
				pInstrumnet[0]=pOrder->InstrumentID;
				g_pQuoteCTP->SubscribeMarketData(pInstrumnet,1);
		   }
		}
	}
}

///成交通知
void CTradeCTP::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if(NULL!=pTrade)
	{
		string strKey=pTrade->TradeID;
		///记录成交
		g_m_Trade.insert(make_pair(strKey,*pTrade));
		///维护本地订单
		strKey=pTrade->OrderSysID;
		map_string_Order::iterator itor=g_m_Order.find(strKey);
		if(itor!=g_m_Order.end())
		{
			itor->second.VolumeTotal=itor->second.VolumeTotal-pTrade->Volume;
		}
		g_v_Trade.push_back(*pTrade);
		Cala_Position_Trade((LPVOID)pTrade);
	}
}
///请求查询成交响应
void CTradeCTP::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pTrade)
	{
		g_v_Trade.push_back(*pTrade);
	}
}
///请求查询投资者持仓响应
void CTradeCTP::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pInvestorPosition)
	{

		///昨仓
		char pcTmp[255];
		string strKey;
		memset(pcTmp,0,sizeof(pcTmp));
		sprintf(pcTmp,"%s_%c",pInvestorPosition->InstrumentID,pInvestorPosition->PosiDirection);
		strKey=pcTmp;
		map_str_InvestorPosition::iterator iter=g_m_InvestorPosition.find(strKey);
		if(iter==g_m_InvestorPosition.end())
		{
			g_m_InvestorPosition.insert(make_pair(strKey,*pInvestorPosition));
		}
		else
		{
			if(pInvestorPosition->Position>0)
			{
				///由于订单回报、成交回报、撤单回报比持仓先到本地，必须覆盖先前生成的持仓
				iter->second=*pInvestorPosition;
			}
		}
		strKey = pInvestorPosition->InstrumentID;
		map_srt_DepthMarketData::iterator iter_MD = g_m_DepthMarketData.find(strKey);
	   if(iter_MD == g_m_DepthMarketData.end())
	   { 
			char * pInstrumnet[1];
			pInstrumnet[0]=pInvestorPosition->InstrumentID;
		    g_pQuoteCTP->SubscribeMarketData(pInstrumnet,1);
	   }
	}
#if _DEBUG
		if(bIsLast)
		{    
			///Show();
			///CreateThread(NULL,0,Thread_Show,NULL,0,NULL);
		}
#endif
}
///请求查询合约保证金率响应
void CTradeCTP::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pInstrumentMarginRate)
	{
		char pKey[256];
		memset(pKey,0,sizeof(pKey));
		sprintf(pKey,"%s_%c",pInstrumentMarginRate->InstrumentID,pInstrumentMarginRate->HedgeFlag);
		///sprintf(pKey,"%s",pInstrumentMarginRate->InstrumentID);
		string strKey=pKey;
		map_str_MarginRate::iterator iter=g_m_MarginRate.find(strKey);
		if(iter==g_m_MarginRate.end())
		{
			g_m_MarginRate.insert(make_pair(strKey,*pInstrumentMarginRate));
		}
		else
		{
			iter->second=*pInstrumentMarginRate;
		}
	}

	if(bIsLast)
	{
		WriteMarginRate2File();
	}
}

///请求查询合约手续费率响应
void CTradeCTP::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pInstrumentCommissionRate)
	{
		string strKey=pInstrumentCommissionRate->InstrumentID;
		map_str_CommissionRate::iterator iter=g_m_CommissionRate.find(strKey);
		if(iter==g_m_CommissionRate.end())
		{
			g_m_CommissionRate.insert(make_pair(strKey,*pInstrumentCommissionRate));
		}
		else
		{
			iter->second=*pInstrumentCommissionRate;
		}
		WriteCommissionRate2File();
	}
#if _DEBUG
	if(bIsLast)
	{
		string strKey="IF";
		map_str_CommissionRate::iterator iter = g_m_CommissionRate.find(strKey);
		if(iter != g_m_CommissionRate.end())
		{
			int a = 0;
		}
	}
#endif
}




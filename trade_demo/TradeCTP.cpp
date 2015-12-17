#include "stdafx.h"
#include "TradeCTP.h"
#include <iostream>
using namespace std;

#include ".\API_CTP\ThostFtdcTraderApi.h"
#pragma warning(disable : 4996)
// USER_API����
extern CThostFtdcTraderApi* g_pTradeApi;
extern CQuoteCTP*           g_pQuoteCTP;

// ���ò���
extern char FRONT_ADDR[];		// ǰ�õ�ַ
extern char BROKER_ID[];		// ���͹�˾����
extern char INVESTOR_ID[];		// Ͷ���ߴ���
extern char PASSWORD[];			// �û�����
extern TThostFtdcPriceType	LIMIT_PRICE;	// �۸�
extern TThostFtdcDirectionType	DIRECTION;	// ��������

// ������
extern int g_iRequestID;
extern TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
extern TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
extern TThostFtdcOrderRefType	ORDER_REF;	//��������

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

///��ƽ��ʱ����ѯƽ�ֵ�λ�ɽ��Ĳ���
int GetFrozenNumOnClose(TThostFtdcInstrumentIDType	InstrumentID, TThostFtdcPosiDirectionType PosiDirection)
{

	int cDirection;
	if(PosiDirection==THOST_FTDC_PD_Long)
	{///ƽ�ֶ���Ĳ�λ�Ƕ�ͷʱ����������
		cDirection=THOST_FTDC_D_Sell;
	}
	else
	{
		cDirection=THOST_FTDC_D_Buy;
	}
	///��������
	int iFrozenNum=0;

	///δ�ɽ�������
	int iLeft=0;

	map_string_Order::iterator iter;
	for(iter=g_m_Order.begin();iter!=g_m_Order.end();iter++)
	{
		if(!(iter->second.OrderStatus == THOST_FTDC_OST_AllTraded || iter->second.OrderStatus ==THOST_FTDC_OST_Canceled) &&  iter->second.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
		{///δ�ɽ���Ϊ������ƽ��

			if(strcmp(iter->second.InstrumentID,InstrumentID)==0)
			{///��Լ

				if(iter->second.Direction==cDirection)
				{///��Ӧ����������

					iLeft = iter->second.VolumeTotal-iter->second.VolumeTraded;
					iFrozenNum+=iLeft;
				}
			}
		}
	}
	return iFrozenNum;
}

///��ƽ��ʱͨ��FIFO�õ�����
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
		{///��Լƥ��
			if(iter->OffsetFlag==THOST_FTDC_OF_Open)
			{///����
				if(iter->Direction==Direction)
				{///����

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
///�õ�����
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

///д����
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

///д����
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
///д��֤����
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
				
				///ȡ��������
				CThostFtdcQryInstrumentCommissionRateField QryInstrumentCommissionRate;
				memset(&QryInstrumentCommissionRate,0,sizeof(QryInstrumentCommissionRate));
				strcpy(QryInstrumentCommissionRate.BrokerID, BROKER_ID);
				strcpy(QryInstrumentCommissionRate.InvestorID, INVESTOR_ID);
				strcpy(QryInstrumentCommissionRate.InstrumentID,iter->second.InstrumentID);
				Sleep(iSleepMic_Secend);
				iResult = g_pTradeApi->ReqQryInstrumentCommissionRate(&QryInstrumentCommissionRate,++g_iRequestID);
				///continue;
				
				///ȡ��֤����
				CThostFtdcQryInstrumentMarginRateField QryInstrumentMarginRate;
				memset(&QryInstrumentMarginRate,0,sizeof(QryInstrumentMarginRate));
				strcpy(QryInstrumentMarginRate.BrokerID, BROKER_ID);
				strcpy(QryInstrumentMarginRate.InvestorID, INVESTOR_ID);
				strcpy(QryInstrumentMarginRate.InstrumentID,iter->second.InstrumentID);
				///Ͷ��
				Sleep(iSleepMic_Secend);
				QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Speculation;
				iResult = g_pTradeApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++g_iRequestID);

				///����
				Sleep(iSleepMic_Secend);
				QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Arbitrage;
				iResult = g_pTradeApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++g_iRequestID);

				///�ױ�
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
		///�洢
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

	///����
	g_TradingAccount.Commission=0;

	///�����ʽ�
	g_TradingAccount.Balance=0;

	///ƽ��ӯ��
	g_TradingAccount.CloseProfit=0;

	///�ֲ�ӯ��
	g_TradingAccount.PositionProfit=0;

	///ռ�ñ�֤��
	g_TradingAccount.CurrMargin=0;

	double dB_Balance=0;

	for(iter = g_m_InvestorPosition.begin();iter!=g_m_InvestorPosition.end();iter++)
	{
		printf("*******************�ֲ�*****************************\r\n");
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
			{///�к�Լ��Ϣʱ������ͨ���������������
				 Average_Prive = 0;
				 PositionProfit=0;
				 Average_Prive = (iter->second.PositionCost)/(iter->second.Position*iter_Info->second.VolumeMultiple);
				 PositionProfit = (iter_MD->second.LastPrice-Average_Prive)*iter->second.Position*iter_Info->second.VolumeMultiple;
				 ///�ղ�ӯ������
				 if(iter->second.PosiDirection==THOST_FTDC_PD_Short)
				 {
					 PositionProfit=0-PositionProfit;
				 }
				 g_TradingAccount.PositionProfit+=PositionProfit;

				if(iter->second.PosiDirection==THOST_FTDC_PD_Long)
				{
					sprintf(pTmp,"%02d %s �� ��:%d ��:%d ���:%d ��ƽ:%d ����:%g �ֲ�ӯ��:%.1f ��֤��:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.ShortFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
				else
				{
					sprintf(pTmp,"%02d %s �� ��:%d ��:%d ���:%d ��ƽ:%d ����:%g �ֲ�ӯ��:%.1f ��֤��:%.1f \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.LongFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
			}
			else
			{///�޺�Լ��Ϣʱ���ý����
				 Average_Prive = (iter->second.PositionCost)/(iter->second.Position*iter_Info->second.VolumeMultiple);;
				 PositionProfit=iter->second.PositionProfit;

				 g_TradingAccount.PositionProfit+=PositionProfit;

				if(iter->second.PosiDirection==THOST_FTDC_PD_Long)
				{	
					
					sprintf(pTmp,"%02d %s �� ��:%d ��:%d ���:%d ��ƽ:%d ����:%g �ֲ�ӯ��:%.1f ��֤��:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.ShortFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}
				else
				{
			
					sprintf(pTmp,"%02d %s �� ��:%d ��:%d ���:%d ��ƽ:%d ����:%g �ֲ�ӯ��:%.1f ��֤��:%.1f  \n",i,iter->second.InstrumentID,iter->second.Position,iter->second.YdPosition,iter->second.TodayPosition,iter->second.Position-iter->second.LongFrozen,Average_Prive,PositionProfit,iter->second.UseMargin);
				}

			}
			printf(pTmp);
			i++;
		}
	}
	///��̬Ȩ��
	g_TradingAccount.Balance=g_TradingAccount.PreBalance+g_TradingAccount.CloseProfit+g_TradingAccount.PositionProfit-g_TradingAccount.Commission;
	double db_Per = 0;
	if(g_TradingAccount.Balance>0.1)
	{
		db_Per= 100*g_TradingAccount.CurrMargin/g_TradingAccount.Balance;
	}

	printf("\r\n***************Ȩ�������***************************");
	memset(pTmp,0,sizeof(pTmp));
	sprintf(pTmp,"\r\n��̬Ȩ��:%.0f ƽ��ӯ��:%.0f �ֲ�ӯ��:%.0f Ȩ����:0 ��̬Ȩ��:%.0f ռ�ñ�֤��:%.0f �µ�����:0 ����Ȩ����:0 �����ʽ�:%.0f ���ն�:%.1f ",
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
{///��ɽ����޸ĳֲ�
	CThostFtdcTradeField Trade = *(CThostFtdcTradeField*)pTrade;
	char pcTmp[255];
	memset(pcTmp,0,sizeof(pcTmp));
	if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
	{
		if(Trade.Direction==THOST_FTDC_D_Buy)
		{
			///��orderΪ�����
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Long);
		}
		else
		{
			///��orderΪ���ղ�
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Short);
		}
	}
	else
	{
		if(Trade.Direction==THOST_FTDC_D_Buy)
		{
			///��orderΪƽ�ղ�
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Short);
		}
		else
		{
			///��orderΪƽ���
			sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,THOST_FTDC_PD_Long);
		}

	}
	///strKeyΪ��IF1505_3��
	string strKey=pcTmp;
	map_str_InvestorPosition::iterator iter = g_m_InvestorPosition.find(strKey);
	strKey = Trade.InstrumentID;
	///strKeyΪ��IF1505��
	map_str_InstrumentInfo::iterator iter_Info = g_m_InstrumentInfo.find(strKey);
	///strKeyΪ��IF1505_1��
	sprintf_s(pcTmp,"%s_%c",Trade.InstrumentID,Trade.HedgeFlag);
	strKey = pcTmp;
	map_str_MarginRate::iterator     iter_MarginRate = g_m_MarginRate.find(strKey);
	///strKeyΪ��IF1505��
	strKey = Trade.InstrumentID;
	map_srt_DepthMarketData::iterator iter_MD = g_m_DepthMarketData.find(strKey);

	CThostFtdcInstrumentCommissionRateField *pCommissionRate=GetCommissionRate(Trade.InstrumentID);
	int YdPosition=0;
	double dTotalCost=0;
	///���η���
	double dCommission=0;


	///�õ�ƽ�ֺ�ľ���
    double db_Average_Now = 0;

	///�ϴξ���
    double db_Average_Last=0;

	///����ƽ��ӯ��
	double db_CloseProfit=0;

	if(iter_Info == g_m_InstrumentInfo.end())
	{  ///û�еõ���Լ��Ϣ֮ǰ������
		return 0;
	}
	if(iter_MarginRate == g_m_MarginRate.end())
	{  ///û�еõ���֤����֮ǰ������
		return 0;
	}
	if(iter==g_m_InvestorPosition.end() )
	{
		///û�и÷���ĳֲֺ�Լ���϶��ǿ��֣������ǿ����������ʽ
		if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
		{
			
			///��ʱҪ�ڱ�������һ���ֲ�
			CThostFtdcInvestorPositionField InvestorPosition;

			memset(&InvestorPosition,0,sizeof(InvestorPosition));

			///��Լ����
			strcpy(InvestorPosition.InstrumentID,Trade.InstrumentID);

			///���͹�˾����
			strcpy(InvestorPosition.BrokerID,Trade.BrokerID);

			///Ͷ���ߴ���
			strcpy(InvestorPosition.InvestorID,Trade.InvestorID);

			///�ֲֶ�շ���
			if(Trade.Direction == THOST_FTDC_D_Buy)
			{
				///��Ϊ��ͷ
				InvestorPosition.PosiDirection=THOST_FTDC_PD_Long;
			}
			else
			{   ///��Ϊ��ͷ
				InvestorPosition.PosiDirection=THOST_FTDC_PD_Short;
			}

			///Ͷ���ױ���־
			InvestorPosition.HedgeFlag=Trade.HedgeFlag;

			///�ֲ�����
			InvestorPosition.PositionDate=THOST_FTDC_PSD_Today;

			///���ճֲ�
			InvestorPosition.YdPosition=0;

			///�ܲ�
			InvestorPosition.Position=Trade.Volume;

			///��ͷ����
			InvestorPosition.LongFrozen = 0;

			///��ͷ����
			InvestorPosition.ShortFrozen = 0;

			///���ֶ�����
			InvestorPosition.LongFrozenAmount=0;

			///���ֶ�����
			InvestorPosition.ShortFrozenAmount=0;

			///������
			InvestorPosition.OpenVolume=Trade.Volume;

			///ƽ����
			InvestorPosition.CloseVolume=0;

			///���ֽ��
			InvestorPosition.OpenAmount=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///ƽ�ֽ��
			InvestorPosition.CloseAmount=0;

			///�ֲֳɱ�
			InvestorPosition.PositionCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;


			///�ϴ�ռ�õı�֤��
			InvestorPosition.PreMargin=0;

			///ռ�õı�֤��
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

			///����ı�֤��
			InvestorPosition.FrozenMargin=0;

			///������ʽ�
			InvestorPosition.FrozenCash=0;

			///�����������
			InvestorPosition.FrozenCommission=0;

			///�ʽ���
			InvestorPosition.CashIn=0;

			///������
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

			///�������ѷ���ռ�õı�֤��
			///InvestorPosition.UseMargin+=dCommission;

			///ƽ��ӯ��
			InvestorPosition.CloseProfit=0;


			///�ֲ�ӯ��
			if(iter_MD != g_m_DepthMarketData.end())
			{
				///������ʱ�������������
				InvestorPosition.PositionProfit = Trade.Volume * iter_MD->second.LastPrice*iter_Info->second.VolumeMultiple -InvestorPosition.PositionCost;
			}
			else
			{
				///������ʱ�����ս���ۼ���
				InvestorPosition.PositionProfit = Trade.Volume * Trade.Price *iter_Info->second.VolumeMultiple-InvestorPosition.PositionCost;
			}
			///�ϴν����
			InvestorPosition.PreSettlementPrice=Trade.Price;

			///���ν����
			InvestorPosition.SettlementPrice=Trade.Price;

			///������
			strcpy(InvestorPosition.TradingDay,Trade.TradingDay);

			///������
			InvestorPosition.SettlementID=0;

			///���ֳɱ�
			InvestorPosition.OpenCost=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///��������֤��
			InvestorPosition.ExchangeMargin=0;
		    if(InvestorPosition.PosiDirection == THOST_FTDC_PD_Long)
			{
				InvestorPosition.ExchangeMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_Info->second.LongMarginRatio;
			}
			else
			{
				InvestorPosition.ExchangeMargin=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple*iter_Info->second.ShortMarginRatio;
			}

			///��ϳɽ��γɵĳֲ�
			InvestorPosition.CombPosition=0;

			///��϶�ͷ����
			InvestorPosition.CombLongFrozen=0;

			///��Ͽ�ͷ����
			InvestorPosition.CombShortFrozen=0;

			///���ն���ƽ��ӯ��
			InvestorPosition.CloseProfitByDate = InvestorPosition.CloseProfit;

			///��ʶԳ�ƽ��ӯ��
			InvestorPosition.CloseProfitByTrade=0;

			///���ճֲ�
			InvestorPosition.TodayPosition=Trade.Volume;

			///��֤����
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


			///��֤����(������)
			InvestorPosition.MarginRateByVolume=0;



			char pcTmp[255];
			string strKey;
			memset(pcTmp,0,sizeof(pcTmp));
			sprintf(pcTmp,"%s_%c",InvestorPosition.InstrumentID,InvestorPosition.PosiDirection);

			strKey = pcTmp;
			///���뱾���ڴ�
			g_m_InvestorPosition.insert(make_pair(strKey,InvestorPosition));

		}
		return 1;
	}
	else
	{
		if(Trade.OffsetFlag==THOST_FTDC_OF_Open)
		{///����

			///��λ
			iter->second.Position+=Trade.Volume;

			///������
			iter->second.OpenVolume+=Trade.Volume;

			///���
			iter->second.TodayPosition+=Trade.Volume;

			///���
			YdPosition = iter->second.Position-iter->second.TodayPosition;
			iter->second.YdPosition=YdPosition;

			///���ֽ��
			iter->second.OpenAmount+=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///�ֲֳɱ�
			iter->second.PositionCost+= Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

  			  ///ռ�õı�֤��
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

			///������
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


	
			///���ν����(�Ǿ���)
			iter->second.SettlementPrice=Trade.Price;

			///�ֲ�ӯ��
			db_Average_Now =iter->second.PositionCost/(iter->second.Position*iter_Info->second.VolumeMultiple);
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.PositionProfit=(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			else
			{
				iter->second.PositionProfit=-(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}

			///���ֳɱ�
			iter->second.OpenCost+=Trade.Volume*Trade.Price*iter_Info->second.VolumeMultiple;

			///��������֤��
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
		{///ƽ��
			///1�����ٲ�λ
			///2��ͬʱ���ٿ�ƽ��
			///3��������
			///4�����
			///���ٲ�λ

			///�õ�ƽ�ֺ�ľ���
			db_Average_Now = GetAveragePriceOnClose(iter->second,Trade.Volume);

			///�ϴξ���
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


			///ƽ����
			iter->second.CloseVolume+=Trade.Volume;

			///ƽ�ֽ��
			iter->second.CloseAmount+=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple;
			

			///���
			if(Trade.OffsetFlag!=THOST_FTDC_OF_Open)
			{///������
				if( Trade.Volume < iter->second.YdPosition )
				{

				}
				else
				{
					if(iter->second.TodayPosition>0 )
						iter->second.TodayPosition-=Trade.Volume;
				}
				
			}

			///���
			YdPosition = iter->second.Position-iter->second.TodayPosition;
			iter->second.YdPosition=YdPosition;

			///�ֲֳɱ�
			iter->second.PositionCost=db_Average_Now*iter->second.Position*iter_Info->second.VolumeMultiple;
			
			///���ֳɱ�
			iter->second.OpenCost -=Trade.Price*Trade.Volume*iter_Info->second.VolumeMultiple;

			///ռ�õı�֤��
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

			///������
			if(NULL!=pCommissionRate)
			{///����һ����ƽ��
				if(Trade.OffsetFlag!=THOST_FTDC_OF_CloseToday)
				{///����ƽ���ƽ��
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
				{///ƽ��
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

		    ///��������֤��
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.ExchangeMargin=iter->second.PositionCost*iter_Info->second.LongMarginRatio;
			}
			else
			{
				iter->second.ExchangeMargin=iter->second.PositionCost*iter_Info->second.ShortMarginRatio;
			}

			///���ν����
			iter->second.SettlementPrice = Trade.Price;
			///ƽ��ӯ��
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
			///�ֲ�ӯ��
			if(iter->second.PosiDirection == THOST_FTDC_PD_Long)
			{
				iter->second.PositionProfit=(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			else
			{
				iter->second.PositionProfit=0-(Trade.Price-db_Average_Now)*iter->second.Position*iter_Info->second.VolumeMultiple;
			}
			
			///���ն���ƽ��ӯ��
			iter->second.CloseProfitByDate +=db_CloseProfit;

			///��ʶԳ�ƽ��ӯ��
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
{///��ƽ���µ����޸Ķ�����
	CThostFtdcOrderField Order = *(CThostFtdcOrderField*)pOrder;
	char pcTmp[255];
	string strKey;
	memset(pcTmp,0,sizeof(pcTmp));
	map_str_InvestorPosition::iterator iter;
	///���ڸ÷���ĳֲֺ�Լ������������
	if(   Order.CombOffsetFlag[0]!=THOST_FTDC_OF_Open 
		)
	{///�ö����Ƿǿ���
		if(Order.Direction==THOST_FTDC_D_Buy)
		{    ///�˶���Ϊ����ƽ��

			sprintf_s(pcTmp,"%s_%c",Order.InstrumentID,THOST_FTDC_PD_Short);
			strKey=pcTmp;
			iter = g_m_InvestorPosition.find(strKey);
			if(iter !=g_m_InvestorPosition.end())
			{
				iter->second.LongFrozen = GetFrozenNumOnClose(Order.InstrumentID,THOST_FTDC_PD_Short);

			}
		}
		else
		{///�˶���Ϊ����ƽ��
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
		///���ֶ�������Ӱ���λ����
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
// �Ƿ��յ�Success����Ӧ
bool CTradeCTP::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	return false;
}
///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void CTradeCTP::OnFrontConnected()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = g_pTradeApi->ReqUserLogin(&req, ++g_iRequestID);
	
}
///��¼������Ӧ
void CTradeCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
		///cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		m_iNextOrderRef = iNextOrderRef;

		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///��ȡ��ǰ������
		///cerr << "--->>> ��ȡ��ǰ������ = " << g_pTradeApi->GetTradingDay() << endl;
		///Ͷ���߽�����ȷ��
		CThostFtdcSettlementInfoConfirmField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		int iResult = g_pTradeApi->ReqSettlementInfoConfirm(&req, ++g_iRequestID);
	}
}
///Ͷ���߽�����ȷ����Ӧ
void CTradeCTP::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{


	if(bIsLast)
	{
	   ///��ѯ�ֲ�
		CThostFtdcQryInvestorPositionField req;
     	memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		g_m_InvestorPosition.clear();
		int iResult = g_pTradeApi->ReqQryInvestorPosition(&req, ++g_iRequestID);

		///��ѯ��Լ
		CThostFtdcQryInstrumentField QryInstrument;
		memset(&QryInstrument,0,sizeof(QryInstrument));
		iResult = g_pTradeApi->ReqQryInstrument(&QryInstrument, ++g_iRequestID);

		///��ѯ�ɽ�
		CThostFtdcQryTradeField QryTrade;
		memset(&QryTrade,0,sizeof(QryTrade));
		iResult = g_pTradeApi->ReqQryTrade(&QryTrade, ++g_iRequestID);


		memset(&g_TradingAccount,0,sizeof(g_TradingAccount));
		///�����ѯ�ʽ��˻�
		CThostFtdcQryTradingAccountField QryTradingAccount;
		memset(&QryTradingAccount,0,sizeof(QryTradingAccount));
	    iResult = g_pTradeApi->ReqQryTradingAccount(&QryTradingAccount, ++g_iRequestID);

	}
}
///�����ѯ�ʽ��˻���Ӧ
void CTradeCTP::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pTradingAccount)
	{
	    g_TradingAccount=*pTradingAccount;
		Show();
	}
}

///�����ѯ��Լ��Ӧ
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
 ///����֪ͨ
void CTradeCTP::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	/////cerr << "--->>> " << "OnRtnOrder"  << endl;
	if(NULL!=pOrder)
	{
		string strKey;
		if(atol(pOrder->OrderSysID)>0 )
		{///ֻȡ�������Ļر����ų�CTP�ر�
			strKey=pOrder->OrderSysID;
			///��¼����,
			map_string_Order::iterator iter_Tmp = g_m_Order.find(strKey);
			if(iter_Tmp == g_m_Order.end())
			{
				g_m_Order.insert(make_pair(strKey,*pOrder));
			}
			else
			{
				///������ʱ�����Գ����Ļر������޸�ԭ���Ķ���
				iter_Tmp->second = *pOrder;
			}
			///�޸ı��ؿ�ƽ������
			Cala_Position_Order((LPVOID) pOrder);

		}
		else
		{
			///��CTP�Ķ����ر����Ͷ������飬������ƽ�ֲ�����
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

///�ɽ�֪ͨ
void CTradeCTP::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if(NULL!=pTrade)
	{
		string strKey=pTrade->TradeID;
		///��¼�ɽ�
		g_m_Trade.insert(make_pair(strKey,*pTrade));
		///ά�����ض���
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
///�����ѯ�ɽ���Ӧ
void CTradeCTP::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pTrade)
	{
		g_v_Trade.push_back(*pTrade);
	}
}
///�����ѯͶ���ֲ߳���Ӧ
void CTradeCTP::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(NULL!=pInvestorPosition)
	{

		///���
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
				///���ڶ����ر����ɽ��ر��������ر��ȳֲ��ȵ����أ����븲����ǰ���ɵĳֲ�
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
///�����ѯ��Լ��֤������Ӧ
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

///�����ѯ��Լ����������Ӧ
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




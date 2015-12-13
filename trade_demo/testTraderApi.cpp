/************************************************************/
/* ���ܣ����򻯽��׹��ܵ�Demo����
/* ��˾�������ڻ����޹�˾
/* ���ߣ�����ɽ
/* ȺQQ��89777472
/* ������20140819
/* �޸ļ�¼��
20140108: ���������
20140116��Ԥ�񵥣������ڱ��أ�����������ִ���µ�ָ��
20140118��Ԥ��ʱ��ƽ��,��15:14:00
20140121��Ԥ��,��CTP����;
20140216��ȡ��ʷK������
20140425��������,��CTP����
20140428���ֱֲ��ػ�����ʾ���ն�
20140528������K��
/************************************************************/
#include "stdafx.h"
#include "Get_MD_K.h"


// �Ự����
TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
TThostFtdcOrderRefType	ORDER_REF;	//��������

///Key�� FRONT_ID_SESSION_ID_OrderRef
map_str_pstrLocalID g_m_Local_Ref; 

///Key��TradeID
map_string_Trade       g_m_Trade;

///Key��OrderSysID
map_string_Order       g_m_Order;

///����Ԥ��
vector_InputOrder      g_v_InputOrder;

///�õ���ʷK��
vector_MD_K            g_v_MD_K;

///���سֲ֣�Key�� IF1505_0 ��ʾIF1505�Ķ��
map_str_InvestorPosition        g_m_InvestorPosition;

///���ڽ��׵ĺ�Լ��Ϣ
map_str_InstrumentInfo          g_m_InstrumentInfo;

///��֤����
map_str_MarginRate              g_m_MarginRate;

///��������
map_str_CommissionRate          g_m_CommissionRate;

///���������б�
map_srt_DepthMarketData         g_m_DepthMarketData;

///��Լ���Լ������չ�ϵ
map_str_str                     g_m_Instrument_ProductID;

///�ɽ�
vector_Trade                    g_v_Trade;

///�ʻ�
CThostFtdcTradingAccountField   g_TradingAccount;

char                   g_cKey[1024];
int                    g_iReturn;
// UserApi����
CThostFtdcTraderApi*   g_pTradeApi;
CThostFtdcMdApi*       g_pMdApi;
CTradeCTP*             g_pTradeCTP;
CQuoteCTP*             g_pQuoteCTP;





// ������
int g_iRequestID = 0;
// ���ò���
///"tcp://ctpfz1-front1.citicsf.com:51205"
///"tcp://180.168.146.181:10200"
#if 1
char  FRONT_ADDR[] = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";		// ǰ�õ�ַ
char  MD_FRONT_ADDR[] = "tcp://asp-sim2-front1.financial-trading-platform.com:26213";	// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "2030";						// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "875000";				    // Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "123456";						// �û�����
#else
char  FRONT_ADDR[] = "tcp://180.168.146.181:10200";					// ǰ�õ�ַ
char  MD_FRONT_ADDR[] = "tcp://180.168.146.181:10210";				// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "";							// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "";						// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "";					// �û�����
#endif


TThostFtdcPriceType	LIMIT_PRICE = 38850;				// �۸�
///��һ���ַ����ָ���ַ�������
bool SplitString ( std::string& strSource, char* cstrSegmentStopC, vector_str& strArray )
{
	
	if ( cstrSegmentStopC == NULL )
	{
		strArray.push_back( strSource );
		return true;
	}
	//
	int iLength = strSource.size( );
	int iFirst = 0, iLast = -1;
	string strKey, strValue;
	string strSubsource = strSource;
	while ( iLast < iLength - 1 )
	{
		iLast = strSource.find(cstrSegmentStopC, iFirst);
		iLast = iLast == string::npos ? iLength : iLast;
		strSubsource = strSource.substr(iFirst, iLast - iFirst);
		strArray.push_back( strSubsource );
		iFirst = iLast + 1;
	}
	return true;
}
///��ȡ��Լ��Ϣ
void ReadInstrumentInfo()
{
	g_m_InstrumentInfo.clear();
	char buf[1024];
	char pPath[256];
	memset(pPath,0,sizeof(pPath));
	memset(buf,0,sizeof(buf));
	_getcwd( pPath, 256 );
	string strTmp;
	string strPath=pPath;
	strPath+="\\InstrumentInfo.csv";
	FILE *      pFile=fopen(strPath.c_str(),"r");
	if(NULL!=pFile)
	{
		while(fgets(buf,sizeof(buf),pFile)!=NULL)
		{
			strTmp=buf;
			vector_str vContent;
			SplitString(strTmp,",",vContent);
			int iSize = vContent.size();
			CThostFtdcInstrumentField InstrumentInfo;
			memset(&InstrumentInfo,0,sizeof(CThostFtdcInstrumentField));
			if(26==iSize)
			{
				string strkey=vContent[0];

				strcpy(InstrumentInfo.InstrumentID,vContent[0].c_str());
				strcpy(InstrumentInfo.ExchangeID,vContent[1].c_str());
				strcpy(InstrumentInfo.InstrumentName,vContent[2].c_str());
				strcpy(InstrumentInfo.ExchangeInstID,vContent[3].c_str());
				strcpy(InstrumentInfo.ProductID,vContent[4].c_str());
				InstrumentInfo.ProductClass = vContent[5].at(0);
				InstrumentInfo.DeliveryYear = atoi(vContent[6].c_str());
				InstrumentInfo.DeliveryMonth = atoi(vContent[7].c_str());
				InstrumentInfo.MaxMarketOrderVolume = atoi(vContent[8].c_str());
				InstrumentInfo.MinMarketOrderVolume = atoi(vContent[9].c_str());
				InstrumentInfo.MaxLimitOrderVolume = atoi(vContent[10].c_str());
				InstrumentInfo.MinLimitOrderVolume = atoi(vContent[11].c_str());
				InstrumentInfo.VolumeMultiple = atoi(vContent[12].c_str());
				InstrumentInfo.PriceTick = atof(vContent[13].c_str());
				strcpy(InstrumentInfo.CreateDate,vContent[14].c_str());
				strcpy(InstrumentInfo.OpenDate,vContent[15].c_str());
				strcpy(InstrumentInfo.ExpireDate,vContent[16].c_str());
				strcpy(InstrumentInfo.StartDelivDate,vContent[17].c_str());
				strcpy(InstrumentInfo.EndDelivDate,vContent[18].c_str());
				InstrumentInfo.InstLifePhase = vContent[19].at(0);
				InstrumentInfo.IsTrading = atoi(vContent[20].c_str());
				InstrumentInfo.PositionType = vContent[21].at(0);
				InstrumentInfo.PositionDateType = vContent[22].at(0);
				InstrumentInfo.LongMarginRatio = atof(vContent[23].c_str());
				InstrumentInfo.ShortMarginRatio = atof(vContent[24].c_str());
				InstrumentInfo.MaxMarginSideAlgorithm=vContent[25].at(0);

				g_m_InstrumentInfo.insert(make_pair(strkey,InstrumentInfo));

			}
		}
	}
}

///������
void ReadCommissionRate()
{
	g_m_CommissionRate.clear();
	char buf[1024];
	char pPath[256];
	memset(pPath,0,sizeof(pPath));
	memset(buf,0,sizeof(buf));
	_getcwd( pPath, 256 );
	string strTmp;
	string strPath=pPath;
	strPath+="\\CommissionRate.csv";
	FILE *      pFile=fopen(strPath.c_str(),"r");
	if(NULL!=pFile)
	{
		while(fgets(buf,sizeof(buf),pFile)!=NULL)
		{
			strTmp=buf;
			vector_str vContent;
			SplitString(strTmp,",",vContent);
			int iSize = vContent.size();
			CThostFtdcInstrumentCommissionRateField CommissionRate;
			memset(&CommissionRate,0,sizeof(CThostFtdcInstrumentCommissionRateField));
			if(10==iSize)
			{
				string strkey=vContent[0];
				strcpy(CommissionRate.InstrumentID,vContent[0].c_str());
				CommissionRate.InvestorRange = atoi(vContent[1].c_str());
				strcpy(CommissionRate.BrokerID,vContent[2].c_str());
				strcpy(CommissionRate.InvestorID,vContent[3].c_str());
				CommissionRate.OpenRatioByMoney=atof(vContent[4].c_str());
				CommissionRate.OpenRatioByVolume=atof(vContent[5].c_str());
				CommissionRate.CloseRatioByMoney=atof(vContent[6].c_str());
				CommissionRate.CloseRatioByVolume=atof(vContent[7].c_str());
				CommissionRate.CloseTodayRatioByMoney=atof(vContent[8].c_str());
				CommissionRate.CloseTodayRatioByVolume=atof(vContent[9].c_str());
				g_m_CommissionRate.insert(make_pair(strkey,CommissionRate));

			}
		}
	}
}
///����֤����
void ReadMarginRateRate()
{
	g_m_MarginRate.clear();
	char buf[1024];
	char pPath[256];
	char pTmp[256];
	memset(pPath,0,sizeof(pPath));
	memset(buf,0,sizeof(buf));
	_getcwd( pPath, 256 );
	string strTmp;
	string strPath=pPath;
	strPath+="\\MarginRate.csv";
	FILE *      pFile=fopen(strPath.c_str(),"r");
	if(NULL!=pFile)
	{
		while(fgets(buf,sizeof(buf),pFile)!=NULL)
		{
			strTmp=buf;
			vector_str vContent;
			SplitString(strTmp,",",vContent);
			int iSize = vContent.size();
			CThostFtdcInstrumentMarginRateField MarginRate;
			memset(&MarginRate,0,sizeof(CThostFtdcInstrumentMarginRateField));
			if(10==iSize)
			{
				string strkey=vContent[0];
				strcpy(MarginRate.InstrumentID,vContent[0].c_str());
				MarginRate.InvestorRange = atoi(vContent[1].c_str());
				strcpy(MarginRate.BrokerID,vContent[2].c_str());
				strcpy(MarginRate.InvestorID,vContent[3].c_str());
				MarginRate.HedgeFlag=vContent[4].at(0);
				MarginRate.LongMarginRatioByMoney=atof(vContent[5].c_str());
				MarginRate.LongMarginRatioByVolume=atof(vContent[6].c_str());
				MarginRate.ShortMarginRatioByMoney=atof(vContent[7].c_str());
				MarginRate.ShortMarginRatioByVolume=atof(vContent[8].c_str());
				MarginRate.IsRelative=atoi(vContent[9].c_str());
				memset(pTmp,0,sizeof(pTmp));
				sprintf(pTmp,"%s_%c",vContent[0].c_str(),MarginRate.HedgeFlag);
				strkey=pTmp;
				g_m_MarginRate.insert(make_pair(strkey,MarginRate));

			}
		}
	}
}

///��ʱƽ��
DWORD WINAPI Thread_TimeClose(LPVOID pParam)
{
	CTime tm=CTime::GetCurrentTime();

	int nHour = 15;
	int nMin  = 14;
	int nSec  = 0;
	while(true)
	{
		if( nHour==tm.GetHour()     &&
			nMin ==tm.GetMinute()   &&
			nSec ==tm.GetSecond()
			)
		{   
			if(NULL!=g_pTradeApi && NULL!=g_pTradeCTP)
			{
      			map_str_InvestorPosition::iterator iter;
				for(iter = g_m_InvestorPosition.begin();iter != g_m_InvestorPosition.end();iter++)
				{

					/*--------------------ƽ��---------------------------*/
					CThostFtdcInputOrderField req;
					memset(&req, 0, sizeof(req));
					///���͹�˾����
					strcpy(req.BrokerID, BROKER_ID);
					///Ͷ���ߴ���
					strcpy(req.InvestorID, INVESTOR_ID);

					///strcpy(req.OrderRef, ORDER_REF);
					///�û�����
					//	TThostFtdcUserIDType	UserID;
					///�����۸�����: �޼�THOST_FTDC_OPT_LimitPrice
					///�����¼�
					req.OrderPriceType = THOST_FTDC_OPT_LastPrice;
					///���Ͷ���ױ���־
					req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
					///�۸�
					///req.LimitPrice = LIMIT_PRICE;
					///��Ч������: ������Ч
					req.TimeCondition = THOST_FTDC_TC_GFD;
					///GTD����
					//	TThostFtdcDateType	GTDDate;
					///�ɽ�������: �κ�����
					req.VolumeCondition = THOST_FTDC_VC_AV;
					///��С�ɽ���: 1
					req.MinVolume = 1;
					///��������: ����
					req.ContingentCondition = THOST_FTDC_CC_Immediately;
					///ֹ���
					//	TThostFtdcPriceType	StopPrice;
					///ǿƽԭ��: ��ǿƽ
					req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
					///�Զ������־: ��
					req.IsAutoSuspend = 0;
					///ҵ��Ԫ
					//	TThostFtdcBusinessUnitType	BusinessUnit;
					///������
					//	TThostFtdcRequestIDType	RequestID;
					///�û�ǿ����־: ��

					req.UserForceClose = 0;

					/*------------------���޸ĵĲ��֣���------------------------*/
					///��������: 
					//if(iter->second.PosiDirection==THOST_FTDC_PD_Long )
					//  req.Direction = THOST_FTDC_D_Sell;
					//else
					//  req.Direction = THOST_FTDC_D_Buy;
					///��Ͽ�ƽ��־: ƽ
					req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					///��Լ����
					strcpy(req.InstrumentID,iter->second.InstrumentID);
					///��������
					g_pTradeCTP->m_iNextOrderRef++;

					sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
					///�۸� �����¼�ί�е�,
					///req.LimitPrice=4459.0;
					///����
					req.VolumeTotalOriginal =iter->second.TodayPosition;
					/*------------------���޸ĵĲ��֣��壩------------------------*/
					/// ƽ��
					g_iReturn = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);

				}
			}
			break;
		}
		else
		{
			Sleep(900);
			tm=CTime::GetCurrentTime();
		}
	}
	return  1;
};
///��˫�������������һ��ʱ����鶩�����
///1�����ȶ��ɽ���������
///2�����ȶ����ɽ���������
///3�����ȳɽ�����δ�ɽ�����ƽ�ѳɽ���
DWORD WINAPI Thread_CheckOrder( LPVOID pParam )
{

	Sleep(10000);//3*60*1000
	int g_iReturn=0;
	strLocalID * pstrLocalID=(strLocalID *)pParam;
	string strKey=pstrLocalID->OrderSysID_1;
	map_string_Order::iterator iter_1=g_m_Order.find(strKey);
	strKey=pstrLocalID->OrderSysID_2;
	map_string_Order::iterator iter_2=g_m_Order.find(strKey);
	if(   iter_1!=g_m_Order.end()&&
		  iter_2!=g_m_Order.end()
		)
	{
		if( iter_1->second.VolumeTotal==1&&
		    iter_2->second.VolumeTotal==1
		  )
		{
			//���ȶ����ɽ�������
			/*--------------------����(��һ��)---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_1->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_1->second.InvestorID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///����������
			strcpy(OrderAction.ExchangeID,iter_1->second.ExchangeID);
			///�������
			strcpy(OrderAction.OrderSysID,iter_1->second.OrderSysID);
			///������־
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///��Լ����
			strcpy(OrderAction.InstrumentID, iter_1->second.InstrumentID);
			///����һ��
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------����(�ڶ���)---------------------------*/
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_2->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_2->second.InvestorID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///����������
			strcpy(OrderAction.ExchangeID,iter_2->second.ExchangeID);
			///�������
			strcpy(OrderAction.OrderSysID,iter_2->second.OrderSysID);
			///������־
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///��Լ����
			strcpy(OrderAction.InstrumentID, iter_2->second.InstrumentID);
			///���ڶ���
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
		}
		if( iter_1->second.VolumeTotal==1&&
			iter_2->second.VolumeTotal==0
			)
		{///��һ��δ�ɽ����ڶ����ѳɽ�������һ�ȣ�ƽ�ڶ���
			/*--------------------����---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_1->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_1->second.InvestorID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///����������
			strcpy(OrderAction.ExchangeID,iter_1->second.ExchangeID);
			///�������
			strcpy(OrderAction.OrderSysID,iter_1->second.OrderSysID);
			///������־
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///��Լ����
			strcpy(OrderAction.InstrumentID, iter_1->second.InstrumentID);

			//����һ��
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------ƽ��---------------------------*/
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///���͹�˾����
			strcpy(req.BrokerID, BROKER_ID);
			///Ͷ���ߴ���
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///�û�����
			//	TThostFtdcUserIDType	UserID;
			///�����۸�����: �޼�
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///���Ͷ���ױ���־
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///�۸�
			req.LimitPrice = LIMIT_PRICE;
			///��Ч������: ������Ч
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD����
			//	TThostFtdcDateType	GTDDate;
			///�ɽ�������: �κ�����
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///��С�ɽ���: 1
			req.MinVolume = 1;
			///��������: ����
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///ֹ���
			//	TThostFtdcPriceType	StopPrice;
			///ǿƽԭ��: ��ǿƽ
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///�Զ������־: ��
			req.IsAutoSuspend = 0;
			///ҵ��Ԫ
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///������
			//	TThostFtdcRequestIDType	RequestID;
			///�û�ǿ����־: ��

			req.UserForceClose = 0;
			/*------------------���޸ĵĲ��֣���------------------------*/
			///��������: 
			if(iter_2->second.Direction ==THOST_FTDC_D_Buy )
			  req.Direction = THOST_FTDC_D_Sell;
			else
		      req.Direction = THOST_FTDC_D_Buy;
			///��Ͽ�ƽ��־: ƽ
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			///��Լ����
			strcpy(req.InstrumentID, iter_2->second.InstrumentID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;

			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///�۸�
			req.LimitPrice=4459.0;
			///����
			req.VolumeTotalOriginal =1;
			/*------------------���޸ĵĲ��֣��壩------------------------*/
			/// ƽ�ڶ���
			g_iReturn = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);
		}
		if( iter_1->second.VolumeTotal==0&&
			iter_2->second.VolumeTotal==1
			)
		{///��һ���ѳɽ����ڶ���δ�ɽ���ƽ��һ�ȣ����ڶ���
			/*--------------------����---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_2->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_2->second.InvestorID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///����������
			strcpy(OrderAction.ExchangeID,iter_2->second.ExchangeID);
			///�������
			strcpy(OrderAction.OrderSysID,iter_2->second.OrderSysID);
			///������־
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///��Լ����
			strcpy(OrderAction.InstrumentID, iter_2->second.InstrumentID);
			///���ڶ���
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------ƽ��---------------------------*/
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///���͹�˾����
			strcpy(req.BrokerID, BROKER_ID);
			///Ͷ���ߴ���
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///�û�����
			//	TThostFtdcUserIDType	UserID;
			///�����۸�����: �޼�
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///���Ͷ���ױ���־
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///�۸�
			req.LimitPrice = LIMIT_PRICE;
			///��Ч������: ������Ч
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD����
			//	TThostFtdcDateType	GTDDate;
			///�ɽ�������: �κ�����
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///��С�ɽ���: 1
			req.MinVolume = 1;
			///��������: ����
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///ֹ���
			//	TThostFtdcPriceType	StopPrice;
			///ǿƽԭ��: ��ǿƽ
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///�Զ������־: ��
			req.IsAutoSuspend = 0;
			///ҵ��Ԫ
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///������
			//	TThostFtdcRequestIDType	RequestID;
			///�û�ǿ����־: ��

			req.UserForceClose = 0;
			/*------------------���޸ĵĲ��֣���------------------------*/
			///��������: 
			if(iter_1->second.Direction ==THOST_FTDC_D_Buy )
			  req.Direction = THOST_FTDC_D_Sell;
			else
		      req.Direction = THOST_FTDC_D_Buy;
			///��Ͽ�ƽ��־: ƽ
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			///��Լ����
			strcpy(req.InstrumentID, iter_1->second.InstrumentID);
			///��������
			g_pTradeCTP->m_iNextOrderRef++;

			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///�۸�
			req.LimitPrice=4410.0;
			///����
			req.VolumeTotalOriginal =1;
			/*------------------���޸ĵĲ��֣��壩------------------------*/
			/// ƽ��һ��
			g_iReturn = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);

		}
	}

	return  1;
};
void GetHistoryData(vector_MD_K & v_MD_K, TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin)
{
	  CGet_MD_K Tmp_MD_K;
	  Tmp_MD_K.GetHistoryData(v_MD_K,InstrumentID,amount,nMin);
 };
///��CTP����
void ParkedOrder()
{
	CThostFtdcParkedOrderField ParkedOrder;
	memset(&ParkedOrder, 0, sizeof(ParkedOrder));

	///���͹�˾����
	strcpy(ParkedOrder.BrokerID, BROKER_ID);
	///Ͷ���ߴ���
	strcpy(ParkedOrder.InvestorID, INVESTOR_ID);
	///��Լ����
	strcpy(ParkedOrder.InstrumentID, "IF1505");
	///��������
	g_pTradeCTP->m_iNextOrderRef++;
	sprintf(ParkedOrder.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
	///�û�����
	strcpy(ParkedOrder.UserID, INVESTOR_ID);
	///�����۸�����
	ParkedOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///��������
	ParkedOrder.Direction = THOST_FTDC_D_Buy;
	///��Ͽ�ƽ��־
	ParkedOrder.CombOffsetFlag[0]=THOST_FTDC_OF_Open;
	///���Ͷ���ױ���־
	ParkedOrder.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	ParkedOrder.LimitPrice = 0.0;
	///����
	ParkedOrder.VolumeTotalOriginal=1;
	///��Ч������
	ParkedOrder.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	///ParkedOrder.GTDDate
	///�ɽ�������: �κ�����
	ParkedOrder.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���
	ParkedOrder.MinVolume=1;
	///��������(���¼�С��������),ע�⣺������Ԥ�񵥵Ĺؼ���
	ParkedOrder.ContingentCondition = THOST_FTDC_CC_LastPriceLesserThanStopPrice;
	///ֹ���
	///ParkedOrder.StopPrice ==

	///ǿƽԭ��
	ParkedOrder.ForceCloseReason=THOST_FTDC_FCC_NotForceClose;
	///�Զ������־
	ParkedOrder.IsAutoSuspend=0;
	///ҵ��Ԫ
	///ParkedOrder.BusinessUnit;
	///������
	///ParkedOrder.RequestID;
	///�û�ǿ����־
	ParkedOrder.UserForceClose=0;
	///����������
	///ParkedOrder.ExchangeID;
	///Ԥ�񱨵����
	///ParkedOrder.ParkedOrderID;
	///�û�����
	///ParkedOrder.UserType;
	///Ԥ��״̬
	///ParkedOrder.Status;
	///�������
	///ParkedOrder.ErrorID;
	///������Ϣ
	///ParkedOrder.ErrorMsg;
	///��������־
	///ParkedOrder.IsSwapOrder;
	g_iReturn = g_pTradeApi->ReqParkedOrderInsert(&ParkedOrder, ++g_iRequestID);
};
void ConditionOrder()
{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///���͹�˾����
			strcpy(req.BrokerID, BROKER_ID);
			///Ͷ���ߴ���
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///�û�����
			//	TThostFtdcUserIDType	UserID;
			///�����۸�����: �޼�
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;///THOST_FTDC_OPT_LimitPrice
			///���Ͷ���ױ���־
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///�۸�
			///req.LimitPrice = 4725;
			///��Ч������: ������Ч
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD����
			//	TThostFtdcDateType	GTDDate;
			///�ɽ�������: �κ�����
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///��С�ɽ���: 1
			req.MinVolume = 1;
			///��������: 
			///����
			/// THOST_FTDC_CC_Immediately '1'
			///ֹ��
			/// THOST_FTDC_CC_Touch '2'
			///ֹӮ
			/// THOST_FTDC_CC_TouchProfit '3'
			///Ԥ��
			/// THOST_FTDC_CC_ParkedOrder '4'
			///���¼۴���������
			/// THOST_FTDC_CC_LastPriceGreaterThanStopPrice '5'
			///���¼۴��ڵ���������
			/// THOST_FTDC_CC_LastPriceGreaterEqualStopPrice '6'
			///���¼�С��������
			/// THOST_FTDC_CC_LastPriceLesserThanStopPrice '7'
			///���¼�С�ڵ���������
			/// THOST_FTDC_CC_LastPriceLesserEqualStopPrice '8'
			///��һ�۴���������
			/// THOST_FTDC_CC_AskPriceGreaterThanStopPrice '9'
			///��һ�۴��ڵ���������
			/// THOST_FTDC_CC_AskPriceGreaterEqualStopPrice 'A'
			///��һ��С��������
			/// THOST_FTDC_CC_AskPriceLesserThanStopPrice 'B'
			///��һ��С�ڵ���������
			/// THOST_FTDC_CC_AskPriceLesserEqualStopPrice 'C'
			///��һ�۴���������
			/// THOST_FTDC_CC_BidPriceGreaterThanStopPrice 'D'
			///��һ�۴��ڵ���������
			/// THOST_FTDC_CC_BidPriceGreaterEqualStopPrice 'E'
			///��һ��С��������
			/// THOST_FTDC_CC_BidPriceLesserThanStopPrice 'F'
			///��һ��С�ڵ���������
			/// THOST_FTDC_CC_BidPriceLesserEqualStopPrice 'H'

			req.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterThanStopPrice;
			
			///TThostFtdcPriceType	StopPrice;
			req.StopPrice = 4720.0;
			///ǿƽԭ��: ��ǿƽ
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///�Զ������־: ��
			req.IsAutoSuspend = 0;
			///ҵ��Ԫ
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///������
			//	TThostFtdcRequestIDType	RequestID;
			///�û�ǿ����־: ��

			req.UserForceClose = 0;
			/*------------------���޸ĵĲ��֣���------------------------*/
			///��������: 
			req.Direction = THOST_FTDC_D_Sell;//THOST_FTDC_D_Buy THOST_FTDC_D_Sell
			///��Ͽ�ƽ��־: ����
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;///THOST_FTDC_OF_Open THOST_FTDC_OF_Close
			///��Լ����
			strcpy(req.InstrumentID, "IF1505");
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///�۸�
			req.LimitPrice=4720.0;
			///����
			req.VolumeTotalOriginal =6;
			/*------------------���޸ĵĲ��֣��壩------------------------*/
			/// ��һ��
			int iResult = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);
}
void main(void)
{
#if 0
	///��ȡ����
	ReadCommissionRate();
	///����֤����
	ReadMarginRateRate();
	///��ȡ��Լ��Ϣ
	ReadInstrumentInfo();
#endif
	char* mymsg=new char[MAXBYTE];
	///��������ʵ��
	g_pQuoteCTP = new CQuoteCTP();
	///�����������
	g_pMdApi =CThostFtdcMdApi::CreateFtdcMdApi("");
	///ע��һ�¼������ʵ�� 
	g_pMdApi->RegisterSpi(g_pQuoteCTP); 
	///���ý����й�ϵͳ�������ĵ�ַ������ע������ַ���� 
	g_pMdApi->RegisterFront(MD_FRONT_ADDR); 
	///ʹ�ͻ��˿�ʼ���̨����������
	g_pMdApi->Init(); 

	///�������׶���
	g_pTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi("");			
	///��������ʵ��
	g_pTradeCTP = new CTradeCTP();
	///ע��һ�¼����״����ʵ�� 
	g_pTradeApi->RegisterSpi((CThostFtdcTraderSpi*)g_pTradeCTP);
	///���Ĺ�����
	g_pTradeApi->SubscribePublicTopic(THOST_TERT_RESTART);	
	///����˽����
	g_pTradeApi->SubscribePrivateTopic(THOST_TERT_RESUME);	///THOST_TERT_RESTART
	///���ý����й�ϵͳ���׷���ĵ�ַ��
	g_pTradeApi->RegisterFront(FRONT_ADDR);		
	///ʹ�ͻ��˿�ʼ���̨����������
	g_pTradeApi->Init();

	while( true )
	{
		system("cls"); 
		printf("/**********************************/\n");
		printf("/* 1:�˳�\n");
		printf("/* 2:���������\n");
		printf("/* 3:Ԥ�񵥣������ڱ��أ�����������ִ���µ�ָ��\n");
		printf("/* 4:Ԥ��ʱ��ƽ��,��15:14:00\n");
		printf("/* 5:Ԥ��,��CTP����\n");
		printf("/* 6:ȡ��ʷK������\n");
		printf("/* 7:������,��CTP����\n");
		printf("/**********************************/\n");
		gets(mymsg);
		if(atol(mymsg)==1) break;
		if(atol(mymsg)==2)
		{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///���͹�˾����
			strcpy(req.BrokerID, BROKER_ID);
			///Ͷ���ߴ���
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///�û�����
			//	TThostFtdcUserIDType	UserID;
			///�����۸�����: �޼�
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///���Ͷ���ױ���־
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///�۸�
			req.LimitPrice = LIMIT_PRICE;
			///��Ч������: ������Ч
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD����
			//	TThostFtdcDateType	GTDDate;
			///�ɽ�������: �κ�����
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///��С�ɽ���: 1
			req.MinVolume = 1;
			///��������: ����
			req.ContingentCondition = THOST_FTDC_CC_Immediately;////THOST_FTDC_CC_Immediately;
			///ֹ���
			///TThostFtdcPriceType	StopPrice;
			///req.StopPrice = 4724;
			///ǿƽԭ��: ��ǿƽ
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///�Զ������־: ��
			req.IsAutoSuspend = 0;
			///ҵ��Ԫ
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///������
			//	TThostFtdcRequestIDType	RequestID;
			///�û�ǿ����־: ��

			req.UserForceClose = 0;
			/*------------------���޸ĵĲ��֣���------------------------*/
			///��������: 
			req.Direction = THOST_FTDC_D_Buy;
			///��Ͽ�ƽ��־: ����
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			///��Լ����
			strcpy(req.InstrumentID, "IF1505");
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///�۸�
			req.LimitPrice=4650;
			///����
			req.VolumeTotalOriginal =1;
			/*------------------���޸ĵĲ��֣��壩------------------------*/
			/// ��һ��
			int iResult = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);

			
			if(0==iResult)
			{
				strLocalID * pstrLocalID=new strLocalID;
				pstrLocalID->Init();
				pstrLocalID->FrontID = FRONT_ID;
				pstrLocalID->SessionID = SESSION_ID;
				strcpy(pstrLocalID->OrderRef_1, req.OrderRef);
				string strKey=req.OrderRef;
				///g_m_Local_Ref.insert(make_pair(strKey,pstrLocalID));

				/// �ڶ���

				/*------------------���޸ĵĲ��֣���------------------------*/
				///��������: 
				req.Direction = THOST_FTDC_D_Sell;
				///��Ͽ�ƽ��־: ����
				req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				///��Լ����
				strcpy(req.InstrumentID, "IF1505");
				///��������
				g_pTradeCTP->m_iNextOrderRef++;
				sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
				///�۸�
				req.LimitPrice=4480;
				///����
				req.VolumeTotalOriginal =1;
				/*------------------���޸ĵĲ��֣��壩------------------------*/
				strcpy(pstrLocalID->OrderRef_2, req.OrderRef);
				iResult = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);

				memset(g_cKey,0,1024);
				sprintf_s(g_cKey,"%d_%x_%s",FRONT_ID,SESSION_ID,pstrLocalID->OrderRef_1);
				strKey=g_cKey;
				g_m_Local_Ref.insert(make_pair(strKey,pstrLocalID));
				memset(g_cKey,0,1024);
				sprintf_s(g_cKey,"%d_%x_%s",FRONT_ID,SESSION_ID,pstrLocalID->OrderRef_2);
				strKey=g_cKey;
				g_m_Local_Ref.insert(make_pair(strKey,pstrLocalID));
				CreateThread(NULL,0,Thread_CheckOrder,(LPVOID)pstrLocalID,0,NULL);

			}
		}
		/******************����Ԥ�񵥴����(s)*****************************************/
		if(atol(mymsg)==3)
		{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///���͹�˾����
			strcpy(req.BrokerID, BROKER_ID);
			///Ͷ���ߴ���
			strcpy(req.InvestorID, INVESTOR_ID);
			///strcpy(req.OrderRef, ORDER_REF);
			///�û�����
			//	TThostFtdcUserIDType	UserID;
			///�����۸�����: �޼�
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///���Ͷ���ױ���־
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///�۸�
			req.LimitPrice = LIMIT_PRICE;
			///��Ч������: ������Ч
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD����
			//	TThostFtdcDateType	GTDDate;
			///�ɽ�������: �κ�����
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///��С�ɽ���: 1
			req.MinVolume = 1;
			///��������: ����
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///ֹ���
			//	TThostFtdcPriceType	StopPrice;
			///ǿƽԭ��: ��ǿƽ
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///�Զ������־: ��
			req.IsAutoSuspend = 0;
			///ҵ��Ԫ
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///������
			//	TThostFtdcRequestIDType	RequestID;
			///�û�ǿ����־: ��
			req.UserForceClose = 0;
			///��������: 
			req.Direction = THOST_FTDC_D_Buy;
			///��Ͽ�ƽ��־: ����
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			///��Լ����
			strcpy(req.InstrumentID, "IF1509");
			///��������
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///�۸�
			req.LimitPrice=4360;
			///����
			req.VolumeTotalOriginal =1;
			if(NULL!=g_pMdApi)
			{
				///���ĸ�����
				char * ppInstrument[1];
				ppInstrument[0]=req.InstrumentID;
				g_pMdApi->SubscribeMarketData(ppInstrument,1);
				///ע���ͷŵ�ʱ��Ҫ��������������ʱ���Ͼͻ�����ʧ�������������õİ취��
				g_v_InputOrder.push_back(req);
			}
		}
		/******************����Ԥ�񵥴����(e)*****************************************/
		/******************����Ԥ��ʱ��ƽ�����(s)*****************************************/
		if(atol(mymsg)==4)
		{
			if(NULL!=g_pTradeApi)
			{
				
				CreateThread(NULL,0,Thread_TimeClose,NULL,0,NULL);
			}
			
		}
		/******************����Ԥ��ʱ��ƽ�����(e)*****************************************/
		if(atol(mymsg)==5)
		{
			ParkedOrder();
		}
		if(atol(mymsg)==6)
		{
			GetHistoryData(g_v_MD_K,"au1508",3,1);
		}
		if(atol(mymsg)==7)
		{
			ConditionOrder();
		}
	}
	delete mymsg;
	mymsg = NULL;
	map_str_pstrLocalID::iterator iter;
	for(iter=g_m_Local_Ref.begin();iter!=g_m_Local_Ref.end();iter++)
	{
		if(NULL!=iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	g_m_Local_Ref.clear();

	if(NULL!=g_pTradeCTP)
	{

		delete g_pTradeCTP;
		g_pTradeCTP = NULL;

	}
	if(NULL!=g_pTradeApi)
	{
		g_pTradeApi->Release();
		g_pTradeApi=NULL;
	}

	if(NULL!=g_pQuoteCTP)
	{

		delete g_pQuoteCTP;
		g_pQuoteCTP = NULL;

	}
	if(NULL!=g_pMdApi)
	{
		g_pMdApi->Release();
		g_pMdApi=NULL;
	}

}
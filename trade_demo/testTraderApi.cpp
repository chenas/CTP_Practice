/************************************************************/
/* 功能：程序化交易功能点Demo程序
/* 公司：中信期货有限公司
/* 作者：陈青山
/* 群QQ：89777472
/* 初发：20140819
/* 修改记录：
20140108: 套利单检查
20140116：预埋单，保留在本地，满足条件，执行下单指令
20140118：预定时间平仓,如15:14:00
20140121：预埋单,由CTP处理;
20140216：取历史K线数据
20140425：条件单,由CTP处理
20140428：持仓本地化，显示风险度
20140528：生成K线
/************************************************************/
#include "stdafx.h"
#include "Get_MD_K.h"


// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用

///Key是 FRONT_ID_SESSION_ID_OrderRef
map_str_pstrLocalID g_m_Local_Ref; 

///Key是TradeID
map_string_Trade       g_m_Trade;

///Key是OrderSysID
map_string_Order       g_m_Order;

///用于预埋单
vector_InputOrder      g_v_InputOrder;

///得到历史K线
vector_MD_K            g_v_MD_K;

///本地持仓，Key是 IF1505_0 表示IF1505的多仓
map_str_InvestorPosition        g_m_InvestorPosition;

///正在交易的和约信息
map_str_InstrumentInfo          g_m_InstrumentInfo;

///保证金率
map_str_MarginRate              g_m_MarginRate;

///手续费率
map_str_CommissionRate          g_m_CommissionRate;

///最新行情列表
map_srt_DepthMarketData         g_m_DepthMarketData;

///合约与合约种类对照关系
map_str_str                     g_m_Instrument_ProductID;

///成交
vector_Trade                    g_v_Trade;

///帐户
CThostFtdcTradingAccountField   g_TradingAccount;

char                   g_cKey[1024];
int                    g_iReturn;
// UserApi对象
CThostFtdcTraderApi*   g_pTradeApi;
CThostFtdcMdApi*       g_pMdApi;
CTradeCTP*             g_pTradeCTP;
CQuoteCTP*             g_pQuoteCTP;





// 请求编号
int g_iRequestID = 0;
// 配置参数
///"tcp://ctpfz1-front1.citicsf.com:51205"
///"tcp://180.168.146.181:10200"
#if 1
char  FRONT_ADDR[] = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";		// 前置地址
char  MD_FRONT_ADDR[] = "tcp://asp-sim2-front1.financial-trading-platform.com:26213";	// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "2030";						// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "875000";				    // 投资者代码
TThostFtdcPasswordType  PASSWORD = "123456";						// 用户密码
#else
char  FRONT_ADDR[] = "tcp://180.168.146.181:10200";					// 前置地址
char  MD_FRONT_ADDR[] = "tcp://180.168.146.181:10210";				// 前置地址
TThostFtdcBrokerIDType	BROKER_ID = "";							// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "";						// 投资者代码
TThostFtdcPasswordType  PASSWORD = "";					// 用户密码
#endif


TThostFtdcPriceType	LIMIT_PRICE = 38850;				// 价格
///把一个字符串分割成字符串数组
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
///读取合约信息
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

///读费率
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
///读保证金率
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

///定时平仓
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

					/*--------------------平仓---------------------------*/
					CThostFtdcInputOrderField req;
					memset(&req, 0, sizeof(req));
					///经纪公司代码
					strcpy(req.BrokerID, BROKER_ID);
					///投资者代码
					strcpy(req.InvestorID, INVESTOR_ID);

					///strcpy(req.OrderRef, ORDER_REF);
					///用户代码
					//	TThostFtdcUserIDType	UserID;
					///报单价格条件: 限价THOST_FTDC_OPT_LimitPrice
					///以最新价
					req.OrderPriceType = THOST_FTDC_OPT_LastPrice;
					///组合投机套保标志
					req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
					///价格
					///req.LimitPrice = LIMIT_PRICE;
					///有效期类型: 当日有效
					req.TimeCondition = THOST_FTDC_TC_GFD;
					///GTD日期
					//	TThostFtdcDateType	GTDDate;
					///成交量类型: 任何数量
					req.VolumeCondition = THOST_FTDC_VC_AV;
					///最小成交量: 1
					req.MinVolume = 1;
					///触发条件: 立即
					req.ContingentCondition = THOST_FTDC_CC_Immediately;
					///止损价
					//	TThostFtdcPriceType	StopPrice;
					///强平原因: 非强平
					req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
					///自动挂起标志: 否
					req.IsAutoSuspend = 0;
					///业务单元
					//	TThostFtdcBusinessUnitType	BusinessUnit;
					///请求编号
					//	TThostFtdcRequestIDType	RequestID;
					///用户强评标志: 否

					req.UserForceClose = 0;

					/*------------------可修改的部分（ｓ）------------------------*/
					///买卖方向: 
					//if(iter->second.PosiDirection==THOST_FTDC_PD_Long )
					//  req.Direction = THOST_FTDC_D_Sell;
					//else
					//  req.Direction = THOST_FTDC_D_Buy;
					///组合开平标志: 平
					req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					///合约代码
					strcpy(req.InstrumentID,iter->second.InstrumentID);
					///报单引用
					g_pTradeCTP->m_iNextOrderRef++;

					sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
					///价格 以最新价委托的,
					///req.LimitPrice=4459.0;
					///数量
					req.VolumeTotalOriginal =iter->second.TodayPosition;
					/*------------------可修改的部分（ｅ）------------------------*/
					/// 平仓
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
///下双腿套利单，间隔一定时间后检查订单情况
///1、两腿都成交，不处理
///2、两腿都不成交，撤两腿
///3、单腿成交，撤未成交单，平已成交单
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
			//两腿都不成交，撤单
			/*--------------------撤单(第一腿)---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_1->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_1->second.InvestorID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///交易所代码
			strcpy(OrderAction.ExchangeID,iter_1->second.ExchangeID);
			///报单编号
			strcpy(OrderAction.OrderSysID,iter_1->second.OrderSysID);
			///操作标志
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///合约代码
			strcpy(OrderAction.InstrumentID, iter_1->second.InstrumentID);
			///撤第一腿
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------撤单(第二腿)---------------------------*/
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_2->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_2->second.InvestorID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///交易所代码
			strcpy(OrderAction.ExchangeID,iter_2->second.ExchangeID);
			///报单编号
			strcpy(OrderAction.OrderSysID,iter_2->second.OrderSysID);
			///操作标志
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///合约代码
			strcpy(OrderAction.InstrumentID, iter_2->second.InstrumentID);
			///撤第二腿
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
		}
		if( iter_1->second.VolumeTotal==1&&
			iter_2->second.VolumeTotal==0
			)
		{///第一腿未成交，第二腿已成交，撤第一腿，平第二腿
			/*--------------------撤单---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_1->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_1->second.InvestorID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///交易所代码
			strcpy(OrderAction.ExchangeID,iter_1->second.ExchangeID);
			///报单编号
			strcpy(OrderAction.OrderSysID,iter_1->second.OrderSysID);
			///操作标志
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///合约代码
			strcpy(OrderAction.InstrumentID, iter_1->second.InstrumentID);

			//撤第一腿
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------平仓---------------------------*/
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy(req.BrokerID, BROKER_ID);
			///投资者代码
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///报单价格条件: 限价
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///组合投机套保标志
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///价格
			req.LimitPrice = LIMIT_PRICE;
			///有效期类型: 当日有效
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD日期
			//	TThostFtdcDateType	GTDDate;
			///成交量类型: 任何数量
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///最小成交量: 1
			req.MinVolume = 1;
			///触发条件: 立即
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///止损价
			//	TThostFtdcPriceType	StopPrice;
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///自动挂起标志: 否
			req.IsAutoSuspend = 0;
			///业务单元
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///用户强评标志: 否

			req.UserForceClose = 0;
			/*------------------可修改的部分（ｓ）------------------------*/
			///买卖方向: 
			if(iter_2->second.Direction ==THOST_FTDC_D_Buy )
			  req.Direction = THOST_FTDC_D_Sell;
			else
		      req.Direction = THOST_FTDC_D_Buy;
			///组合开平标志: 平
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			///合约代码
			strcpy(req.InstrumentID, iter_2->second.InstrumentID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;

			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///价格
			req.LimitPrice=4459.0;
			///数量
			req.VolumeTotalOriginal =1;
			/*------------------可修改的部分（ｅ）------------------------*/
			/// 平第二腿
			g_iReturn = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);
		}
		if( iter_1->second.VolumeTotal==0&&
			iter_2->second.VolumeTotal==1
			)
		{///第一腿已成交，第二腿未成交，平第一腿，撤第二腿
			/*--------------------撤单---------------------------*/
			CThostFtdcInputOrderActionField OrderAction;
			memset(&OrderAction, 0, sizeof(OrderAction));
			strcpy(OrderAction.BrokerID, iter_2->second.BrokerID);
			strcpy(OrderAction.InvestorID, iter_2->second.InvestorID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(OrderAction.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///交易所代码
			strcpy(OrderAction.ExchangeID,iter_2->second.ExchangeID);
			///报单编号
			strcpy(OrderAction.OrderSysID,iter_2->second.OrderSysID);
			///操作标志
			OrderAction.ActionFlag = THOST_FTDC_AF_Delete;
			///合约代码
			strcpy(OrderAction.InstrumentID, iter_2->second.InstrumentID);
			///撤第二腿
			g_iReturn = g_pTradeApi->ReqOrderAction(&OrderAction,++g_iRequestID);
			/*--------------------平仓---------------------------*/
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy(req.BrokerID, BROKER_ID);
			///投资者代码
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///报单价格条件: 限价
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///组合投机套保标志
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///价格
			req.LimitPrice = LIMIT_PRICE;
			///有效期类型: 当日有效
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD日期
			//	TThostFtdcDateType	GTDDate;
			///成交量类型: 任何数量
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///最小成交量: 1
			req.MinVolume = 1;
			///触发条件: 立即
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///止损价
			//	TThostFtdcPriceType	StopPrice;
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///自动挂起标志: 否
			req.IsAutoSuspend = 0;
			///业务单元
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///用户强评标志: 否

			req.UserForceClose = 0;
			/*------------------可修改的部分（ｓ）------------------------*/
			///买卖方向: 
			if(iter_1->second.Direction ==THOST_FTDC_D_Buy )
			  req.Direction = THOST_FTDC_D_Sell;
			else
		      req.Direction = THOST_FTDC_D_Buy;
			///组合开平标志: 平
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			///合约代码
			strcpy(req.InstrumentID, iter_1->second.InstrumentID);
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;

			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///价格
			req.LimitPrice=4410.0;
			///数量
			req.VolumeTotalOriginal =1;
			/*------------------可修改的部分（ｅ）------------------------*/
			/// 平第一腿
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
///由CTP处理
void ParkedOrder()
{
	CThostFtdcParkedOrderField ParkedOrder;
	memset(&ParkedOrder, 0, sizeof(ParkedOrder));

	///经纪公司代码
	strcpy(ParkedOrder.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(ParkedOrder.InvestorID, INVESTOR_ID);
	///合约代码
	strcpy(ParkedOrder.InstrumentID, "IF1505");
	///报单引用
	g_pTradeCTP->m_iNextOrderRef++;
	sprintf(ParkedOrder.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
	///用户代码
	strcpy(ParkedOrder.UserID, INVESTOR_ID);
	///报单价格条件
	ParkedOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///买卖方向
	ParkedOrder.Direction = THOST_FTDC_D_Buy;
	///组合开平标志
	ParkedOrder.CombOffsetFlag[0]=THOST_FTDC_OF_Open;
	///组合投机套保标志
	ParkedOrder.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	ParkedOrder.LimitPrice = 0.0;
	///数量
	ParkedOrder.VolumeTotalOriginal=1;
	///有效期类型
	ParkedOrder.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD日期
	///ParkedOrder.GTDDate
	///成交量类型: 任何数量
	ParkedOrder.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量
	ParkedOrder.MinVolume=1;
	///触发条件(最新价小于条件价),注意：这里是预埋单的关键点
	ParkedOrder.ContingentCondition = THOST_FTDC_CC_LastPriceLesserThanStopPrice;
	///止损价
	///ParkedOrder.StopPrice ==

	///强平原因
	ParkedOrder.ForceCloseReason=THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志
	ParkedOrder.IsAutoSuspend=0;
	///业务单元
	///ParkedOrder.BusinessUnit;
	///请求编号
	///ParkedOrder.RequestID;
	///用户强评标志
	ParkedOrder.UserForceClose=0;
	///交易所代码
	///ParkedOrder.ExchangeID;
	///预埋报单编号
	///ParkedOrder.ParkedOrderID;
	///用户类型
	///ParkedOrder.UserType;
	///预埋单状态
	///ParkedOrder.Status;
	///错误代码
	///ParkedOrder.ErrorID;
	///错误信息
	///ParkedOrder.ErrorMsg;
	///互换单标志
	///ParkedOrder.IsSwapOrder;
	g_iReturn = g_pTradeApi->ReqParkedOrderInsert(&ParkedOrder, ++g_iRequestID);
};
void ConditionOrder()
{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy(req.BrokerID, BROKER_ID);
			///投资者代码
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///报单价格条件: 限价
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;///THOST_FTDC_OPT_LimitPrice
			///组合投机套保标志
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///价格
			///req.LimitPrice = 4725;
			///有效期类型: 当日有效
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD日期
			//	TThostFtdcDateType	GTDDate;
			///成交量类型: 任何数量
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///最小成交量: 1
			req.MinVolume = 1;
			///触发条件: 
			///立即
			/// THOST_FTDC_CC_Immediately '1'
			///止损
			/// THOST_FTDC_CC_Touch '2'
			///止赢
			/// THOST_FTDC_CC_TouchProfit '3'
			///预埋单
			/// THOST_FTDC_CC_ParkedOrder '4'
			///最新价大于条件价
			/// THOST_FTDC_CC_LastPriceGreaterThanStopPrice '5'
			///最新价大于等于条件价
			/// THOST_FTDC_CC_LastPriceGreaterEqualStopPrice '6'
			///最新价小于条件价
			/// THOST_FTDC_CC_LastPriceLesserThanStopPrice '7'
			///最新价小于等于条件价
			/// THOST_FTDC_CC_LastPriceLesserEqualStopPrice '8'
			///卖一价大于条件价
			/// THOST_FTDC_CC_AskPriceGreaterThanStopPrice '9'
			///卖一价大于等于条件价
			/// THOST_FTDC_CC_AskPriceGreaterEqualStopPrice 'A'
			///卖一价小于条件价
			/// THOST_FTDC_CC_AskPriceLesserThanStopPrice 'B'
			///卖一价小于等于条件价
			/// THOST_FTDC_CC_AskPriceLesserEqualStopPrice 'C'
			///买一价大于条件价
			/// THOST_FTDC_CC_BidPriceGreaterThanStopPrice 'D'
			///买一价大于等于条件价
			/// THOST_FTDC_CC_BidPriceGreaterEqualStopPrice 'E'
			///买一价小于条件价
			/// THOST_FTDC_CC_BidPriceLesserThanStopPrice 'F'
			///买一价小于等于条件价
			/// THOST_FTDC_CC_BidPriceLesserEqualStopPrice 'H'

			req.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterThanStopPrice;
			
			///TThostFtdcPriceType	StopPrice;
			req.StopPrice = 4720.0;
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///自动挂起标志: 否
			req.IsAutoSuspend = 0;
			///业务单元
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///用户强评标志: 否

			req.UserForceClose = 0;
			/*------------------可修改的部分（ｓ）------------------------*/
			///买卖方向: 
			req.Direction = THOST_FTDC_D_Sell;//THOST_FTDC_D_Buy THOST_FTDC_D_Sell
			///组合开平标志: 开仓
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;///THOST_FTDC_OF_Open THOST_FTDC_OF_Close
			///合约代码
			strcpy(req.InstrumentID, "IF1505");
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///价格
			req.LimitPrice=4720.0;
			///数量
			req.VolumeTotalOriginal =6;
			/*------------------可修改的部分（ｅ）------------------------*/
			/// 第一腿
			int iResult = g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);
}
void main(void)
{
#if 0
	///读取费率
	ReadCommissionRate();
	///读保证金率
	ReadMarginRateRate();
	///读取合约信息
	ReadInstrumentInfo();
#endif
	char* mymsg=new char[MAXBYTE];
	///创建行情实例
	g_pQuoteCTP = new CQuoteCTP();
	///建立行情对象
	g_pMdApi =CThostFtdcMdApi::CreateFtdcMdApi("");
	///注册一事件处理的实例 
	g_pMdApi->RegisterSpi(g_pQuoteCTP); 
	///设置交易托管系统行情服务的地址，可以注册多个地址备用 
	g_pMdApi->RegisterFront(MD_FRONT_ADDR); 
	///使客户端开始与后台服务建立连接
	g_pMdApi->Init(); 

	///建立交易对象
	g_pTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi("");			
	///创建行情实例
	g_pTradeCTP = new CTradeCTP();
	///注册一事件交易处理的实例 
	g_pTradeApi->RegisterSpi((CThostFtdcTraderSpi*)g_pTradeCTP);
	///订阅公共流
	g_pTradeApi->SubscribePublicTopic(THOST_TERT_RESTART);	
	///订阅私有流
	g_pTradeApi->SubscribePrivateTopic(THOST_TERT_RESUME);	///THOST_TERT_RESTART
	///设置交易托管系统交易服务的地址，
	g_pTradeApi->RegisterFront(FRONT_ADDR);		
	///使客户端开始与后台服务建立连接
	g_pTradeApi->Init();

	while( true )
	{
		system("cls"); 
		printf("/**********************************/\n");
		printf("/* 1:退出\n");
		printf("/* 2:套利单检查\n");
		printf("/* 3:预埋单，保留在本地，满足条件，执行下单指令\n");
		printf("/* 4:预定时间平仓,如15:14:00\n");
		printf("/* 5:预埋单,由CTP处理\n");
		printf("/* 6:取历史K线数据\n");
		printf("/* 7:条件单,由CTP处理\n");
		printf("/**********************************/\n");
		gets(mymsg);
		if(atol(mymsg)==1) break;
		if(atol(mymsg)==2)
		{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy(req.BrokerID, BROKER_ID);
			///投资者代码
			strcpy(req.InvestorID, INVESTOR_ID);

			///strcpy(req.OrderRef, ORDER_REF);
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///报单价格条件: 限价
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///组合投机套保标志
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///价格
			req.LimitPrice = LIMIT_PRICE;
			///有效期类型: 当日有效
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD日期
			//	TThostFtdcDateType	GTDDate;
			///成交量类型: 任何数量
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///最小成交量: 1
			req.MinVolume = 1;
			///触发条件: 立即
			req.ContingentCondition = THOST_FTDC_CC_Immediately;////THOST_FTDC_CC_Immediately;
			///止损价
			///TThostFtdcPriceType	StopPrice;
			///req.StopPrice = 4724;
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///自动挂起标志: 否
			req.IsAutoSuspend = 0;
			///业务单元
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///用户强评标志: 否

			req.UserForceClose = 0;
			/*------------------可修改的部分（ｓ）------------------------*/
			///买卖方向: 
			req.Direction = THOST_FTDC_D_Buy;
			///组合开平标志: 开仓
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			///合约代码
			strcpy(req.InstrumentID, "IF1505");
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///价格
			req.LimitPrice=4650;
			///数量
			req.VolumeTotalOriginal =1;
			/*------------------可修改的部分（ｅ）------------------------*/
			/// 第一腿
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

				/// 第二腿

				/*------------------可修改的部分（ｓ）------------------------*/
				///买卖方向: 
				req.Direction = THOST_FTDC_D_Sell;
				///组合开平标志: 开仓
				req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				///合约代码
				strcpy(req.InstrumentID, "IF1505");
				///报单引用
				g_pTradeCTP->m_iNextOrderRef++;
				sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
				///价格
				req.LimitPrice=4480;
				///数量
				req.VolumeTotalOriginal =1;
				/*------------------可修改的部分（ｅ）------------------------*/
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
		/******************关于预埋单代码段(s)*****************************************/
		if(atol(mymsg)==3)
		{
			CThostFtdcInputOrderField req;
			memset(&req, 0, sizeof(req));
			///经纪公司代码
			strcpy(req.BrokerID, BROKER_ID);
			///投资者代码
			strcpy(req.InvestorID, INVESTOR_ID);
			///strcpy(req.OrderRef, ORDER_REF);
			///用户代码
			//	TThostFtdcUserIDType	UserID;
			///报单价格条件: 限价
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///组合投机套保标志
			req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			///价格
			req.LimitPrice = LIMIT_PRICE;
			///有效期类型: 当日有效
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///GTD日期
			//	TThostFtdcDateType	GTDDate;
			///成交量类型: 任何数量
			req.VolumeCondition = THOST_FTDC_VC_AV;
			///最小成交量: 1
			req.MinVolume = 1;
			///触发条件: 立即
			req.ContingentCondition = THOST_FTDC_CC_Immediately;
			///止损价
			//	TThostFtdcPriceType	StopPrice;
			///强平原因: 非强平
			req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			///自动挂起标志: 否
			req.IsAutoSuspend = 0;
			///业务单元
			//	TThostFtdcBusinessUnitType	BusinessUnit;
			///请求编号
			//	TThostFtdcRequestIDType	RequestID;
			///用户强评标志: 否
			req.UserForceClose = 0;
			///买卖方向: 
			req.Direction = THOST_FTDC_D_Buy;
			///组合开平标志: 开仓
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			///合约代码
			strcpy(req.InstrumentID, "IF1509");
			///报单引用
			g_pTradeCTP->m_iNextOrderRef++;
			sprintf(req.OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
			///价格
			req.LimitPrice=4360;
			///数量
			req.VolumeTotalOriginal =1;
			if(NULL!=g_pMdApi)
			{
				///订阅该行情
				char * ppInstrument[1];
				ppInstrument[0]=req.InstrumentID;
				g_pMdApi->SubscribeMarketData(ppInstrument,1);
				///注意释放的时候，要加锁，但是这样时间上就会有损失，还有其他更好的办法吗？
				g_v_InputOrder.push_back(req);
			}
		}
		/******************关于预埋单代码段(e)*****************************************/
		/******************关于预定时间平仓码段(s)*****************************************/
		if(atol(mymsg)==4)
		{
			if(NULL!=g_pTradeApi)
			{
				
				CreateThread(NULL,0,Thread_TimeClose,NULL,0,NULL);
			}
			
		}
		/******************关于预定时间平仓码段(e)*****************************************/
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
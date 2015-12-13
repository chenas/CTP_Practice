#include "StdAfx.h"
#include "QuoteCTP.h"
#pragma warning(disable : 4996)
extern TThostFtdcBrokerIDType	BROKER_ID ;				// 经纪公司代码
extern TThostFtdcInvestorIDType INVESTOR_ID;			// 投资者代码
extern TThostFtdcPasswordType  PASSWORD;			// 用户密码
extern CThostFtdcMdApi *g_pMdApi;
extern vector_InputOrder      g_v_InputOrder;
extern CThostFtdcTraderApi*   g_pTradeApi;
extern CTradeCTP*             g_pTradeCTP;
extern int                    g_iRequestID;
extern int                    g_iReturn;
extern map_srt_DepthMarketData    g_m_DepthMarketData;

CQuoteCTP::CQuoteCTP()
{


}
CQuoteCTP::~CQuoteCTP(void)
{

}
void CQuoteCTP::OnFrontConnected()
{
	if(g_pMdApi==NULL) return;
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	g_pMdApi->ReqUserLogin(&req, ++m_nReq);
}
void CQuoteCTP::OnFrontDisconnected(int nReason)
{

}
void CQuoteCTP::OnHeartBeatWarning(int nTimeLapse)
{

}
void CQuoteCTP::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	 ///char *ppInstrumentID[] = {"IF1506", "IF1505"};			// 行情订阅列表
     ///int iInstrumentID = 2;									// 行情订阅数量
	 ///int iResult = g_pMdApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
}
bool CQuoteCTP::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}
int CQuoteCTP::SubscribeMarketData(char *ppInstrumentID[], int nCount)
{
		return g_pMdApi->SubscribeMarketData(ppInstrumentID,nCount);
}
///退订行情。
///@param ppInstrumentID 合约ID  
///@param nCount 要订阅/退订行情的合约个数
///@remark 
int CQuoteCTP::UnSubscribeMarketData(char *ppInstrumentID[], int nCount)
{
		return g_pMdApi->UnSubscribeMarketData(ppInstrumentID,nCount);
}
void CQuoteCTP::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{

	if(NULL==pDepthMarketData) return;
	string strkey = pDepthMarketData->InstrumentID;
	map_srt_DepthMarketData::iterator iter = g_m_DepthMarketData.find(strkey);
	if(iter == g_m_DepthMarketData.end())
	{
		g_m_DepthMarketData.insert(make_pair(strkey,*pDepthMarketData));
	}
	else
	{
		iter->second = *pDepthMarketData;
	}
#if 0
	/******************关于预埋单代码段(s)*****************************************/
	vector_InputOrder::iterator iter;
	bool bInsertFlag = true;
	for(iter=g_v_InputOrder.begin();iter!=g_v_InputOrder.end();)
	{
		if(strcmp(pDepthMarketData->InstrumentID,iter->InstrumentID)==0)
		{
			double Price_OffSet = iter->LimitPrice-pDepthMarketData->LastPrice;
			/***********************************************************/
			///自己写要触发条件,不满足条件时 bInsertFlag=false
			/***********************************************************/
			if(NULL!=g_pTradeApi && NULL!=g_pTradeCTP && bInsertFlag  )
			{
				///报单引用
				g_pTradeCTP->m_iNextOrderRef++;
				sprintf(iter->OrderRef,"%d",g_pTradeCTP->m_iNextOrderRef);
				///要重新设定为新的价格吗？
				iter->LimitPrice = pDepthMarketData->LastPrice;
				CThostFtdcInputOrderField req=(CThostFtdcInputOrderField)*iter;
			    memset(&req, 0, sizeof(req));
				g_iReturn=g_pTradeApi->ReqOrderInsert(&req, ++g_iRequestID);
				if(0==g_iReturn)
				{
    				///要加锁？
					g_v_InputOrder.erase(iter);
					///回到初始位置,从头开始循环，这个算法不是最优的，
					iter=g_v_InputOrder.begin();
					bInsertFlag = true;
					continue;
				}
			}
			bInsertFlag = true;
		}
		iter++;
	}
	/******************关于预埋单代码段(e)*****************************************/
#endif
}

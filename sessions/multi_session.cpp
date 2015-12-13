#include "multi_session.h"

extern HANDLE g_hEvent;

Session::Session()
{
	reqId = 0;
	frontID = 0;
	sessionID = 0;
}

Session::~Session()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
}

//连接前置
void Session::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	//SetEvent(g_hEvent);
	ResetEvent(g_hEvent);
}

//断开前置时调用
void Session::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
}

///登录请求响应
void Session::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if(pRspInfo->ErrorID == 0)
	{
		cerr << " 登录成功 " << endl;
		frontID = pRspUserLogin->FrontID;
		sessionID = pRspUserLogin->SessionID;
		strcpy(orderRef, pRspUserLogin->MaxOrderRef);
		cerr << "frontID: " << frontID << " sessionID: " << sessionID << " orderRef: " << orderRef << endl;
		
	}
	else 
	{
		cerr << " 登录失败 reason " << pRspInfo->ErrorMsg << endl;
	}
}

void Session::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
}
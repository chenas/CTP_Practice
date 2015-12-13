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

//����ǰ��
void Session::OnFrontConnected()
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	//SetEvent(g_hEvent);
	ResetEvent(g_hEvent);
}

//�Ͽ�ǰ��ʱ����
void Session::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
}

///��¼������Ӧ
void Session::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
	if(pRspInfo->ErrorID == 0)
	{
		cerr << " ��¼�ɹ� " << endl;
		frontID = pRspUserLogin->FrontID;
		sessionID = pRspUserLogin->SessionID;
		strcpy(orderRef, pRspUserLogin->MaxOrderRef);
		cerr << "frontID: " << frontID << " sessionID: " << sessionID << " orderRef: " << orderRef << endl;
		
	}
	else 
	{
		cerr << " ��¼ʧ�� reason " << pRspInfo->ErrorMsg << endl;
	}
}

void Session::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << __FUNCTION__ << endl;
}
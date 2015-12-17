#include "cfgutil.h"
#include "multi_session.h"

#include <windows.h>

HANDLE  g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);


DWORD WINAPI Login(LPVOID lpParameter)
{

}


int main()
{
	//请求编号
	int reqId = 0;
	CThostFtdcReqUserLoginField loginField; //登录请求数据

	CfgUtil getCfg("./cfg/rsh.cfg");

	string mdip = getCfg.getPara("MarketFrontIp");
	string tradeip = getCfg.getPara("TradeFrontIp");

	string instrument = getCfg.getPara("InstrumentId");
	string exchange = getCfg.getPara("ExchangeID");

	string brokerid = getCfg.getPara("BrokerId");
	string user = getCfg.getPara("BrokerUser");
	string passwd = getCfg.getPara("BrokerUserPasswd");

	int sessionSize = atoi(getCfg.getPara("SessionSize").c_str());

	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, brokerid.c_str());
	strcpy(loginField.UserID, user.c_str());
	strcpy(loginField.Password, passwd.c_str());
	strcpy(loginField.UserProductInfo, "ashu");

	for (int i=0; i<sessionSize; i++)
	{	
		Session *s = new Session;
		CThostFtdcTraderApi *api = CThostFtdcTraderApi::CreateFtdcTraderApi("./");
		api->RegisterFront(const_cast<char*>(tradeip.c_str()));
		api->RegisterSpi(s);
		api->Init();
		//api->Join();
		Sleep(1000);
		int rtn = api->ReqUserLogin(&loginField, ++reqId);
		std::cout << rtn << std::endl;
	}

	getchar();

	return 0;
}
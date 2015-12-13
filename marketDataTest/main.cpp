
#include "../apiImpl/mdimpl.h"
#include "../utility/cfgutil.h"
#include "../utility/stringutil.h"
#include <iostream>
#include <windows.h>
using namespace std;

HANDLE  g_hEvent_MD = CreateEvent(NULL, FALSE, FALSE, NULL);

int main()
{
	CThostFtdcReqUserLoginField loginField;
	
	
	CfgUtil* getCfg = CfgUtil::getInstance("../cfg/rsh.cfg");
	
	string mdip = getCfg->getPara("MarketFrontIp");
	string tradeip = getCfg->getPara("TradeFrontIp");

	string instrument = getCfg->getPara("InstrumentId");
	string exchange = getCfg->getPara("ExchangeID");

	string brokerid = getCfg->getPara("BrokerId");
	string user = getCfg->getPara("BrokerUser");
	string passwd = getCfg->getPara("BrokerUserPasswd");

	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, brokerid.c_str());
	strcpy(loginField.UserID, user.c_str());
	strcpy(loginField.Password, passwd.c_str());
	
	cout << "trade front ip " << tradeip << endl;
	cout << "mdfront ip " << mdip << endl;
	
	int isUdp = StringUtil::stringToInt(getCfg->getPara("IsUsingUdpMd"));
	int isMul = StringUtil::stringToInt(getCfg->getPara("IsMulticast"));

	MiniMd *md = new MiniMd;
	md->iniMd(user, passwd, brokerid, mdip, isUdp, isMul);




	system("pause");
}
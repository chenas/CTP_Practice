#include "marketutil.h"

MarketUtil::MarketUtil():_reqId(0)
{
	_mdIp = _getCfg->getPara("MarketFrontIp");
	_mdRspImpl = new MdRspImpl();

	//初始化行情接口
	_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi("./" , false, false);
	_pMdApi->RegisterSpi(_mdRspImpl); 
	_pMdApi->RegisterFront(const_cast<char*>(_mdIp.c_str()));
	_pMdApi->Init();
	WaitForSingleObject(g_hEvent, INFINITE);
	userLogin();
}

MarketUtil::~MarketUtil()
{
	SetEvent(g_hEvent);
	if (_pMdApi != NULL)
	{
		_pMdApi->Release();
		_pMdApi = NULL;
	}
	if (_mdRspImpl != NULL)
	{
		delete _mdRspImpl;
		_mdRspImpl = NULL;
	}
}

///行情订阅请求
int MarketUtil::subcribeMarketData(vector<PriceData *> vData)
{
	int vSize = vData.size();
	vector<char*> vInstrument;
	vector<PriceData *>::iterator viPriceData;
	for (viPriceData=vData.begin(); viPriceData != vData.end(); viPriceData++)
	{
		vInstrument.push_back((*viPriceData)->InstrumentId);
	}

	char** ppInstr = new char * [500];
	for (int i=0; i<vSize; i++)
	{
		const char* strContent=vInstrument[i];
		size_t strLength=strlen(strContent);		//当然要先获取要开辟内存的大小
		char *strArray=new char[strLength+1];		//首先，为新数组开辟指定大小的内存
		ppInstr[i]=strArray;	//然后，把新开辟的内存首地址赋值给指针
		strcpy(strArray, strContent);		//最后，在新开辟的内存里写入值
	}
	int rtn = _pMdApi->SubscribeMarketData(ppInstr, vSize);
	std::cerr << "---->>>发送订阅行情请求" << (rtn == 0 ? "成功":"失败") << std::endl;
	delete []ppInstr;
	return rtn;
}

///取消行情订阅
int MarketUtil::unSubscribeMarketData(vector<PriceData *> vData)
{
	int vSize = vData.size();
	vector<char*> vInstrument;
	vector<PriceData *>::iterator viPriceData;
	for (viPriceData=vData.begin(); viPriceData != vData.end(); viPriceData++)
	{
		vInstrument.push_back((*viPriceData)->InstrumentId);
	}

	char** ppInstr = new char * [500];

	for (int i=0; i<vSize; i++)
	{
		const char* strContent=vInstrument[i];
		size_t strLength=strlen(strContent);		//当然要先获取要开辟内存的大小
		char *strArray=new char[strLength+1];		//首先，为新数组开辟指定大小的内存
		ppInstr[i]=strArray;	//然后，把新开辟的内存首地址赋值给指针
		strcpy(strArray, strContent);		//最后，在新开辟的内存里写入值
	}
	int rtn = _pMdApi->UnSubscribeMarketData(ppInstr, vSize);
	std::cerr << "---->>>发送取消订阅行情请求" << (rtn == 0 ? "成功":"失败") << std::endl;
	delete []ppInstr;
	return rtn;
}

///打开记录行情数据文件
bool MarketUtil::openMdLog()
{
	return _mdRspImpl->openFile();
}

bool MarketUtil::closeMdLog()
{
	return _mdRspImpl->closeFile();
}

void MarketUtil::writeSeparator(const char* msg)
{
	_mdRspImpl->writeSeparator(msg);
}

void MarketUtil::userLogin()
{
	CThostFtdcReqUserLoginField loginField;
	memset(&loginField,0,sizeof(loginField));
	strcpy(loginField.BrokerID, "");
	strcpy(loginField.UserID, "");
	strcpy(loginField.Password, "");
	strcpy(loginField.UserProductInfo, "ashu");
	int rtn = _pMdApi->ReqUserLogin(&loginField, ++_reqId);
	std::cerr << "---->>>发送登录请求" << ((rtn == 0) ? "成功":"失败") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
}








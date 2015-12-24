#include "marketutil.h"

MarketUtil::MarketUtil():_reqId(0)
{
	_mdIp = _getCfg->getPara("MarketFrontIp");
	_mdRspImpl = new MdRspImpl();

	//��ʼ������ӿ�
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

///���鶩������
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
		size_t strLength=strlen(strContent);		//��ȻҪ�Ȼ�ȡҪ�����ڴ�Ĵ�С
		char *strArray=new char[strLength+1];		//���ȣ�Ϊ�����鿪��ָ����С���ڴ�
		ppInstr[i]=strArray;	//Ȼ�󣬰��¿��ٵ��ڴ��׵�ַ��ֵ��ָ��
		strcpy(strArray, strContent);		//������¿��ٵ��ڴ���д��ֵ
	}
	int rtn = _pMdApi->SubscribeMarketData(ppInstr, vSize);
	std::cerr << "---->>>���Ͷ�����������" << (rtn == 0 ? "�ɹ�":"ʧ��") << std::endl;
	delete []ppInstr;
	return rtn;
}

///ȡ�����鶩��
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
		size_t strLength=strlen(strContent);		//��ȻҪ�Ȼ�ȡҪ�����ڴ�Ĵ�С
		char *strArray=new char[strLength+1];		//���ȣ�Ϊ�����鿪��ָ����С���ڴ�
		ppInstr[i]=strArray;	//Ȼ�󣬰��¿��ٵ��ڴ��׵�ַ��ֵ��ָ��
		strcpy(strArray, strContent);		//������¿��ٵ��ڴ���д��ֵ
	}
	int rtn = _pMdApi->UnSubscribeMarketData(ppInstr, vSize);
	std::cerr << "---->>>����ȡ��������������" << (rtn == 0 ? "�ɹ�":"ʧ��") << std::endl;
	delete []ppInstr;
	return rtn;
}

///�򿪼�¼���������ļ�
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
	std::cerr << "---->>>���͵�¼����" << ((rtn == 0) ? "�ɹ�":"ʧ��") << std::endl;
	WaitForSingleObject(g_hEvent, INFINITE);
}








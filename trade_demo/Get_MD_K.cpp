#include "stdafx.h"
#include "Get_MD_K.h"
#include <iostream>
#include <algorithm>


CGet_MD_K::CGet_MD_K(void)
{
}


CGet_MD_K::~CGet_MD_K(void)
{
}
void CGet_MD_K::GetHistoryData(vector_MD_K & v_MD_K, TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin)
{
	///vect_MarketData Tmp_v_MarketData;
	ReadHisData(InstrumentID,amount,nMin);

	char pTmp[MAX_PATH];
	char pTmp_Time[MAX_PATH];
	CThostFtdcDepthMarketDataField  t_md;
	memset(&t_md,0,sizeof(CThostFtdcDepthMarketDataField));
	sprintf_s(t_md.InstrumentID,InstrumentID);
	time_t  t;  
	tm  *tp_Tmp;  
	t=time(NULL);
	tp_Tmp=localtime(&t);  
	sprintf_s(t_md.TradingDay,"%04d%02d%02d",tp_Tmp->tm_year+1900,tp_Tmp->tm_mon+1,tp_Tmp->tm_mday);
	sprintf_s(t_md.UpdateTime,"%d:%d:%d",tp_Tmp->tm_hour,tp_Tmp->tm_min,tp_Tmp->tm_sec); 
	int offSet=0;
	string  strTradingDay=t_md.TradingDay;
	string  strUpdateTime=t_md.UpdateTime;
	string	strKey;
	GetTimeMod(strTradingDay,strUpdateTime,nMin);


	strcpy_s(t_md.UpdateTime,strUpdateTime.c_str());
	strcpy_s(t_md.TradingDay,strTradingDay.c_str());
	Time_vect_MarketData::iterator iter ;
	int iHour,iMin,iSec;

	///int    iCnt_Min = nMin*amount;
	v_MD_K.clear();
	for(int iMin_offSet=amount;iMin_offSet>0;iMin_offSet--)
	{	
		GetBackTime(iMin_offSet*nMin,t_md,strTradingDay,strUpdateTime,offSet);

		t = GetSecondFrom1900(strTradingDay,strUpdateTime);
	    tp_Tmp=localtime(&t); 

		memset(pTmp_Time,0,sizeof(pTmp_Time));
		sprintf_s(pTmp_Time,"%02d:%02d:00",tp_Tmp->tm_hour+1,tp_Tmp->tm_min+1);
	
		int  iVol=0;
		bool bFlag_Close=true;
		strMD_K Tmp_strMD_K;
		Tmp_strMD_K.Init();
		for(int i=nMin;i>0;i--)
		{
			memset(pTmp,0,sizeof(pTmp));
			GetBackTime(iMin_offSet*nMin-i+1,t_md,strTradingDay,strUpdateTime,offSet);
			//strcpy_s(pTmp,strUpdateTime.c_str());
			sscanf(strUpdateTime.c_str(),"%02d:%02d:%02d",&iHour,&iMin,&iSec);
			for(int j = 59 ;j>-1;j--)
			{

				memset(pTmp,0,sizeof(pTmp));
				sprintf_s(pTmp,"%s %02d:%02d:%02d",strTradingDay.c_str(),iHour,iMin,j);
				strKey=pTmp;
				iter =  m_m_Time_vect_MarketData.find(strKey);
				if(iter!=m_m_Time_vect_MarketData.end())
				{
					int iCnt = iter->second.size();
					for(int k = iCnt-1;k>-1;k--)
					{
						CThostFtdcDepthMarketDataField  t_md;
						memset(&t_md,0,sizeof(CThostFtdcDepthMarketDataField));
						t_md = iter->second[k];
						///交易日
						strcpy_s(Tmp_strMD_K.TradingDay,strTradingDay.c_str());
						///最后修改时间
						sprintf_s(Tmp_strMD_K.UpdateTime,"%s",pTmp_Time);
						///合约代码
						sprintf_s(Tmp_strMD_K.InstrumentID,t_md.InstrumentID);
						///开盘价
						Tmp_strMD_K.Price_Open = t_md.LastPrice;
						///最高价
						if(t_md.LastPrice>Tmp_strMD_K.Price_High)
						{
							Tmp_strMD_K.Price_High = t_md.LastPrice;
						}
						///最低价
						if(t_md.LastPrice<Tmp_strMD_K.Price_Low)
						{
							Tmp_strMD_K.Price_Low = t_md.LastPrice;
						}
						///收盘价
						if(bFlag_Close)
						{
							Tmp_strMD_K.Price_Close= t_md.LastPrice;
							bFlag_Close = false;
							iVol=t_md.Volume;
						}
						///成交量 
						Tmp_strMD_K.Vol=iVol-t_md.Volume;
					}
				}

			}
		}
		if(strcmp(Tmp_strMD_K.InstrumentID,InstrumentID)==0)
		{
			v_MD_K.push_back(Tmp_strMD_K);
		}
	}
}

///取nMin的Mod
void CGet_MD_K::GetTimeMod(string & strTradingDay ,string & strUpdateTime,int nMin)
{
	char cTmp[MAX_PATH];
	string Tmp_strUpdateTime = strUpdateTime;
	string Tmp_strTradingDay = strTradingDay;
	time_t time_Start = GetSecondFrom1900(Tmp_strTradingDay,Tmp_strUpdateTime)-1;
	///得到分钟数
	time_t iTotal_Min = time_Start/60;
	time_t iTotal_Mod = iTotal_Min/nMin;
	time_t time_Mod =  iTotal_Mod*nMin*60;
	///
	tm  *tp;  
	tp=localtime(&time_Mod);  
	memset(cTmp,0,sizeof(cTmp));
	sprintf_s(cTmp,"%04d%02d%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
	strTradingDay = cTmp;
	//sprintf_s(t_md.UpdateTime,"%d:%d:%d",tp->tm_hour,tp->tm_min,tp->tm_sec); 
	sprintf_s(cTmp,"%d:%d:%d",tp->tm_hour+1,tp->tm_min+1,tp->tm_sec);
	strUpdateTime = cTmp;
}

///取得历史数据
void CGet_MD_K::ReadHisData(TThostFtdcInstrumentIDType InstrumentID, int amount,int nMin)
{
	m_m_Time_vect_MarketData.clear();
	CThostFtdcDepthMarketDataField  t_md;
	memset(&t_md,0,sizeof(CThostFtdcDepthMarketDataField));
	sprintf_s(t_md.InstrumentID,InstrumentID);
	time_t  t;  
	tm  *tp;  
	t=time(NULL);
	tp=localtime(&t);  
	sprintf_s(t_md.TradingDay,"%04d%02d%02d",tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
	//sprintf_s(t_md.UpdateTime,"%d:%d:%d",tp->tm_hour,tp->tm_min,tp->tm_sec); 
	sprintf_s(t_md.UpdateTime,"%d:%d:%d",tp->tm_hour,tp->tm_min,tp->tm_sec); 
	string  strTradingDay =t_md.TradingDay;
	string  strUpdateTime =t_md.UpdateTime;
	GetTimeMod(strTradingDay,strUpdateTime,nMin);
	int nTotalMin=amount*nMin;


	///string  strTradingDay ,strUpdateTime;
	int offSet =0;
	GetBackTime(nTotalMin,t_md,strTradingDay,strUpdateTime,offSet);

	char cTmp[MAX_PATH];
	char buf[MAX_PATH];  
	memset(buf,0,sizeof(buf)); 
	memset(cTmp,0,sizeof(cTmp));
	sprintf_s(cTmp,"HisData\\%s.txt",strTradingDay.c_str());
	GetCurrentPath(buf,cTmp);
	if(_access(buf,0)==-1)
	{
		///不存在
	}
	else
	{
		FILE * streamFile = NULL ;
		streamFile=fopen(buf,"r");
		if(NULL!=streamFile)
		{
			time_t time_HisData;
			time_t time_Start = GetSecondFrom1900(strTradingDay,strUpdateTime)-1;
			char md_Msg[1024];
			///排除第一行
			fgets(md_Msg,1024,streamFile);
			while(!feof(streamFile))	
			{	
				if(fgets(md_Msg,1024,streamFile))	
				{
					memset(&t_md,0,sizeof(CThostFtdcDepthMarketDataField));
					sprintf_s(t_md.InstrumentID,InstrumentID);
					sprintf_s(t_md.TradingDay,strTradingDay.c_str());


					char *arrInstance[MAXBYTE];
					const char *del = ",";               //分隔符
					int iarr =1;
					iarr= strsplinum(md_Msg,del);
					split(arrInstance,md_Msg,del);
					if(iarr<MAXBYTE && NULL!=arrInstance[0] && strcmp(arrInstance[0],InstrumentID)==0 )
					{
						if(NULL!=arrInstance[2])
						{
						   int iHour,iMin,iSec,mSec;
						   sscanf(arrInstance[2],"%d:%d:%d.%d",&iHour,&iMin,&iSec,&mSec);
						   sprintf_s(t_md.UpdateTime,"%02d:%02d:%02d",iHour,iMin,iSec);
						   time_HisData = GetSecondFrom1900(t_md.TradingDay,t_md.UpdateTime);
						   if(time_HisData>time_Start)
						   {
							   	if(NULL!=arrInstance[0])
								    strcpy_s(t_md.InstrumentID,arrInstance[0]);
								if(NULL!=arrInstance[1])
								   strcpy_s(t_md.TradingDay,arrInstance[1]);
								if(NULL!=arrInstance[2])
							    	t_md.UpdateMillisec = mSec;
								if(NULL!=arrInstance[3])
									t_md.LastPrice = atof(arrInstance[3]);
								if(NULL!=arrInstance[4])
									t_md.Volume = atol(arrInstance[4]);
								if(NULL!=arrInstance[5])
									t_md.BidPrice1 = atof(arrInstance[5]);
								if(NULL!=arrInstance[6])
									t_md.BidVolume1 = atol(arrInstance[6]);
								if(NULL!=arrInstance[7])
									t_md.AskPrice1 = atof(arrInstance[7]);
								if(NULL!=arrInstance[8])
									t_md.AskVolume1 = atol(arrInstance[8]);

								memset(cTmp,0,MAX_PATH);
								sprintf_s(cTmp,"%s %s",t_md.TradingDay,t_md.UpdateTime);
								string strKey=cTmp;
								Time_vect_MarketData::iterator iter =  m_m_Time_vect_MarketData.find(strKey);
								if(iter==m_m_Time_vect_MarketData.end())
								{
									vect_MarketData Tmp_v_MarketData;
									m_m_Time_vect_MarketData.insert(make_pair(strKey,Tmp_v_MarketData));
									iter =  m_m_Time_vect_MarketData.find(strKey);
								}
								iter->second.push_back(t_md);
						   }
						}
					}
				}
			}
		}
	}
}
//判断总共有多少个分隔符，目的是在main函数中构造相应的arr指针数组
int CGet_MD_K::strsplinum(char *str, const char*del)
{    
	char *first = NULL; 
	char *second = NULL;  
	int num = 0;  
	first = strstr(str,del);  
	while(first != NULL) 
	{  
		second = first+1;  
		num++;   
		first = strstr(second,del); 
	} 
	return num;
};

//字符分割函数的简单定义和实现
void CGet_MD_K::split( char **arr, char *str, const char *del)
{ 
	char *s =NULL; 
	s=strtok(str,del); 
	while(s != NULL) 
	{  
		*arr++ = s;
		s = strtok(NULL,del);
	}
};
///得到从19000101 00:00:01到现在的总秒数
time_t CGet_MD_K::GetSecondFrom1900(string strTradingDay ,string  strUpdateTime)
{
	struct tm  tm_Curr;
	sscanf(strTradingDay.c_str(), "%4d%02d%02d", &tm_Curr.tm_year, &tm_Curr.tm_mon, &tm_Curr.tm_mday);
	tm_Curr.tm_year-=1900;
	tm_Curr.tm_mon--;
	sscanf(strUpdateTime.c_str(), "%02d:%02d:%02d",&tm_Curr.tm_hour, &tm_Curr.tm_min, &tm_Curr.tm_sec );
	tm_Curr.tm_hour--;
	tm_Curr.tm_min--;
	time_t time_Curr=mktime(&tm_Curr); 
	return time_Curr;
}
	//获取当前程序目录 
 int CGet_MD_K::GetCurrentPath(char buf[],char *pFileName)  
	{  
#ifdef WIN32  
		GetModuleFileName(NULL,buf,MAX_PATH);   
#else  
		char pidfile[64];  
		int bytes;  
		int fd;  
		sprintf(pidfile, "/proc/%d/cmdline", getpid());  

		fd = open(pidfile, O_RDONLY, 0);  
		bytes = read(fd, buf, 256);  
		close(fd);  
		buf[MAX_PATH] = '\0';  
#endif  
		char * p = &buf[strlen(buf)];  
		do   
		{  
			*p = '\0';  
			p--;  
#ifdef WIN32  
		} while( '\\' != *p );  
#else  
		} while( '/' != *p );  
#endif  
		p++;  
		//配置文件目录  
		memcpy(p,pFileName,strlen(pFileName));  
		return 0;  
	};
///得到t_Min分前的时间
void CGet_MD_K::GetBackTime(int t_Min,CThostFtdcDepthMarketDataField  t_md,string & strTradingDay ,string & strUpdateTime,int &offSet)
{
	///商品：9点到11点半，下午1点半到3点。早上10：15到10：30休盘15分钟
	///股指：9点15到11半点  下午1点到3点15
	///夜盘： 21点到日凌晨2:30分

	

	t_Min*=60;
	string strInstru=t_md.InstrumentID;
	char cTmp[MAX_PATH];


	transform(strInstru.begin(), strInstru.end(), strInstru.begin(), ::toupper);
	string strTmp = strInstru.substr(0,2);
	bool IsStockIndex=false;
	if(strcmp(strTmp.c_str(),"IF")==0)
	{
		IsStockIndex = true;
	}


	///合成时间
	struct tm  *tm_Tmp;
	time_t time_Curr= GetSecondFrom1900(t_md.TradingDay,t_md.UpdateTime);
	time_t time_BackTime=time_Curr;
	tm_Tmp = localtime(&time_Curr);
	if(NULL==tm_Tmp) return;
	///注意“struct tm”都是以0未开始
	int iSpaceCurr = (tm_Tmp->tm_hour+1)*3600 + (tm_Tmp->tm_min+1)*60+tm_Tmp->tm_sec;
	int iSpacDiff = iSpaceCurr-t_Min;


	if(IsStockIndex)
	{///股指
		///股指：9点15到11半点  下午1点到3点15
        int iSpace0915 = 9*3600+15*60+0;
		int iSpace1130 = 11*3600+30*60+0;
		int iSpace1300 = 13*3600+0*60+0;
		int iSpace1515 = 15*3600+15*60+0;
		if(iSpacDiff < iSpace0915)
		{
			///与时间间隔之差在9:15之前,取上一个交易日的数据
            ///当天零时零分零秒的时间
			time_Curr=time_Curr-iSpaceCurr;
			offSet = iSpaceCurr - iSpace0915;
			///前一天下午15:15:00的时间
			time_BackTime=time_Curr - (24*3600-15*3600-15*60);
		}

		if(iSpace1130<iSpacDiff && iSpacDiff<iSpace1300) 
		{
			///与时间间隔之差在11:30与13：00之间
			time_BackTime=time_Curr-5400;
		}
		if(iSpacDiff > iSpace1515)
		{
			time_BackTime = iSpace1515;
			time_BackTime=time_Curr-iSpaceCurr+iSpace1515;

		}
		tm_Tmp = localtime(&time_BackTime);
		if(NULL==tm_Tmp) return;
		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
		strTradingDay  = cTmp;
		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
		strUpdateTime  = cTmp;

		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"HisData\\%s.txt",strTradingDay.c_str());
		string strPath = cTmp;

		char cBuf[MAX_PATH];
		memset(cTmp,0,sizeof(cTmp));
		memset(cBuf,0,sizeof(cBuf));
		strcpy_s(cBuf,strPath.c_str());
		GetCurrentPath(cTmp,cBuf);
		strPath = cTmp;
		if(_access(strPath.c_str(),0)==-1)
		{
			time_BackTime=time_BackTime - 24 *3600;
			tm_Tmp = localtime(&time_BackTime);
			if(NULL==tm_Tmp) return;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
			strTradingDay  = cTmp;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
			strUpdateTime  = cTmp;

			CThostFtdcDepthMarketDataField  t_md_Tmp = t_md;
			///不存在,就及递归
			sprintf_s(t_md_Tmp.TradingDay,strTradingDay.c_str());
			sprintf_s(t_md_Tmp.UpdateTime,strUpdateTime.c_str());
			t_Min = t_Min/60;
			GetBackTime(t_Min,t_md_Tmp,strTradingDay ,strUpdateTime,offSet);
		}
		else
		{
			time_BackTime = time_BackTime-t_Min+offSet;
			tm_Tmp = localtime(&time_BackTime);
			if(NULL==tm_Tmp) return;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
			strTradingDay  = cTmp;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
			strUpdateTime  = cTmp;
		}
	}

	else
	{///商品

		///商品：9点到11点半，下午1点半到3点。早上10：15到10：30休盘15分钟
		///夜盘： 21点到日凌晨2:30分
		int iSpace0230 = 2*3600+30*60+0;
		int iSpace0900 = 9*3600+0*60+0;
		int iSpace1015 = 10*3600+15*60+0;
		int iSpace1030 = 10*3600+30*60+0;
		int iSpace1130 = 11*3600+30*60+0;
		int iSpace1330 = 13*3600+30*60+0;
		int iSpace1500 = 15*3600+0*60+0;
		int iSpace2100 = 21*3600+0*60+0;
		int iSpace2400 = 24*3600+0*60+0;
		///在2:30-9:00之间,取9:00
		if(iSpace0230<iSpacDiff && iSpacDiff<iSpace0900) 
		{
			///与时间间隔之差在2:30-9:00之间,取2:30
			///当天零时零分零秒的时间
			time_Curr=time_Curr-iSpaceCurr;
			time_BackTime=time_Curr + iSpace0230;
			offSet = iSpaceCurr - iSpace0900;
			if(offSet<0)
				offSet = 0;
		}

		if((iSpace1015-t_Min)<iSpacDiff && iSpacDiff<iSpace1030)
		{
			///与时间间隔之差在10:15与t_Min之差--10:30之间
			time_BackTime = time_Curr-900;
		}
		if(iSpace1130<iSpacDiff && iSpacDiff<iSpace1330)
		{
			///与时间间隔之差在11:30--13:30,要去掉2小时(7200秒)的时间
			time_BackTime = time_Curr-7200;
		}
		if(iSpace1500<iSpacDiff && iSpacDiff<iSpace2100)
		{
			///与时间间隔之差在15:00-（21:00与t_Min之和）,要去掉6小时分钟(21600秒)的时间
			time_BackTime = time_Curr-6*3600;
		}
		if(iSpace2100<iSpacDiff && iSpacDiff<iSpace2400)
		{
			switch (tm_Tmp->tm_wday)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				 ///周一到周四,取下一天的数据
				 time_BackTime = time_Curr+iSpace2400;
			break;

			case 5:
				 ///周五时取周一的数据
				time_BackTime = time_Curr+iSpace2400*3;
				break;

			}

		}
		tm_Tmp = localtime(&time_BackTime);
		if(NULL==tm_Tmp) return;
		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
		strTradingDay  = cTmp;
		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
		strUpdateTime  = cTmp;

		memset(cTmp,0,sizeof(cTmp));
		sprintf_s(cTmp,"HisData\\%s.txt",strTradingDay.c_str(),strTradingDay);
		string strPath = cTmp;

		char cBuf[MAX_PATH];
		memset(cTmp,0,sizeof(cTmp));
		memset(cBuf,0,sizeof(cBuf));
		strcpy_s(cBuf,strPath.c_str());
		GetCurrentPath(cTmp,cBuf);
		strPath = cTmp;
		if(_access(strPath.c_str(),0)==-1)
		{
			time_BackTime=time_BackTime - 24 *3600;
			tm_Tmp = localtime(&time_BackTime);
			if(NULL==tm_Tmp) return;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
			strTradingDay  = cTmp;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
			strUpdateTime  = cTmp;

			CThostFtdcDepthMarketDataField  t_md_Tmp = t_md;
			///不存在,就及递归
			sprintf_s(t_md_Tmp.TradingDay,strTradingDay.c_str());
			sprintf_s(t_md_Tmp.UpdateTime,strUpdateTime.c_str());
			t_Min = t_Min/60;
			GetBackTime(t_Min,t_md_Tmp,strTradingDay ,strUpdateTime,offSet);
		}
		else
		{
			time_BackTime = time_BackTime-t_Min+offSet;
			tm_Tmp = localtime(&time_BackTime);
			if(NULL==tm_Tmp) return;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%04d%02d%02d",(tm_Tmp->tm_year+1900),(tm_Tmp->tm_mon+1),tm_Tmp->tm_mday);
			strTradingDay  = cTmp;
			memset(cTmp,0,sizeof(cTmp));
			sprintf_s(cTmp,"%02d:%02d:%02d",(tm_Tmp->tm_hour+1),(tm_Tmp->tm_min+1),tm_Tmp->tm_sec);
			strUpdateTime  = cTmp;
		}
	}

}

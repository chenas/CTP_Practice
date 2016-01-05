#include "stringutil.h"

StringUtil::StringUtil()
{
}

StringUtil::~StringUtil()
{
}

string StringUtil::shortToStr(short s)
{
	char tmpbuff[16];
	sprintf_s(tmpbuff,"%d",s);
	string res=tmpbuff;
	return res;
}

string StringUtil::intToStr(int d)
{
	char buff[32];
	sprintf_s(buff, "%d", d);
	string res = buff;
	return res;
}

string StringUtil::doubleToStr(double d)
{

	char buff[32];
	sprintf_s(buff, "%f", d);
	string res = buff;
	return res;
}

int StringUtil::stringToInt(string s)
{
	return atoi(s.c_str());
}

double StringUtil::stringToDouble(string s)
{
	return atof(s.c_str());
}

string StringUtil::arrayToStr(char a[])
{
	string s(&a[0], &a[strlen(a)]);
	return s;
}

void StringUtil::stringToVector(string source, string separator, vector<string>& v)
{
	vector<string>().swap(v);
	if (separator == "")
	{
		v.push_back( source );
	}

	int iLength = source.size( );
	int iFirst = 0, iLast = -1;
	string strKey, strValue;
	string strSubsource = source;
	while ( iLast < iLength - 1 )
	{
		iLast = source.find(separator, iFirst);
		iLast = iLast == string::npos ? iLength : iLast;
		strSubsource = source.substr(iFirst, iLast - iFirst);
		v.push_back( strSubsource );
		iFirst = iLast + 1;
	}
}

void StringUtil::stringToVector(string source, string separator, vector<int>& v)
{
	vector<int>().swap(v);
	if (separator == "")
	{
		v.push_back( StringUtil::stringToInt(source) );
	}

	int iLength = source.size( );
	int iFirst = 0, iLast = -1;
	string strKey, strValue;
	string strSubsource = source;
	while ( iLast < iLength - 1 )
	{
		iLast = source.find(separator, iFirst);
		iLast = iLast == string::npos ? iLength : iLast;
		strSubsource = source.substr(iFirst, iLast - iFirst);
		v.push_back( StringUtil::stringToInt(strSubsource) );
		iFirst = iLast + 1;
	}
}

string StringUtil::delNum(string s)
{
	string del("0123456789");
	string::size_type pos=0;
	while ((pos = s.find_first_of(del, pos)) != string::npos)
	{
		s.erase(pos, pos);
	}
	return s;
}
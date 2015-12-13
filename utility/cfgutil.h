
/****************************************************************************
*   目的:  读取配置文件的信息，以map的形式存入
*   要求:  配置文件的格式，以#作为行注释，配置的形式是key = value，中间可有空格，也可没有空格
*****************************************************************************/
#ifndef CFGUTIL_H
#define CFGUTIL_H
 
#include <string>
#include <map>

using namespace std;

#define COMMENT_CHAR '#'


class CfgUtil
{
public:
	static CfgUtil *getInstance(string cfgPath);
	void reloadCfg(string cfgPath);
	string getPara(string key);
	~CfgUtil();

protected:
	CfgUtil();

private:
	static CfgUtil* _cfgutil;
	static bool isSpace(char c);
	static bool isCommentChar(char c);
	static void trim(string & str);
	static bool analyseLine(const string & line, string & key, string & value);
	
	static map<string, string> _map;
	
};

#endif
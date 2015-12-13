
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
	CfgUtil(string filepath);
	string getPara(string key);
private:
	map<string, string> _map;
	bool isSpace(char c);
	bool isCommentChar(char c);
	void trim(string & str);
	bool analyseLine(const string & line, string & key, string & value);
};

#endif
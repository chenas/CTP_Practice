
/****************************************************************************
*   Ŀ��:  ��ȡ�����ļ�����Ϣ����map����ʽ����
*   Ҫ��:  �����ļ��ĸ�ʽ����#��Ϊ��ע�ͣ����õ���ʽ��key = value���м���пո�Ҳ��û�пո�
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
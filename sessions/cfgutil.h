
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
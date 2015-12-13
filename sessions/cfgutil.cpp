#include "cfgutil.h"

#include <fstream>
#include <iostream>
using namespace std;

CfgUtil::CfgUtil(string fielpath)
{
	 _map.clear();
    ifstream infile(fielpath.c_str());
    if (!infile) {
        cout << "file open error" << endl;
    }
    string line, key, value;
    while (getline(infile, line)) {
        if (analyseLine(line, key, value)) {
            _map[key] = value;
        }
    }
    infile.close();
}

bool CfgUtil::isSpace(char c)
{
    if (' ' == c || '\t' == c)
        return true;
    return false;
}

bool CfgUtil::isCommentChar(char c)
{
    switch(c) {
    case COMMENT_CHAR:
        return true;
    default:
        return false;
    }
}

void CfgUtil::trim(string & str)
{
    if (str.empty()) {
        return;
    }
    int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i) {
        if (!isSpace(str[i])) {
            break;
        }
    }
    if (i == str.size()) { // 全部是空白字符串
        str = "";
        return;
    }
    
    start_pos = i;
    
    for (i = str.size() - 1; i >= 0; --i) {
        if (!isSpace(str[i])) {
            break;
        }
    }
    end_pos = i;
    
    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool CfgUtil::analyseLine(const string & line, string & key, string & value)
{
    if (line.empty())
        return false;
    int start_pos = 0, end_pos = line.size() - 1, pos;
    if ((pos = line.find(COMMENT_CHAR)) != -1) {
        if (0 == pos) {  // 行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分
    
    if ((pos = new_line.find('=')) == -1)
        return false;  // 没有=号
        
    key = new_line.substr(0, pos);
    value = new_line.substr(pos + 1, end_pos + 1- (pos + 1));
    
    trim(key);
    if (key.empty()) {
        return false;
    }
    trim(value);
    return true;
}

string CfgUtil::getPara(string key)
{	
    map<string, string>::const_iterator mite = _map.begin();
    for (; mite != _map.end(); ++mite) {
		if(mite->first == key)
		{
			return mite->second;
		}
    }
	return "";
}


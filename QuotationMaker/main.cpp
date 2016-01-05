#include "cases.h"

HANDLE  g_hEvent = NULL; //事件信号

HANDLE g_hFile;  //异步写入日志

DWORD g_dwWritenSize = 0;

void main()
{
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hFile = CreateFile(TEXT("../cfg/trading_log.txt"), 
		GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS,  //文件存在则打开，不存在则新建
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	SetFilePointer(g_hFile, 0, NULL, FILE_END);  //指向文件末尾
	Cases *cases = new Cases();
	cases->run();
	FlushFileBuffers(g_hFile);
	system("pause");
	CloseHandle(g_hEvent);
	CloseHandle(g_hFile);
}
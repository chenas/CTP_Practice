#include "stdafx.h"
#include <windows.h>
#include <iostream>

#define NAME_LINE   40

using namespace std;

//�����̺߳�����������Ľṹ��
typedef struct __THREAD_DATA
{
  int nMaxNum;
  char strThreadName[NAME_LINE];

  __THREAD_DATA() : nMaxNum(0)
  {
      memset(strThreadName, 0, NAME_LINE * sizeof(char));
  }
}THREAD_DATA;

HANDLE g_hMutex = NULL;    
//������

//�̺߳���
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
  THREAD_DATA* pThreadData = (THREAD_DATA*)lpParameter;

  for (int i = 0; i < pThreadData->nMaxNum; ++ i)
  {
     
//������һ����������
      WaitForSingleObject(g_hMutex, INFINITE);
      cout << pThreadData->strThreadName << " --- " << i << endl;
      //Sleep(100);
     
//�ͷŻ�������
      ReleaseMutex(g_hMutex);
  }
  return 0L;
}

int main()
{
 
//����һ��������
  g_hMutex = CreateMutex(NULL, FALSE, NULL);

 
//��ʼ���߳�����
  THREAD_DATA threadData1, threadData2;
  threadData1.nMaxNum = 5;
  strcpy(threadData1.strThreadName, "�߳�1");
  threadData2.nMaxNum = 10;
  strcpy(threadData2.strThreadName, "�߳�2");


 
//������һ�����߳�
  HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc, &threadData1, 0, NULL);
 
//�����ڶ������߳�
  HANDLE hThread2 = CreateThread(NULL, 0, ThreadProc, &threadData2, 0, NULL);
 
//�ر��߳�
  CloseHandle(hThread1);
  CloseHandle(hThread2);

 
//���̵߳�ִ��·��
  for (int i = 0; i < 5; ++ i)
  {
     
//������һ����������
      WaitForSingleObject(g_hMutex, INFINITE);
      cout << "���߳� === " << i << endl;
      //Sleep(100);
     
//�ͷŻ�������
      ReleaseMutex(g_hMutex);
  }

  system("pause");
  return 0;
}
#include "cases.h"

HANDLE  g_hEvent = NULL;

void main()
{
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	Cases *cases = new Cases();
	cases->run();
	system("pause");
}
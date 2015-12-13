#include "cases.h"

string cfgFilePath = "../cfg/rsh.cfg";

int main()
{
	Cases *c = new Cases();
	c->run();

	return 0;
}
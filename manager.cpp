#include <iostream>
#include "CRuleManager.h"
#include <pthread.h>
#include <signal.h>

extern UINT32 debug_level;
int main(int argc, char *argv[])
{
    debug_level = 1;
	CRuleManager manager = CRuleManager();
	if(argc != 2)
	{
		printf("./manager port_number\n");
		return -1;
	}
	char ip[20] = "127.0.0.1";
	unsigned port = atoi(argv[1]);
	manager.Init(INTERFACE_TYPE_SOCKET,ip,port,NULL);
	manager.Run();
	return 0;
}

#include "syscall.h"


void main()
{
	int pingPID, pongPID;
	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");
	Join(pingPID);
	Join(pongPID);	
}


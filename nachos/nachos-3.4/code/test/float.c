#include "syscall.h"

int main() 
{
	int f;
	f = ReadFloat();
	PrintFloat(f);
	Halt();
	return 0;
}

#include "syscall.h"

int main() 
{
	int f;
	PrintString("Enter float number: ");
	f = ReadFloat();
	PrintString("Newly entered number: ");
	PrintFloat(f);
	Halt();
	return 0;
}

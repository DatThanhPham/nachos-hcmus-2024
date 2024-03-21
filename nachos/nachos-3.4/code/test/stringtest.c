#include "syscall.h"

int main ()
{
    char* str = ReadString(str, 255);
    PrintString(str);
    Halt();
    return 0;
}
#include "syscall.h"

int main ()
{
    char* str;
    ReadString(str, 5);
    PrintString("String read: ");
    PrintString(str);
    Halt();
    return 0;
}

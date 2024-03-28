#include "syscall.h"

int main ()
{
    char* str;
    ReadString(str, 5); // Read 5 characters from the console
    PrintString("String read: "); 
    PrintString(str); // Print the string read
    Halt(); // Stop Nachos
    return 0;
}

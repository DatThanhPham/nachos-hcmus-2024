#include "syscall.h"

int main() {
    float a;
    a = ReadFloat();
    PrintFloat(a);

    Halt(); // Halt the program

    return 0;
}

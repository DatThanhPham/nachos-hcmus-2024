#include "syscall.h"

int main() {
    int n;
    n = CreateSemaphore("semaphore", 1);
    if (n == 0) {
        PrintString("Create semaphore successfully\n");
    } else {
        PrintString("Create semaphore failed\n");
    }

    Halt(); // Halt the program

    return 0;
}

#include "syscall.h"

int str_length(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

void intToStr(int n, char *s) {
    int i = 0;
    int j = 0;
    int len = 0;
    char temp[255];
    if (n == 0) {
        s[i++] = '0';
        s[i] = '\0';
        return;
    }
    while (n > 0) {
        temp[i] = n % 10 + '0';
        n = n / 10;
        i++;
    }
    temp[i] = '\0';
    len = str_length(temp);
    for (j = 0; j < len; j++) {
        s[j] = temp[len - j - 1];
    }
    s[len] = '\0';
}

int main(){
    int i;
    char c;
    char buffer[257];
    char* filename;
    int fileID;
    filename = "ascii.txt";
    fileID = Open(filename, 0);
    if (fileID == -1) {
        PrintString("\nFAILED: Can't open file ");
        PrintString(filename);
    } else {
        for (i = 0; i < 256; i++) {
            c = i;
            intToStr(i, buffer);
            Write("ASCII code: ", 12, fileID);
            Write(buffer, str_length(buffer), fileID);
            Write(" Character: ", 11, fileID);
            Write(&c, 1, fileID);
            Write("\n", 1, fileID);
        }
        CloseFile(fileID);
    }
    for (i = 0; i < 256; i++) {
        c = i;
        PrintString("ASCII code: ");
        PrintInt(i);
        PrintString(" Character: ");
        PrintChar(c);
        PrintString("\n");
    }
    PrintString("Write file ascii.txt successfully\n");
    Halt();
    return 0;
}

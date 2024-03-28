#include "syscall.h"

#define MAX_BUFFER_LENGTH 1000

int main()
{
    char *filename = "mota.txt"; // Ten mac dinh cua file
    int fileID;

    fileID = Open(filename, 1); // Mo file voi quyen doc
    if (fileID == -1) // Neu khong mo duoc file, in ra thong bao loi
    {
        PrintString("\nFAILED: Can't open file ");
        PrintString(filename);
    }
    else
    {
        char buffer[MAX_BUFFER_LENGTH + 1];
        int fileLength = Seek(-1, fileID); // Lay do dai cua file
        Seek(0, fileID); // Reset file pointer ve dau file
        
        if (Read(buffer, fileLength, fileID) == -1)
        {
            // Neu khong doc duoc noi dung file, in ra thong bao loi
            PrintString("\nFAILED: Can't read file ");
            PrintString(filename);
        }

        // Neu doc duoc noi dung file, in noi dung file ra console
        else
        {
            buffer[fileLength] = '\0';
            PrintString("\nSUCCESS: Content of file");
            PrintString(filename);
            PrintString("\n");
            PrintString(buffer);
        }
        CloseFile(fileID); // Dong file
    }
    
    Halt();
    return 0;
}
#include "syscall.h"

#define MAX_BUFFER_LENGTH 1000

int main()
{
    char *filename = "mota.txt";
    int fileID;

    fileID = Open(filename, 1);
    if (fileID == -1)
    {
        PrintString("\nFAILED: Can't open file ");
        PrintString(filename);
    }
    else
    {
        char buffer[MAX_BUFFER_LENGTH + 1];
        int fileLength = Seek(-1, fileID); // Get file length
        Seek(0, fileID); // Reset file pointer to beginning of file
        
        if (Read(buffer, fileLength, fileID) == -1)
        {
            // If unable read file, show error notification
            PrintString("\nFAILED: Can't read file ");
            PrintString(filename);
        }

        // If able to read file's content, print it to console
        else
        {
            buffer[fileLength] = '\0';
            PrintString("\nSUCCESS: Content of file ");
            PrintString(filename);
            PrintString(":\n");
            PrintString(buffer);
        }
        CloseFile(fileID);
    }
    
    Halt();
    return 0;
}
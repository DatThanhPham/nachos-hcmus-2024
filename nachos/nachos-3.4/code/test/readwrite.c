
#include "syscall.h"

#define READONLY 1

int getFileLength(int fileID)
{
    int value = Seek(-1, fileID);
    Seek(0, fileID);
    return value;
}

int string_length(const char *str) {
    int length = 0;
    while (*str != '\0') {
        ++length;
        ++str;
    }
    return length;
}

void inputFilename(char *buffer)
{
    int filenameLength;
    do
    {
        PrintString("Enter length of filename (maximum 256 characters): ");
        filenameLength = ReadInt();
        if (!(filenameLength > 0 && filenameLength < 257))
            PrintString("Please enter filename's length value in range [1, 256]\n");
    } while (!(filenameLength > 0 && filenameLength < 257));

    // Get filename from keyboard
    PrintString("Enter filename (exactly ");
    PrintInt(filenameLength);
    PrintString(" characters): ");
    ReadString(buffer, filenameLength);
}

/*void main()
{
    char filename[256];
    int fileID;

    inputFilename(filename);

    // Try opening file
    fileID = Open(filename, READONLY);

    // If can't open file, show error notification
    if (fileID == -1)
    {
        PrintString("\nFAILED: Can't open file ");
        PrintString(filename);
    }

    // If open file successfully, try reading file
    else
    {
        int fileLength = getFileLength(fileID);
        char buffer[fileLength + 1];

        PrintString("File length: ");
        PrintInt(fileLength);
        PrintChar('\n');
        
        if (Read(buffer, fileLength, fileID) == -1)
        {
            // If can't read file, show error notification
            PrintString("\nFAILED: Can't read file ");
            PrintString(filename);
        }

        // If read file's content successfully, show it
        else
        {
            buffer[fileLength] = '\0';
            PrintString("\nSUCCESS: Content of file ");
            PrintString(filename);
            PrintString(":\n");
            PrintString(buffer);
        }

        // Always remembers to close the opened file.
        CloseFile(fileID);
    }

    PrintChar('\n');

    // Halt machine!
    Halt();
}*/

void main()
{
    char filename[256];
    int fileID;

    inputFilename(filename);

    // Try opening file for writing
    fileID = Open(filename, 0); // Use 0 to indicate write mode

    // If can't open file, show error notification
    if (fileID == -1)
    {
        PrintString("\nFAILED: Can't open file ");
        PrintString(filename);
    }

    // If open file successfully, try writing to file
    else
    {
        char buffer[256];
        int bufferLength;

        // Prompt user to enter content to be written to the file
        PrintString("Enter content to be written to the file: ");
        ReadString(buffer, 256); // Read content from user input
	bufferLength = string_length(buffer);
 
        // Write content to the file
        if (Write(buffer, bufferLength, fileID) == -1)
        {
            // If can't write to file, show error notification
            PrintString("\nFAILED: Can't write to file ");
            PrintString(filename);
        }
        else
        {
            // If write successful, show success notification
            PrintString("\nSUCCESS: Content written to file ");
            PrintString(filename);
        }

        // Always remember to close the opened file.
        CloseFile(fileID);
    }

    PrintChar('\n');

    // Halt machine!
    Halt();
}

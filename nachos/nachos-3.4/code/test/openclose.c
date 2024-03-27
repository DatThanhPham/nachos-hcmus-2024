#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
	int openFileId;
	int fileSize;
	char c; 
	char fileName[MAX_LENGTH];
	int i; 
	PrintString("Enter filename: ");
	ReadString(fileName, MAX_LENGTH);

	openFileId = Open(fileName, 1);
	
	if (openFileId != -1)
	{
		PrintString("Open file successfully.");
		CloseFile(openFileId);
	}
	else
	{
		PrintString("Error: Unable to open file.\n\n");
	}
	Halt();
	return 0;
}

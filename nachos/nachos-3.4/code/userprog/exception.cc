// exception.cc

//	Entry point into the Nachos kernel from user programs.

//	There are two kinds of things that can cause control to

//	transfer back to here from user code:

//

//	syscall -- The user code explicitly requests to call a procedure

//	in the Nachos kernel.  Right now, the only function we support is

//	"Halt".

//

//	exceptions -- The user code does something that the CPU can't handle.

//	For instance, accessing memory that doesn't exist, arithmetic errors,

//	etc.

//

//	Interrupts (which can also cause control to transfer from user

//	code into the Nachos kernel) are handled elsewhere.

//

// For now, this only handles the Halt() system call.

// Everything else core dumps.

//

// Copyright (c) 1992-1993 The Regents of the University of California.

// All rights reserved.  See copyright.h for copyright notice and limitation

// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"

#include "syscall.h"

#include "synch.h"

#include <cctype>

#define MaxFileLength 32
//----------------------------------------------------------------------

// ExceptionHandler

// 	Entry point into the Nachos kernel.  Called when a user program

//	is executing, and either does a syscall, or generates an addressing

//	or arithmetic exception.

//

// 	For system calls, the following is the calling convention:

//

// 	system call code -- r2

//		arg1 -- r4

//		arg2 -- r5

//		arg3 -- r6

//		arg4 -- r7

//

//	The result of the system call, if any, must be put back into r2.

//

// And don't forget to increment the pc before returning. (Or else you'll

// loop making the same system call forever!

//

//	"which" is the kind of exception.  The list of possible exceptions

//	are in machine.h.

//----------------------------------------------------------------------

char *User2System(int virtAddr, int limit)

{

	int i; // index

	int oneChar;

	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1]; // need for terminal string

	if (kernelBuf == NULL)

		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);

	// printf("\n Filename u2s:");

	for (i = 0; i < limit; i++)

	{

		machine->ReadMem(virtAddr + i, 1, &oneChar);

		kernelBuf[i] = (char)oneChar;

		// printf("%c",kernelBuf[i]);

		if (oneChar == 0)

			break;
	}

	return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)

{

	if (len < 0)

		return -1;

	if (len == 0)

		return len;

	int i = 0;

	int oneChar = 0;

	do

	{

		oneChar = (int)buffer[i];

		machine->WriteMem(virtAddr + i, 1, oneChar);

		i++;

	} while (i < len && oneChar != 0);

	return i;
}

// Function to increment the program counter (PC) and update relevant registers
void IncreasePC()

{
	// Read the current value of the program counter (PC)
	int counter = machine->ReadRegister(PCReg);
	
	// Save the current PC value to the previous PC register (PrevPCReg)
	machine->WriteRegister(PrevPCReg, counter);
	
	// Read the value of the next PC register (NextPCReg)
	counter = machine->ReadRegister(NextPCReg);
	
	// Write the current PC value to the PC register	
	machine->WriteRegister(PCReg, counter);

	// Update the next PC register to point to the next instruction (PC + 4)
	machine->WriteRegister(NextPCReg, counter + 4);
}

void ExceptionHandler(ExceptionType which)
{
	//Read type from register 2
	int type = machine->ReadRegister(2);

	switch (which)
	{
	// Handle the case when no exception occurs
	case NoException:

		return;

	// Handle the case when a page fault exception occurs
	case PageFaultException:

		DEBUG('a', "\n Translation not found");

    		printf("\n\n Translation not found");

		interrupt->Halt();

		break;
	
	// Handle the case when a read-only exception occurs
	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to read-only page");

    		printf("\n\n Write attempted to read-only page");

    		interrupt->Halt();

		break;

	// Handle the case when a bus error exception occurs
	case BusErrorException:
		DEBUG('a', "\n Invalid physical address translation");

    		printf("\n\n Invalid physical address translation");

		interrupt->Halt();

		break;

	// Handle the case when an address error exception occurs
	case AddressErrorException:
		DEBUG('a', "\n Unaligned or out-of-bounds memory access");

    		printf("\n\n Unaligned or out-of-bounds memory access");

		interrupt->Halt();

		break;

	// Handle the case when an overflow exception occurs
	case OverflowException:
		DEBUG('a', "\n Integer overflow in arithmetic operation");

		printf("\n\n Integer overflow in arithmetic operation");

		interrupt->Halt();

		break;

	// Handle the case when an illegal instruction exception occurs
	case IllegalInstrException:
		DEBUG('a', "\n Unsupported or reserved instruction");

		printf("\n\n Unsupported or reserved instruction");

		interrupt->Halt();

		break;

	// Handle the case when an unknown exception type occurs
	case NumExceptionTypes:
		DEBUG('a', "\n Number of exception types exceeded");

		printf("\n\n Number of exception types exceeded");

		interrupt->Halt();

		break;

	case SyscallException:
		switch (type)

		{

		case SC_Halt:

			DEBUG('a', "Shutdown, initiated by user program.\n");

			interrupt->Halt();

			break;
		case SC_ReadChar:

		{
			int maxBytes = 255;

			char *buffer = new char[255];

			int numBytes = gSynchConsole->Read(buffer, maxBytes);

			if (numBytes > 1)

			{
				// Print error message for exceeding maximum allowed characters
				printf("Maximum 1 character allowed for input");

				// Log error message
				DEBUG('a', "\nERROR: Maximum 1 character allowed for input!");
			
				// Set return value to indicate error
				machine->WriteRegister(2, 0);
			}

			else if (numBytes == 0) //empty
			{	
				
				// Print error message for empty input
				printf("Empty character!");
	
			        // Log error message
				DEBUG('a', "\nERROR: Empty character!");
	
				// Set return value to indicate error
				machine->WriteRegister(2, 0);
			}

			else

			{
				// Read character from buffer
				char c = buffer[0];
		
			        // Write character to register 2
				machine->WriteRegister(2, c);
			}
	
			// Free dynamically allocated buffer memory
			delete buffer;
		
			// Increment program counter
			IncreasePC();

			break;
		}

		case SC_PrintChar:
		{
			// Read character from register 4
			char c = (char)machine->ReadRegister(4);
			
			// Write character to console
			gSynchConsole->Write(&c, 1);
			
			// Increment program counter
			IncreasePC();

			break;
		}
		case SC_ReadFloat: 
		{

			int maxBytes = 255;

			char buffer[256];

			int numBytes = gSynchConsole->Read(buffer, maxBytes);


			bool check = true;

			//kiem tra la so thuc

			for (int i = 0; i < numBytes; i++)

			{

				if (buffer[i] != '.' && (buffer[i] < '0' || buffer[i] > '9'))

				{

					printf("Khong phai so thuc!");

					DEBUG('a', "\nERROR: Khong phai so thuc!");

					machine->WriteRegister(2, 0.0);
					check = false;

					break;

				}

			}

			float f;
			sscanf(buffer, "%f", &f);

			// Chuyển đổi số thực thành số nguyên
			int f_int = *(int*)&f;


			// Ghi giá trị số nguyên vào thanh ghi số 2
			machine->WriteRegister(2, f_int);


			IncreasePC();

			break;
		}

		case SC_PrintFloat:

		{
		    	// Đọc giá trị từ thanh ghi số 4
			int f_int = machine->ReadRegister(4);

			// Chuyển đổi số nguyên thành số thực
		    	float f = *(float*)&f_int;
			int count = 0;
			char buffer[256];


			// In số thực ra console
			sprintf(buffer, "%f", f);
			while(buffer[count] != '\0'){
				count ++;	
			}	
			gSynchConsole->Write(buffer, count + 1);


		    	IncreasePC();

		    	break;

		}

		case SC_ReadInt:
		{
			int maxBytes = 256;
			char* buffer = new char[256];
			int bytesRead = gSynchConsole->Read(buffer, maxBytes); // doc chuoi tu console

			for (int i = 0; i < bytesRead; i++)
			{
				if (i == 0 && buffer[i] == '-') // kiem tra so am
					continue;
				if (buffer[i] < '0' || buffer[i] > '9') // kiem tra so nguyen
				{
					printf("Khong phai so nguyen!");
					DEBUG('a', "\nERROR: Khong phai so nguyen!");
					machine->WriteRegister(2, 0); // ghi so 0 vao thanh ghi r2
					delete[] buffer;
					IncreasePC();
					break;
				}
			}

			int number = atoi(buffer); // chuyen chuoi thanh so nguyen
			machine->WriteRegister(2, number); // ghi so nguyen vao thanh ghi r2
			IncreasePC();
			break;
		}
		case SC_PrintInt:
		{
			int number = machine->ReadRegister(4); // doc so nguyen tu thanh ghi r4
			if (number == 0) // truong hop so bang 0
			{
				gSynchConsole->Write("0", 1);
				IncreasePC();
				break;
			}

			// dem so chu so cua so nguyen
			int temp = number;
			int digitCount = 0;
			while (temp != 0)
			{
				temp /= 10;
				digitCount++;
			}
			if (number < 0) // truong hop so am
				digitCount++;

			char* buffer = new char[digitCount + 1];
			bool negative = false;
			buffer[digitCount] = '\0'; // sentinel
			if (number < 0) // truong hop so am
			{
				negative = true;
				number *= -1;
			}	
			for (int i = digitCount - 1; i >= 0; i--) // chuyen so nguyen thanh chuoi
			{
				buffer[i] = (char)(abs(number % 10) + '0');
				number /= 10;
			}
			if (negative) // truong hop so am
				buffer[0] = '-';

			gSynchConsole->Write(buffer, digitCount + 1); // in chuoi ra console
			IncreasePC();
			break;
		}	
		case SC_ReadString:
		{

			int virtAddr = machine->ReadRegister(4); // Đọc địa chỉ ảo của chuỗi từ thanh ghi r4
			int length = machine->ReadRegister(5); // Đọc độ dài chuỗi từ thanh ghi r5
			char *buffer = new char[length + 1]; // Cấp phát bộ nhớ động cho chuỗi, tạo thêm một ký tự kết thúc chuỗi
			gSynchConsole->Read(buffer, length); // Sử dụng console để đọc chuỗi từ bàn phím
			System2User(virtAddr, length, buffer); // Copy chuỗi từ kernel space sang user space
			delete[] buffer; // Giải phóng bộ nhớ
			IncreasePC(); // tăng program counter
			break;
		}

		case SC_PrintString:
		{
			int virtAddr = machine->ReadRegister(4); // Đọc địa chỉ ảo của chuỗi từ thanh ghi r4
			char *buffer = User2System(virtAddr, 255); // Copy chuỗi từ user space sang kernel space
			int length = 0; // Độ dài của chuỗi
			while (buffer[length] != '\0') // Nếu chưa kết thúc chuỗi thì tăng độ dài
			{
				length++;
			}
			gSynchConsole->Write(buffer, length + 1); // In chuỗi ra console
			IncreasePC(); // tăng program counter
			//printf("\n");
			break;
		}
		case SC_Create:
		{
			// Read the virtual address of the file name from register 4
			int virtualAddress;

			char* fileName;
			
			virtualAddress = machine->ReadRegister(4);
			
			// Convert virtual address to system address and retrieve the file name
			fileName = User2System(virtualAddress, 32 + 1);

			// Check if the file name is empty
			if (strlen(fileName) == 0) 
			{
				printf("\n File name is not valid");
	
				// Set return value to -1
				machine->WriteRegister(2, -1);

				// Increment program counter
				IncreasePC();

				break;
			}
			// Check if system has enough memory
			if (fileName == NULL)
			{
				printf("\n Not enough memory in system");
				
				// Set return value to -1
				machine->WriteRegister(2,-1);

				delete fileName;
			
			        // Increment program counter
				IncreasePC();

				break;
			}
	
			// Check if file creation is successful
			if (!fileSystem->Create(fileName, 0)) 
			{
				printf("\n Cannot create file'%s'", fileName);

				// Set return value to -1		
				machine->WriteRegister(2,-1);

				delete fileName;

				// Increment program counter
				IncreasePC();

				break;
			}

			// File creation successful, set return value to 0
			machine->WriteRegister(2,0);

			delete fileName;
			
			// Increment program counter
			IncreasePC();

			break;
		}
		case SC_Open:
		{

			// Read the virtual address of the file name from register 4
			int bufferAddress = machine->ReadRegister(4);
			
			// Read the type of file from register 5
			int type = machine->ReadRegister(5);

			char* buffer;
		
			// Check if maximum number of files opened (10) has been reached
			if (fileSystem->index > 10)
			{	
				// Set return value to -1 indicating failure
				machine->WriteRegister(2,-1);

				delete [] buffer;

				break;
			}
			// Convert virtual address to system address and retrieve the file name
			buffer = User2System(bufferAddress, 256);

			// Check if file is stdin or stdout
			if (strcmp(buffer, "stdin") == 0)
			{

				printf("Stdin mode\n");

				// Set return value to 1 indicating success
				machine->WriteRegister(2,1);

				delete [] buffer;

				break;
			}
		
			if (strcmp(buffer, "stdout") == 0)
			{
				printf("Stdout mode\n");

				// Set return value to 0 indicating success
				machine->WriteRegister(2,0);

				delete [] buffer;

				break;
			}
			// Attempt to open the file
			if ((fileSystem->openfile[fileSystem->index] = fileSystem->Open(buffer, type)) != NULL)
			{
				printf("\n Open file success '%s'", buffer);
			
				// Set return value to index of the opened file
				machine->WriteRegister(2, fileSystem->index - 1);
			}
			else
			{
				printf("Cannot open file '%s'", buffer);
				
				// Set return value to -1 indicating failure
				machine->WriteRegister(2, -1);
			}
			delete [] buffer;
			
			// Increment program counter
			IncreasePC();

			break;
		}

		case SC_Close:
		{
			// Read the file descriptor from register 4
			int fileDescriptor = machine->ReadRegister(4);

			int numOpenFiles = fileSystem->index;

			// Check if file descriptor is valid
			if (numOpenFiles < fileDescriptor)
			{
				printf("Close file failed \n");

				// Set return value to -1 indicating failure
				machine->WriteRegister(2, -1);

				break;
			}

			// Close the file
			fileSystem->openfile[fileDescriptor] = NULL;

			delete fileSystem->openfile[fileDescriptor];

			// Set return value to 0 indicating success
			machine->WriteRegister(2, 0);

			printf("Close file success \n");

			// Increment program counter
			IncreasePC();

			break;

		}

		case SC_Read:
		{
			// Read the virtual address of the buffer from register 4
			int bufferAddr = machine->ReadRegister(4);

			// Read the number of characters to read from register 5
			int charCount = machine->ReadRegister(5);

			// Read the file descriptor from register 6
			int fileId = machine->ReadRegister(6);

			// Get the number of open files
			int numOpenFiles = fileSystem->index;

			// Check if file descriptor is valid
			if (fileId > numOpenFiles || fileId < 0 || fileId == 1) // Invalid file descriptor or attempting to read from stdout
			{
				printf("Try to open invalid file");
				machine->WriteRegister(2, -1);
				break;
			}

			// Check if the file is open
			if (fileSystem->openfile[fileId] == NULL)
			{
				machine->WriteRegister(2, -1);
				break;
			}

			// Allocate buffer in kernel space
			char *kernelBuffer = User2System(bufferAddr, charCount);

			if (fileId == 1) // Read from stdin
			{
				// Read from console
				int bytesRead = gSynchConsole->Read(kernelBuffer, charCount);

				// Copy data from kernel to user space
				System2User(bufferAddr, bytesRead, kernelBuffer);

				// Set the number of bytes read as return value
				machine->WriteRegister(2, bytesRead);

				delete[] kernelBuffer;
				break;
			}

			// Record current file position
			int beforePos = fileSystem->openfile[fileId]->GetCurrentPos();

			// Read from file
			int bytesRead = fileSystem->openfile[fileId]->Read(kernelBuffer, charCount);

			if (bytesRead > 0)
			{
				// Record new file position
				int afterPos = fileSystem->openfile[fileId]->GetCurrentPos();

				// Copy data from kernel to user space
				System2User(bufferAddr, charCount, kernelBuffer);

				// Set the number of bytes read as return value
				machine->WriteRegister(2, afterPos - beforePos + 1);
			}
			else
			{
				// Set return value to -1 indicating failure
				machine->WriteRegister(2, -1);
			}

			delete[] kernelBuffer;

			// Increment program counter
			IncreasePC();

			break;
		}

		case SC_Write: 
		{
			// Read the virtual address of the buffer from register 4
			int bufferAddr = machine->ReadRegister(4);

			// Read the number of characters to write from register 5
			int charCount = machine->ReadRegister(5);

			// Read the file descriptor from register 6
			int fileId = machine->ReadRegister(6);

			// Get the number of open files
			int numOpenFiles = fileSystem->index;

			// Check if file descriptor is valid
			if (fileId > numOpenFiles || fileId < 0 || fileId == 0) // Invalid file descriptor or attempting to write to stdin 
			{
				machine->WriteRegister(2, -1);

				IncreasePC();

				break;
			}

			// Check if the file is open
			if (fileSystem->openfile[fileId] == NULL)
			{
				machine->WriteRegister(2, -1);

				break;
			}

			// Check if file is read-only
			if (fileSystem->openfile[fileId]->type == 1)
			{
				printf("Try to modify read-only file");

				machine->WriteRegister(2, -1);

				IncreasePC();

				break;
			}

			// Write to console
			char *kernelBuffer = User2System(bufferAddr, charCount);

			if (fileId == 0) // Write to stdout
			{
				int i = 0;

				while (kernelBuffer[i] != '\0' && kernelBuffer[i] != '\n')
				{
				    gSynchConsole->Write(kernelBuffer + i, 1);

				    i++;
				}
				kernelBuffer[i] = '\n';

				gSynchConsole->Write(kernelBuffer + i, 1); // Write last character

				machine->WriteRegister(2, i - 1);

				delete[] kernelBuffer;

				IncreasePC();

				break;
			}

			// Write into file
			int beforePos = fileSystem->openfile[fileId]->GetCurrentPos();

			int bytesWritten = fileSystem->openfile[fileId]->Write(kernelBuffer, charCount);
			if (bytesWritten != 0)
			{
				int afterPos = fileSystem->openfile[fileId]->GetCurrentPos();

				System2User(bufferAddr, afterPos - beforePos, kernelBuffer);

				machine->WriteRegister(2, afterPos - beforePos + 1);

				delete[] kernelBuffer;

				IncreasePC();

				break;
			}		
		}
		case SC_WriteFloat: 

		{
			int charCount = 0;
			char buffer[256];
			// Đọc giá trị từ thanh ghi số 4
			int f_int = machine->ReadRegister(4);

			// Chuyển đổi số nguyên thành số thực
		    	float f = *(float*)&f_int;
			sprintf(buffer, "%f", f);
			while(buffer[charCount] != '\0'){
				charCount ++;	
			}

			// Read the file descriptor from register 
			int fileId = machine->ReadRegister(5);

			// Get the number of open files
			int numOpenFiles = fileSystem->index;

			// Check if file descriptor is valid
			if (fileId > numOpenFiles || fileId < 0 || fileId == 0) // Invalid file descriptor or attempting to write to stdin 
			{
				machine->WriteRegister(2, -1);

				IncreasePC();

				break;
			}

			// Check if the file is open
			if (fileSystem->openfile[fileId] == NULL)
			{
				machine->WriteRegister(2, -1);

				break;
			}

			// Check if file is read-only
			if (fileSystem->openfile[fileId]->type == 1)
			{
				printf("Try to modify read-only file");

				machine->WriteRegister(2, -1);

				IncreasePC();

				break;
			}



			if (fileId == 0) // Write to stdout
			{
				int i = 0;

				while (buffer[i] != '\0' && buffer[i] != '\n')
				{
				    gSynchConsole->Write(buffer + i, 1);

				    i++;
				}
				buffer[i] = '\n';

				gSynchConsole->Write(buffer + i, 1); // Write last character

				machine->WriteRegister(2, i - 1);

				IncreasePC();

				break;
			}

			// Write into file
			int beforePos = fileSystem->openfile[fileId]->GetCurrentPos();

			int bytesWritten = fileSystem->openfile[fileId]->Write(buffer, charCount);
			if (bytesWritten != 0)
			{
				int afterPos = fileSystem->openfile[fileId]->GetCurrentPos();

				machine->WriteRegister(2, afterPos - beforePos + 1);

				IncreasePC();

				break;
			}		
		}
		case SC_Seek: 
		{
			// Read the position to seek to from register 4
			int position = machine->ReadRegister(4);

			// Read the file descriptor from register 5
			int fileId = machine->ReadRegister(5);

			// Check if the file descriptor is valid
			if (fileId < 1 || fileId > fileSystem->index || fileSystem->openfile[fileId] == NULL)
			{
				machine->WriteRegister(2, -1);
				break;
			}

			// Get the length of the file
			int fileLength = fileSystem->openfile[fileId]->Length();

			// Check if the position is valid
			if (position > fileLength)
			{
				machine->WriteRegister(2, -1);
				break;
			}

			// Move the file pointer to the specified position
			if (position == -1) // Move file pointer to the beginning of file
			{
				position = fileLength;
			}

			fileSystem->openfile[fileId]->Seek(position);

			// Write the new position to register 2
			machine->WriteRegister(2, position);

			IncreasePC();

			break;
		}
		case SC_CreateSemaphore:
		{
			// Read the virtual address of the semaphore name from register 4
			int bufferAddress = machine->ReadRegister(4);

			// Read the initial value of the semaphore from register 5
			int initValue = machine->ReadRegister(5);

			// Convert virtual address to system address and retrieve the semaphore name
			char *semName = User2System(bufferAddress, 32);

			// Check if the semaphore name is empty
			if (strlen(semName) == 0)
			{
				printf("\n Semaphore name is not valid");

				// Set return value to -1
				machine->WriteRegister(2, -1);

				// Increment program counter
				IncreasePC();

				break;
			}

			// Check if system has enough memory
			if (semName == NULL)
			{
				printf("\n Not enough memory in system");

				// Set return value to -1
				machine->WriteRegister(2, -1);

				// Increment program counter
				IncreasePC();

				break;
			}

			// Create a new semaphore
			Semaphore *sem = new Semaphore(semName, initValue);

			// Check if semaphore creation is successful
			if (sem == NULL)
			{
				printf("\n Cannot create semaphore '%s'", semName);

				// Set return value to -1
				machine->WriteRegister(2, -1);

				// Increment program counter
				IncreasePC();

				break;
			}

			// Semaphore creation successful, set return value to 0
			machine->WriteRegister(2, 0);

			// Increment program counter
			IncreasePC();

			break;
		}
		case SC_Join:
		{
			int ID = machine->ReadRegister(4); // Read the ID of the space to join from register 4
			int result = pTab->JoinUpdate(ID); // Call the JoinUpdate function to update the join status of the space
			machine->WriteRegister(2, result); // Write the result to register 2
			IncreasePC(); // Increment the program counter
			break;
		}
		case SC_Exit:
		{
			int exitStatus = machine->ReadRegister(4); // Read the exit status from register 4
			if (exitStatus != 0) // Check if the exit status is not 0 (indicating an error)
			{
				IncreasePC(); // Increment the program counter
				break;
			}
			int result = pTab->ExitUpdate(exitStatus); // Call the ExitUpdate function to update the exit status of the space
			machine->WriteRegister(2, result); // Write the result to register 2
			IncreasePC(); // Increment the program counter
			break;
		}
		case SC_Exec:
		{
			int addName = machine->ReadRegister(4);
			char* name = User2System(addName, 256);
			if(name == NULL){
				DEBUG('a', "\n Khong du bo nho trong bo nho");
				printf("\n Khong du bo nho trong bo nho");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			OpenFile *of = fileSystem->Open(name);
			if(of == NULL){
				printf("\nExec:: Khong the mo file nay");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			delete of;

			int id = pTab->ExecUpdate(name);
			machine->WriteRegister(2, id);

			delete [] name;
			IncreasePC();
			return;
		}
		case SC_Down:			
		{
			//read name from register r4
			int virtAddr = machine->ReadRegister(4);
		
			//Change from user space to system space
			char *name = User2System(virtAddr, MaxFileLength + 1);

			//check if name is null
			if(name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");

				printf("\n Not enough memory in System");

				machine->WriteRegister(2, -1); //return error

				delete[] name;

				IncreasePC();

				return;
			}
			
			int res = semTab->Wait(name); //call Wait in class Stable

			if(res == -1)
			{
				DEBUG('a', "\n Invalide semaphor name!");

				printf("\n Invalid semaphore name!");

				machine->WriteRegister(2, -1);

				delete[] name;

				IncreasePC();

				return;				
			}
			
			delete[] name;

			machine->WriteRegister(2, res); //save result in register r2

			IncreasePC();

			return;
		}
		case SC_Up:		
		{
			int virtAddr = machine->ReadRegister(4);

			char *name = User2System(virtAddr, MaxFileLength + 1);

			if(name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");

				printf("\n Not enough memory in System");

				machine->WriteRegister(2, -1);

				delete[] name;

				IncreasePC();

				return;
			}
			
			int res = semTab->Signal(name);

			if(res == -1)
			{
				DEBUG('a', "\n Khong ton tai ten semaphore nay!");

				printf("\n Khong ton tai ten semaphore nay!");

				machine->WriteRegister(2, -1);

				delete[] name;

				IncreasePC();

				return;				
			}
			
			delete[] name;

			machine->WriteRegister(2, res);

			IncreasePC();

			return;
		}
		default:
			break;
		}
		
	}
}

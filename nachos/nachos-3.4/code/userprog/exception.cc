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

// Increase Program counter

void IncreasePC()

{

	int counter = machine->ReadRegister(PCReg);

	machine->WriteRegister(PrevPCReg, counter);

	counter = machine->ReadRegister(NextPCReg);

	machine->WriteRegister(PCReg, counter);

	machine->WriteRegister(NextPCReg, counter + 4);
}

void ExceptionHandler(ExceptionType which)

{

	int type = machine->ReadRegister(2);

	switch (which)

	{

	case NoException:

		return;

	case PageFaultException:

		DEBUG('a', "\n No valid translation found");

		printf("\n\n No valid translation found");

		interrupt->Halt();

		break;

	case ReadOnlyException:

		DEBUG('a', "\n Write attempted to page marked read-only");

		printf("\n\n Write attempted to page marked read-only");

		interrupt->Halt();

		break;

	case BusErrorException:

		DEBUG('a', "\n Translation resulted invalid physical address");

		printf("\n\n Translation resulted invalid physical address");

		interrupt->Halt();

		break;

	case AddressErrorException:

		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");

		printf("\n\n Unaligned reference or one that was beyond the end of the address space");

		interrupt->Halt();

		break;

	case OverflowException:

		DEBUG('a', "\nInteger overflow in add or sub.");

		printf("\n\n Integer overflow in add or sub.");

		interrupt->Halt();

		break;

	case IllegalInstrException:

		DEBUG('a', "\n Unimplemented or reserved instr.");

		printf("\n\n Unimplemented or reserved instr.");

		interrupt->Halt();

		break;

	case NumExceptionTypes:

		DEBUG('a', "\n Number exception types");

		printf("\n\n Number exception types");

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

				printf("Nhap toi da 1 ki tu!");

				DEBUG('a', "\nERROR: Chi nhap toi da 1 ki tu!");

				machine->WriteRegister(2, 0);
			}

			else if (numBytes == 0) // rong

			{

				printf("Ki tu rong!");

				DEBUG('a', "\nERROR: Ki tu rong!");

				machine->WriteRegister(2, 0);
			}

			else

			{

				char c = buffer[0];

				machine->WriteRegister(2, c);
			}

			delete buffer;

			IncreasePC();

			break;
		}

		case SC_PrintChar:

		{
			char c = (char)machine->ReadRegister(4);
			gSynchConsole->Write(&c, 1);
			IncreasePC();
			printf("\n");
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
					machine->WriteRegister(2, 0);
					delete[] buffer;
					IncreasePC();
					break;
				}
			}

			int number = atoi(buffer); // chuyen chuoi thanh so nguyen
			machine->WriteRegister(2, number);
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
			if (number < 0)
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
			printf("\n");
			IncreasePC();
			break;
		}	
		case SC_ReadString:
		{

			int virtAddr = machine->ReadRegister(4);
			int length = machine->ReadRegister(5);
			char *buffer = new char[length + 1];
			gSynchConsole->Read(buffer, length);
			System2User(virtAddr, length, buffer);
			delete[] buffer;
			IncreasePC();
			break;
		}

		case SC_PrintString:
		{
			int virtAddr = machine->ReadRegister(4);
			char *buffer = User2System(virtAddr, 255);
			int length = 0;
			while (buffer[length] != '\0')
			{
				length++;
			}
			gSynchConsole->Write(buffer, length + 1);
			IncreasePC();
			printf("\n");
			break;
		}
		}
	}
}

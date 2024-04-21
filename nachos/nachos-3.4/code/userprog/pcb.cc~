#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"


PCB::PCB(int id)
{	
	joinSem= new Semaphore("JoinSem",0);
	exitSem= new Semaphore("ExitSem",0);
	mutex= new Semaphore("Mutex",1);
	pid= id;
	exitCode= 0;
	numwait= 0;
	if(id)
		parentID= currentThread->processID;
	else
		parentID= 0;
	thread= NULL;
	JoinStatus= -1;
}

PCB::~PCB()
{
	
	if(joinSem != NULL)
		delete this->joinSem;
	if(exitSem != NULL)
		delete this->exitSem;
	if(mutex != NULL)
		delete this->mutex;
	if(thread != NULL)
	{		
		thread->FreeSpace();
		thread->Finish();
		
	}
}
int PCB::GetID(){ return this->thread->processID; }
int PCB::GetNumWait() { return this->numwait; }
int PCB::GetExitCode() { return this->exitCode; }

void PCB::SetExitCode(int ec){ this->exitCode = ec; }

void PCB::JoinWait()
{

    joinSem->P();
}

void PCB::JoinRelease()
{ 
    joinSem->V();
}

void PCB::ExitWait()
{ 
    exitSem->P();
}

void PCB::ExitRelease() 
{
    exitSem->V();
}

void PCB::IncNumWait()
{
	mutex->P();
	++numwait;
	mutex->V();
}

void PCB::DecNumWait()
{
	mutex->P();
	if(numwait > 0)
		--numwait;
	mutex->V();
}

void PCB::SetFileName(char* fn){ strcpy(FileName,fn);}
char* PCB::GetFileName() { return this->FileName; }

int PCB::Exec(char* filename, int pID)
{  
   mutex->P();
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		mutex->V();
		return -1;
	}
	thread->processID= pID;
	thread->Fork(MyStartProcess,pID);
	mutex->V();
	return pID;

}

void MyStartProcess(int pID)
{
	char *filename= pTab->GetFileName(pID);
	AddrSpace *space= new AddrSpace(filename);
	if(space == NULL)
	{
		printf("\nLoi: Khong du bo nho de cap phat cho tien trinh !!!\n");
		return; 
	}
	currentThread->space= space;

	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;
						// the address space exits
						// by doing the syscall "exit"
}



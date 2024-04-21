#include "ptable.h"
#include "system.h"
#include "openfile.h"
#define MAXPROCESS 10


PTable::PTable(int size)
{

    if (size < 0)
        return;

    pSize = size;
    bm = new BitMap(size);
    bmSem = new Semaphore("bmsem",1);

    int i;
    for(i = 0 ; i < MAXPROCESS ; ++i)
		pcb[i] = NULL;

    bm->Mark(0);

    pcb[0] = new PCB(0);
    pcb[0]->SetFileName("./test/scheduler");
    pcb[0]->parentID = -1;
}

PTable::~PTable()
{
	int i=0;
	if(bm!=NULL)
		delete bm;
	if(bmSem!=NULL)
		delete bmSem;
	for(i=0; i<MAXPROCESS; i++)
		if(pcb[i]!=NULL)
			delete pcb[i];
}

int PTable::ExecUpdate(char* name)
{

	bmSem->P();
	
	if(name == NULL)
	{
		printf("\nPTable::Exec : Can't not execute name is NULL.\n");
		bmSem->V();
		return -1;
	}

	if( strcmp(name,"./test/scheduler") == 0 || strcmp(name,currentThread->getName()) == 0 )
	{
		printf("\nPTable::Exec : Can't not execute itself.\n");		
		bmSem->V();
		return -1;
	}

	int index = this->GetFreeSlot();

	if(index < 0)
	{
		printf("\nPTable::Exec :There is no free slot.\n");
		bmSem->V();
		return -1;
	}

	pcb[index] = new PCB(index);
	pcb[index]->SetFileName(name);

    	pcb[index]->parentID = currentThread->processID;

	int pid = pcb[index]->Exec(name,index);

	bmSem->V();
	return pid;
}

int PTable::JoinUpdate(int id)
{
	if(id < 0)
	{
		printf("\nPTable::JoinUpdate : id = %d", id);
		return -1;
	}

	if(currentThread->processID != pcb[id]->parentID)
	{
		printf("\nPTable::JoinUpdate Can't join in process which is not it's parent process.\n");
		return -1;
	}

	pcb[pcb[id]->parentID]->IncNumWait();
	
	
	pcb[id]->JoinWait();
	
	int ec = pcb[id]->GetExitCode();

	pcb[id]->ExitRelease();

	return ec;
}
int PTable::ExitUpdate(int exitCode)
{              
	int id = currentThread->processID;
	if(id == 0)
	{
		
		currentThread->FreeSpace();		
		interrupt->Halt();
		return 0;
	}
    
        if(IsExist(id) == false)
	{
		printf("\nPTable::ExitUpdate: This %d is not exist. Try again?", id);
		return -1;
	}


	pcb[id]->SetExitCode(exitCode);
	pcb[pcb[id]->parentID]->DecNumWait();
    

	pcb[id]->JoinRelease();
    
	pcb[id]->ExitWait();
	
	Remove(id);
	return exitCode;
}

int PTable::GetFreeSlot()
{
	return bm->Find();
}


bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

char* PTable::GetFileName(int id)
{
	return (pcb[id]->GetFileName());
}



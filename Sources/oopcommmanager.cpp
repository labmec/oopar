
#include "oopcommmanager.h"

OOPCommunicationManager::OOPCommunicationManager() : fTM(0) {
	f_num_proc = f_myself = 0;
	fCM = this;
}

OOPCommunicationManager::~OOPCommunicationManager(){}

int OOPCommunicationManager::Initialize (char *process_name, int num_of_process){
	return 0;
}

int OOPCommunicationManager::GetProcID(){
	return f_myself;
}

int OOPCommunicationManager::NumProcessors (){
	return f_num_proc;
}

int OOPCommunicationManager::IAmTheMaster(){
	return 0;
}

char * OOPCommunicationManager::ClassName(){
	return ("");
}

void OOPCommunicationManager::SetTaskManager(TPZAutoPointer<OOPTaskManager> TM)
{
	fTM = TM;
}

TPZAutoPointer<OOPTaskManager> OOPCommunicationManager::TM()
{
	return fTM;
}

TPZAutoPointer<OOPCommunicationManager> OOPCommunicationManager::CM()
{
	return fCM;
}

void OOPCommunicationManager::ClearPointer()
{
	fCM = TPZAutoPointer<OOPCommunicationManager>(0);
	fTM = TPZAutoPointer<OOPTaskManager>(0);
}


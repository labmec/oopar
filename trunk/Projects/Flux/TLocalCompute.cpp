#include "TLocalCompute.h"
#include "TContribution.h"
#include "TPartitionRelation.h"
#include "TTaskComm.h"
#include "oopmetadata.h"

void TLocalCompute::InitializePartitionRelationPointer() {
  OOPMetaData *objptr = fDataDepend[0].ObjPtr();
  fPartRelationPtr = dynamic_cast<TPartitionRelation *> (objptr->Ptr());
}

void TLocalCompute::ComputeLocalFluxes(){
  OOPMetaData *ptr = fDataDepend[3].ObjPtr();
  OOPDataVersion ver = ptr->Version();
  //  int nlevel = ver.GetNLevels();
  int ncontr = fPartRelationPtr->IncomingContribution(fPartition);
  ver.IncrementLevel(ncontr);
  ++ver;
  ptr->SetVersion(ver,Id());

}

void TLocalCompute::TransmitFLuxes(TContribution &relation){
  //Criar tarefa de comunicação
  //como fazer isso ?
  //  int numout = fPartRelationPtr->OutgoingContribution(fProc);
  int npartitions = fPartRelationPtr->GetNPartitions();
  int i = 0;
  OOPDataVersion rhsver = GetCommunicationVersion();
  for(i = 1; i < npartitions; i++){
    if(i==fPartition) continue;
    TContribution *cont = &fPartRelationPtr->GetRelation(fPartition,i);
    if(cont->IsEmpty()) continue;
    TTaskComm * task = new TTaskComm(GetProcID());
    task->AddDependentData(fDataDepend[3].fDataId, EWriteAccess, rhsver);
    task->Submit();
  }
}
void TLocalCompute::ComputeFrontierFluxes(){}

OOPMReturnType TLocalCompute::Execute(){
  this->InitializePartitionRelationPointer();
  
  // message #1.1 to this:TLocalCompute
  this->ComputeFrontierFluxes();
  
  // message #1.3 to this:TLocalCompute
  this->ComputeLocalFluxes();
  return ESuccess;
}
TLocalCompute::TLocalCompute(int ProcId, int partition): OOPTask(ProcId), fPartition(partition) {}


#include "TLocalCompute.h"
#include "TContribution.h"
#include "TPartitionRelation.h"
#include "TTaskComm.h"
#include "oopmetadata.h"
void TLocalCompute::InitializePartitionRelationPointer() {
  OOPMetaData *objptr = fDataDepend[0].ObjPtr();
  fPartRelationPtr = dynamic_cast<TPartitionRelation *> (objptr->Ptr());
}
void TLocalCompute::ComputeLocalFluxes(){}
void TLocalCompute::TransmitFLuxes(TContribution &relation){
  //Criar tarefa de comunicação
  //como fazer isso ?
  int numout = fPartRelationPtr->OutgoingContribution(fProc);
  vector<int> out(numout);
  //Eu preciso de mais uma classe para comunicação !!!!!
  //Preciso saber ainda o OjectId de cada fRhs para contribuir corretamente !
  //TLocalCompute terá um lista com os Ids dos objetos aos quais irá contribuir
  //Assume-se que a dependência local é o primeiro da fila e que os demais são
  //referentes às contribuições externas
  int i = 0;
  int ncontr = GetNDependentData();
  OOPDataVersion version;
  for(i = 1; i < ncontr; i++){
    TTaskComm * task = new TTaskComm(fProc);
    task->AddDependentData(fDataDepend[i].fDataId, EWriteAccess, version);
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
TLocalCompute::TLocalCompute(int ProcId): OOPTask(ProcId) {}

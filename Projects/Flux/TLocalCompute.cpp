#include "TLocalCompute.h"
#include "TContribution.h"
#include "TPartitionRelation.h"
#include "TTaskComm.h"
#include "oopmetadata.h"

void TLocalCompute::InitializePartitionRelationPointer() {
  OOPMetaData *objptr = fDataDepend.Dep(0).ObjPtr();
  fPartRelationPtr = dynamic_cast<TPartitionRelation *> (objptr->Ptr());
}

void TLocalCompute::ComputeLocalFluxes(){
  OOPMetaData *ptr = fDataDepend.Dep(3).ObjPtr();
  OOPDataVersion ver = ptr->Version();
  //  int nlevel = ver.GetNLevels();
  int ncontr = fPartRelationPtr->IncomingContribution(fPartition);
  ver.IncrementLevel(ncontr);
  ++ver;
  cout << "TLocalCompute new version " << endl;
  ver.Print(cout);
  ptr->SetVersion(ver,Id());

}
void TLocalCompute::SetRhsIds(vector<OOPObjectId> &rhsids, OOPDataVersion &rhsversion){
	fRhsIds = rhsids;
	fRhsVersion = rhsversion;
}

void TLocalCompute::TransmitFLuxes(){
  int npartitions = fPartRelationPtr->GetNPartitions();
  int i = 0;
  OOPMetaData *ptr = fDataDepend.Dep(3).ObjPtr();
  OOPDataVersion rhsver = ptr->Version();
  rhsver.IncrementLevel(-1);
  int nver = rhsver.GetNLevels();
  rhsver.SetLevelVersion(nver-1,-1);
  for(i = 1; i < npartitions; i++){
    if(i==fPartition) continue;
    TContribution *cont = &fPartRelationPtr->GetRelation(fPartition,i);
    if(cont->IsEmpty()) continue;
    TTaskComm * task = new TTaskComm(GetProcID());
    OOPMDataDepend depend(fDataDepend.Dep(3).Id(), EWriteAccess, rhsver);
    cout << "TLocalCompute::TransmitFluxes depend on version " << endl;
    rhsver.Print(cout);
    task->AddDependentData(depend);
    task->Submit();
  }
}
void TLocalCompute::ComputeFrontierFluxes(){
#warning "ComputeFrontierFluxes is empty"
	cout << "Nothing Implemented in ComputeFrontierFluxes\n";
	cout.flush();	
}

OOPMReturnType TLocalCompute::Execute(){
  cout << "Executing TLocalCompute task\n";
  
  this->InitializePartitionRelationPointer();
  
  // message #1.1 to this:TLocalCompute
  this->ComputeFrontierFluxes();
  
  this->TransmitFLuxes();
  // message #1.3 to this:TLocalCompute
  this->ComputeLocalFluxes();
  return ESuccess;
}
TLocalCompute::TLocalCompute(int ProcId, int partition): OOPTask(ProcId), fPartition(partition) {}
OOPDataVersion TLocalCompute::GetCommunicationVersion(){
	return fPartRelationPtr->Version();
}

#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"


OOPMReturnType TParCompute::Execute(){

    //submit subtasks to the Task Manager
    int i;
    // increment the level of the state and rhs objects with appropriate cardinality
    for(i=0;i<fNPartitions;i++){
      TLocalCompute *ltask = new TLocalCompute(GetProcID(),i);
      ltask->AddDependentData(fPartRelationId,EReadAccess,fPartRelationVersion);
      ltask->AddDependentData(fMeshIds[i],EReadAccess,fMeshVersions);
      ltask->AddDependentData(fStateIds[i],EWriteAccess,fDataVersions);
      ltask->AddDependentData(fRhsIds[i],EWriteAccess,fDataVersions);
      ltask->SetRhsIds(fRhsIds,fDataVersions);
      ltask->Submit();
    }
    return ESuccess;
}

void TParCompute::SetPartitionRelationId(OOPObjectId & Id, OOPDataVersion &version){
    fPartRelationId = Id;
    fPartRelationVersion = version;
}

TParCompute::TParCompute(int procid, int numpartitions): OOPTask(procid) , fNPartitions(numpartitions) {
}

void TParCompute::SetStateId(vector<OOPObjectId> & Id, OOPDataVersion &stateversion){
    //Check if Id was already inserted
    fStateIds = Id;
    fDataVersions = stateversion;
}
void TParCompute::SetRhsId(vector<OOPObjectId> & Id, OOPDataVersion &rhsversion){
    fRhsIds = Id;
    fDataVersions = rhsversion;
}
void TParCompute::SetMeshId(vector<OOPObjectId> & Id, OOPDataVersion &meshversion){
    fMeshIds = Id;
    fMeshVersions = meshversion;
}

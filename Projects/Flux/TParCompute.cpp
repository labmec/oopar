#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"


OOPMReturnType TParCompute::Execute(){
    //submit subtasks to the Task Manager
    int i;
    for(i=0;i<fNPartitions;i++){
        fTaskIds[i]=fTasks[i].Submit();
    }
	return ESuccess;
}

void TParCompute::InitializeSolutionVersion(){
    /**
      For each Flux object, obtain the number of contribution from
      and set the version object related to each flux accordingly
    */
//   TPartitionRelation * table;
//    table = DM->GetObjPtr(fPartRelationId);
//    int ncont=0;
//    for(i=0;i<table->GetNPartitions();i++){
//        ncont = Relation.IncomingContribution(i);
        //Isso deve ser substituído por um acesso a partir da tarefa, que terá
        //acesso à versão.
//        DM->IncrementLevel(fRhsIds[i], Id(), ncont, fProc);
//    }


}
void TParCompute::CreateFluxesTasks(  ){
    int i;
    //Pointers to LocalTasks created
    fTasks = new TLocalCompute[fPartRelationPtr->GetNPartitions()] (fProc);

    //For each LocalTask its necessary data dependence attribution
    OOPMDataState st_read = EReadAccess;
    OOPMDataState st_w = EWriteAccess;
    OOPDataVersion ver;
    //Atençao com versão!
    ver.SetLevelVersion(0,-1);
    ver.IncrementLevel(0);
    ver.SetLevelVersion(1,-1);

    OOPDataVersion part_version;
    int ncontributions;
	OOPObjectId aux_Id = Id();
    //Set data dependence and submit tasks to the TM;
    for(i=0;i<fPartRelationPtr->GetNPartitions();i++){
        //Data dependence on the Tasks objects
        fTasks[i].AddDependentData(fPartRelationId, st_read, part_version);
        fTasks[i].AddDependentData(fStateIds[i], st_read, ver);
        fTasks[i].AddDependentData(fRhsIds[i], st_w, ver);

        //Contributions to each partition
        ncontributions = fPartRelationPtr->IncomingContribution(i);
		DM->IncrementLevel(aux_Id, fRhsIds[i], ncontributions, (long)fProc);
    }
}

OOPObjectId TParCompute::GetPartitionRelationId(){
    return fPartRelationId;
}
void TParCompute::SetPartitionRelationId(OOPObjectId & Id, OOPDataVersion &version){
    fPartRelationId = Id;
    fPartRelationVersion = version;
}

OOPObjectId &TParCompute::GetRhsId(int pos){
    return fRhsIds[pos];
}

OOPObjectId &TParCompute::GetStateId(int pos){
    return fStateIds[pos];
}
TParCompute::TParCompute(int procid): OOPTask(procid) {
    fTasks=0;
}

void TParCompute::SetStateId(OOPObjectId & Id){
    //Check if Id was already inserted
    fStateIds.push_back(Id);
}
void TParCompute::SetRhsId(OOPObjectId & Id){
    fRhsIds.push_back(Id);
}
void TParCompute::SetStateId(vector<OOPObjectId> & Id){
    //Check if Id was already inserted
    fStateIds = Id;
}
void TParCompute::SetRhsId(vector<OOPObjectId> & Id){
    fRhsIds = Id;
}

void TParCompute::InitializeTaskDependencies() {
	AddDependentData(fPartRelationId,EReadAccess,fPartRelationVersion);
}

void TParCompute::InitializePartitionRelationPointer() {
	OOPSaveable *objptr = fDataDepend[0].ObjPtr();
	fPartRelationPtr = dynamic_cast<TPartitionRelation *> (objptr);
}

#include "TParCompute.h"
#include "TLocalCompute.h"
#include "oopdatamanager.h"


OOPMReturnType TParCompute::Execute(){

    //submit subtasks to the Task Manager
    InitializePartitionRelationPointer();
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
        //Isso deve ser substitu�o por um acesso a partir da tarefa, que ter�        //acesso �vers�.
//        DM->IncrementLevel(fRhsIds[i], Id(), ncont, fProc);
//    }


}
void TParCompute::CreateFluxesTasks(  ){
    int i, j;
    //Pointers to LocalTasks created
    fTasks = new TLocalCompute[fPartRelationPtr->GetNPartitions()] (fProc);
    //Aten�o com DELETE !!!!

    //For each LocalTask its necessary data dependence attribution
    OOPMDataState st_read = EReadAccess;
    OOPMDataState st_w = EWriteAccess;
    OOPDataVersion ver;
    //Aten�o com vers�!
    ver.SetLevelVersion(0,-1);
    ver.IncrementLevel(0);
    ver.SetLevelVersion(1,-1);

    OOPDataVersion part_version;
    int ncontributions;
	OOPObjectId aux_Id = Id();
    vector<int> out;
    //Set data dependence and submit tasks to the TM;
    for(i=0;i<fPartRelationPtr->GetNPartitions();i++){
        //Data dependence on the Tasks objects
        fTasks[i].AddDependentData(fPartRelationId, st_read, part_version);
        fTasks[i].AddDependentData(fStateIds[i], st_read, ver);
        fTasks[i].AddDependentData(fRhsIds[i], st_w, ver);

        //Contributions to each partition
        ncontributions = fPartRelationPtr->IncomingContribution(i);
        
        //Passar para as subtarefas.
		DM->IncrementLevel(aux_Id, fRhsIds[i], ncontributions, (long)fProc);
    }

    OOPDataVersion * version = new OOPDataVersion[fPartRelationPtr->GetNPartitions()];
    //Inserir as depend�cias de escrita sobre os fluxos de outros parti�es.
    for(i=0;i<fPartRelationPtr->GetNPartitions();i++)
        version[i]=DM->GetDataVersion(fRhsIds[i]);

    //Para cada Rhs, deve-se ainda estabelecer as depend�cias referentes �comu-
    //nica�o
    for(i=0;i<fPartRelationPtr->GetNPartitions();i++){
      // so para conseguir compilar - Philippe
        out.resize(fPartRelationPtr->OutgoingContribution(i));
        for(j=0;j<(signed int)out.size();j++){
            version[out[j]].operator++();
            fTasks[i].AddDependentData(fRhsIds[out[j]], st_w, version[out[j]]);
        }

    }
    delete [] version;

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
TParCompute::TParCompute(int procid, int numpartitions): OOPTask(procid) , fNPartitions(numpartitions) {
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

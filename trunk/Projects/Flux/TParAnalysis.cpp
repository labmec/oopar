/* Generated by Together */

#include "TParAnalysis.h"

void TParAnalysis::SetupEnvironment(){
	
  TPartitionRelation *table = TPartitionRelation::CreateRandom(fNumPartitions);
  //  int ProcId = DM->GetProcID();
	
  // message #1.2 to table:TPartitionRelation
  // TPartitionRelation * table = ;

  int npartitions = table->GetNPartitions();


  int ip;
  // message #1.3 to rhs:TParRhs
  vector<TParRhs *> RhsVec(npartitions);
  fRhsId.resize(npartitions);
  for(ip=0; ip<npartitions; ip++) RhsVec[ip] = new TParRhs();

  // message #1.4 to state:TParState
  vector<TParState *> StateVec(npartitions);
  fStateId.resize(npartitions);
  for(ip=0; ip<npartitions; ip++) StateVec[ip] = new TParState();

  // message #1.5 to mesh:TParMesh
  vector<TParMesh *> MeshVec(npartitions);
  fMeshId.resize(npartitions);
  for(ip=0; ip<npartitions; ip++) MeshVec[ip] = new TParMesh();

  int i;
  // message #1.7 to ver:OOPDataVersion
  for (i = 0; i < npartitions; i++) {
    // message #1.8.1 to DM:OOPDataManager
    fRhsId[i] = DM->SubmitObject(RhsVec[i], 1);

    // message #1.8.3 to DM:OOPDataManager
    fStateId[i] = DM->SubmitObject(StateVec[i], 1);

    // message #1.8.5 to DM:OOPDataManager
    fMeshId[i] = DM->SubmitObject(MeshVec[i], 1);
    table->SetMeshId(i,fMeshId[i]);
  }
  // message #1.6 to ver:OOPDataVersion
  OOPDataVersion ver;
  int level = 0;
  int version = 0;
  ver.SetLevelVersion(level, version);
  OOPMDataState st = EReadAccess;
  fRelationTable = DM->SubmitObject(table, 1);

  ReleaseAccessRequests();

  AddDependentData(fRelationTable,st,ver);

  st = EVersionAccess;
  for(ip=0; ip<npartitions; ip++) {
    AddDependentData(fMeshId[ip],st,ver);
  }

  for(ip=0; ip<npartitions; ip++) {
    AddDependentData(fStateId[ip],st,ver);
  }

  for(ip=0; ip<npartitions; ip++) {
    AddDependentData(fRhsId[ip],st,ver);
  }

}

void TParAnalysis::CreateParCompute() {

  //  int ndepend = GetNDependentData();
  deque<OOPMDataDepend>::iterator dep;
  dep = fDataDepend.begin();
  OOPObjectId tableid = (*dep).fDataId;
  dep++;

  OOPDataVersion randver;
  randver.SetLevelVersion(0,10);
  randver.IncrementLevel(13);
  randver.SetLevelVersion(1,12);
  randver.IncrementLevel(25);
  randver.SetLevelVersion(2,24);
  OOPDataVersion taskver(randver);
  taskver.SetLevelVersion(1,-1);
  fTaskVersion = taskver;
  // skipping the mesh dependency
  int count =0;
  while(count < fNumPartitions) {
    dep ++;
    count++;
  }
  count = 0;
  // Setting the data version
  while(count < 2*fNumPartitions) {
    (*dep).ObjPtr()->SetVersion(randver,Id());
    dep ++;
    count++;
  }


  // message #1.1 to pc:TParCompute
  TParCompute * pc = new TParCompute(GetProcID(),fNumPartitions);

  OOPDataVersion ver;
  pc->SetMeshId(fMeshId,ver);
  pc->SetRhsId(fRhsId,taskver);
  pc->SetStateId(fStateId,taskver);
  pc->SetPartitionRelationId(tableid,ver);
  OOPMDataState st = EReadAccess;
  //  pc->AddDependentData(tableid,st,ver);

  //	pc->SetPartitionRelationId(tableId , ver);
  // message #1.9 to pc:TParCompute
  pc->Submit();
  ReleaseAccessRequests();

  dep = fDataDepend.begin();
  dep++;
  count =0;
  while(count < fNumPartitions) {
    dep ++;
    count++;
  }
  count = 0;
  // Setting the data version
  while(count < 2*fNumPartitions) {
    OOPDataVersion ver((*dep).ObjPtr()->Version());
    ver.Increment();
    AddDependentData((*dep).ObjPtr()->Id(),st,ver);
    dep ++;
    count++;
  }
}

void TParAnalysis::SetAppropriateVersions() {
  OOPDataVersion ver;
  //  OOPMDataState st = EReadAccess;
  deque<OOPMDataDepend>::iterator dep;
  dep = fDataDepend.begin();
  while(dep != fDataDepend.end()) {
    OOPDataVersion solver = (*dep).ObjPtr()->Version();
    AdaptSolutionVersion(solver);
    (*dep).ObjPtr()->SetVersion(solver,Id());
  }
}

void TParAnalysis::AdaptSolutionVersion(OOPDataVersion &version) {

  int depth = fTaskVersion.GetNLevels();
  int versdepth = version.GetNLevels();
  int d;
  for(d=versdepth; d<depth; d++) {
    int taskver = fTaskVersion.GetLevelVersion(d);
    if(taskver != -1) {
      version.IncrementLevel(taskver+1);
      version.SetLevelVersion(d,taskver);
    } else {
      version.IncrementLevel(1);
      version.SetLevelVersion(d,1);
    }
  }
}

OOPMReturnType TParAnalysis::Execute() {

  if(fRelationTable.IsZero()) SetupEnvironment();
  else if(fTaskVersion.GetNLevels() <= 1) CreateParCompute();
  else SetAppropriateVersions();
  return ESuccess;
}

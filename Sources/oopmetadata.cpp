#include "oopmetadata.h"
#include "oopdatamanager.h"
#include "oopcommmanager.h"
#include "ooptaskmanager.h"
#include <vector>
class OOPDataVersion;
class OOPMetaData;
class OOPDMOwnerTask;
//class OOPMDataState;
class OOPSaveable;

class OOPObjectId;


void OOPMetaData::SetCardinality(int level, int cardinality){
  fVersion.SetLevelCardinality(level, cardinality);
}
void OOPMetaData::IncrementVersionLevel(int cardinality){
  fVersion.IncrementLevel(cardinality);
}
void OOPMetaData::DecreaseVersionLevel(){
  fVersion.DecreaseLevel();
}


int OOPMetaData::HasAccess(OOPObjectId & TaskId, OOPMDataState AccessType, OOPDataVersion & version){
  deque<OOPAccessInfo>::iterator i;
  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    if ((i->fTaskId == TaskId) &&
	(i->fState == AccessType) && 
	(i->fVersion == version)) return 1;
  }
  return 0;
}

void OOPMetaData::VerifyAccessRequests(){
	deque<OOPAccessInfo>::iterator i;
	for(i=fTaskList.begin();i!=fTaskList.end();i++){
		/*if ((i->fVersion == fVersion) &&
		  (i->fState == State())){
		  if (i->fProc == fProc)*/
		// CanExecute is badly implemented. Therefore this method will not work well either
		if(CanExecute(i->fVersion, i->fState)){
			OOPObjectId objid;
			objid=i->fTaskId;
			if (i->fProc == fProc){
				//Avisar ao TM para validar o acesso ao dado requrido
				OOPDataVersion ver = i->fVersion;
				OOPObjectId objData = Id();
				OOPMDataDepend depend(Id(),i->fState,ver);
				TM->NotifyAccessGranted(objid, depend, this);
				switch (i->fState)
				{
					case  EWriteAccess:
						GrantWriteAccess(i->fTaskId, i->fState, i->fVersion);
						break;
					case  EVersionAccess:
						GrantVersionAccess(i->fTaskId, i->fProc, i->fState, i->fVersion);
						break;
					case  EReadAccess:
						GrantReadAccess(i->fTaskId, i->fProc, i->fState, i->fVersion);
						break;
					default:
						break;
				}

			}else if(i->fState==EReadAccess){
				//Criar mensagem
				if (!HasReadAccess(i->fProc)){
					fAccessProcessors.push_back(i->fProc);
					GrantReadAccess(i->fTaskId, i->fProc, i->fState, i->fVersion);
				}
			}else if(i->fState==EVersionAccess){
				//Do something
			}
		}
	}
}

OOPObjectId OOPMetaData::Id() const { return fObjId;}
bool OOPMetaData::CanExecute(const OOPDataVersion & version, OOPMDataState access){

#ifndef WIN32
#warning "OOPMetaData::CanExecute BADLY IMPLEMENTED METHOD"
#endif
	
  // THIS METHOD IS OBSOLETE ANYWAY
  //	if(fTaskWrite) return false;
  //cout << "Inside CanExecute method -----------------------------------------------" << endl;
  //fVersion.Print(cout);
  //version.Print(cout);
  //cout << "Inside CanExecute method -----------------------------------------------" << endl;
  //if (fVersion==version)	return true;
  bool can_I = true;
  int i;
  for(i=0;i < version.GetNLevels();i++){
    if(fVersion.GetLevelVersion(i)!=version.GetLevelVersion(i) && version.GetLevelVersion(i)!=-1){
      can_I = false;
    }
  }

  return can_I;
}

void OOPMetaData::ReleaseAccess(const OOPObjectId &id, const OOPMDataState &st,const OOPDataVersion & ver){
  deque<OOPAccessInfo>::iterator i;
  bool bobj = false;
  bool bstate = false;
  bool bversion = false;
  cout << fTaskList.size() << endl;
  int l_size = fTaskList.size();
  int k;
  cout.flush();
  i=fTaskList.begin();
  for(k = 0; k<l_size;k++){
    if(i->fTaskId == id){
      bobj=true;
      if (i->fState == st){
	bstate=true;
	if(st==EWriteAccess) fTaskWrite.IsZero();
	if(i->fVersion == ver) {
	  bversion=true;
	  fTaskList.erase(i);
	}
      }
    }
    i++;
  }
  if(st == EWriteAccess && id == fTaskWrite) fTaskWrite.Zero();
  if(st == EVersionAccess && id == fTaskVersionAccess) fTaskVersionAccess.Zero();
  if(!bobj) cerr << "Release request missmatch! File:" << __FILE__ << " Line:" << __LINE__ << endl;
  if(!bstate) cerr << "State missmatch! File:" << __FILE__ << " Line:" << __LINE__ << endl;
  if(!bversion) cerr << "Version missmatch! File:" << __FILE__ << " Line:" << __LINE__ << endl;
// A task has given up its access request. Check whether another task can be granted access
  VerifyAccessRequests();
}

OOPMDataState OOPMetaData::State(){
  if (fTaskWrite.IsZero()) {
    return EWriteAccess;
  }else{
    if (fAccessProcessors.size() > 0){
      return EReadAccess;
    }else{
      return ENoAccess;
    }
  }
}

void OOPMetaData::SubmitAccessRequest(const OOPObjectId &taskId,const OOPDataVersion &version, OOPMDataState access, long proc){
  OOPAccessInfo ac(taskId, access, version, proc);
  fTaskList.push_back(ac);
#ifdef DEBUG
  cout << "Access Request submitted on TData" << endl;
  cout << "Task List size " << fTaskList.size() << endl;
#endif
  if (CanExecute(version, access)){
    //take appropriate actions
    //Pegar ponteiro para o objeto TTask no TM e avisar ao tal Task que este
    //dado está pronto para ser usado.
	
	switch (access )
	{
		case  EReadAccess:
			//do something;
			break;
		case  EVersionAccess:
			fTaskVersionAccess = taskId;
			break;
		case  EWriteAccess:
			fTaskWrite = taskId;
			break;
		defaults:
			break;
	}

	OOPMDataDepend depend(fObjId,access,version);
    TM->NotifyAccessGranted(taskId, depend, this);

  }
  
  if(fProc != proc){
    if (!HasReadAccess(proc)){
      fAccessProcessors.push_back(proc);
      GrantReadAccess(taskId,proc,access,version);
    }
  }
}




OOPMetaData::OOPMetaData(OOPSaveable *ObPtr, OOPObjectId & ObjId, int ProcId, OOPMDataState st) //:
  //TData::TData(TSaveable *ObPtr, OOPObjectId ObjId, MDataState st) :
  /*fAccessProcessors(0), fBlockingReadProcesses(0)*/ {
  fProc = ProcId;
  fObjPtr = ObPtr;
  fObjId = ObjId;
  //fTaskWrite = 0;
  //Initializes fVersion to Level 0 and cardinality 0;
  //fVersion.Initialize();
  fTrans = ENoTransition;
  fNumConfirm = 0;
  fToDelete = 0;
  //fAccessState not initialize yet
  //fAccessState=0;
  //fAccessProcessors.resize(CM->NumProcessors());
  //fAccessProcessors.fill(ENoAccess);
  //fAccessProcessors[DM->GetProcID()] = st;
  fTrace = 0;
}

void OOPMetaData::SetId(OOPObjectId & id){
  fObjId = id;
}

/*
  TSaveable *TData::Ptr() {
  return fObjPtr;
  }

  TSaveable *TData::operator->() {
  return fObjPtr;
  }
*/
void OOPMetaData::TransferObject(int ProcId) {
  //int TData::TransferObject(int ProcId, OOPObjectId TaskId, MDataState AccessRequest, OOPDataVersion version) {
  // if the data does not belong to this processor, issue only a request
  //	if(HasAccess(ProcId, TaskId, AccessRequest,version)) return 1;
  //Check if versions are compatible, when such things happen
  //(version <= fVersion && fProc != DM->GetProcID())
  //    RequestTransferObject(ProcId, TaskId, AccessRequest, version);
#ifndef WIN32
#warning "OOPMetaData::TransferObject is not implemented"
#endif
}


void OOPMetaData::HandleMessage(OOPDMOwnerTask & ms) {
  int myproc = DM->GetProcID();

  if(fTrace)//Erico
    TraceMessage(ms);

  switch(ms.fType) {
  case ENoMessage:
    break;
  case ENotifyAccessState:
    fAccessProcessors = ms.fAccessProcessors;
    //			fBlockingReadProcesses = ms.fBlockingReadProcesses;
    fVersion = ms.fVersion;
    break;
  case ECancelReadAccess: {
    // check whether a task has blocking read access
    // check whether a task has exclusive write access
    // check whether this processor is not owner of this data
    //fAccessProcessors[myproc] = ENoAccess;
    fAccessProcessors.resize(0);
    fAccessProcessors.push_back(myproc);
    if(fObjPtr) {
      delete fObjPtr;
      fObjPtr = 0;
    }
    OOPDMOwnerTask m(ECancelReadAccessConfirmation,ms.fProcOrigin);
    m.fProcDestination = ms.fProcOrigin;
    m.fProcOrigin = myproc;
    m.fObjId = fObjId;
    CM->SendTask(&m);
    break;
  }
  case ECancelReadAccessConfirmation :
    // check whether this processor is owner of the data
    // check whether the transition state is ReadTransition
    fNumConfirm--;
    fAccessProcessors[ms.fProcOrigin] = ENoAccess;
    if(!fNumConfirm) {
      fTrans = ENoTransition;
      //NotifyAccessStates();
      VerifyAccessRequests();
      if(fToDelete) DeleteObject(fObjId);
      //				 BroadCastAccessState();
    }
    // check whether fNumConfirm is not less than zero
    break;
  case ETransferOwnership : {
    OOPDMOwnerTask m(ETransferOwnershipConfirmation,ms.fProcOrigin);
    m.fProcDestination = ms.fProcOrigin;
    m.fProcOrigin = myproc;
    m.fObjId = fObjId;
    CM->SendTask(&m);

    fAccessProcessors = ms.fAccessProcessors;
    //			fBlockingReadProcesses = ms.fBlockingReadProcesses;


    fObjId.SetProcId(myproc);
    fVersion = ms.fVersion;
    if(ms.fObjPtr) {
      if(fObjPtr) delete fObjPtr;
      fObjPtr = ms.fObjPtr;
    }
    ms.fObjPtr = 0;		// prevent the object from being deleted

    GrantWriteAccess(ms.fTaskId,ms.fState,ms.fVersion);
    // try to enforce write access
    break;
  }
  case ETransferOwnershipConfirmation :
    fNumConfirm--;
    if(!fNumConfirm) {
      fTrans = ENoTransition;
      if(fToDelete) DeleteObject(fObjId);
    }
    break;
  case EGrantReadAccess : {
    if(ms.fObjPtr) {
      // this statement should never be called !
      if(!fObjPtr) delete fObjPtr;

      fObjPtr = ms.fObjPtr;
      ms.fObjPtr = 0;
    }
    GrantReadAccess(ms.fTaskId, myproc, ms.fState, ms.fVersion);
    OOPDMOwnerTask m(EGrantReadAccessConfirmation,ms.fProcOrigin);
    m.fProcDestination = ms.fProcOrigin;
    m.fProcOrigin = myproc;
    m.fState = ms.fState;
    m.fObjId = fObjId;
    CM->SendTask(&m);
    break;
  }
  case EGrantReadAccessConfirmation :
    fNumConfirm--;
    if(!fNumConfirm) {
      // check whether the transition state is compatible
      fTrans = ENoTransition;
      // broadcast the access state
      //NotifyAccessStates();
      VerifyAccessRequests();
      if(fToDelete) DeleteObject(fObjId);
    }
    break;
  case ENotifyDeleteObject : {
    OOPDMOwnerTask m(ENotifyDeleteObjectConfirmation,ms.fProcOrigin);
    // verify whether the originating processor initiated the deletion
    m.fProcDestination = ms.fProcOrigin;
    m.fProcOrigin = myproc;
    m.fObjId = fObjId;
    CM->SendTask(&m);
    if(fObjPtr) delete fObjPtr;
    fObjPtr = 0;
    fObjId.SetProcId( -1);
    fTaskWrite.Zero();
    fAccessProcessors[myproc] = ENoAccess;
    break;
  }
  case ENotifyDeleteObjectConfirmation :
    fNumConfirm--;
    if(!fNumConfirm) {
      // check whether the transition state corresponds
      // check whether fNumConfirm is not less than zero
      // check whether fObjPtr exists
      OOPObjectId ObjId = fObjId;
      if(fObjPtr) delete fObjPtr;
      fObjPtr = 0;
      fObjId.SetProcId( -1);
      fTaskWrite.Zero();
      fAccessProcessors[myproc] = ENoAccess;
      DM->DeleteObject(ObjId);
    }
    break;
  case ENotifyCreateObject:
    break;
  default:
    exit(-1);
    break;
  }
  // this statement should never be called
  if(ms.fObjPtr) {
    exit(-1);
    delete ms.fObjPtr;
    ms.fObjPtr = 0;
  }
}

void OOPMetaData::RequestTransferObject(int ProcId, OOPObjectId & TaskId,
					OOPMDataState AccessRequest, OOPDataVersion & version){
  //void TData::RequestTransferObject(OOPObjectId TaskId, MDataState AccessRequest, OOPDataVersion version){
  switch(AccessRequest) {
  case EReadAccess :
    /*		case EBlockingReadAccess : {
		OOPDMRequestTask m(ERequestReadAccess,fObjId.GetProcId());
		m.fAccessState = AccessRequest;
		m.fVersion = version;
		m.fProcOrigin = ProcId;
		m.fProcDestination = fObjId.GetProcId();//fProc;
		m.fObjId = fObjId;
		m.fTaskId = TaskId;
		CM->SendTask(&m);
		break;
		}*/
    //case EWriteAccess :
    //case EExclusiveWriteAccess : {
  case EWriteAccess:{
    OOPDMRequestTask m(ERequestWriteAccess,fProc);
    m.fAccessState = AccessRequest;
    m.fVersion = version;
    m.fProcOrigin = ProcId;
    m.fProcDestination = fProc;
    m.fObjId = fObjId;
    m.fTaskId = TaskId;
    CM->SendTask(&m);
    break;
  }
  default:
    break;
  }
}

void OOPMetaData::TransferOwnerShip(OOPObjectId & TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion &version) {
  if(fObjId.GetProcId() != DM->GetProcID()) return;
  //if(fVersion < version) return;
  if(!(fVersion.AmICompatible(version))) return;

  if(fTrans != ENoTransition) {
    cout << "TransferOwnerShip called with data in transition = " << fTrans << endl;
    exit(-1);
  }

  fAccessProcessors[fObjId.GetProcId()] = ENoAccess;
  OOPDMOwnerTask m(ETransferOwnership,ProcId);
  m.fTaskId = TaskId;
  m.fProcDestination = ProcId;
  m.fState = AccessRequest;
  m.fVersion = fVersion;
  m.fObjPtr = 0;
  m.fObjId = fObjId;
  m.fAccessProcessors = fAccessProcessors;
  //	m.fBlockingReadProcesses = fBlockingReadProcesses;
  if(!HasReadAccess(ProcId)) m.fObjPtr = fObjPtr;
  CM->SendTask(&m);

  fTrans = EOwnerTransition;
  fNumConfirm = 1;	// we await confirmation from the new owner
  fObjId.SetProcId(ProcId);//fProc = ProcId;

}


int OOPMetaData::HasReadAccess(long Procid) {
  if (!fTaskWrite.IsZero()) return 0;
  vector<long>::iterator i;
#ifdef WIN32
  i = fAccessProcessors.begin();
  while(i != fAccessProcessors.end() && *i != Procid) i++;
#else
  i=find(fAccessProcessors.begin(), fAccessProcessors.end(), Procid);
#endif
  if(i==fAccessProcessors.end()){
    return 0;
  }else{
    return 1;
  }

}

/*int TData::HasWriteAccess(int Procid) {
  if (!fTaskWrite) return 0;

  if(ProcId == fAccessProcessors[0]) return 1;
  return 0;
  if
  return fAccessProcessors[Procid] == EWriteAccess ||
  fAccessProcessors[Procid] == EExclusiveWriteAccess;
  }
*/
int OOPMetaData::HasWriteAccess(OOPObjectId &ProcId){
  if (fTaskWrite == ProcId) return 1;
  return 0;
}
/*if(fObjId.GetProcId()== myproc && fAccessProcessors[myproc] == EExclusiveWriteAccess) {
  if(fTaskWrite >= 0 && !TM->ExistsTask(fTaskWrite)) {
  // adjust the exclusive write access for a task which doesnt exist (anymore)

  fTaskWrite = -1;
  fAccessProcessors[myproc] = EWriteAccess;
  return 0;
  } else if(fTaskWrite < 0) {
  return 0;
  } else {
  return 1;
  }
  }
  int numproc = fAccessProcessors.capacity();
  int result = 0;
  for(int i=0; i<numproc; i++) {
  if(fAccessProcessors[i] == EExclusiveWriteAccess) result++;
  }
  return result;

  }
*/
//To be transformed to readaccess only
/*int TData::HasBlockingReadAccess(){

// verify whether the tasks which requested blocking read access still exist

int numread = fBlockingReadProcesses.capacity();
int adjust = 0;
OOPObjectId taskid;
int i=0;
for(i=0; i<numread; i++) {
taskid = fBlockingReadProcesses[i].taskId;
if(taskid >= 0 && !TM->ExistsTask(taskid)) {
RemoveBlockingRead(taskid);
adjust++;
numread--;
i--;
}
}
if(adjust) AdjustBlockingRead();	// processors with blocking read access
// whose tasks have been terminated

int numproc = fAccessProcessors.capacity();
int result = 0;
for(i=0; i<numproc; i++) {
if(fAccessProcessors[i] == EBlockingReadAccess) {
result++;
}
}
return result;
}*/


int OOPMetaData::DeleteObject(OOPObjectId & ObjId){	// attempts to delete the object pointed to by the object
  //Isso era realizado no método antigo.
  //precisa ser exclarecido o que é fProc, que é "owner" ?????
  //

  /*Não tenho certeza disso*/
  if(fProc != CM->GetProcID()) {
    RequestDelete(ObjId);
    return 0;
  }
  /*Notify current processor*/


  if(fTrans != ENoTransition) return 0;
  fTrans = EDeleteTransition;
  int myproc = CM->GetProcID();
  int numproc = fAccessProcessors.size();
  for(int i=0; i<numproc; i++) {
    if(i != myproc && fAccessProcessors[i] != ENoAccess) {	// issue a deletion message
      fNumConfirm++;
      OOPDMOwnerTask m(ENotifyDeleteObject,i);
      m.fProcDestination = i;
      m.fProcOrigin = myproc;
      CM->SendTask(&m);

    }
    //Uma vez que existe uma pendência sobre o objeto não deletar automaticamente
    return 0;
  }

  /*Notify processor which created the object*/
  int procorigin = ObjId.GetProcId();
  if (procorigin != myproc) {	// issue a deletion message
    fNumConfirm++;
    OOPDMOwnerTask morig(ENotifyDeleteObject,procorigin);
    morig.fProcDestination = procorigin;
    morig.fProcOrigin = myproc;
    CM->SendTask(&morig);
    return 0;
  }
  return 1;

  /*
    Implementação inicial envia uma mensagem para todos os processadore de o corrente objeto está sendo deletado,
    Será que isso ainda é necessário
    fToDelete = 1;
    if(fProc != CM->GetProcID()) {
    RequestDelete(ObjId);
    return 0;
    }
    if(fTrans != ENoTransition) return 0;
    fTrans = EDeleteTransition;
    int numproc = fAccessProcessors.capacity()/2;
    int myproc = CM->GetProcID();
    for(int i=0; i<numproc; i++) {
    if(i != myproc && fAccessProcessors[2*i] != ENoAccess) {	// issue a deletion message
    fNumConfirm++;
    OOPDMOwnerTask m(ENotifyDeleteObject,i);
    m.fProcDestination = i;
    m.fProcOrigin = myproc;
    CM->SendTask(&m);
    }
    }
    return 0;
  */
}


void OOPMetaData::RequestDelete(OOPObjectId & ObjId) {
  OOPDMRequestTask m(ERequestReadAccess,fProc);
  m.fType = ERequestDelete;
  m.fProcOrigin = fProc;
  m.fProcDestination = fProc;
  m.fObjId = ObjId;
  CM->SendTask(&m);
}
//rewritten: longhin 30/01/2003
void OOPMetaData::CancelReadAccess() {

  fTrans = EReadTransition;
  int numproc = fAccessProcessors.size();
  int myproc = DM->GetProcID();
  if(fProc != myproc) {
    exit(-1);
  }
  //Write access should be given only when all cancel read were confirmed
  if(!fTaskWrite.IsZero()) exit(-1);

  for(int i=0; i<numproc; i++) {
    //Not applicable anymore
    //if(i != myproc && fAccessProcessors[i] != ENoAccess) {

    fNumConfirm++;
    //OOPDMOwnerTask m(ECancelReadAccess,i);
    OOPDMOwnerTask m(ECancelReadAccess,fAccessProcessors[i]);
    m.fProcDestination = fAccessProcessors[i];
    // Found the BUG !!!  14/5/96 -- the objid was not set
    // Philippe
    m.fObjId = fObjId;
    m.fProcOrigin = myproc;
    CM->SendTask(&m);
    //}
  }
  if(!fNumConfirm) fTrans = ENoTransition;
}

void OOPMetaData::GrantWriteAccess(OOPObjectId & TaskId, OOPMDataState st, OOPDataVersion & version){
  if(st != EWriteAccess /*&& st != EExclusiveWriteAccess*/) return;
  if(fVersion < version) return;
  int numproc = fAccessProcessors.size();
  int myproc = DM->GetProcID();
  if(fProc != myproc) {
    exit(-1);
  }
  //Object is already being written
  if(!fTaskWrite.IsZero()){
    exit(-1);
  }
  int i=0;
  for (i=0;i<numproc;i++){
    //Issue a cancel read access to all processors on the list.
  }
  CancelReadAccess();
  /*
    for(int i=0; i<numproc; i++) {
    if(i != myproc && fAccessProcessors[i] != ENoAccess) {	// issue a deletion message
    CancelReadAccess();
    return;
    }
    }
  */
  fTaskWrite = TaskId;
  /*if(st == EExclusiveWriteAccess) {
    fTaskWrite = TaskId;
    }*/
  fAccessProcessors.empty();//[myproc] = st;
  fAccessProcessors.resize(1);
  fAccessProcessors.push_back(myproc);
  //NotifyAccessStates();
  VerifyAccessRequests();
  //	TM->TryTask(TaskId);
}

void OOPMetaData::GrantReadAccess(OOPObjectId TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion version){

  // Changes the access state of the data, notify the TaskManager
  if(version > fVersion) {
	  cerr << "GrantReadAccess version inconsistency" << endl;
	  cerr << "Keep going anyway" << endl;
    //exit(-1);
  }

  if(AccessRequest != EReadAccess) return;
  int myproc = DM->GetProcID();
  if(fProc != myproc) {
    // I cannot do anything
    exit(-1);
  }

  fNumConfirm++;
  fTrans = EReadTransition;
  //fAccessProcessors[ProcId] = AccessRequest;
  //fAccessProcessors.push_back(ProcId);
  /*if(AccessRequest == EBlockingReadAccess) {
    AddBlockingReadProcess(TaskId);
    }*/
  OOPDMOwnerTask m(EGrantReadAccess,ProcId);
  m.fProcDestination = ProcId;
  m.fProcOrigin = myproc;
  m.fObjId = fObjId;
  m.fObjPtr = fObjPtr;
  m.fTaskId = TaskId;
  m.fState = AccessRequest;
  m.fVersion = version;
  CM->SendTask(&m);
}
void OOPMetaData::GrantVersionAccess(OOPObjectId TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion version){
  // Changes the access state of the data, notify the TaskManager
  if(version > fVersion) {
	  cerr << "GrantVersionAccess inconsistent" << endl;
	  cerr << "Keep going anyway !" << endl;
    //exit(-1);
  }

  if(AccessRequest != EVersionAccess) return;
  int myproc = DM->GetProcID();
  if(fProc != myproc) {
    // I cannot do anything
    exit(-1);
  }
  fTaskVersionAccess = TaskId;
}

/*void TData::NotifyAccessStates() {

int myproc = CM->GetProcID();
// este procedimento so pode ser chamado pelo dono do dado
if(fProc != myproc) {
cout << "TData.NotifyAccessStates called when data is not owner myproc = "
<< myproc << " fProc = " << fProc << endl;
return;//exit(-1);
}
if(!fTaskList.size()){
cerr << "Task List size = 0. File:" << __FILE__ << " Line:" << __LINE__ << endl;
return;//exit(-1);
}
deque<OOPAccessInfo>::iterator i;
for(i=fTaskList.begin();i!=fTaskList.end();i++){
if (CanExecute(i->fVersion, i->fState)){
if(i->fProc == fProc){
TM->NotifyAccessGranted(i->fTaskId, fObjId, i->fState, i->fVersion, fObjPtr);
}else{
//Mandar mensagem para o processador especificado
//Identificar AccessState e traduzir para access request.

}
}
}


OOPDMOwnerTask m(ENotifyAccessState,-1);
m.fProcDestination = -1;
m.fProcOrigin = myproc;
m.fObjId = fObjId;
m.fAccessProcessors = fAccessProcessors;
//	 m.fBlockingReadProcesses = fBlockingReadProcesses;
m.fVersion = fVersion;
CM->SendTask(&m);

}
*/



OOPDataVersion OOPMetaData::Version() const {
  return fVersion;
}

void OOPMetaData::IncrementVersion() {
  if(fTrace) {
    TraceMessage("Incrementing version");
  }
  if(fProc != DM->GetProcID()) {
    cout << "TData::IncrementVersion called at a processor which is not owner"
      " of the data \nfProc = " << fProc << " current proc = " << DM->GetProcID() << endl;
    return;
  }
  //fVersion++;
#ifdef DEBUG
  cout << "Incrementing data version on TData" << endl;
#endif
  ++fVersion;

  //NotifyAccessStates();
  //VerifyAccessRequests();
}

void OOPMetaData::TraceMessage(OOPDMOwnerTask &ms) {
  int myproc = CM->GetProcID();
  ofstream tout("trace.txt", ios::app);
  tout << "DataID:" ;
  ms.fObjId.Print(tout);
  tout << " fObjId:" ;
  fObjId.Print(tout);
  tout << " fTrans:" << fTrans
       << " Proc:" << myproc
       << " TaskId:";
  ms.fTaskId.Print(tout);
  tout << " ProcOrigin:" << ms.fProcOrigin
       << " ProcDestination:" << ms.fProcDestination << endl;
  switch(ms.fType) {
  case ENoMessage:
    tout << "\tENoMessage" << endl;
    break;
  case ENotifyAccessState:
    tout << "\tENotifyAccessState" << endl;
    break;
  case ECancelReadAccess: {
    tout << "\tECancelReadAccess" << endl;
    break;
  }
  case ECancelReadAccessConfirmation :
    tout << "\tECancelReadAccessConfirmation" << endl;
    break;
  case ETransferOwnership : {
    tout << "\tETransferOwnership" << endl;
    break;
  }
  case ETransferOwnershipConfirmation :
    tout << "\tETransferOwnershipConfirmation" << endl;
    break;
  case EGrantReadAccess : {
    tout << "\tEGrantReadAccess" << endl;
    break;
  }
  case EGrantReadAccessConfirmation :
    tout << "\tEGrantReadAccessConfirmation" << endl;
    break;
  case ENotifyDeleteObject : {
    tout << "\tENotifyDeleteObject" << endl;
    break;
  }
  case ENotifyDeleteObjectConfirmation :
    tout << "\tENotifyDeleteObjectConfirmation" << endl;
    break;
  case ENotifyCreateObject:
    tout << "\tENotifyCreateObject" << endl;
    break;
  default:
    tout << "\tCannot Trace!" << endl;
    break;
  }
}

void OOPMetaData::TraceMessage(char *message) {
  ofstream tout("trace.txt", ios::app);
  tout << "fObjId:";
  fObjId.Print(tout);
  tout << " fProc:" << fProc
       << " fVersion:";
  fVersion.Print(tout);
  tout << " fNumConfirm:" << fNumConfirm
       << " fTrans:" << fTrans
       << message << endl;
}
void OOPMetaData::Print(ostream & out) {
  out << "TData structure" << endl;
  out << "TaskList size " << fTaskList.size() << endl;
  deque<OOPAccessInfo>::iterator i;
  for(i=fTaskList.begin();i!=fTaskList.end();i++){
    out << "---------------------------------------------------------------" << endl;
    i->Print(out);
  }
  out << "---------------------------------------------------------------" << endl;
  out.flush();
}

/*MDataState TData::State() {
  return (MDataState) fAccessProcessors[DM->GetProcID()];
  }*/

void OOPMetaData::SetVersion(const OOPDataVersion &ver, const OOPObjectId &taskid) {
  if(fTaskWrite == taskid || fTaskVersionAccess == taskid) fVersion = ver;
}

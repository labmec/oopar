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


void OOPMetaData::VerifyAccessRequests(){
	OOPObjectId taskid;
	while(fAccessList.HasIncompatibleTask(fVersion,taskid)) {
		cout << "OOPMetaData::Verify.. task canceled ";
		taskid.Print(cout);
		TM->CancelTask(taskid);
	}
	OOPAccessInfo *ac;
	if(DM->GetProcID() == fProc && fAccessList.HasVersionAccessRequests(fVersion) && !fAccessList.HasWriteAccessGranted() && !fAccessList.HasReadAccessGranted()) {
		// we should invoke a procedure to revoke all access requests
		this->fProcVersionAccess = fProc;
	} else if (DM->GetProcID() == fProc && fAccessList.HasWriteAccessRequests(fVersion)) {
		// we should invoke a procedure to revoke all read access requests
	}
	// We need to put verification if there is a valid version access request
	// or valid write access request
	// In these cases all read access must be either suspended or revoked
	while(fAccessList.VerifyAccessRequests(*this,ac)) {
		ac->fIsGranted = 1;
		OOPMDataDepend depend(this->Id(),ac->fState,ac->fVersion);
		if(ac->fState == EWriteAccess) fTaskWrite = ac->fTaskId;
		if(ac->fState == EVersionAccess) fTaskVersion = ac->fTaskId;
		TM->NotifyAccessGranted(ac->fTaskId,depend,this);
	}
}

OOPObjectId OOPMetaData::Id() const { return fObjId;}

bool OOPMetaData::CanGrantAccess() const {
	if(fTrans != ENoTransition) return false;
	if(fProcVersionAccess != -1 && fProcVersionAccess != DM->GetProcID()) return false;
	if(fToDelete) return false;
	if(!fTaskVersion.IsZero()) return false;
	return true;
}


void OOPMetaData::ReleaseAccess(const OOPObjectId &taskid, const OOPMDataDepend &depend){
	fAccessList.ReleaseAccess(taskid,depend);
	if(depend.State() == EVersionAccess) {
		this->fProcVersionAccess = -1;
		this->fTaskVersion.Zero();
	} else if (depend.State() == EWriteAccess) {
		fTaskWrite.Zero();
	}
	VerifyAccessRequests();
}

OOPMDataState OOPMetaData::State() const {
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

void OOPMetaData::SubmitAccessRequest(const OOPObjectId &taskId, const OOPMDataDepend &depend){
	if(this->fProc != DM->GetProcID()) {
		cout << "a request must be sent to the owning processor\n";
	}
	fAccessList.AddAccessRequest(taskId,depend);
	VerifyAccessRequests();
}




OOPMetaData::OOPMetaData(OOPSaveable *ObPtr,const OOPObjectId & ObjId,const int ProcId) {
  fProc = ProcId;
  fObjPtr = ObPtr;
  fObjId = ObjId;
  fTrans = ENoTransition;
  fToDelete = 0;
  fTrace = 0;
  fProcVersionAccess = -1;
  fTaskVersion.Zero();
  fTaskWrite.Zero();
}

void OOPMetaData::SetId(OOPObjectId & id){
  fObjId = id;
}

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
}

void OOPMetaData::RequestTransferObject(int ProcId, OOPObjectId & TaskId,
					OOPMDataState AccessRequest, OOPDataVersion & version){
}

void OOPMetaData::TransferOwnerShip(OOPObjectId & TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion &version) {
}

bool OOPMetaData::HasReadAccess() const {
	if(fProc == DM->GetProcID() && fTaskWrite.IsZero() && fTaskVersion.IsZero()) return true;
	return HasReadAccess(DM->GetProcID());
}

bool OOPMetaData::HasVersionAccess() const {
	if(fProcVersionAccess == DM->GetProcID()) return true;
	return false;
}

bool OOPMetaData::HasReadAccess(const int Procid) const {
  if (!fTaskWrite.IsZero() || !fTaskVersion.IsZero()) return false;
#ifdef WIN32
  list<int>::const_iterator i = fAccessProcessors.begin();

  while(i != fAccessProcessors.end() && *i != Procid) i++;
  if(i== fAccessProcessors.end()) return false;
  return true;
#else
  list<int>::const_iterator i = fAccessProcessors.begin();
  i=find(fAccessProcessors.begin(), fAccessProcessors.end(), Procid);
  if(i==fAccessProcessors.end()){
    return false;
  }else{
    return true;
  }
#endif
}



bool OOPMetaData::HasWriteAccess(const OOPObjectId &taskid) const {
  if (fTaskWrite == taskid) return true;
  return false;
}

int OOPMetaData::DeleteObject(OOPObjectId & ObjId){	
	return 1;
}


void OOPMetaData::RequestDelete(OOPObjectId & ObjId) {
}
//rewritten: longhin 30/01/2003
void OOPMetaData::CancelReadAccess() {

}

void OOPMetaData::GrantWriteAccess(OOPObjectId & TaskId, OOPMDataState st, OOPDataVersion & version){
}

void OOPMetaData::GrantReadAccess(OOPObjectId TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion version){

}
void OOPMetaData::GrantVersionAccess(OOPObjectId TaskId, int ProcId, OOPMDataState AccessRequest, OOPDataVersion version){
}



OOPDataVersion OOPMetaData::Version() const {
  return fVersion;
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
  tout << " fTrans:" << fTrans
       << message << endl;
}
void OOPMetaData::Print(ostream & out) {
  out << "TData structure" << endl;
  out << "fAccessList size " << fAccessList.NElements() << endl;
  out.flush();
}

void OOPMetaData::SetVersion(const OOPDataVersion &ver, const OOPObjectId &taskid) {
	if(fTaskWrite == taskid || fTaskVersion == taskid) {
		fVersion = ver;
	} else {
		cout << "OOPMetaData::SetVersion not executed\n";
	}
}

long OOPMetaData::GetClassID() {
	return OOPMETADATA_ID;
}

#include "ooptask.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"
#include "oopcommmanager.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
//#include "../gnu/gnudefs.h"

void OOPTask::SetRecurrence(bool recurrence){
  fIsRecurrent = recurrence;
}
int OOPTask::IsRecurrent(){
  return fIsRecurrent;
}

class OOPSendStorage;
class OOPReceiveStorage;
class OOPObjectId;
class OOPDataVersion;
class OOPDaemonTask;
class OOPMDataDepend;
class OOPSaveable;

using namespace std;

bool OOPTask::AmICompatible(){
  bool result = true;
  deque<OOPMDataDepend>::iterator j;
  for(j=fDataDepend.begin();j!=fDataDepend.end();j++){
    OOPMetaData * auxdata;
    OOPDataVersion ver;
    auxdata = DM->MetaData(j->fDataId);
    ver = auxdata->Version();
    if(!ver.AmICompatible(j->fVersion)){
      result = false;
      break;
    }
  }
  return result;
}

void OOPTask::Print(ostream & out){
  out << "OOPTask Id" << endl;
  fTaskId.Print(out);
  out << "Priority\t" << fPriority << endl;
  out << "Processor\t" << fProc << endl;
  out << "Data Dependence\t" << endl;
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    i->Print(out);
  }
}	

void OOPTask::TaskFinished(){
  //faltando
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++)
    DM->ReleaseAccessRequest(fTaskId, i->fDataId, i->fVersion, i->fNeed);
	
  //Deletar da fila de tarefas.
  //TM->
  //ReleaseAccessRequests();
}

void OOPTask::SubmitDependentData(){
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    DM->SubmitAccessRequest(fTaskId, i->fDataId, i->fVersion, i->fNeed, fProc);
  }
}
int OOPTask::GetNDependentData(){
  return fDataDepend.size();
}
OOPTask::OOPTask(int proc){
  fProc=proc;
  fPriority=0;
  fIsRecurrent=0;
}

void OOPTask::AddDependentData(const OOPObjectId &DataId,
			       OOPMDataState access,
			       const OOPDataVersion &version)
{
  OOPMDataDepend dd(DataId, access, version);
  deque<OOPMDataDepend>::iterator res;
  res = find(fDataDepend.begin(),fDataDepend.end(),dd);
  if(res == fDataDepend.end()){
    fDataDepend.push_back(dd);
  }else{
    cerr << "Duplicated entry on fDataDepend File:" << __FILE__ << " Line:" << __LINE__ << endl;
    exit (-1);
  }
	
	
}

void OOPTask::GrantAccess(OOPObjectId &id, OOPMDataState st, OOPMetaData *objptr){
  deque<OOPMDataDepend>::iterator i;
  bool found = false;
  bool compatible = false;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    if(i->fDataId == id){
#ifdef DEBUG
      cout << "Data found" << endl;
#endif
      found = true;
      if(i->fNeed == st){
#ifdef DEBUG
	cout << "State found" << endl;
#endif
	compatible = true;
	cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
	if(!(i->ObjPtr())){
	  cout << "Assigning pointer " << objptr << "-----$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$___________" << endl;
	  i->SetObjPtr(objptr);
	}else{
	  cerr << "Pointer not NULL. Access already granted !"
	       << " File:" << __FILE__
	       << " Line:" << __LINE__ 
	       << "Overwriteing " << endl;
	  //i->fObjPtr = objptr;
	  //return;
	}
      }
    }
  }
  if (!found){
    cerr << "Required data not on Required list!"
	 << " File:" << __FILE__
	 << " Line:" << __LINE__ << endl;
    exit(-1);
  }

  if (!compatible) {
    cerr << "Data found with incompatible access request!"
	 << " File:" << __FILE__
	 << " Line:" << __LINE__ << endl;
    exit(-1);
  }
			
		
  //Notify someone that I have the access ?

}

void OOPTask::RevokeAccess(OOPObjectId &id, OOPMDataState st){
  deque<OOPMDataDepend>::iterator i;
  bool found = false;
  bool compatible = false;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    if(i->fDataId == id){
      found = true;
      if(i->fNeed == st){
	compatible = true;
	if(i->ObjPtr()){
	  i->SetObjPtr(NULL);
	}else{
	  cerr << "Pointer alredy NULL. Access already revoked !"
	       << " File:" << __FILE__
	       << " Line:" << __LINE__ << endl;
	}
      }
    }
  }
  if (!found){
    cerr << "Required data not on Required list!"
	 << " File:" << __FILE__
	 << " Line:" << __LINE__ << endl;
		
  }

  if (!compatible) {
    cerr << "Data found with incompatible access request!"
	 << " File:" << __FILE__
	 << " Line:" << __LINE__ << endl;
		
  }
			
		
  //Notify someone that I have revoked the access ?
	
}

int OOPTask::CanExecute(){
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    cout << "Data ID ";
    i->fDataId.Print(cout);
    if(!i->Status()) return 0;
  }
  return 1;
}
		
long OOPTask::ExecTime() {
  return -1;
}



void OOPTask::RequestAccess() {
  /* Take appropriate actions
   * Run through all elements in fDataDepend and
   * request appropriate access.
   * this is performed by DM
   */
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    if(!i->Status()) //Fazer algum outro check ?
      i->RequestAccess(fTaskId);
  }
	
}

void OOPTask::ReleaseAccessRequests(){
  deque<OOPMDataDepend>::iterator i;
  //poderia ser feito no mesmo loop 
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    //DM->SubmitAccessRequest(fTaskId,i->fDataId, i->fVersion, ENoAccess, (long)fProc);
    OOPMetaData *data = (OOPMetaData *) i->ObjPtr();
    OOPMDataState st = i->fNeed;
    OOPDataVersion ver = i->fVersion;
    OOPObjectId id = Id();
    data->ReleaseAccess(id, st, ver);
    RevokeAccess(i->fDataId, i->fNeed);
  }
  
  /*for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    fDataDepend.erase(i);
    }*/
	
}



OOPObjectId OOPTask::Submit() {
  fTaskId = TM->Submit(this);
  if(!GetNDependentData()){
    cerr << "Task submitted with no data dependency" << endl;
    cerr << "Possible inconsistency" << endl;
    cerr << "File:" << __FILE__ << " Line:" << __LINE__ << endl;
  }else{
    SubmitDependentData();
  }
	
  return fTaskId;
}

OOPMDataDepend::OOPMDataDepend(const OOPObjectId &id, OOPMDataState st, const OOPDataVersion &ver) {
  fDataId = id;
  fNeed = st;
  fVersion = ver;
}
OOPMDataDepend::OOPMDataDepend(const ::OOPMDataDepend &dd) {
  fDataId = dd.fDataId;
  fNeed = dd.fNeed;
  fVersion = dd.fVersion;
}
OOPMetaData * OOPMDataDepend::ObjPtr(){
  return fObjPtr;
}
void OOPMDataDepend::SetObjPtr(OOPMetaData * objptr){
  fObjPtr = objptr;
}


int OOPMDataDepend::CanExecute(OOPObjectId TaskId) {
  return DM->HasAccess(fDataId,TaskId,fNeed,fVersion);
}

void OOPMDataDepend::RequestAccess(OOPObjectId TaskId) {
  DM->TransferObject(fDataId,CM->GetProcID(),TaskId,fNeed,fVersion);
}
bool OOPMDataDepend::operator == (const OOPMDataDepend & dd){
  return ((fDataId == dd.fDataId && fNeed == dd.fNeed && fVersion == dd.fVersion) != false);//return true;
  //return false;
}

void OOPMDataDepend::Print(ostream & out){
  out << "Data Id" << endl;
  fDataId.Print(out);
  out << "Access State \t" << fNeed << endl;
  out << "Object Pointer \t" << fObjPtr << endl;
  out << "Required version " << endl;
  fVersion.Print(out);
}





OOPDaemonTask::OOPDaemonTask(int Procid) : OOPTask(Procid) {
}

long OOPDaemonTask::ExecTime() {
  return 0;
}

int OOPDaemonTask::CanExecute() {
  return 1;
}


OOPMReturnType OOPTask::Execute(){
  return ESuccess; // execute the task, verifying that
}

int OOPTask::GetProcID() {
  return fProc;
}

void OOPTask::ChangePriority(int newpriority) {
  fPriority = newpriority;
}

int OOPTask::Priority() {
  return fPriority;
}

OOPObjectId OOPTask::Id() {
  return fTaskId;
}


int OOPTask::DerivedFrom(long Classid){
  if(Classid == GetClassID()) return 1;
  return OOPSaveable::DerivedFrom(Classid);
}

int OOPTask::DerivedFrom(char *classname){
  if(!strcmp(ClassName(),classname)) return 1;
  return OOPSaveable::DerivedFrom(classname);
}

OOPSaveable *OOPTask::Restore(OOPReceiveStorage *buf) {
  OOPTask *v = new OOPTask(0);
  v->Unpack(buf);
  return v;
}

int OOPTask::Pack(OOPSendStorage *buf)
{
  deque<OOPMDataDepend>::iterator i;
  //If any fObjPtr is not NULL issue and error message.
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++) 
    if(i->ObjPtr()){
      cerr << "Inconsistent Task communication !"
	   << " File:" << __FILE__ 
	   << " Line:" << __LINE__ << endl;
      exit(-1);
    }
  OOPSaveable::Pack(buf);
  //ObjectId packing and unpacking
  fTaskId.Pack(buf);
		
  buf->PkInt(&fProc);	// Processor where the task should be executed
  //buf->PkLong(&fTaskId);
  buf->PkInt(&fPriority);
  // Id assigned to the task after having been submitted
  //int numdep = fDataDepend.length();
  int numdep = fDataDepend.size();
  buf->PkInt(&numdep);

  for(i=fDataDepend.begin();i!=fDataDepend.end();i++){
    //Packing OOPObjectId data information
    i->fDataId.Pack(buf);
    //Finished OOPObjectId
		
    int st = i->fNeed;
    buf->PkInt(&st);
		
    //packing stl vectors for OOPDataVersion
    i->fVersion.Pack(buf);
    //finished packing stl vectors for 
		
    //Still missing packing fObjPtr

    //deque<OOPMDataDepend>::iterator qq = find(fDataDepend.begin(),fDataDepend.end(),d);
    //fDataDepend.next(i);
  }

  return 0;
}

int OOPTask::Unpack( OOPReceiveStorage *buf )
{
  OOPSaveable::Unpack(buf);
  //OOPObjectId unpacking

  fTaskId.Unpack(buf);
	
  //Finished OOPObjectId unpacking

  buf->UpkInt(&fProc);
  //buf->UpkLong(&fTaskId);
  buf->UpkInt(&fPriority);
  int numdep;
  buf->UpkInt(&numdep);
  fDataDepend.resize(numdep);
  deque<OOPMDataDepend>::iterator i;
  for(i=fDataDepend.begin();i!=fDataDepend.end();i++) {
    //Unpacking OOPObjectId data information
    i->fDataId.Unpack(buf);
    //Finished OOPObjectId

    int st;
    buf->UpkInt(&st);
    i->fNeed = (OOPMDataState) st;
		
    //unpacking stl vectors for OOPDataVersion
    i->fVersion.Unpack(buf);
    //Falta ainda fObjPtr
  }
#define LONGVEC_ID 123456
  long clid;
  buf->UpkLong(&clid);
  if(clid != LONGVEC_ID) {
    exit(-1);
  }
  //fTaskDepend.Unpack(buf);
  return 0;
}

int OOPDaemonTask::DerivedFrom(long Classid){
  if(Classid == GetClassID()) return 1;
  return OOPTask::DerivedFrom(Classid);
}

int OOPDaemonTask::DerivedFrom(char *classname){
  if(!strcmp(ClassName(),classname)) return 1;
  return OOPTask::DerivedFrom(classname);
}

OOPSaveable *OOPDaemonTask::Restore(OOPReceiveStorage *buf) {
  OOPDaemonTask *v = new OOPDaemonTask(0);
  v->Unpack(buf);
  return v;
}

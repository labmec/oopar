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


void OOPTask::Print(ostream & out){
  out << "OOPTask Id" << endl;
  fTaskId.Print(out);
  out << "Priority\t" << fPriority << endl;
  out << "Processor\t" << fProc << endl;
  out << "Data Dependence\t" << endl;
  fDataDepend.Print(out);
}	

void OOPTask::TaskFinished(){
  fDataDepend.ReleaseAccessRequests(fTaskId);
}


OOPTask::OOPTask(int proc){
  fProc=proc;
  fPriority=0;
  fIsRecurrent=0;
}

void OOPTask::AddDependentData(const OOPMDataDepend &depend)
{
  fDataDepend.AppendDependency(depend);	
}



		
long OOPTask::ExecTime() {
  return -1;
}




OOPObjectId OOPTask::Submit() {
  fTaskId = TM->Submit(this);
  return fTaskId;
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
#warning "OOPTask::Pack should be implemented"
  OOPSaveable::Pack(buf);
  //ObjectId packing and unpacking
  fTaskId.Pack(buf);
		
  buf->PkInt(&fProc);	// Processor where the task should be executed
  //buf->PkLong(&fTaskId);
  buf->PkInt(&fPriority);
  // Id assigned to the task after having been submitted
  //int numdep = fDataDepend.length();
	
#warning "Something else?"
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

  
  return 0;
}

int OOPTask::Unpack( OOPReceiveStorage *buf )
{
#warning "OOPTask::Unpack should be implemented"
  OOPSaveable::Unpack(buf);
  //OOPObjectId unpacking

  fTaskId.Unpack(buf);
	
  //Finished OOPObjectId unpacking

  buf->UpkInt(&fProc);
  //buf->UpkLong(&fTaskId);
  buf->UpkInt(&fPriority);
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

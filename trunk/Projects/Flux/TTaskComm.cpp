/* Generated by Together */
#include "oopdatamanager.h"
#include "TTaskComm.h"


int TTaskComm::Unpack( OOPReceiveStorage *buf )
{
  OOPSaveable::Unpack(buf);
  OOPTask::Unpack(buf);
  return 0;
}
int TTaskComm::Pack(OOPSendStorage *buf)
{
  int i;
  //If any fObjPtr is not NULL issue and error message.
  for(i=0; i<fDataDepend.NElements(); i++) 
    if(fDataDepend.Dep(i).ObjPtr()){
      cerr << "Inconsistent Task communication !"
	   << " File:" << __FILE__ 
	   << " Line:" << __LINE__ << endl;
      exit(-1);
  }
  OOPSaveable::Pack(buf);
  OOPTask::Pack(buf);
  return 0;
}
int TTaskComm::DerivedFrom(long Classid){
  if(Classid == GetClassID()) return 1;
  return OOPSaveable::DerivedFrom(Classid);
}
int TTaskComm::DerivedFrom(char *classname){
  if(!strcmp(ClassName(),classname)) return 1;
  return OOPSaveable::DerivedFrom(classname);
}

OOPMReturnType TTaskComm::Execute(){
    int i;
	for(i=0;i<fDataDepend.NElements();i++){
        if (fDataDepend.Dep(i).State() == EWriteAccess){
	        //dat->IncrementVersion();
	        //DM->GetObjPtr(i->fDataId)->IncrementVersion();
	        fDataDepend.Dep(i).ObjPtr()->IncrementVersion();
	    }
	    
	}
    TaskFinished();
    return ESuccess; // execute the task, verifying that
}
TTaskComm::TTaskComm(int ProcId) : OOPTask(ProcId){}

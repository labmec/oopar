// -*- c++ -*-

#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopfilecomm.h"
#include "ooperror.h"
#include "TParVector.h"
#include "tsmalltask.h"
#include "oopwaittask.h"
#include "fluxdefs.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include <iostream>
#include <fstream>
using namespace std;
class OOPCommunicationManager;
class OOPDataManager;
class OOPTaskManager;

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;
ofstream TaskLog("tasklog.log");
ofstream DataLog("datalog.log");
ofstream DataManLog("datamanlog.log");
ofstream TransferDataLog("transferdatalog.log");
ofstream TaskQueueLog("taskqueue.log");
ofstream TaskManLog("taskmanlog.log");
ofstream DataQueueLog("dataqueuelog.log");

void InsertTasks(int numtasks);
void RegisterPhilFluxRestore();

#ifdef MPI
int mpimain (int argc, char **argv)
{

  RegisterOOParRestore();
  RegisterPhilFluxRestore();
  CM = new OOPMPICommManager (argc, argv);
  CM->Initialize((char*)argv, argc);
  
  char filename[256];
  sprintf(filename,"datalogger%d", CM->GetProcID());
  OOPDataLogger * LogDM = new OOPDataLogger(filename);
  ::LogDM = LogDM;
  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());

  //int numproc = CM->NumProcessors();//atoi(argv[argc-1]);
  // At this point the environment will lock because it will go into a blocking receive...
  cout << "Entering execute for " << CM->GetProcID() << endl;
  TM->Execute();
  cout << "After TM->Execute\n";
  cout.flush();

	if(CM->IAmTheMaster())
  {
    cout << "Inserting tasks\n";
    cout.flush();
    InsertTasks(30);
  } else {
    cout << "IAmTheMaster returned " << CM->IAmTheMaster() << endl;
    cout.flush();
  }
  
  TM->Wait();
  cout << "Deleting DM\n";
  delete  DM;
  cout << "Deleting TM\n";
  delete  TM;
  cout << "Deleting CM\n";
  delete  CM;
  delete LogDM;

  cout << "Leaving mpimain\n";
  cout.flush();
  return 0;
}
#endif

int main(int argc, char **argv) {

  cout << "Entering main program \n";
  cout.flush();
#ifdef MPI
  cout << "Entering mpimain program \n";
  cout.flush();
  return mpimain(argc,argv);
#else
  return 0;
#endif

}

void InsertTasks(int numtasks) 
{
  TParVector *victim = new TParVector();
  victim->Resize(1000);
  OOPObjectId vicid = DM->SubmitObject(victim,1);
  cout << "The object id for victim is " << vicid << endl;
  // build a task such that the current thread has access to internal OOP data
  OOPWaitTask *wt = new OOPWaitTask(CM->GetProcID());
  OOPDataVersion ver;
  wt->AddDependentData(OOPMDataDepend(vicid,EWriteAccess,ver));
  cout << "Before sumitting the wait task" << endl;
  TM->Submit(wt);
  // This will put the current thread to wait till the data is available
  cout << "Before waiting\n";
  wt->Wait();
  cout << "Got out of wait\n";
  // At this point I have version access to victim
  OOPMetaData *obj = wt->Depend().Dep(0).ObjPtr();
  ver= obj->Version();
  ver.IncrementLevel(numtasks);
  obj->SetVersion(ver,wt->Id());
  cout << "Before wait finish\n";
  wt->Finish();
  cout << "After wait finish\n";
  int it, numproc = CM->NumProcessors();
  for(it=0; it<numtasks; it++) {
    TSmallTask *st = new TSmallTask(it%numproc);
    OOPDataVersion stver;
    stver.IncrementLevel(-1);
    stver.SetLevelVersion(1,it);
    cout << "Created the smalltask numproc " << numproc << "\n";
    if((numproc>1) && it%(numproc-1)) 
    {
      OOPMDataDepend dp(vicid,EWriteAccess,stver);
      st->AddDependentData(dp);
    }
    else
    {
      cout << "Adding data dependency\n";
      cout.flush();
      OOPMDataDepend dp(vicid,EVersionAccess,stver);
      st->AddDependentData(dp);
    }
    cout << "Submitting the small task\n";
    TM->Submit(st);
  }
  cout << "Before another wait task\n";
  wt = new OOPWaitTask(CM->GetProcID());
  ver = OOPDataVersion();
  ver.Increment();
  wt->AddDependentData(OOPMDataDepend(vicid,EWriteAccess,ver));
  TM->Submit(wt);
  // This will put the current thread to wait till the data is available
  wt->Wait();
  wt->Finish();
  cout << "I GOT THROUGH\n";
  
  OOPTerminationTask * tt;
  for(it=0;it < numproc;it++){
	  tt = new OOPTerminationTask(it);
	  tt->Submit();
  }
}
void RegisterPhilFluxRestore() 
{
  OOPReceiveStorage::AddClassRestore (TSMALLTASKID, TSmallTask::Restore);
  OOPReceiveStorage::AddClassRestore (TPARVECTOR_ID, TParVector::Restore);
}

// -*- c++ -*-

#include "oopdatamanager.h"
#include "ooptaskmanager.h"
//#include "oopfilecomm.h"
#include "ooperror.h"
#include "TParVector.h"
#include "tsmalltask.h"
#include "oopwaittask.h"
//#include "fluxdefs.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include <iostream>
#include <fstream>
#include <map>
#include <list>
using namespace std;
//class OOPCommunicationManager;
//class OOPDataManager;
//class OOPTaskManager;

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
// void RegisterPhilFluxRestore();


struct DataAccessOrg {
  int fTaskid;
  int fDataId;
  int fVersion;
  OOPMDataState fAccessType;
  DataAccessOrg(){}
  DataAccessOrg (int taskid,int id, int version, OOPMDataState access){
    fTaskid = taskid;
    fDataId = id;
    fVersion = version;
    fAccessType = access; 
  }
};

void WriteDataAccess(ostream &out, vector<OOPMDataDepend> &list){
  cout << "Entering WriteDataAccess...";
  int i,nt = list.size();
//  out << ": Number of tasks = " << endl;
  out << nt << endl;
//  out << ": Tasks\n Data_Id \t Version \t Access type" << endl;
  for (i=0;i<nt;i++){
    out << list[i].Id().GetId();
    vector <int> level;
    vector <int> cardin;
    OOPDataVersion version = list[i].Version();
    version.GetData(cardin,level);
    int sz = level.size();
    for (int c=0;c<sz;c++){
      out  << "\t" << level[c] << "\t" << cardin[c]; 
    }
    out << "\t" << (int)(list[i].State()) << endl;
  }
  cout << " exiting!!!" << endl;
}

void ReadDataAccess (istream &arq, std::list<DataAccessOrg> &mylist) {
  cout << "Entering ReadDataAccess...";
  //first description line
//  char buf[512];
//  getline(&buf,512,);
  //second description line
  
//  arq.getline();
//  list.resize(nt);
  char buf[512];
  arq.getline(buf,512);
  while(arq) {
    DataAccessOrg ac;
    int accesstype;
    arq >> ac.fTaskid >> ac.fDataId >> ac.fVersion >> accesstype;
    ac.fAccessType = (OOPMDataState)accesstype;
    if(!arq) break;
    mylist.push_back(ac);
  }
  cout << " exiting!!!" << endl;
}

void CreateObjIds (list<DataAccessOrg> &mylist, 
                   map<int,OOPObjectId,less<int> >&fileId2globalId,
                   vector<OOPMDataDepend> &depend,
                   map<OOPObjectId,OOPMDataDepend> &higVersions){
  
  cout << "Entering CreateObjIds...";
  int i,nt = mylist.size();
  depend.resize(nt);
  list<DataAccessOrg>::iterator ac_It;
  
  for (i=0,ac_It=mylist.begin();i<nt,ac_It!=mylist.end();i++,ac_It++){
    //Verifies if the object_id was already created
    int file_id = (*ac_It).fDataId;//mylist[i].fDataId;
    map<int,OOPObjectId,less<int> >::iterator file_id_It;
    file_id_It = fileId2globalId.find(file_id);
    
    //Get the object_id
    OOPObjectId vicid;
    if (fileId2globalId.find(file_id) == fileId2globalId.end()){
      TParVector *victim = new TParVector();
      victim->Resize(10);
      vicid = DM->SubmitObject(victim,1);
      fileId2globalId [ file_id ] = vicid;
    } else {
      vicid = (*file_id_It).second;
    }
    //cout << "The file object id is " << mylist[i].fDataId << " the global id is " << vicid << endl;
    
    //Create the depend object
    OOPDataVersion localversion;
    localversion.SetLevelVersion(0,/*mylist[i]*/(*ac_It).fVersion);
    OOPMDataDepend dp (vicid, /*mylist[i]*/(*ac_It).fAccessType , localversion);
    depend [i] = dp;

    //Identify the higher versions for each object_id    
    map<OOPObjectId,OOPMDataDepend>::iterator hig_It;
    hig_It = higVersions.find(vicid);
    if (hig_It == higVersions.end()){
      higVersions[vicid] = dp;
      continue;
    }
    if (dp.State()==EWriteAccess || dp.State() == EVersionAccess) dp.IncrementVersion();
    OOPDataVersion prevHg = ((*hig_It).second).Version();
    OOPDataVersion curr_ver = dp.Version();
    if (curr_ver > prevHg) {
      higVersions[vicid] = dp; 
    }
  }
  cout << " exiting!!!" << endl;
}


void CreateTaskFromFile(string &file) {
  int i,nt;

  ifstream arq (file.c_str());
  list<DataAccessOrg> mylist;
  
  //Read the file
  ReadDataAccess (arq, mylist);

  //Create the depends
  map<int,OOPObjectId,less<int> > fileId2globalId;
  vector<OOPMDataDepend> depend;
  map<OOPObjectId,OOPMDataDepend> higVersions;
  
  CreateObjIds (mylist, fileId2globalId, depend, higVersions);
  
  int /*it,*/ numproc = CM->NumProcessors();
  int lasttask = -1;
  list<DataAccessOrg>::iterator it;
  int numdeps = depend.size();
  TSmallTask *st = 0;
  int counter;
  for(it=mylist.begin(),counter=0; it!=mylist.end(); it++,counter++) {
    int task = (*it).fTaskid;
    if(task != lasttask) {
      if(st) TM->Submit(st);
      st = new TSmallTask(0);
    }
    st->AddDependentData(depend[counter]);
  }
  TM->Submit(st);

  // build a task such that the current thread has access to internal OOP data
  OOPWaitTask *wt = new OOPWaitTask(CM->GetProcID());
  map<OOPObjectId,OOPMDataDepend>::iterator hig_It;
  for (hig_It = higVersions.begin();hig_It != higVersions.end();hig_It++){
    OOPMDataDepend depend = (*hig_It).second;
    wt->AddDependentData(depend);      
  }  
  cout << "Before sumitting the wait task" << endl;
  TM->Submit(wt);
  // This will put the current thread to wait till the data is available
  cout << "Before waiting\n";
  wt->Wait();
  cout << "Got out of wait\n";
  // At this point I have version access to victim
//     OOPMetaData *obj = wt->Depend().Dep(0).ObjPtr();
//     ver= obj->Version();
//     ver.IncrementLevel(numtasks);
//     obj->SetVersion(ver,wt->Id());
//     cout << "Before wait finish\n";
  wt->Finish();
  cout << "After wait finish\n";
  cout << "I GOT THROUGH\n";
  
  OOPTerminationTask * tt;
  for(i=0;i < numproc;i++){
    tt = new OOPTerminationTask(i);
    tt->Submit();
  }
}



#ifdef MPI
int mpimain (int argc, char **argv)
{

//  RegisterOOParRestore();
//  RegisterPhilFluxRestore();
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
    //InsertTasks(30);
    string file ("teste_oopar.txt");
    CreateTaskFromFile(file);
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
/*
void RegisterPhilFluxRestore() 
{
  OOPStorageBuffer::AddClassRestore (TSMALLTASKID, TSmallTask::Restore);
  OOPStorageBuffer::AddClassRestore (TPARVECTOR_ID, TParVector::Restore);
}
*/


#ifdef OOP_MPI
#include "oopmpicomm.h"
#endif

#include <iostream>
#include <sstream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopwaittask.h"
#include "oopsnapshottask.h"
#include "pzlog.h"

#ifdef LOGPZ
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif
#include "oopaccesstag.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif


#include "TTaskTest.h"
#include "OOPInt.h"




using namespace std;
//class OOPCommunicationManager;
//class OOPDataManager;
//class OOPTaskManager;

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;


int debugmpimain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0] << std::endl;
  CM = new OOPMPICommManager (argc, argv);
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif

#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif
  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());


  TM->SetNumberOfThreads(1);
  TM->Execute();
  OOPObjectId IdA, IdB;
  OOPDataVersion ver, verb;
  if(!CM->GetProcID())
  {
    OOPInt * inta = new OOPInt;
    OOPInt * intb = new OOPInt;


    IdA = DM->SubmitObject(inta);
    IdB = DM->SubmitObject(intb);
    cout << "Submitted OOPInt object Id " << IdA << endl;
    int i = 0;
    for(i = 0; i< 10;i++)
    {
      TTaskTest * tta = new TTaskTest(0);
      TTaskTest * ttb = new TTaskTest(1);
      TTaskTest * ttc = new TTaskTest(2);
  
      tta->AddDependentData(OOPAccessTag(
                              IdA, EWriteAccess, ver,0));
      tta->Submit();
      ++ver;
      ttb->AddDependentData( OOPAccessTag(
                              IdA, EWriteAccess, ver,0));
      ttb->Submit();
      ++ver;
      ttc->AddDependentData( OOPAccessTag(
                              IdA, EWriteAccess, ver,0));
      ttc->AddDependentData( OOPAccessTag(
                              IdB, EWriteAccess, verb,0));
      ttc->Submit();
      ++verb;
    }
    OOPWaitTask * wt = new OOPWaitTask(0);
    wt->AddDependentData(  OOPAccessTag(
                             IdB, EWriteAccess, verb,0));
    wt->Submit();
    DM->PostRequestDelete(IdA);
    wt->Wait();
    wt->Finish();
    //sleep(5);

    
    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->Submit();
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->Submit();
  }
  TM->Wait();
  //sleep(2);
  int proc = CM->GetProcID();
  cout << "Deleting DM on Processor " << proc << endl;
  delete  DM;
  cout << "Deleting TM on Processor " << proc << endl;
  delete  TM;
  cout << "Deleting CM on Processor " << proc << endl;
  delete  CM;

  cout << "Leaving mpimain\n";
  cout.flush();
  return 0;
}

//const int MAXSIZE = 50000000;

int debugmain(int argc, char **argv)
{
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient0.cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif
  
  {
    TPZManVector<char,MAXSIZE> f_recv_buffr;
    TPZManVector<char,MAXSIZE> f_send_buffr;
  }
  {
    OOPMPIStorageBuffer buff;
  }
  CM = new OOPMPICommManager;//(argc, argv);
  //OOPMPIStorageBuffer buff;
  TM = new OOPTaskManager (0);//CM->GetProcID ());
  DM = new OOPDataManager (0);//CM->GetProcID ());
  
  

//   OOPInt * inta = new OOPInt;
//   OOPInt * intb = new OOPInt;
// 
//   OOPObjectId IdA, IdB;
//   IdA = DM->SubmitObject(inta);
//   IdB = DM->SubmitObject(intb);
// 
//   TTaskTest * tta = new TTaskTest(0);
//   TTaskTest * ttb = new TTaskTest(0);
//   TTaskTest * ttc = new TTaskTest(0);
//   OOPDataVersion ver, verB;
//   tta->AddDependentData(OOPAccessTag(
//                           IdA, EReadAccess, ver,0));
//   tta->AddDependentData(OOPAccessTag(
//                           IdB, EWriteAccess, verB,0));
//   tta->Submit();
//   //++ver;
//   ++verB;
//   ttb->AddDependentData( OOPAccessTag(
//                            IdA, EReadAccess, ver,0));
//   ttb->AddDependentData( OOPAccessTag(
//                            IdB, EReadAccess, verB,0));
//   ttb->Submit();
//   ttc->AddDependentData( OOPAccessTag(
//                            IdA, EWriteAccess, ver,0));
//   ttc->AddDependentData( OOPAccessTag(
//                            IdB, EWriteAccess, verB,0));
//   ttc->Submit();
//   ++ver;
//   ++verB;
//   OOPTerminationTask * tt = new OOPTerminationTask(0);
//   tt->AddDependentData(  OOPAccessTag(
//                            IdA, EWriteAccess, ver,0));
//   tt->AddDependentData(  OOPAccessTag(
//                            IdB, EWriteAccess, verB,0));
// 
//   tt->Submit();
// 
//   //colocar para dentro do Execute do TM
//   //TM->ExecuteMTBlocking(TM);
//   TM->SetKeepGoing( true);
//   while (TM->KeepRunning())
//   {
//     //    TM->TransferSubmittedTasks();
//     DM->HandleMessages();
//     TM->HandleMessages();
//     DM->FlushData();
//     TM->TriggerTasks();
//     TM->WaitWakeUpCall();
//   }

  return 0;
}
int main(int argc, char **argv)
{
  debugmpimain(argc, argv); 
  //debugmain(argc, argv);
  return 0;
}

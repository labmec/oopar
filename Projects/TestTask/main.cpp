

#ifdef OOP_MPI
#include "oopmpicomm.h"
#endif

#include <iostream>
#include <sstream>
//#include <istream>
#include "oopdatamanager.h" 
#include "ooptaskmanager.h"
//#include "oopfilecomm.h"
#include "ooperror.h"
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


int mainorig (int argc, char **argv)
{
	CM = new OOPMPICommManager (argc, argv);

	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());

		
	TM->Execute();
	if(!CM->GetProcID()){
		OOPInt * inta = new OOPInt;
		OOPInt * intb = new OOPInt;
		OOPObjectId IdA, IdB;
		IdA = DM->SubmitObject(inta);
		IdB = DM->SubmitObject(intb);

		TTaskTest * tta = new TTaskTest(0);
		TTaskTest * ttb = new TTaskTest(0);
		OOPDataVersion ver;
		
		ttb->AddDependentData(OOPAccessTag(
				IdA, EReadAccess, ver,0));
		ttb->AddDependentData(OOPAccessTag(
				IdB, EWriteAccess, ver,0));
		tta->AddDependentData(OOPAccessTag(
				IdA, EWriteAccess, ver,0));
		++ver;			
		tta->AddDependentData(OOPAccessTag(
				IdB, EReadAccess, ver,0));
	
	
		cout << "Task A " << tta->Submit() << endl;
 		cout << "Task B " << ttb->Submit() << endl;
		
		
		OOPWaitTask * wt = new OOPWaitTask(0);
		wt->AddDependentData(OOPAccessTag(
				IdA, EWriteAccess, ver,0));
		wt->AddDependentData(OOPAccessTag(
				IdB, EWriteAccess, ver,0));
		cout << "Wait Task " << wt->Submit()  << endl;
		cout << "Calling Wait Task\n";
		wt->Wait();
		wt->Finish();
		cout << "Wait task finished\n";
		int iproc;
		for(iproc=1; iproc<CM->NumProcessors(); iproc++)
		{
			OOPTerminationTask *task = new OOPTerminationTask(iproc);
			task->Submit();
		}
		sleep(1);
		OOPTerminationTask *task = new OOPTerminationTask(0);
		task->Submit();
	}

	
	TM->Wait();
	delete  DM;
	delete  TM;
	delete  CM;
 
	cout << "Leaving mpimain\n";
	cout.flush();
	return 0;
	
}
int debugmpimain(int argc, char **argv)
{
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
  

  TM->Execute();
  OOPObjectId IdA;
  OOPDataVersion ver;
  if(!CM->GetProcID())
  {
    OOPInt * inta = new OOPInt;
  
    
    IdA = DM->SubmitObject(inta);
    cout << "Submitted OOPInt object Id " << IdA << endl;
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
    sleep(7);
    cout << "Acordou -----------------------------------" << endl;
    ttc->Submit();
    ++ver;
    OOPWaitTask * wt = new OOPWaitTask(0);
    wt->AddDependentData(  OOPAccessTag(
                      IdA, EWriteAccess, ver,0));
    wt->Submit();
    wt->Wait();
    wt->Finish();
    int i;
    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->Submit();
    }
    //sleep(1);
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
int debugmain(int argc, char **argv)
{
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient0.cfg";
  log4cxx::PropertyConfigurator::configure(sin.str()); 
#endif
  CM = new OOPMPICommManager ();//(argc, argv); 
  TM = new OOPTaskManager (0);//CM->GetProcID ());
  DM = new OOPDataManager (0);//CM->GetProcID ());
  
  OOPInt * inta = new OOPInt;
  OOPInt * intb = new OOPInt;
  
  OOPObjectId IdA, IdB;
  IdA = DM->SubmitObject(inta);
  IdB = DM->SubmitObject(intb);

  TTaskTest * tta = new TTaskTest(0);
  TTaskTest * ttb = new TTaskTest(0);
  TTaskTest * ttc = new TTaskTest(0);
  OOPDataVersion ver, verB;
  tta->AddDependentData(OOPAccessTag(
                    IdA, EReadAccess, ver,0));
  tta->AddDependentData(OOPAccessTag(
                    IdB, EWriteAccess, verB,0));
  tta->Submit();
  //++ver;
  ++verB;
  ttb->AddDependentData( OOPAccessTag(
                    IdA, EReadAccess, ver,0));
  ttb->AddDependentData( OOPAccessTag(
                    IdB, EReadAccess, verB,0));
  ttb->Submit();
  ttc->AddDependentData( OOPAccessTag(
                    IdA, EWriteAccess, ver,0));
  ttc->AddDependentData( OOPAccessTag(
                    IdB, EWriteAccess, verB,0));
  ttc->Submit();
  ++ver;
  ++verB;
  OOPTerminationTask * tt = new OOPTerminationTask(0);
  tt->AddDependentData(  OOPAccessTag(
                    IdA, EWriteAccess, ver,0));
  tt->AddDependentData(  OOPAccessTag(
                    IdB, EWriteAccess, verB,0));
                    
  tt->Submit();
  
  //colocar para dentro do Execute do TM
  //TM->ExecuteMTBlocking(TM);
  TM->SetKeepGoing( true);
  while (TM->KeepRunning())
  {
//    TM->TransferSubmittedTasks();
    DM->HandleMessages();
    TM->HandleMessages();
    DM->FlushData();
    TM->TriggerTasks();
    TM->WaitWakeUpCall();
  }
  
  return 0;
}
int main(int argc, char **argv)
{
  debugmpimain(argc, argv);
  //debugmain(argc, argv);
  return 0;
}
int mpimain (int argc, char **argv)
{
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
  TM->Execute();
  if(!CM->GetProcID()){
    OOPInt * inta = new OOPInt;
    //OOPInt * intb = new OOPInt;
    OOPObjectId IdA, IdB;
    IdA = DM->SubmitObject(inta);
    cout << "Submitted OOPInt object Id " << IdA << endl;
    //IdB = DM->SubmitObject(intb, 1);
  
    TTaskTest * tta = new TTaskTest(0);
    TTaskTest * ttb = new TTaskTest(0);
    TTaskTest * ttc = new TTaskTest(0);
    TTaskTest * ttd = new TTaskTest(0);
    
/*    OOPSnapShotTask * sst0 = new OOPSnapShotTask(0);
    OOPSnapShotTask * sst1 = new OOPSnapShotTask(1);
    OOPSnapShotTask * sst2 = new OOPSnapShotTask(2);
    OOPSnapShotTask * sst3 = new OOPSnapShotTask(3);*/
    OOPDataVersion ver;
    ttb->AddDependentData(OOPAccessTag(
                    IdA, EWriteAccess, ver,0));
/*		ttb->AddDependentData(OOPMDataDepend(
				IdB, EWriteAccess, ver));			*/
    ++ver;
    tta->AddDependentData(OOPAccessTag(
                    IdA, EWriteAccess, ver,0));
    ++ver;			
    ttc->AddDependentData(OOPAccessTag(
                    IdA, EWriteAccess, ver,0));
    ++ver;			
    ttd->AddDependentData(OOPAccessTag(
                    IdA, EWriteAccess, ver,0));

/*    cout << "Task A " << tta->Submit() << endl;
    TM->TransferSubmittedTasks();*/
    cout << "Task B " << ttb->Submit() << endl;
//    TM->TransferSubmittedTasks();
//    DM->SubmitAllObjects();
//    cout << "Task C " << ttc->Submit() << endl;
//    cout << "Task D " << ttd->Submit() << endl;
		
		
/*    sleep(5);
    sst0->Submit();    		
    sst1->Submit();    		
    sst2->Submit();    		
    sst3->Submit();    		*/
    		
    ++ver;
    OOPWaitTask * wt = new OOPWaitTask(0);
    wt->AddDependentData(OOPAccessTag(
                    IdA, EReadAccess, ver,0));
/*		wt->AddDependentData(OOPMDataDepend(
				IdB, EWriteAccess, ver));*/
    cout << "Wait Task " << wt->Submit()  << endl;
    cout << "Calling Wait Task\n";
    wt->Wait();
    wt->Finish();
    cout << "Wait task finished\n";
    int iproc;
    for(iproc=1; iproc<CM->NumProcessors(); iproc++)
    {
      OOPTerminationTask *task = new OOPTerminationTask(iproc);
      task->Submit();
    }
    sleep(1);
    OOPTerminationTask *task = new OOPTerminationTask(0);
    task->Submit();
  }
// 	 
//   TM->Wait();

#ifdef LOGPZ
  {
    LOGPZ_DEBUG(logger,"before SubmitAllObjects");
  }
#endif
/**
  int ic;
  for(ic=0; ic<1000; ic++)
  {
    DM->SubmitAllObjects();
  }
*/
#ifdef LOGPZ
{
  LOGPZ_DEBUG(logger,"after SubmitAllObjects");
}
#endif
  sleep(3);
  TM->SetKeepGoing(false);
  sleep(1);
  TM->Wait();
  cout << "Deleting DM\n";
  delete  DM;
  cout << "Deleting TM\n";
  delete  TM;
  cout << "Deleting CM\n";
  delete  CM;

  cout << "Leaving mpimain\n";
  cout.flush();
  return 0;
	
}

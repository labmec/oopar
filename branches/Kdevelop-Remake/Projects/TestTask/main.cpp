

#ifdef OOP_MPI
#include "oopmpicomm.h"
#endif

#include <iostream>
//#include <istream>
#include "oopdatamanager.h" 
#include "ooptaskmanager.h"
//#include "oopfilecomm.h"
#include "ooperror.h"
#include "oopwaittask.h"


#include "OOPDataLogger.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif


#include "TTaskTest.h" 
#include "OOPInt.h"



ofstream TaskLog;
ofstream DataLog;
ofstream DataManLog;
ofstream TransferDataLog;
ofstream TaskQueueLog;
ofstream TaskManLog;
ofstream DataQueueLog;

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
	char filename[256];
	sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
	::LogDM = LogDM;
	sprintf(filename,"tasklog.log%d", CM->GetProcID());
	TaskLog.open(filename);
	sprintf(filename,"datalog.log%d", CM->GetProcID());
	DataLog.open(filename);
	sprintf(filename,"datamanlog.log%d", CM->GetProcID());
	DataManLog.open(filename);
	sprintf(filename,"transferdata.log%d", CM->GetProcID());
	TransferDataLog.open(filename);
	sprintf(filename,"taskqueue.log%d", CM->GetProcID());
	TaskQueueLog.open(filename);
	sprintf(filename,"taskmanlog.log%d", CM->GetProcID());
	TaskManLog.open(filename);
	sprintf(filename,"dataqueuelog.log%d", CM->GetProcID());
	DataQueueLog.open(filename);

	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());

		
	TM->Execute();
	if(!CM->GetProcID()){
		OOPInt * inta = new OOPInt;
		OOPInt * intb = new OOPInt;
		OOPObjectId IdA, IdB;
		IdA = DM->SubmitObject(inta, 1);
		IdB = DM->SubmitObject(intb, 1);

		TTaskTest * tta = new TTaskTest(0);
		TTaskTest * ttb = new TTaskTest(1);
		OOPDataVersion ver;
		
		ttb->AddDependentData(OOPMDataDepend(
				IdA, EReadAccess, ver));
		ttb->AddDependentData(OOPMDataDepend(
				IdB, EWriteAccess, ver));			
		tta->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));
		++ver;			
		tta->AddDependentData(OOPMDataDepend(
				IdB, EReadAccess, ver));			
	
	
		cout << "Task A " << tta->Submit() << endl;
 		cout << "Task B " << ttb->Submit() << endl;
		
		
		OOPWaitTask * wt = new OOPWaitTask(0);
		wt->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));
		wt->AddDependentData(OOPMDataDepend(
				IdB, EWriteAccess, ver));
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
	delete LogDM;
 
	cout << "Leaving mpimain\n";
	cout.flush();
	return 0;
	
}
int main (int argc, char **argv)
{
	CM = new OOPMPICommManager (argc, argv); 

#ifdef OOP_MPE
        gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
        gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
        gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
        gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
        
  
#endif
	char filename[256];
	sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
	::LogDM = LogDM;
	sprintf(filename,"tasklog.log%d", CM->GetProcID());
	TaskLog.open(filename);
	sprintf(filename,"datalog.log%d", CM->GetProcID());
	DataLog.open(filename); 
	sprintf(filename,"datamanlog.log%d", CM->GetProcID());
	DataManLog.open(filename); 
	sprintf(filename,"transferdata.log%d", CM->GetProcID());
	TransferDataLog.open(filename);
	sprintf(filename,"taskqueue.log%d", CM->GetProcID());
	TaskQueueLog.open(filename);
	sprintf(filename,"taskmanlog.log%d", CM->GetProcID());
	TaskManLog.open(filename);
	sprintf(filename,"dataqueuelog.log%d", CM->GetProcID());
	DataQueueLog.open(filename);

	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());

		 
	TM->Execute();
	if(!CM->GetProcID()){
		OOPInt * inta = new OOPInt;
		OOPInt * intb = new OOPInt;
		OOPObjectId IdA, IdB;
		IdA = DM->SubmitObject(inta, 1);
//		IdB = DM->SubmitObject(intb, 1);

		TTaskTest * tta = new TTaskTest(1);
		TTaskTest * ttb = new TTaskTest(0);
		TTaskTest * ttc = new TTaskTest(2);
		TTaskTest * ttd = new TTaskTest(3);
		OOPDataVersion ver;
		
		ttb->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));
/*		ttb->AddDependentData(OOPMDataDepend(
				IdB, EWriteAccess, ver));			*/
                ++ver;
		tta->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));
		++ver;			
		ttc->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));			
		++ver;			
		ttd->AddDependentData(OOPMDataDepend(
				IdA, EWriteAccess, ver));			
	
		cout << "Task A " << tta->Submit() << endl;
 		cout << "Task B " << ttb->Submit() << endl;
 		cout << "Task C " << ttc->Submit() << endl;
 		cout << "Task D " << ttd->Submit() << endl;
		
		++ver;
		OOPWaitTask * wt = new OOPWaitTask(0);
		wt->AddDependentData(OOPMDataDepend(
				IdA, EReadAccess, ver));
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
        
	 
	TM->Wait();
       

	delete  DM; 
	delete  TM;    
	delete  CM;
	delete LogDM;

	cout << "Leaving mpimain\n";
	cout.flush();
	return 0;
	
}

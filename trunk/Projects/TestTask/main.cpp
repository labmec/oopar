
#include <iostream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
//#include "oopfilecomm.h"
#include "ooperror.h"
#include "oopwaittask.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif


#include "OOPDataLogger.h"


#include "TTaskTest.h"
#include "OOPInt.h"



ofstream TaskLog("tasklog.log");
ofstream DataLog("datalog.log");
ofstream DataManLog("datamanlog.log");
ofstream TransferDataLog("transferdatalog.log");
ofstream TaskQueueLog("taskqueue.log");
ofstream TaskManLog("taskmanlog.log");
ofstream DataQueueLog("dataqueuelog.log");

using namespace std;
//class OOPCommunicationManager;
//class OOPDataManager;
//class OOPTaskManager;

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;


int main (int argc, char **argv)
{
	CM = new OOPMPICommManager (argc, argv);
	char filename[256];
	sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
    ::LogDM = LogDM;
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
		cout << "Calling Wait Task\n";
		wt->Wait();
		wt->Finish();
		
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

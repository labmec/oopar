#include "TPartitionRelation.h"
#include "oopstorage.h"
#include "oopfilecomm.h"
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include <iostream>
#include "TParAnalysis.h"
#include "TParCompute.h"
#include "TLocalCompute.h"
#include "TTaskComm.h"
#include "fluxdefs.h"
#include "OOPDataLogger.h"
OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;
OOPDataLogger *LogDM;
using namespace std;
void    ParAddClass ();
int     multimain ();
int     singlemain ();
int     mpimain (int argc, char *argv[]);

void * TesteThread(void *t);
int main (int argc, char *argv[])
{
	OOPReceiveStorage::AddClassRestore (TPARCOMPUTE_ID,
					    TParCompute::Restore);
	OOPReceiveStorage::AddClassRestore (TPARANAYSIS_ID,
					    TParAnalysis::Restore);
	OOPReceiveStorage::AddClassRestore (TLOCALCOMPUTE_ID,
					    TLocalCompute::Restore);
	OOPReceiveStorage::AddClassRestore (TTASKCOMM_ID, TTaskComm::Restore);
	OOPDataLogger * LogDM = new OOPDataLogger("datalogger.log");
    ::LogDM = LogDM;
	
	// return multimain();
#ifdef MPI
	pthread_t testethread_1;
	pthread_create(&testethread_1, NULL, TesteThread, NULL);

	return mpimain (argc, argv);
#else
	return multimain();
#endif
}
int singlemain ()
{
	CM = new OOPFileComManager ("test", 1, 0);
	// CM->Initialize( argv[0], 0 );
	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());
	TParAnalysis *paranalysis = new TParAnalysis (DM->GetProcID (), 2,1);
	paranalysis->Submit ();
	TM->Execute ();
	delete  DM;
	delete  TM;
	delete  CM;
	// cout.flush();
	return 0;
}
void * TesteThread(void *T){
	int i;
	for(i=0;i<10;i++)
		cout << i << " na  thread\n;";
	return NULL;
}

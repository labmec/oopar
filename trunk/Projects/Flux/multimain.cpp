// -*- c++ -*-
#include <iostream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopfilecomm.h"
#include "ooperror.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include "TParAnalysis.h"
#include "TParCompute.h"
#include "TLocalCompute.h"
#include "TTaskComm.h"
#include "TPartitionRelation.h"
#include "TParMesh.h"
#include "TParVector.h"
#include "fluxdefs.h"

#include "OOPDataLogger.h"

ofstream TaskLog("tasklog.log");
ofstream DataLog("datalog.log");
ofstream DataManLog("datamanlog.log");
ofstream TransferDataLog("transferdatalog.log");
ofstream TaskQueueLog("taskqueue.log");
ofstream TaskManLog("taskmanlog.log");
ofstream DataQueueLog("dataqueuelog.log");

int GLogMsgCounter;
int numproc = 4;

vector < OOPCommunicationManager * >CMList (numproc);
vector < OOPDataManager * >DMList (numproc);
vector < OOPTaskManager * >TMList (numproc);
int     NumTasks ();

void    Load (int iproc);
using namespace std;
#include <pthread.h>
//pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;
int multimain ()
{

	//cout << "Number of processors\n";
	//cin >> numproc;
	
	GLogMsgCounter=0;
	int     iproc;
	for (iproc = 0; iproc < numproc; iproc++) {
#ifndef MPI
		CMList[iproc] =
			new OOPFileComManager ("filecom", numproc, iproc);
#else
		char   *argv = "main";
		CMList[iproc] = new OOPMPICommManager (numproc, &argv);
		CMList[iproc]->Initialize (argv, numproc);
#endif
		TMList[iproc] =
			new OOPTaskManager (CMList[iproc]->GetProcID ());
		DMList[iproc] =
			new OOPDataManager (CMList[iproc]->GetProcID ());
	}
	OOPReceiveStorage::AddClassRestore (TPARANAYSIS_ID,
					    TParAnalysis::Restore);
	OOPReceiveStorage::AddClassRestore(TPARCOMPUTE_ID,TParCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TLOCALCOMPUTE_ID,TLocalCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TTASKCOMM_ID,TTaskComm::Restore);
	OOPReceiveStorage::AddClassRestore(TPARMESH_ID,TParMesh::Restore);
	OOPReceiveStorage::AddClassRestore(TPARTITIONRELATION_ID,TPartitionRelation::Restore);
	OOPReceiveStorage::AddClassRestore(TDMOWNERTASK_ID,OOPDMOwnerTask::Restore);
	OOPReceiveStorage::AddClassRestore(TDMREQUESTTASK_ID,OOPDMRequestTask::Restore);
	OOPReceiveStorage::AddClassRestore(TPARVECTOR_ID,TParVector::Restore);
	OOPReceiveStorage::AddClassRestore(TTERMINATIONTASK_ID,OOPTerminationTask::Restore);
	Load (0);
	
	//sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger("filedmlogger.log");
    ::LogDM = LogDM;
	
	
	TParAnalysis *partask = new TParAnalysis (1, numproc, numproc);
	TM->Submit (partask);
	int nsteps=100;
	int k=0;
	while (NumTasks () && k<nsteps) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
			k++;
		}
	}
	TaskLog.close();
	DataLog.close();
	DataManLog.close();
	TaskManLog.close();
	TransferDataLog.close();
	TaskQueueLog.close();
	DataQueueLog.close();
	delete LogDM;
	return 0;
}
#ifdef MPI
int mpimain (int argc, char **argv)
{

	OOPReceiveStorage::AddClassRestore (TPARANAYSIS_ID,
					    TParAnalysis::Restore);
	OOPReceiveStorage::AddClassRestore(TPARCOMPUTE_ID, TParCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TLOCALCOMPUTE_ID, TLocalCompute::Restore);
	OOPReceiveStorage::AddClassRestore(TTASKCOMM_ID, TTaskComm::Restore);
	OOPReceiveStorage::AddClassRestore(TPARMESH_ID, TParMesh::Restore);
	OOPReceiveStorage::AddClassRestore(TPARTITIONRELATION_ID, TPartitionRelation::Restore);
	OOPReceiveStorage::AddClassRestore(TDMOWNERTASK_ID, OOPDMOwnerTask::Restore);
	OOPReceiveStorage::AddClassRestore(TDMREQUESTTASK_ID, OOPDMRequestTask::Restore);
	OOPReceiveStorage::AddClassRestore(TPARVECTOR_ID, TParVector::Restore);
	OOPReceiveStorage::AddClassRestore(TTERMINATIONTASK_ID, OOPTerminationTask::Restore);

	CM = new OOPMPICommManager (argc, argv);
	CM->Initialize((char*)argv, argc);
	char filename[256];
	sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
    ::LogDM = LogDM;
	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());
				    
//	Load (0);
	numproc = CM->NumProcessors();//atoi(argv[argc-1]);
	if(!CM->GetProcID()){
		cout << "Create ParAnalysis on processor " << CM->GetProcID() << endl;
		cout.flush();
		TParAnalysis *partask = new TParAnalysis (0, numproc, numproc);
		TM->Submit (partask);
	}
	TM->Execute();
	/*while (NumTasks ()) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
		}
	}*/
	delete  DM;
	delete  TM;
	delete  CM;
	delete LogDM;

	cout << "Leaving mpimain\n";
	cout.flush();
	return 0;
	
}
#endif
int NumTasks ()
{
	int     numproc = TMList.size ();
	int     iproc, numtask = 0;
	for (iproc = 0; iproc < numproc; iproc++) {
		numtask += TMList[iproc]->NumberOfTasks ();
	}
	return numtask;
}
void Load (int iproc)
{
	CM = CMList[iproc];
	TM = TMList[iproc];
	DM = DMList[iproc];
}

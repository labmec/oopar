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
int numproc;

vector < OOPCommunicationManager * >CMList (numproc);
vector < OOPDataManager * >DMList (numproc);
vector < OOPTaskManager * >TMList (numproc);
int     NumTasks ();

void    Load (int iproc);
using namespace std;
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
	Load (0);
	
	
	
	TParAnalysis *partask = new TParAnalysis (5, numproc, numproc);
	TM->Submit (partask);
	int nsteps=500;
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
int mpimain (int argc, char **argv)
{
	
/*	
	for (iproc = 0; iproc < numproc; iproc++) {
#ifndef MPI
		CMList[iproc] =
			new OOPFileComManager ("filecom", numproc, iproc);
#else
		CMList[iproc] = new OOPMPICommManager (numproc, argv);
		CMList[iproc]->Initialize (*(argv), numproc);
#endif
		TMList[iproc] =
			new OOPTaskManager (CMList[iproc]->GetProcID ());
		DMList[iproc] =
			new OOPDataManager (CMList[iproc]->GetProcID ());
	}
*/
	//pthread_join(testethread_1,0);
	
	//int numproc = argc;
	CM = new OOPMPICommManager (argc, argv);
	CM->Initialize((char*)argv, argc);
	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID ());
	
	OOPReceiveStorage::AddClassRestore (TPARANAYSIS_ID,
					    TParAnalysis::Restore);
//	Load (0);

	TParAnalysis *partask = new TParAnalysis (0, numproc, numproc);
	TM->Submit (partask);
	TM->Execute();
	/*while (NumTasks ()) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
		}
	}*/
	return 0;
}

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

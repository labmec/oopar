// -*- c++ -*-

#include <iostream>

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

#include "fluxdefs.h"

ofstream TaskLog("tasklog.log");
ofstream DataLog("datalog.log");
ofstream DataManLog("datamanlog.log");
ofstream TaskManLog("taskmanlog.log");

int GLogMsgCounter;

const int numproc = 2;
vector < OOPCommunicationManager * >CMList (numproc);
vector < OOPDataManager * >DMList (numproc);
vector < OOPTaskManager * >TMList (numproc);

int     NumTasks ();

void    Load (int iproc);

int multimain ()
{

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

	Load (0);
	TParAnalysis *partask = new TParAnalysis (1, 2, numproc);
	TM->Submit (partask);


	while (NumTasks ()) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
		}
	}


	TaskLog.close();
	DataLog.close();
	DataManLog.close();
	TaskManLog.close();
	return 0;
	

}
int mpimain (int argc, char *argv[])
{

	int     iproc;
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
	OOPReceiveStorage::AddClassRestore (TPARANAYSIS_ID,
					    TParAnalysis::Restore);

	Load (0);
	TParAnalysis *partask = new TParAnalysis (1, 2, numproc);
	TM->Submit (partask);


	while (NumTasks ()) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
		}
	}


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

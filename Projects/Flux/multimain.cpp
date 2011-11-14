// -*- c++ -*-
#include <iostream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopfilecomm.h"

#ifdef OOP_MPI
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

int numproc = 4;

int GLogMsgCounter = 0;

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;

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
#ifndef OOP_MPI
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
		new OOPDataManager (CMList[iproc]->GetProcID (), TMList[iproc]);
	}
	/*	OOPStorageBuffer::AddClassRestore (TPARANAYSIS_ID,
	 TParAnalysis::Restore);
	 OOPStorageBuffer::AddClassRestore(TPARCOMPUTE_ID,TParCompute::Restore);
	 OOPStorageBuffer::AddClassRestore(TLOCALCOMPUTE_ID,TLocalCompute::Restore);
	 OOPStorageBuffer::AddClassRestore(TTASKCOMM_ID,TTaskComm::Restore);
	 OOPStorageBuffer::AddClassRestore(TPARMESH_ID,TParMesh::Restore);
	 OOPStorageBuffer::AddClassRestore(TPARTITIONRELATION_ID,TPartitionRelation::Restore);
	 OOPStorageBuffer::AddClassRestore(TDMOWNERTASK_ID,OOPDMOwnerTask::Restore);
	 OOPStorageBuffer::AddClassRestore(TDMREQUESTTASK_ID,OOPDMRequestTask::Restore);
	 OOPStorageBuffer::AddClassRestore(TPARVECTOR_ID,TParVector::Restore);
	 OOPStorageBuffer::AddClassRestore(TTERMINATIONTASK_ID,OOPTerminationTask::Restore);
	 */
	Load (0);
	
	//sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger("filedmlogger.log");
    ::LogDM = LogDM;
	
	
	TParAnalysis *partask = new TParAnalysis (1, numproc, numproc);
	TMList[0]->Submit (partask);
	int nsteps=100;
	int k=0;
	while (NumTasks () && k<nsteps) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TMList[iproc]->Execute ();
			k++;
		}
	}
	TaskLog.close();
	delete LogDM;
	return 0;
}

#ifdef OOP_MPI
int mpimain (int argc, char **argv)
{
	CM = new OOPMPICommManager (argc, argv);
	CM->Initialize((char*)argv, argc);
	char filename[256];
	sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger(filename);
    ::LogDM = LogDM;
	TM = new OOPTaskManager (CM->GetProcID ());
	DM = new OOPDataManager (CM->GetProcID (), TM);
	
	numproc = CM->NumProcessors();//atoi(argv[argc-1]);
	if(!CM->GetProcID()){
		cout << "Create ParAnalysis on processor " << CM->GetProcID() << endl;
		cout.flush();
		TParAnalysis *partask = new TParAnalysis (1, numproc, numproc);
		TM->Submit (partask);
	}
	TM->Execute();
	
	TM->Wait();
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

// -*- c++ -*-

#include <iostream>

#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopfilecomm.h"
#ifdef MPI
#include "oopmpicomm.h"
#endif
#include "TParAnalysis.h"
#include "fluxdefs.h"
const int numproc = 3;
vector < OOPCommunicationManager * >CMList (numproc);
vector < OOPDataManager * >DMList (numproc);
vector < OOPTaskManager * >TMList (numproc);

int     NumTasks ();

void    Load (int iproc);

int multimain ()
{

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

	Load (0);
	TParAnalysis *partask = new TParAnalysis (1, 2);
	TM->Submit (partask);


	while (NumTasks ()) {
		for (iproc = 0; iproc < numproc; iproc++) {
			Load (iproc);
			TM->Execute ();
		}
	}


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
	TParAnalysis *partask = new TParAnalysis (1, 2);
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

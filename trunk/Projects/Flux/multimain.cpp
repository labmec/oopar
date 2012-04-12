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
#include "oopterminationtask.h"

#include "OOPDataLogger.h"
#include "oopinitializeenvironment.h"

#include "pzlog.h"

int numproc = 4;

int GLogMsgCounter = 0;

using namespace std;
#include <pthread.h>
//pthread_mutex_t fCommunicate = PTHREAD_MUTEX_INITIALIZER;

int main (int argv, char *argc[])
{
	
	//cout << "Number of processors\n";
	//cin >> numproc;
    int numproc = 2;
    int numthread = 4;
	TPZAutoPointer<OOPTaskManager> TM = InitializeEnvironment(argv,argc,numproc,numthread);
	GLogMsgCounter=0;

#ifdef LOG4CXX
	std::stringstream sin;
	sin << "log4cxxclient" << TM->CM()->GetProcID() << ".cfg";
    InitializePZLOG();
#endif

	//sprintf(filename,"datalogger%d", CM->GetProcID());
	OOPDataLogger * LogDM = new OOPDataLogger("filedmlogger.log");
    ::LogDM = LogDM;

	TParAnalysis *partask = new TParAnalysis (numproc-1 , numproc, numproc);
	TM->Submit (partask);
	TaskLog.close();
	delete LogDM;
    ShutDownEnvironment(TM);
	return 0;
}



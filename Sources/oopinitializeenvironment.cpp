//
//  oopinitializeenvironment.cpp
//  OOPAR
//
//  Created by Philippe Devloo on 4/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "oopinitializeenvironment.h"
#include "pzlog.h"
#include "ooptaskmanager.h"
#include "oopdatamanager.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.initialize"));
#endif


#ifdef OOP_INTERNAL
#include "oopinternalcommanager.h"
static void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> >&TMVec);
#endif

#include <iostream>

TPZAutoPointer<OOPTaskManager> InitializeEnvironment(int numproc, int numthreads)
{
    OOPCommunicationManager *pCM = 0;
#ifdef OOP_MPI
	pCM = new OOPMPICommManager(argc, argv);
#elif OOP_SOCKET
	pCM = new OOPSocketCommManager;
	((OOPSocketCommManager *)pCM)->Initialize();
	//CM->Initialize(argv[0], 4);
	((OOPSocketCommManager *)pCM)->Barrier();
#else
	pCM = new OOPInternalCommunicationManager(0, numproc);
#endif
	TPZAutoPointer<OOPCommunicationManager> CM(pCM->CM());
	pCM = 0;
#ifdef LOG4CXX
#ifdef OOP_SOCKET
	std::stringstream sin;
	sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
	log4cxx::PropertyConfigurator::configure(sin.str());
#elif OOP_MPI
	std::stringstream sin;
	sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
	log4cxx::PropertyConfigurator::configure(sin.str());
#elif OOP_INTERNAL
	std::stringstream sin;
	sin << "log4cxxclient.cfg";
	log4cxx::PropertyConfigurator::configure(sin.str());
	LOGPZ_DEBUG(logger,"Estou testando o log");
#endif

#endif
	OOPTaskManager *pTM = new OOPTaskManager(pCM->GetProcID());
	TPZAutoPointer<OOPTaskManager> TM(pTM);
	CM->SetTaskManager(TM);
	OOPDataManager *pDM = new OOPDataManager(CM->GetProcID(), TM);
	TPZAutoPointer<OOPDataManager> DM(pDM);
	
	TM->SetDataManager(DM);
	TM->SetCommunicationManager(CM);
	
	TM->SetNumberOfThreads(numthreads);
	TM->Execute();
    
#ifdef OOP_INTERNAL
    TPZVec<TPZAutoPointer<OOPTaskManager> > TMVec(numproc);
    TMVec[0] = CM->TM();
    SetupEnvironment(TMVec);
#endif
    return TM;
}

#ifdef OOP_INTERNAL
static void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> >&TMVec)
{
	int iproc, jproc;
    int NumProcessors = TMVec.NElements();
	std::vector<OOPInternalCommunicationManager *> AllCMp(NumProcessors);
	std::vector<TPZAutoPointer<OOPCommunicationManager> > AllCM(NumProcessors);
	AllCM[0] = TMVec[0]->CM();
	AllCMp[0] = dynamic_cast<OOPInternalCommunicationManager *>(TMVec[0]->CM().operator->());
	if(!AllCMp[0])
	{
		cout << "SetupEnvironment will only work with OOPInternalCommunicationManager\n";
		DebugStop();
	}
    int NumThreads = TMVec[0]->NumberOfThreads();
    
	for(iproc=1; iproc<NumProcessors; iproc++)
	{
		OOPTaskManager *pTM;
		OOPDataManager *pDM;
		AllCMp[iproc]
		= new OOPInternalCommunicationManager(iproc, NumProcessors);
		AllCM[iproc] = AllCMp[iproc]->CM();
		pTM = new OOPTaskManager(AllCM[iproc]->GetProcID());
		TPZAutoPointer<OOPTaskManager> TM(pTM->TM());
		TMVec[iproc] = TM;
		pTM = 0;
		TM->SetCommunicationManager(AllCM[iproc]);
		AllCM[iproc]->SetTaskManager(TM);
		pDM = new OOPDataManager(AllCM[iproc]->GetProcID(), TM);
		TPZAutoPointer<OOPDataManager> DM(pDM->DM());
		pDM = 0;
		TM->SetDataManager(DM);
		
		TM->SetNumberOfThreads(NumThreads);
		TM->Execute();
	}
	for(iproc=0; iproc<NumProcessors; iproc++)
	{
		for(jproc=0; jproc<NumProcessors; jproc++)
		{
			AllCMp[iproc]->SetCommunicationManager(jproc,AllCM[jproc]);
		}
		AllCM[iproc]->Initialize("Dummy",-1);
	}
}
#endif

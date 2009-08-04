

#ifdef OOP_MPI
#include "oopmpicomm.h"
#endif

#ifdef OOP_SOCKET
#include "oopsocketcommmanager.h"
#endif

#ifdef OOP_INTERNAL
#include "oopinternalcommanager.h"
#endif

#include <iostream>
#include <sstream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopwaittask.h"
#include "oopsnapshottask.h"
#include "oopterminationtask.h"
#include "ooplock.h"
#include "pzlog.h"

#ifdef LOG4CXX
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif
#include "oopaccesstag.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif


#include "TTaskTest.h"
#include "OOPInt.h"

#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
extern OOPGenericLockService gMutex;
#endif


using namespace std;
//class OOPCommunicationManager;
//class OOPDataManager;
//class OOPTaskManager;


const int NumThreads = 10;

#ifdef OOP_INTERNAL
const int NumProcessors = 2;
void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> > &TMVec);
#endif

int debugmpimain(int argc, char **argv)
{
	// keep only the main thread active
#ifdef STEP_MUTEX
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "waiting for lock");
#endif
		OOPLock<OOPGenericLockService> lock(&gMutex);
#endif

	OOPCommunicationManager *pCM;
	OOPDataManager *pDM;
	OOPTaskManager *pTM;

  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0] << std::endl;
#ifdef OOP_MPI
	pCM = new OOPMPICommManager(argc, argv);
#endif
#ifdef OOP_SOCKET
  pCM = new OOPSocketCommManager;
	//
	pCM->Initialize(argv[0], 3);
#endif
#ifdef OOP_INTERNAL
	pCM = new OOPInternalCommunicationManager(0,1);
#endif
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient" << pCM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif

#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",pCM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",pCM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",pCM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",pCM->GetProcID()==0);
#endif
  pTM = new OOPTaskManager (pCM->GetProcID ());
  TPZAutoPointer<OOPTaskManager> TM(pTM);
  TPZAutoPointer<OOPCommunicationManager> CM(pCM);
  CM->SetTaskManager(TM);
  pDM = new OOPDataManager (CM->GetProcID (), TM);
  TPZAutoPointer<OOPDataManager> DM(pDM);

  TM->SetDataManager(DM);
  TM->SetCommunicationManager(CM);

  TM->SetNumberOfThreads(NumThreads);
  TM->Execute();
  OOPObjectId IdA, IdB;
  OOPDataVersion ver, verb;
  if(!CM->GetProcID())
  {
    OOPInt * inta = new OOPInt;
    OOPInt * intb = new OOPInt;


    IdA = DM->SubmitObject(inta);
    IdB = DM->SubmitObject(intb);
    cout << "Submitted OOPInt object Id " << IdA << endl;
    cout << "Submitted OOPInt object Id " << IdB << endl;
    int i = 0;

    for(i = 0; i < 4; i++)
    {
		  TTaskTest * tta = new TTaskTest(0);
      TTaskTest * ttb = new TTaskTest(1);
      TTaskTest * ttc = new TTaskTest(2);
		  TTaskTest * tta1 = new TTaskTest(3);
      TTaskTest * ttb1 = new TTaskTest(4);


      tta->AddDependentData(OOPAccessTag(
                              IdA, EWriteAccess, ver,0));

      ++ver;
      ttb->AddDependentData( OOPAccessTag(
                              IdA, EWriteAccess, ver,0));

      ++ver;
      ttc->AddDependentData( OOPAccessTag(
                              IdA, EWriteAccess, ver,0));
      ++ver;
			tta1->AddDependentData(OOPAccessTag(
																				 IdA, EWriteAccess, ver,0));

      ++ver;
      ttb1->AddDependentData( OOPAccessTag(
																					IdA, EWriteAccess, ver,0));
			ttb1->AddDependentData( OOPAccessTag(
                              IdB, EWriteAccess, verb,0));
      ++verb;
			++ver;
      TM->Submit(tta);
      TM->Submit(ttb);
      TM->Submit(ttc);
      TM->Submit(tta1);
      TM->Submit(ttb1);

    }

    OOPWaitTask * wt = new OOPWaitTask(0);
    wt->AddDependentData(  OOPAccessTag(
                             IdB, EWriteAccess, verb,0));
    TM->Submit(wt);
    //DM->PostRequestDelete(IdA);
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "waiting for process termination");
#endif
#ifdef STEP_MUTEX
		lock.Unlock();
#endif
    wt->Wait();
    wt->Finish();
    //sleep(5);
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "passed through the wait task, locking on the main thread");
#endif
#ifdef STEP_MUTEX
		lock.Lock();
#endif


    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      TM->Submit(tt);
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    TM->Submit(tt);
  }
	std::cout << "Calling wait on all TMs\n";
	std::cout.flush();
#ifdef STEP_MUTEX
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "All tasks submitted, unlocking the action of the task manager");
#endif
		lock.Unlock();
#endif
  TM->Wait();
  //sleep(2);
  int proc = CM->GetProcID();
#ifdef OOP_SOCKET
	((OOPSocketCommManager *)CM.operator->())->Barrier();
#endif
	DM = TPZAutoPointer<OOPDataManager>(0);
	CM = TPZAutoPointer<OOPCommunicationManager>(0);
	TM = TPZAutoPointer<OOPTaskManager>(0);
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "All finished, terminating the main process");
#endif

  cout << "Leaving mpimain\n";
  cout.flush();
  return 0;
}

//const int MAXSIZE = 50000000;
#ifdef NUNCA
int debugmain(int argc, char **argv)
{
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient0.cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif

  {
    TPZManVector<char,MAXSIZE> f_recv_buffr;
    TPZManVector<char,MAXSIZE> f_send_buffr;
  }
  {
    OOPMPIStorageBuffer buff;
  }
  CM = new OOPMPICommManager;//(argc, argv);
  //OOPMPIStorageBuffer buff;
  TM = new OOPTaskManager (0);//CM->GetProcID ());
  DM = new OOPDataManager (0);//CM->GetProcID ());



//   OOPInt * inta = new OOPInt;
//   OOPInt * intb = new OOPInt;
//
//   OOPObjectId IdA, IdB;
//   IdA = DM->SubmitObject(inta);
//   IdB = DM->SubmitObject(intb);
//
//   TTaskTest * tta = new TTaskTest(0);
//   TTaskTest * ttb = new TTaskTest(0);
//   TTaskTest * ttc = new TTaskTest(0);
//   OOPDataVersion ver, verB;
//   tta->AddDependentData(OOPAccessTag(
//                           IdA, EReadAccess, ver,0));
//   tta->AddDependentData(OOPAccessTag(
//                           IdB, EWriteAccess, verB,0));
//   tta->Submit();
//   //++ver;
//   ++verB;
//   ttb->AddDependentData( OOPAccessTag(
//                            IdA, EReadAccess, ver,0));
//   ttb->AddDependentData( OOPAccessTag(
//                            IdB, EReadAccess, verB,0));
//   ttb->Submit();
//   ttc->AddDependentData( OOPAccessTag(
//                            IdA, EWriteAccess, ver,0));
//   ttc->AddDependentData( OOPAccessTag(
//                            IdB, EWriteAccess, verB,0));
//   ttc->Submit();
//   ++ver;
//   ++verB;
//   OOPTerminationTask * tt = new OOPTerminationTask(0);
//   tt->AddDependentData(  OOPAccessTag(
//                            IdA, EWriteAccess, ver,0));
//   tt->AddDependentData(  OOPAccessTag(
//                            IdB, EWriteAccess, verB,0));
//
//   tt->Submit();
//
//   //colocar para dentro do Execute do TM
//   //TM->ExecuteMTBlocking(TM);
//   TM->SetKeepGoing( true);
//   while (TM->KeepRunning())
//   {
//     //    TM->TransferSubmittedTasks();
//     DM->HandleMessages();
//     TM->HandleMessages();
//     DM->FlushData();
//     TM->TriggerTasks();
//     TM->WaitWakeUpCall();
//   }

  return 0;
}
#endif

#include "OOPParMatrix.h"

#include "OOPMatVecMultiply.h"
#include "OOPMergeMatrix.h"
#include "OOPParMatIndexation.h"

#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
OOPGenericLockService gMutex;
#endif

int matmain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0][1] << std::endl;
	std::cout.flush();
	{
		std::ofstream test("testando.out");
		test << "Este e um teste\n";
	}
	OOPCommunicationManager *pCM;
	OOPDataManager *pDM;
	OOPTaskManager *pTM;
#ifdef STEP_MUTEX
		OOPLock<OOPGenericLockService> lock(&gMutex);
#endif

#ifdef OOP_MPI
	pCM = new OOPMPICommManager(argc, argv);
#elif OOP_SOCKET
	pCM = new OOPSocketCommManager;
	((OOPSocketCommManager *)pCM)->Initialize();
	//CM->Initialize(argv[0], 4);
	((OOPSocketCommManager *)pCM)->Barrier();
#else
	pCM = new OOPInternalCommunicationManager(0,NumProcessors);
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

#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif
  pTM = new OOPTaskManager (CM->GetProcID ());
  TPZAutoPointer<OOPTaskManager> TM(pTM->TM());
#ifdef OOP_INTERNAL
  TPZVec<TPZAutoPointer<OOPTaskManager> > TMVec(NumProcessors);
  TMVec[0] = TM;
#endif
  pTM = 0;
  TM->SetCommunicationManager(CM);
  CM->SetTaskManager(TM);
  pDM = new OOPDataManager (CM->GetProcID (), TM);
  TPZAutoPointer<OOPDataManager> DM(pDM->DM());
  pDM = 0;
  TM->SetDataManager(DM);


  TM->SetNumberOfThreads(NumThreads);
  TM->Execute();

#ifdef OOP_INTERNAL
  SetupEnvironment(TMVec);
#endif
  if(!CM->GetProcID())
  {

		std::cout << "Inside Processor 0 \n";
		std::cout.flush();

		int size = 0;
		cout << "Size\n";
		//cin >> size;
		size = 120;
		OOPParMatrix parMatrix;
		TPZFMatrix * theMatrix = new TPZFMatrix;
		TPZFMatrix theUvec;
		parMatrix.BuildMe(size, theMatrix, theUvec);
		TPZFMatrix * theFVec = new TPZFMatrix(size, 1, 0.0);
		std::vector<TPZMatrix * > subm;
		std::vector< std::pair< std::vector<int>, std::vector<int> > > Indices;
		int nparts = 0;
		cout << "NParts\n";
		//cin >> nparts;
		nparts = 2;

#ifdef LOG4CXX_NOT
		{
			std::stringstream sout;
			theMatrix->Print("Matrix", sout, EMathematicaInput);
			theUvec.Print("Vector", sout, EMathematicaInput);

			TPZFMatrix res(theUvec.Rows(), theUvec.Cols());
			theMatrix->Multiply(theUvec, res);
			theMatrix->Print("Matrix", sout, EFormatted);
			theUvec.Print("Vector", sout, EFormatted);
			res.Print("Resultado inicial", sout, EFormatted);
			res.Print("Resultado inicial Math", sout, EMathematicaInput);
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif


		cout << "Dividing the Original Matrix\n";

		parMatrix.DivideMe(nparts, theMatrix, subm, Indices);
		cout << "Original Matrix divided\n Subm size " <<  subm.size() << endl;

		std::vector<TPZMatrix *> vectors;
		vectors.resize(subm.size());
  	int i, j;
		cout.flush();
		for(i = 0; i < vectors.size(); i++)
		{
			int lsize = Indices[i].second.size();
			TPZFMatrix * lvec = new TPZFMatrix(lsize, 1);
			for(j = 0; j < lsize; j++)
			{
				lvec->Put(j, 0, theUvec.Get(Indices[i].second[j], 0));
			}
			cout.flush();
			vectors[i]=lvec;
		}


#ifdef LOG4CXX_NOT
		{
			std::stringstream sout;
			for(i = 0; i < subm.size() ; i++)
			{
				subm[i]->Print("Part", sout, EFormatted);
			}
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif


		OOPObjectId IndexId;
		OOPParMatIndexation * Indexation = new OOPParMatIndexation;
		Indexation->SetIndexationVector(Indices);
		IndexId = DM->SubmitObject(Indexation);

		std::cout << "IndexId " << IndexId << endl;


		OOPObjectId theVecFId;
		theVecFId = DM->SubmitObject(theFVec);
#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "The id of the global vector is " << theVecFId;
			LOGPZ_DEBUG(logger,sout.str())
		}
#endif

		OOPObjectId SubMatIds;

		OOPDataVersion Version;
		std::cout.flush();
		for(i = 0;i < subm.size();i++)
		{
			SubMatIds = DM->SubmitObject(subm[i]);
			OOPMatVecMultiply * tm = new OOPMatVecMultiply(i);
			tm->SetUVector(*vectors[i]);
			tm->SetIndexId(IndexId);
			tm->SetGlobalVecId(theVecFId);
			tm->AddDependentData(OOPAccessTag(SubMatIds, EWriteAccess, Version, 0));
			TM->Submit(tm);
		}

    OOPWaitTask * wt = new OOPWaitTask(0);
		OOPDataVersion wtVersion;
		wtVersion.SetLevelVersion(0, CM->NumProcessors());

		wt->AddDependentData( OOPAccessTag(theVecFId, EWriteAccess, wtVersion,0));
    TM->Submit(wt);
		cout << "Submitting WT with version " << wtVersion << endl;
		cout.flush();
#ifdef STEP_MUTEX
		lock.Unlock();
#endif
#ifdef LOG4CXX
		LOGPZ_DEBUG(logger, "before waiting");
#endif
    wt->Wait();
#ifdef STEP_MUTEX
		lock.Lock();
#endif

#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "Leaving after WaitTask\n";
			TPZFMatrix * resVec = dynamic_cast<TPZFMatrix *> (wt->GetDepObjPtr(0));
			resVec->Print("Resultado Principal", sout, EFormatted);
			resVec->Print("Resultado Principal Math", sout, EMathematicaInput);
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif

    wt->Finish();

#ifdef LOG4CXX
		{
			std::stringstream sout;
			sout << "Leaving after WaitTask\n";
			LOGPZ_DEBUG(logger, sout.str());
		}
#endif

    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->SetProcOrigin(CM->GetProcID());
      TM->Submit(tt);
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->SetProcOrigin(CM->GetProcID());
    TM->Submit(tt);

	}
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Calling wait on all TMs TM.Count " << TM.Count();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef STEP_MUTEX
		lock.Unlock();
#endif
	TM->Wait();
	TM->ClearPointer();
	TM = TPZAutoPointer<OOPTaskManager>(0);
	CM = TPZAutoPointer<OOPCommunicationManager>(0);
	DM = TPZAutoPointer<OOPDataManager>(0);
#ifdef OOP_INTERNAL
	int iproc;
	for(iproc=1; iproc < NumProcessors; iproc++)
	{
		TMVec[iproc]->Wait();
		TMVec[iproc]->ClearPointer();
		TMVec[iproc] = TPZAutoPointer<OOPTaskManager>(0);
	}
#endif
#ifdef STEP_MUTEX
		lock.Lock();
#endif
#ifdef LOG4CXX
	{
		std::stringstream sout ;
		sout << "Leaving wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef OOP_SOCKET
//	((OOPSocketCommManager *)CM)->Barrier();
#endif
//#ifdef LOG4CXX
//	{
//		std::stringstream sout;
//		sout << "Leaving Application on Processor " << CM->GetProcID();
//		LOGPZ_DEBUG(logger, sout.str().c_str());
//	}
//#endif
  return 0;
}
int main(int argc, char **argv)
{
  //debugmpimain(argc, argv);
	matmain(argc, argv);
  //debugmain(argc, argv);
  return 0;
}

#ifdef OOP_INTERNAL
void SetupEnvironment(TPZVec<TPZAutoPointer<OOPTaskManager> >&TMVec)
{
	int iproc, jproc;
	std::vector<OOPInternalCommunicationManager *> AllCMp(NumProcessors);
	std::vector<TPZAutoPointer<OOPCommunicationManager> > AllCM(NumProcessors);
	AllCM[0] = TMVec[0]->CM();
	AllCMp[0] = dynamic_cast<OOPInternalCommunicationManager *>(TMVec[0]->CM().operator->());
	if(!AllCMp[0])
	{
		cout << "SetupEnvironment will only work with OOPInternalCommunicationManager\n";
		exit(-1);
	}
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

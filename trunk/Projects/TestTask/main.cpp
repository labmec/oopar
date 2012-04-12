
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

#include "oopinitializeenvironment.h"

#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
extern OOPGenericLockService gMutex;
#endif

using namespace std;

const int NumThreads = 10;


int main2(int argc, char **argv)
{
	// keep only the main thread active
#ifdef STEP_MUTEX
#ifdef LOG4CXX
	LOGPZ_DEBUG(logger, "waiting for lock");
#endif
	OOPLock<OOPGenericLockService> lock(&gMutex);
#endif
	
	std::cout << "argc " << argc << std::endl;
	std::cout << "argv " << argv[0] << std::endl;
    
    int numproc = 2;
    TPZAutoPointer<OOPTaskManager> TM = InitializeEnvironment(argc, argv, numproc, NumThreads);
#ifdef LOG4CXX
	std::stringstream sin;
	sin << "log4cxxclient" << TM->CM()->GetProcID() << ".cfg";
	log4cxx::PropertyConfigurator::configure(sin.str());
#endif
	
#ifdef OOP_MPE
	gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",pCM->GetProcID()==0);
	gEvtDB.AddStateEvent("waittask","Wait Task Call","red",pCM->GetProcID()==0);
	gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",pCM->GetProcID()==0);
	gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",pCM->GetProcID()==0);
#endif
	TPZAutoPointer<OOPCommunicationManager> CM(TM->CM());
	TPZAutoPointer<OOPDataManager> DM(TM->DM());
	
	OOPObjectId IdA, IdB;
	OOPDataVersion ver, verb;
	if (!CM->GetProcID())
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
			
			tta->AddDependentData(OOPAccessTag(IdA, EWriteAccess, ver, 0));
			
			++ver;
			ttb->AddDependentData(OOPAccessTag(IdA, EWriteAccess, ver, 0));
			
			++ver;
			ttc->AddDependentData(OOPAccessTag(IdA, EWriteAccess, ver, 0));
			++ver;
			tta1->AddDependentData(OOPAccessTag(IdA, EWriteAccess, ver, 0));
			
			++ver;
			ttb1->AddDependentData(OOPAccessTag(IdA, EWriteAccess, ver, 0));
			ttb1->AddDependentData(OOPAccessTag(IdB, EWriteAccess, verb, 0));
			++verb;
			++ver;
			TM->Submit(tta);
			TM->Submit(ttb);
			TM->Submit(ttc);
			TM->Submit(tta1);
			TM->Submit(ttb1);
			
		}
		
		OOPWaitTask * wt = new OOPWaitTask(0);
		wt->AddDependentData(OOPAccessTag(IdB, EWriteAccess, verb, 0));
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
		
	}
	std::cout << "Calling wait on all TMs\n";
	std::cout.flush();
#ifdef STEP_MUTEX
#ifdef LOG4CXX
	LOGPZ_DEBUG(logger, "All tasks submitted, unlocking the action of the task manager");
#endif
	lock.Unlock();
#endif
	//sleep(2);
	int proc = CM->GetProcID();
#ifdef OOP_SOCKET
	((OOPSocketCommManager *)CM.operator->())->Barrier();
#endif
	DM = TPZAutoPointer<OOPDataManager> (0);
	CM = TPZAutoPointer<OOPCommunicationManager> (0);
#ifdef LOG4CXX
	LOGPZ_DEBUG(logger, "All finished, terminating the main process");
#endif

	ShutDownEnvironment(TM);
	cout << "Leaving main\n";
	cout.flush();
	return 0;
}


#include "OOPParMatrix.h"

#include "OOPMatVecMultiply.h"
#include "OOPMergeMatrix.h"
#include "OOPParMatIndexation.h"
#include "oopinitializeenvironment.h"

#ifdef STEP_MUTEX
#include "oopgenericlockservice.h"
OOPGenericLockService gMutex;
#endif

int main(int argc, char **argv)
{
	std::cout << "argc " << argc << std::endl;
	std::cout << "argv " << argv[0][1] << std::endl;
	std::cout.flush();
	{
		std::ofstream test("testando.out");
		test << "Este e um teste\n";
	}
    
    int numproc = 2;
    TPZAutoPointer<OOPTaskManager> TM = InitializeEnvironment(argc, argv, numproc, NumThreads);
	TPZAutoPointer<OOPCommunicationManager> CM(TM->CM());
    TPZAutoPointer<OOPDataManager> DM = TM->DM();
#ifdef LOG4CXX
	std::stringstream sin;
	sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
	InitializePZLOG(sin.str());
#endif
	
#ifdef OOP_MPE
	gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
	gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
	gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
	gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif

	if (CM->GetProcID() == 0)
	{
		
		std::cout << "Inside Processor 0 \n";
		std::cout.flush();
		
		int size = 0;
		cout << "Size\n";
		//cin >> size;
		size = 120;
		OOPParMatrix parMatrix;
		TPZFMatrix<REAL> * theMatrix = new TPZFMatrix<REAL>;
		TPZFMatrix<REAL> theUvec;
		parMatrix.BuildMe(size, theMatrix, theUvec);
		TPZFMatrix<REAL> * theFVec = new TPZFMatrix<REAL>(size, 1, 0.0);
		std::vector<TPZMatrix<REAL> *> subm;
		std::vector<std::pair<std::vector<int>, std::vector<int> > > Indices;
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
		cout << "Original Matrix divided\n Subm size " << subm.size() << endl;
		
		std::vector<TPZMatrix<REAL> *> vectors;
		vectors.resize(subm.size());
		int i, j;
		cout.flush();
		for(i = 0; i < vectors.size(); i++)
		{
			int lsize = Indices[i].second.size();
			TPZFMatrix<REAL> * lvec = new TPZFMatrix<REAL>(lsize, 1);
			for(j = 0; j < lsize; j++)
			{
				lvec->Put(j, 0, theUvec.Get(Indices[i].second[j], 0));
			}
			cout.flush();
			vectors[i] = lvec;
		}
		
#ifdef LOG4CXX_NOT
		{
			std::stringstream sout;
			for(i = 0; i < subm.size(); i++)
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
		for(i = 0; i < subm.size(); i++)
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
		
		wt->AddDependentData(OOPAccessTag(theVecFId, EWriteAccess, wtVersion, 0));
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
			TPZFMatrix<REAL> * resVec = dynamic_cast<TPZFMatrix<REAL> *> (wt->GetDepObjPtr(0));
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
		
		for(i = 1; i < CM->NumProcessors(); i++)
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
	CM = TPZAutoPointer<OOPCommunicationManager> (0);
	DM = TPZAutoPointer<OOPDataManager> (0);
#ifdef STEP_MUTEX
	lock.Lock();
#endif
#ifdef LOG4CXX
	{
		std::stringstream sout;
		sout << "Leaving wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	
    ShutDownEnvironment(TM);
	return 0;
}



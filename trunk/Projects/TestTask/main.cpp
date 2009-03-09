

#ifdef OOP_MPI
#include "oopmpicomm.h"
#else
#include "oopsocketcommmanager.h"
#endif



#include <iostream>
#include <sstream>
//#include <istream>
#include "oopdatamanager.h"
#include "ooptaskmanager.h"
#include "oopwaittask.h"
#include "oopsnapshottask.h"
#include "pzlog.h"

#ifdef LOGPZ
static LoggerPtr logger(Logger::getLogger("OOPAR.mainprogram"));
#endif
#include "oopaccesstag.h"

#ifdef OOP_MPE
#include "oopevtmanager.h"
#endif


#include "TTaskTest.h"
#include "OOPInt.h"




using namespace std;
//class OOPCommunicationManager;
//class OOPDataManager;
//class OOPTaskManager;

OOPCommunicationManager *CM;
OOPDataManager *DM;
OOPTaskManager *TM;


int debugmpimain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0] << std::endl;
#ifdef OOP_MPI
	CM = new OOPMPICommManager(argc, argv);
#else
  CM = new OOPSocketCommManager;
	//
	CM->Initialize(argv[0], 3);
#endif
#ifdef LOG4CXX
  std::stringstream sin;
  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#endif

#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif
  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());


  TM->SetNumberOfThreads(10);
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
      tta->Submit();
      ttb->Submit();
      ttc->Submit();
      tta1->Submit();
      ttb1->Submit();
			
    }
		 
    OOPWaitTask * wt = new OOPWaitTask(0);
    wt->AddDependentData(  OOPAccessTag(
                             IdB, EWriteAccess, verb,0));
    wt->Submit();
    //DM->PostRequestDelete(IdA);
    wt->Wait();
    wt->Finish();
    //sleep(5);

    
    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->Submit();
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->Submit();
  }
	std::cout << "Calling wait on all TMs\n";
	std::cout.flush();
  TM->Wait();
  //sleep(2);
  int proc = CM->GetProcID();
  cout << "Deleting DM on Processor " << proc << endl;
  delete  DM;
  cout << "Deleting TM on Processor " << proc << endl;
  delete  TM;
#ifdef OOP_SOCKET	
	((OOPSocketCommManager *)CM)->Barrier();
#endif
  cout << "Deleting CM on Processor " << proc << endl;
  delete  CM;

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

int matmain(int argc, char **argv)
{
  std::cout << "argc " << argc << std::endl;
  std::cout << "argv " << argv[0][1] << std::endl;
	std::cout.flush();
#ifdef OOP_MPI
	CM = new OOPMPICommManager(argc, argv);
#else
  CM = new OOPSocketCommManager;
	((OOPSocketCommManager *)CM)->Initialize();
	//CM->Initialize(argv[0], 4);
	((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOG4CXX
#ifdef OOP_SOCKET
  std::stringstream sin;
  sin << "/Users/longhin/Projetos/XCode/OOParApp/build/Debug/log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
#elif OOP_MPI
  std::stringstream sin;
  sin << "log4cxxclient" << CM->GetProcID() << ".cfg";
  log4cxx::PropertyConfigurator::configure(sin.str());
	
#endif
	
#endif
	
#ifdef OOP_MPE
  gEvtDB.AddStateEvent("taskexec","Task Execution", "blue",CM->GetProcID()==0);
  gEvtDB.AddStateEvent("waittask","Wait Task Call","red",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("grantaccess","Grant Access", "green",CM->GetProcID()==0);
  gEvtDB.AddSoloEvent("incrementversion","Inc Version", "red",CM->GetProcID()==0);
#endif
  TM = new OOPTaskManager (CM->GetProcID ());
  DM = new OOPDataManager (CM->GetProcID ());
	
	
  TM->SetNumberOfThreads(10);
  TM->Execute();
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
		
#ifdef LOGPZ
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
		
		
#ifdef LOGPZ
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
			tm->Submit();
		}

    OOPWaitTask * wt = new OOPWaitTask(0);
		OOPDataVersion wtVersion;
		wtVersion.SetLevelVersion(0, CM->NumProcessors());
		
		wt->AddDependentData( OOPAccessTag(theVecFId, EWriteAccess, wtVersion,0));
    wt->Submit();
		cout << "Submitting WT with version " << wtVersion << endl;
		cout.flush();
    wt->Wait();
		
#ifdef LOGPZ
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
		
#ifdef LOGPZ
		{
			std::stringstream sout;
			sout << "Leaving after WaitTask\n";
			LOGPZ_DEBUG(logger, sout.str().c_str());
		}
#endif

    for(i = 1;i< CM->NumProcessors();i++)
    {
      OOPTerminationTask * tt = new OOPTerminationTask(i);
      tt->Submit();
    }
    OOPTerminationTask * tt = new OOPTerminationTask(0);
    tt->Submit();
 
	}
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Calling wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	TM->Wait();
#ifdef LOGPZ
	{
		std::stringstream sout ;
		sout << "Leaving wait on all TMs\n";
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting DM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	delete  DM;
#ifdef OOP_SOCKET
//	((OOPSocketCommManager *)CM)->Barrier();
#endif
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting TM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
	delete  TM;
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Deleting CM on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
#ifdef OOP_SOCKET
	//((OOPSocketCommManager *)CM)->Barrier();
#endif
	delete  CM;
#ifdef LOGPZ
	{
		std::stringstream sout;
		sout << "Leaving Application on Processor " << CM->GetProcID();
		LOGPZ_DEBUG(logger, sout.str().c_str());
	}
#endif
  return 0;
}
int main(int argc, char **argv)
{
  //debugmpimain(argc, argv); 
	matmain(argc, argv);
  //debugmain(argc, argv);
  return 0;
}


